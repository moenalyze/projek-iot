#define BLYNK_TEMPLATE_ID "TMPL6PN8u7IyE"
#define BLYNK_TEMPLATE_NAME "LED Blynk"
#define BLYNK_AUTH_TOKEN "YVxJ2pr33qo4FqpAxcKWET3Etya5KsWB"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Lab IoT";
char pass[] = "iflabiot";

#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, pass);                    // Koneksi ke Hospot kita
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

  Serial.println("Menghubungkan ke Blynk...");
  Blynk.begin(auth, ssid, pass);


}

BLYNK_WRITE(V2) {
  int ledState = param.asInt(); // nyimpen data dari tombol
  digitalWrite(LED_PIN, ledState);
  Serial.print("LED State: ");
  Serial.println(ledState);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
}
