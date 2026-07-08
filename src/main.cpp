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

// ---- Timing ----
// Sample locally every 2s (cheap, no network involved).
// Send ONE batched HTTPS request every ~10s containing all readings collected
// since the last flush. This matters because each HTTPS/TLS handshake in the
// Wokwi simulator is slow - batching keeps sampling fast while keeping the
// number of slow network calls low.
const unsigned long READ_INTERVAL_MS = 2000;
const unsigned long BATCH_INTERVAL_MS = 10000;
const int MAX_READINGS_PER_BATCH = 8; // safety cap on the buffer

unsigned long lastReadMillis = 0;
unsigned long lastBatchMillis = 0;

struct SensorReading {
    time_t timestamp; // Unix epoch seconds - needs NTP sync, see setupTime()
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
// Without this, all points sent in one batch would share the same server-assigned
// write time and InfluxDB would keep only the LAST one (same series + same
// timestamp = overwrite, not append).
void setupTime() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Syncing time");
    time_t now = time(nullptr);
    unsigned long start = millis();
    // 1700000000 ~ Nov 2023 - anything before that means we haven't synced yet
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

// Sends every buffered reading as ONE multi-line InfluxDB write (one HTTPS call total).
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

    // Build one line per buffered reading, newline-separated (line protocol batch format)
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

    bufferCount = 0; // clear regardless, so a failed send doesn't grow the buffer forever
    return httpCode == 204;
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

    // ---- Sample every 2s (fast, local only, no network) ----
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

    // ---- Flush the whole batch in ONE HTTPS call every ~10s ----
    if (now - lastBatchMillis >= BATCH_INTERVAL_MS) {
        lastBatchMillis = now;
        flushBatchToInfluxDB();
    }
}