Strathmore University

Bachelor Of Science in Informatics and Computer Science (BICS)

Embedded Systems and Internet of Things (IOT)

ICS 4A

Deliverable 1

Group: Motion Masters

Group Picture



Question 1:

The carnation (Dianthus caryophyllus), commonly known as the "clove pink," is a perennial flower native to the Mediterranean region. It grows 30-80 cm tall with narrow blue-green leaves and flowers available in colours including red, white, pink, yellow, and purple. It is widely grown as a cut flower valued for its long vase life.

Temperature: 18 - 24 °C (day), 10 - 15 °C (night)

Carnations are cool-weather plants. Daytime temperatures of 18-24°C support efficient photosynthesis and stem development, while cooler nights of 10-15°C promote bud formation. Temperatures above 30°C cause heat stress, resulting in thin stems.

Relative Humidity: 50-70%

This range provides adequate atmospheric moisture for healthy growth without creating conditions that favour fungal diseases such as Botrytis and rust. Below 40%, the air becomes too dry, stressing the plant, while above 70% promotes mould and disease in enclosed greenhouse environments.

Soil Type: Sandy Loam

Carnation roots are highly susceptible to rot. Sandy loam provides the balance of free drainage and adequate moisture retention that prevents waterlogging while keeping roots supplied with nutrients. Heavy clay soils must be amended with compost before planting

Soil Moisture: 40-60% of Field Capacity

Carnations need consistent moisture for nutrient uptake and flower development, but waterlogged soil suffocates roots and encourages disease. Staying within 40-60% of field capacity keeps the soil damp enough to sustain growth without crossing into harmful saturation.

Soil pH: 6.0-7.0

This slightly acidic to neutral range ensures key nutrients, particularly phosphorus, calcium, and iron, which remain soluble and available to the plant. A pH below 6.0 locks out nutrients, while above 7.0 causes micronutrient deficiencies that result in yellowing leaves and stunted growth.

Sunlight Exposure: 6-8 hours/day

Adequate sunlight drives photosynthesis and supports stem strength. Insufficient light produces weak stems and fewer flowers.

PART 2. Suitable hardware components required.

DHT22 (AM2302)
Arduino Uno Microcontroller
Soil Moisture Sensor
Soil pH Meter Kit
BH1750 Light Sensor (I2C)
MQ-2 Gas Sensor
10kΩ resistor
16x2 LCD display
Breadboard
Capacitors
Jumper wires
Micro USB cable
3. Component Documentation and Links

1.3" White IIC 128X64 OLED LCD -
https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

ESP32S DevKIT WIFI + BLE Module (30Pin) -https://www.es.co.th/Schemetic/PDF/ESP32.PDF
DHT22 AM2302 Temperature and Humidity Sensor - https://ardustore.dk/error/DHT22%20Datasheet.pdf
MQ-5 LPG, Natural Gas, Coal Gas Sensor -
https://www.winsen-sensor.com/d/files/MQ-5.pdf

5V 1-Channel Low Level Trigger Relay Module -
https://handsontec.com/dataspecs/relay/1Ch-relay.pdf

Question 4:

1 ESP32S connected to 1 MQ-5, 1 DHT22 and 1 LCD


1 ESP32S connected to 1 MQ-5 interfaced directly with another ESP32S connected to 1 DHT22


1 ESP32S connected to 1 DHT22 connected to 1 relay which is connected to another ESP32S connected to 1 MQ-5
