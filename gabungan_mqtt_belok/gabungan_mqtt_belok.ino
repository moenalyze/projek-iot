#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "HX711.h"

const char* ssid = "ssid";
const char* password = "password";

const char* mqtt_server = "3577e8e742b544419045e2a77c8ec76d.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "Maan123";
const char* mqtt_password = "iniProjekIoT123";
const char* device_id = "esp32_001";

// Topik MQTT
const char* topic_weight_data = "device/weight/data";
const char* topic_control = "device/control";
const char* topic_settings = "device/settings";
const char* topic_status = "device/status";

WiFiClientSecure espClient;
PubSubClient client(espClient);

const int ENA_PIN = 14;
const int IN1_PIN = 26;
const int IN2_PIN = 27;
const int ENB_PIN = 12;
const int IN3_PIN = 33;
const int IN4_PIN = 25;
const int MAX_SPEED = 200;
const int TURN_SPEED = 155;

const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;
HX711 scale;
float calibration_factor = (116.66 * 1000) / 232;

const int BUZZER_PIN = 32;

float maxWeight = 300.0;
bool motorEnabled = true;
bool alarmEnabled = false;
bool isOverload = false;

String currentDirection = "stop";
String lastDirection = "";        
bool lastMotorEnabled = false;

unsigned long lastWeightSend = 0;
const long weightSendInterval = 1000;

unsigned long lastReconnectAttempt = 0;

void beep(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(duration);
    digitalWrite(BUZZER_PIN, LOW);
    delay(duration);
  }
}

void applyMotorState() {
  Serial.print(">>> MOTOR STATE: ");
  
  if (!motorEnabled) {
    Serial.println("FORCE STOP");
    analogWrite(ENA_PIN, 0); analogWrite(ENB_PIN, 0);
    digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, LOW);
    return;
  }

  if (currentDirection == "forward") {
    Serial.println("MAJU");
    // Kiri Maju, Kanan Maju
    digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW); // Kiri A
    digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW); // Kanan B
    analogWrite(ENA_PIN, MAX_SPEED); analogWrite(ENB_PIN, MAX_SPEED);
  } 
  else if (currentDirection == "reverse") {
    Serial.println("MUNDUR");
    // Kiri Mundur, Kanan Mundur
    digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH);
    analogWrite(ENA_PIN, MAX_SPEED); analogWrite(ENB_PIN, MAX_SPEED);
  } 
  // --- TAMBAHAN BELOK KIRI ---
  else if (currentDirection == "left") {
    Serial.println("BELOK KIRI");
    // Kiri Mundur, Kanan Maju (Pivot Turn)
    digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, HIGH); // Kiri Mundur
    digitalWrite(IN3_PIN, HIGH); digitalWrite(IN4_PIN, LOW); // Kanan Maju
    analogWrite(ENA_PIN, TURN_SPEED); analogWrite(ENB_PIN, TURN_SPEED);
  }
  // --- TAMBAHAN BELOK KANAN ---
  else if (currentDirection == "right") {
    Serial.println("BELOK KANAN");
    // Kiri Maju, Kanan Mundur (Pivot Turn)
    digitalWrite(IN1_PIN, HIGH); digitalWrite(IN2_PIN, LOW); // Kiri Maju
    digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, HIGH); // Kanan Mundur
    analogWrite(ENA_PIN, TURN_SPEED); analogWrite(ENB_PIN, TURN_SPEED);
  }
  else { // stop
    Serial.println("BERHENTI");
    analogWrite(ENA_PIN, 0); analogWrite(ENB_PIN, 0);
    digitalWrite(IN1_PIN, LOW); digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW); digitalWrite(IN4_PIN, LOW);
  }
}

void setup_wifi() {
  delay(10);
  Serial.println(); Serial.print("WiFi: "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500); Serial.print(".");
    retry++;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
    beep(3, 100); // 3 kali bunyi pendek
  } else {
    Serial.println("\nWiFi Failed (Lanjut tanpa WiFi dulu)");
  }
  
  espClient.setInsecure();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) message += (char)payload[i];

  if (String(topic) == topic_control) {
    if (message.indexOf("\"motor_enabled\":true") > 0) motorEnabled = true;
    else if (message.indexOf("\"motor_enabled\":false") > 0) motorEnabled = false;
    
    if (message.indexOf("\"alarm_enabled\":true") > 0) alarmEnabled = true;
    else if (message.indexOf("\"alarm_enabled\":false") > 0) alarmEnabled = false;

    if (message.indexOf("\"is_overload\":true") > 0) isOverload = true;
    else if (message.indexOf("\"is_overload\":false") > 0) isOverload = false;

    bool gerak = false;
    if (message.indexOf("\"direction\":\"forward\"") > 0) { currentDirection = "forward"; gerak = true; }
    else if (message.indexOf("\"direction\":\"reverse\"") > 0) { currentDirection = "reverse"; gerak = true; }
    else if (message.indexOf("\"direction\":\"left\"") > 0) { currentDirection = "left"; gerak = true; }
    else if (message.indexOf("\"direction\":\"right\"") > 0) { currentDirection = "right"; gerak = true; }
    else if (message.indexOf("\"direction\":\"stop\"") > 0) { currentDirection = "stop"; } // Stop gak perlu bunyi

    if (gerak) {
       beep(2, 50); 
    }
  }
  
  if (String(topic) == topic_settings) {
    int idx = message.indexOf("\"max_weight\":");
    if (idx > 0) {
       String val = message.substring(idx + 13);
       maxWeight = val.toFloat();
    }
  }
}

boolean reconnect() {
  if (WiFi.status() != WL_CONNECTED) return false; // Jangan coba connect MQTT kalau WiFi mati

  String clientId = "ESP32-" + String(random(0xffff), HEX);
  if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("MQTT Connected");
    client.subscribe(topic_control);
    client.subscribe(topic_settings);
    return true;
  }
  return false;
}

void sendWeightData(float weight) {
  char jsonBuffer[128];
  snprintf(jsonBuffer, sizeof(jsonBuffer), 
           "{\"device_id\":\"%s\",\"weight\":%.2f,\"timestamp\":\"%lu\"}", 
           device_id, weight, millis());
           
  client.publish(topic_weight_data, jsonBuffer);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(IN1_PIN, OUTPUT); pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT); pinMode(IN4_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT); pinMode(ENB_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(BUZZER_PIN, LOW);
  
  currentDirection = "stop";
  applyMotorState(); 

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);
  
  delay(2000);
  scale.tare();
  
  Serial.println("Sistem Siap.");
}

void loop() {
  if (!client.connected()) {
    unsigned long now = millis();

    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }

  if (scale.is_ready()) {
    unsigned long now = millis();
    if (now - lastWeightSend >= weightSendInterval) {
      float berat = scale.get_units(1);
      if (berat < 3.0) {
         berat = 0.0;
      }
      sendWeightData(berat);
      lastWeightSend = now;
    }
  }

  if (alarmEnabled) digitalWrite(BUZZER_PIN, HIGH);
  else digitalWrite(BUZZER_PIN, LOW);

  if (currentDirection != lastDirection || motorEnabled != lastMotorEnabled) {
    applyMotorState();
    lastDirection = currentDirection;
    lastMotorEnabled = motorEnabled;
  }
}