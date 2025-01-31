#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "secrets.h"

// Function to initialize Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Connected to WiFi!");
}

// Function to initialize MQTT with optional callback
void mqttSetup(PubSubClient &client, MQTT_CALLBACK_SIGNATURE = nullptr) {
  client.setServer(SECRET_MQTT_BROKER, SECRET_MQTT_PORT);
  if (callback) {
      client.setCallback(callback); // Set callback only if provided
  }
}

// Function to establish an MQTT connection
void connectToMQTT(PubSubClient &client, const char* clientId, const char* topic) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId)) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe(topic);
    } else {
      Serial.print("Failed. Retrying in 1 second...");
      delay(1000);
    }
  }
}

// Function to publish a message to a specific MQTT topic
void mqttPublish(const char* topic, const char* message) {
  if (client.publish(topic, message)) {
    Serial.print("Message published to topic: ");
    Serial.println(topic);
  } else {
    Serial.println("Failed to publish message");
  }
}
