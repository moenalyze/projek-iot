#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int pinTrig = 8;
int pinEcho = 7;

long duration;
float distance;

void setup() {
  // init ultrasonik
  pinMode(pinTrig, OUTPUT); // memancarkan gelombang
  pinMode(pinEcho, INPUT); // menangkap gelombang

  // init serial
  Serial.begin(9600);
  Serial.println("Program berjalan...");

  // init lcd
  lcd.init(); // init lcd
  lcd.backlight(); // menyalakan lampu belakang sesuai n

  // set tampil lcd
  lcd.setCursor(0,0);
  lcd.print("Ready");

  delay(1000);
  lcd.clear();
}

void loop() {
  // set pulse trigger
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  // echo membaca durasi
  duration = pulseIn(pinEcho, HIGH);

  // menghitung jarak
  distance = (duration * 0.034) / 2.0;

  // tampilan di serial
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  // tampilan di lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Jarak: ");
  lcd.print(distance);
  lcd.println(" cm");

  // next - tampil di lcd
  delay(1000);
}
