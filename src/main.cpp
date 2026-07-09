#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

#include "secrets.h"

// Display
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Sensors
DHTesp dhtSensor;
const int DHT_PIN = 4;
const int MQ5_AO = 1;

//Timing
// Sample locally every 2s.
const unsigned long READ_INTERVAL_MS = 2000;
const unsigned long BATCH_INTERVAL_MS = 10000;
const int MAX_READINGS_PER_BATCH = 8; // safety cap on the buffer

unsigned long lastReadMillis = 0;
unsigned long lastBatchMillis = 0;

struct SensorReading {
    time_t timestamp;
    float temp;
    float hum;
    int gas;
};

SensorReading buffer[MAX_READINGS_PER_BATCH];
int bufferCount = 0;

// Cached latest values just for the OLED
float lastTemp = NAN;
float lastHum = NAN;
int lastGas = 0;

void connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(300);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("WiFi connected. IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("WiFi connection failed.");
    }
}

// Syncs real time over NTP so each batched point gets a distinct, correct timestamp.
void setupTime() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Syncing time");
    time_t now = time(nullptr);
    unsigned long start = millis();

    while (now < 1700000000 && millis() - start < 15000) {
        delay(300);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println();

    if (now < 1700000000) {
        Serial.println("Time sync failed - timestamps will be wrong.");
    } else {
        Serial.print("Time synced: ");
        Serial.println((long)now);
    }
}

// Sends every buffered reading as ONE multi-line InfluxDB write.
bool flushBatchToInfluxDB() {
    if (bufferCount == 0) return true;

    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
        if (WiFi.status() != WL_CONNECTED) return false;
    }

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String org = INFLUXDB_ORG;
    org.replace(" ", "%20");

    String url = String(INFLUXDB_URL) +
                 "/api/v2/write?org=" + org +
                 "&bucket=" + INFLUXDB_BUCKET +
                 "&precision=s";

    // Build one line per buffered reading, newline-separated
    String body = "";
    for (int i = 0; i < bufferCount; i++) {
        char line[160];
        snprintf(line, sizeof(line),
                 "sensor_data,device=esp32_s2 temperature=%.2f,humidity=%.2f,gas=%d %lld\n",
                 buffer[i].temp, buffer[i].hum, buffer[i].gas,
                 (long long)buffer[i].timestamp);
        body += line;
    }

    Serial.print("Flushing batch of ");
    Serial.print(bufferCount);
    Serial.println(" readings in ONE request:");
    Serial.print(body);

    http.begin(client, url);
    http.addHeader("Authorization", "Token " + String(INFLUXDB_TOKEN));
    http.addHeader("Content-Type", "text/plain; charset=utf-8");

    int httpCode = http.POST(body);
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    String response = http.getString();
    if (response.length() > 0) {
        Serial.println(response);
    }

    http.end();

    bool success = (httpCode == 204);
    if (success) {
        bufferCount = 0; // safe to clear - InfluxDB confirmed the write
    } else {
        Serial.println("Write failed - keeping buffered readings for next attempt.");
        // Buffer is intentionally NOT cleared here so failed readings aren't lost.
        // the oldest readings get overwritten.
    }
    return success;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("System starting up...");

    Wire.begin(8, 9);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED initialization failed.");
        while (true) delay(1000);
    }

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);

    dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

    connectWiFi();
    setupTime();
}

void loop() {
    unsigned long now = millis();

    //Sample every 2s
    if (now - lastReadMillis >= READ_INTERVAL_MS) {
        lastReadMillis = now;

        TempAndHumidity data = dhtSensor.getTempAndHumidity();
        lastTemp = data.temperature;
        lastHum = data.humidity;
        lastGas = analogRead(MQ5_AO);

        if (bufferCount < MAX_READINGS_PER_BATCH) {
            buffer[bufferCount].timestamp = time(nullptr);
            buffer[bufferCount].temp = lastTemp;
            buffer[bufferCount].hum = lastHum;
            buffer[bufferCount].gas = lastGas;
            bufferCount++;
        }

        display.clearDisplay();
        display.setCursor(0, 0);
        display.printf("Temp: %.1f C\n", lastTemp);
        display.printf("Hum : %.1f %%\n", lastHum);
        display.printf("Gas : %d\n", lastGas);
        display.print("WiFi: ");
        display.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
        display.display();
    }

    //Flush the whole batch in ONE HTTPS
    if (now - lastBatchMillis >= BATCH_INTERVAL_MS) {
        lastBatchMillis = now;
        if (!flushBatchToInfluxDB()) {
            Serial.println("Upload failed this cycle - will retry with buffered + new readings.");
        }
    }
}
