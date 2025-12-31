#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>          // Libray DHT Sensor Library by Adafruit
#include <ArduinoJson.h>  // Library ArduinoJson by Benoit

#define DHTPIN 14      // pin DHT Pin - Data Sensor
#define DHTTYPE DHT11  // pin Type DHT11 - Tipe Sensor
DHT dht(DHTPIN, DHTTYPE);

#define LED_BUILTIN 2

// konfigurasi WiFi dan server
const char* ssid = "Lab IoT";                       // Nama WiFi Kamu
const char* password = "iflabiot";              // Password WiFI Kamu
const char* server = "http://192.168.0.174/iot_app/api";  // IP PC kamu
// const char* API_KEY = "S3cretAPIKeyReplaceThis";   // (opsional) untuk keamanan aja
const char* DEVICE_ID = "esp32-unit-053";             // (opsional) untuk ngasih tau Device aja

// Untuk ngatur waktu
unsigned long lastSend = 0;
const unsigned long sendInterval = 10 * 1000UL; // kirim tiap 60 detik

void setup() {
  // Setup Basic
  Serial.begin(115200);             // Jalanin Serial Monitor
  dht.begin();                      // Jalanin DHT Sensor

  // Koneksi WiFI
	WiFi.begin(ssid, password);                    // Koneksi ke Hospot kita
	Serial.println("Connecting to WiFi...");       // Ngasih tau kalo lagi konek
	int retries = 0;
	while (WiFi.status() != WL_CONNECTED) {        // Handler untuk status
	  delay(1000);
	  Serial.print("Status: ");
	  Serial.println(WiFi.status());               // Kode Status cek "Contoh Status"
	  retries++;
	  if (retries > 20) {                          // Kalo gak konek konek langsung tampil error
	    Serial.println("Failed to connect to WiFi.");  
	    return; // keluar dari setup
	  }
	}
	Serial.println("Connected!");                  // Berhasil Connect
	Serial.println(WiFi.localIP());                // Nampilin IP Laptop
	// === Contoh Status ===
	// WL_NO_SSID_AVAIL (1) → SSID gak ditemukan
	// WL_CONNECT_FAILED (4) → password salah
	// WL_IDLE_STATUS (0) → belum mulai koneksi
	// WL_DISCONNECTED (6) → koneksi gagal
}

void loop() {
  unsigned long now = millis();
  
  // Ngirim data sesuai sendInterval (berapa waktu yang diset tadi)
  if (now - lastSend >= sendInterval) {
    lastSend = now;
    sendSensorData();
  }
}

// -- Fungsi untuk mengirimkan Data Sensor
void sendSensorData(){
	// Nangkap Data DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();
	  
	// Control Handler Data
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT");
    return;
  }
  
  // post JSON ke save_data.php
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Buat URL tujuan
    String url = String(server) + "/save_data.php";
    http.begin(url);
    
    // Tambahkan header agar server tahu data yang dikirim berupa JSON
    http.addHeader("Content-Type", "application/json");
		
	  // Buat data JSON untuk dikirim
    StaticJsonDocument<256> doc;
    doc["device_id"] = DEVICE_ID;
    doc["sensor_type"] = "DHT11_temp";
    doc["value"] = t;
    doc["raw"] = String("h=") + String(h);
		
		// Ubah JSON menjadi string dan kirim via HTTP POST
    String body;
    serializeJson(doc, body);
    int code = http.POST(body);
		
		// Tampilkan hasil pengiriman (response dari server)
    if (code > 0) {
      String resp = http.getString();
      Serial.printf("sendSensorData code=%d resp=%s\n", code, resp.c_str());
    } else {
      Serial.printf("POST failed, error: %s\n", http.errorToString(code).c_str());
    }
    
    // Tutup koneksi HTTP
    http.end();
  }
}