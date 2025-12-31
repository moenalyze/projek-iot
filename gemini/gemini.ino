#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// Definisikan pin HX711
const int LOADCELL_DOUT_PIN = 18; // Pin DT
const int LOADCELL_SCK_PIN = 19;  // Pin SCK

// Inisialisasi LCD I2C (Sesuaikan alamat I2C dan ukuran LCD Anda)
// Alamat I2C umum: 0x27 atau 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Inisialisasi objek HX711
HX711 scale;

// Faktor Kalibrasi (Nilai ini harus didapatkan melalui proses kalibrasi)
// Ganti dengan nilai yang Anda dapatkan setelah kalibrasi
float calibration_factor = (116.66 * 1000) / 232; // Contoh nilai, harus diganti!

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("Timbangan Siap");
  delay(1000);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); // Terapkan faktor kalibrasi
  scale.tare(); // Atur nilai nol (tare)
  
  Serial.println("Reset nol (tare) selesai. Letakkan beban!");
}

void loop() {
  if (scale.is_ready()) {
    // Ambil nilai berat dalam satuan (misalnya gram)
    float berat = scale.get_units(20); // Ambil rata-rata dari 10 kali pembacaan
    a
    // Tampilkan di Serial Monitor
    Serial.print("Berat: ");
    Serial.print(berat, 2); // Tampilkan 2 angka di belakang koma
    Serial.println(" gram");

    // Tampilkan di LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Berat:");
    lcd.setCursor(0, 1);
    lcd.print("                "); // Hapus baris lama
    lcd.setCursor(0, 1);
    lcd.print(berat, 2);
    lcd.print(" gram");
  }
  delay(500);
}