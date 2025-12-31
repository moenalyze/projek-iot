#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

// =========================================
// 1. KONFIGURASI PIN & VARIABEL GLOBAL
// =========================================

// --- Konfigurasi Motor DC ---
const int ENA_PIN = 14;
const int IN1_PIN = 26;
const int IN2_PIN = 27;
const int ENB_PIN = 12;
const int IN3_PIN = 33;
const int IN4_PIN = 25;
const int MAX_SPEED = 255;

// --- Konfigurasi Load Cell & LCD ---
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;
// LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 scale;
// Ganti dengan nilai kalibrasi Anda yang benar!
float calibration_factor = (116.66 * 1000) / 232; // Contoh, sesuaikan dengan hasil kalibrasi Anda

// --- Konfigurasi Buzzer ---
const int BUZZER_PIN = 32;
const float BERAT_BATAS = 300.0;

// --- VARIABEL UNTUK MULTITASKING (MILLIS) ---
unsigned long previousMillisBerat = 0;  // Penyimpanan waktu terakhir baca berat
// const long intervalBerat = 200;         // Interval baca berat (misal setiap 200ms)
const long intervalBerat = 50;         // Interval baca berat (misal setiap 200ms)

unsigned long previousMillisMotor = 0;  // Penyimpanan waktu terakhir ubah gerakan motor
int stateMotor = 0;                     // Status gerakan motor saat ini
bool isMoving = false;

// =========================================
// 2. FUNGSI SETUP
// =========================================
void setup() {
  Serial.begin(115200);
  
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // lcd.init();
  // lcd.backlight();
  // lcd.print("Robot Siap!");
  // delay(1000); // Delay awal tidak masalah

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  Serial.println("Ready...");
  // lcd.clear();
}

// =========================================
// 3. LOOP UTAMA (NON-BLOCKING)
// =========================================
void loop() {  
  unsigned long currentMillis = millis();

  // --- TUGAS 1: Membaca & Menampilkan Berat (Setiap 200ms) ---
  if (currentMillis - previousMillisBerat >= intervalBerat) {
    previousMillisBerat = currentMillis; // Simpan waktu sekarang

    if (scale.is_ready()) {
      float berat = scale.get_units(3); // Ambil rata-rata sedikit saja agar cepat

      // Serial Monitor
      // Serial.print("Berat: "); Serial.print(berat, 1); Serial.println(" g");

      // Tampilan LCD & Logika Buzzer
      if (berat > BERAT_BATAS) {
        digitalWrite(BUZZER_PIN, HIGH);

        if (isMoving) {
          stopMotor();
          isMoving = false; // Tandai bahwa robot sudah berhenti
        }
        // lcd.setCursor(0, 0);
        // lcd.print("OVERLOAD!       ");
        // stopMotor();
        // lcd.setCursor(0, 1); lcd.print("Status: Stop    ");
      } else {
        digitalWrite(BUZZER_PIN, LOW);

        // HANYA kirim perintah jalan jika sebelumnya BERHENTI
        if (!isMoving) { 
          setMotorSpeed(MAX_SPEED);
          moveForward();
          isMoving = true; // Tandai bahwa robot sudah bergerak
        }
        // // lcd.setCursor(0, 0);
        // // lcd.print("Berat: ");
        // // lcd.print(berat, 1); // 1 angka di belakang koma
        // // lcd.print(" g    "); // Spasi untuk menghapus sisa karakter
        // setMotorSpeed(MAX_SPEED);
        // moveForward();
      }
    }
  }

  // --- TUGAS 2: Mengontrol Gerakan Motor (Berdasarkan Waktu) ---
  // State Machine Sederhana untuk Gerakan Robot

  // switch(stateMotor) {
  //   case 0:
  //     Serial.println("State 0: Maju");
  //     setMotorSpeed(MAX_SPEED);
  //     moveForward();
  //     // Tampilkan status di baris kedua LCD
  //     // lcd.setCursor(0, 1); lcd.print("Status: Maju Terus   ");
      
  //     // previousMillisMotor = currentMillis;
  //     stateMotor = 1; // Pindah ke state menunggu
  //     break;

  //   case 1: 
  //   // State diam: Tidak melakukan apa-apa, biarkan motor terus maju.
  //   // Robot akan terus maju selamanya sambil loop() terus membaca sensor.
  //   break;
  // }
  
  // switch (stateMotor) {
  //   case 0: // MULAI: Maju
  //     Serial.println("State 0: Maju");
  //     setMotorSpeed(MAX_SPEED);
  //     moveForward();
  //     // Tampilkan status di baris kedua LCD
  //     lcd.setCursor(0, 1); lcd.print("Status: Maju    ");
      
  //     previousMillisMotor = currentMillis;
  //     stateMotor = 1; // Pindah ke state menunggu
  //     break;

  //   case 1: // MENUNGGU: Selama 10 detik (10000ms)
  //     if (currentMillis - previousMillisMotor >= 10000) {
  //       stateMotor = 2; // Waktu habis, pindah ke stop
  //       // stateMotor = 0; // Waktu habis, pindah ke stop
  //     }
  //     break;

  //   case 2: // MULAI: Berhenti
  //     Serial.println("State 2: Berhenti");
  //     stopMotor();
  //     lcd.setCursor(0, 1); lcd.print("Status: Stop    ");

  //     previousMillisMotor = currentMillis;
  //     stateMotor = 3;
  //     break;

  //   case 3: // MENUNGGU: Selama 1 detik (1000ms)
  //     if (currentMillis - previousMillisMotor >= 1000) {
  //       stateMotor = 4; // Waktu habis, pindah ke mundur
  //     }
  //     break;

  //   case 4: // MULAI: Mundur
  //     Serial.println("State 4: Mundur");
  //     setMotorSpeed(MAX_SPEED);
  //     moveBackward();
  //     lcd.setCursor(0, 1); lcd.print("Status: Mundur  ");

  //     previousMillisMotor = currentMillis;
  //     stateMotor = 5;
  //     break;

  //   case 5: // MENUNGGU: Selama 5 detik (5000ms)
  //     if (currentMillis - previousMillisMotor >= 5000) {
  //       stateMotor = 6; // Waktu habis, pindah ke stop lagi
  //     }
  //     break;
      
  //   case 6: // MULAI: Berhenti Akhir
  //      Serial.println("State 6: Berhenti Akhir");
  //      stopMotor();
  //      lcd.setCursor(0, 1); lcd.print("Status: Stop    ");
       
  //      previousMillisMotor = currentMillis;
  //      stateMotor = 7;
  //      break;

  //   case 7: // MENUNGGU: Selama 1 detik, lalu ULANGI dari awal
  //     if (currentMillis - previousMillisMotor >= 1000) {
  //       stateMotor = 0; // Kembali ke state 0 (Maju lagi)
  //     }
  //     break;
  // }
}

// =========================================
// 4. FUNGSI KONTROL MOTOR
// =========================================
void setMotorSpeed(int speed) {
  analogWrite(ENA_PIN, speed);
  analogWrite(ENB_PIN, speed);
}
void moveForward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}
void moveBackward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}
void stopMotor() {
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}