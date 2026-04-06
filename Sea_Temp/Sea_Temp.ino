#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
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
const char* webhook_url = DISCORD_WEBHOOK;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void send_to_discord(float temp) {
	WiFi.begin(ssid, password);
	
	Serial.print("Connecting to WiFi");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println(" connected");

	WiFiClientSecure client;
	client.setInsecure();
	HTTPClient http;

	http.begin(client, webhook_url);
	http.addHeader("Content-Type", "application/json");

	String payload = "{\"content\":\"Sea temp: " + String(temp, 1) + " °C\"}";
	int response = http.POST(payload);
	
	Serial.print("Discord response: ");
	Serial.println(response);
	
	http.end();
	WiFi.disconnect(true);
}

void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_OFF);

	sensors.begin();
	sensors.requestTemperatures();
	float temp = sensors.getTempCByIndex(0);

	if (temp == DEVICE_DISCONNECTED_C) {
		Serial.println("Error: sensor not found");
	} else {
		Serial.print("Temperature: ");
		Serial.print(temp);
		Serial.println(" °C");
		send_to_discord(temp);
	}

	Serial.print("Sleeping for ");
	Serial.print(SLEEP_SECONDS);
	Serial.println("s...");
	
	ESP.deepSleep(SLEEP_SECONDS * 1000000UL);
}

void loop() {}