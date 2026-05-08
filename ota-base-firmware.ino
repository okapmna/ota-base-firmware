#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <ArduinoOTA.h>
#include <esp_system.h>
#include "secret.h"

// CONFIG
const char* current_version = "1.0.0";
const char* device_type     = "esp32-base-device";

// OTA DEBUG VARIABLES
bool updateAvailable = false;
String pendingUpdateUrl = "";
String latest_version = "";

// GLOBAL OBJECTS   
WiFiClientSecure espClient;
PubSubClient client(espClient);
Preferences pref;

// SHARED VARIABLES 
String storage_ssid, storage_pass, storage_mqtt_host, storage_dev_id, storage_ota_server;
int storage_mqtt_port;

// FUNCTION PROTOTYPES 
void setupWiFi();
void setupOTA();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void checkOTAUpdate();
void executeOTAUpdate();

void setup() {
  Serial.begin(9600);

  pref.begin("config", false);

  esp_reset_reason_t resetReason = esp_reset_reason();
  bool isOtaReboot = (resetReason == ESP_RST_SW);

  if (!isOtaReboot) {
    // Flash via kabel USB atau power on: tulis ulang kredensial dari secret.h
    Serial.println("[CONFIG] Flash via kabel / power on terdeteksi. Reset kredensial dari secret.h");
    pref.putString("ssid", ssid);
    pref.putString("pass", password);
    pref.putString("mqtt_host", mqtt_server);
    pref.putInt("mqtt_port", mqtt_port);
    pref.putString("dev_id", device_id);
    pref.putString("ota_srv", ota_server);
  } else {
    // Reboot dari OTA: pertahankan kredensial yang sudah ada di memori
    Serial.println("[CONFIG] Reboot dari OTA terdeteksi. Kredensial dipertahankan.");
  }

  storage_ssid = pref.getString("ssid");
  storage_pass = pref.getString("pass");
  storage_mqtt_host = pref.getString("mqtt_host");
  storage_mqtt_port = pref.getInt("mqtt_port");
  storage_dev_id = pref.getString("dev_id");
  storage_ota_server = pref.getString("ota_srv");
  Serial.print("[CONFIG] Device ID yang digunakan: ");
  Serial.println(storage_dev_id);
  Serial.print("[CONFIG] OTA Server: ");
  Serial.println(storage_ota_server);
  
  setupWiFi();
  setupOTA(); 

  espClient.setInsecure();
  client.setServer(storage_mqtt_host.c_str(), storage_mqtt_port);
  client.setCallback(callback);

  Serial.println("\n--- DEBUG MODE ACTIVE ---");
  Serial.println("Ketik 'check' untuk cek update manual");
  Serial.println("Ketik 'update' untuk eksekusi jika update tersedia");
  Serial.println("--------------------------");
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) reconnect();
  client.loop();

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.equalsIgnoreCase("check")) {
      checkOTAUpdate();
    } 
    else if (input.equalsIgnoreCase("update")) {
      if (updateAvailable) {
        executeOTAUpdate();
      } else {
        Serial.println("[DEBUG] Tidak ada update. Ketik 'check' dulu.");
      }
    }
  }
}
