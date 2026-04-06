#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"


#define ONE_WIRE_BUS D1
#define TESTING true

#if TESTING
	#define SLEEP_SECONDS 5
#else
	#define SLEEP_SECONDS 3600
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_topic = "norway/hovik/sea/temperature";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
WiFiClient espClient;
PubSubClient client(espClient);

void connect_wifi() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	Serial.print("Connecting to WiFi");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println(" connected");
}

void connect_mqtt() {
	client.setServer(mqtt_server, 1883);

	Serial.print("Connecting to MQTT");
	while (!client.connected()) {
		if (client.connect("wemos_sea_temp")) {
			Serial.println(" connected");
		} else {
			Serial.print(".");
			delay(500);
		}
	}
}

void setup() {
	Serial.begin(115200);
	sensors.begin();
	sensors.requestTemperatures();
	float temp = sensors.getTempCByIndex(0);

	if (temp == DEVICE_DISCONNECTED_C) {
		Serial.println("Error: sensor not found");
	} else {
		Serial.print("Temperature: ");
		Serial.print(temp);
		Serial.println(" °C");

		connect_wifi();
		connect_mqtt();

		String payload = String(temp, 1);
		client.publish(mqtt_topic, payload.c_str());
		Serial.print("Published: ");
		Serial.println(payload);

		client.disconnect();
		WiFi.disconnect(true);
	}

	Serial.print("Sleeping for ");
	Serial.print(SLEEP_SECONDS);
	Serial.println("s...");

	ESP.deepSleep(SLEEP_SECONDS * 1000000UL);
}

void loop() {}