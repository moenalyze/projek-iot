#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// ----------------------
// Konfigurasi Pin
// ----------------------
#define DOUT 18   // Pin DT HX711
#define CLK 19    // Pin SCK HX711

// ----------------------
// Objek
// ----------------------
HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2); // ubah ke 0x3F jika tidak tampil

// ----------------------
// Variabel Kalibrasi
// ----------------------
// Mulai dengan 1.0 dulu, nanti sesuaikan setelah kalibrasi
float calibration_factor = 1.0;

// ----------------------
// Setup
// ----------------------
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Inisialisasi...");
  delay(1000);

  scale.begin(DOUT, CLK);

  if (!scale.is_ready()) {
    lcd.clear();
    lcd.print("HX711 Error!");
    Serial.println("HX711 tidak terdeteksi, cek koneksi!");
    while (1);
  }

  scale.set_scale();  // tanpa kalibrasi dulu
  scale.tare();       // nolkan beban

  lcd.clear();
  lcd.print("Tare selesai");
  Serial.println("Tare selesai. Letakkan beban kalibrasi.");
  delay(1000);
}

// ----------------------
// Loop utama
// ----------------------
void loop() {
  // Ambil data rata-rata 10x biar stabil
  float rawData = scale.get_units(10);

  // ----------------------
  // Tampilkan di Serial Monitor
  // ----------------------
  Serial.print("Data mentah: ");
  Serial.println(rawData);

  // ----------------------
  // Tampilkan di LCD
  // ----------------------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Berat:");

  lcd.setCursor(0, 1);
  lcd.print(rawData, 2);
  lcd.print(" g");

  delay(500);
}
