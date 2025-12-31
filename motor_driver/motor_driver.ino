// Definisikan Pin Kontrol menggunakan nomor GPIO ESP32
// Pastikan nomor pin ini SESUAI dengan wiring Anda di expansion board!
const int ENA_PIN = 14; // Pin PWM untuk Kecepatan (misal, GPIO 14)
const int IN1_PIN = 26; // Pin Digital untuk Arah 1 (misal, GPIO 26)
const int IN2_PIN = 27; // Pin Digital untuk Arah 2 (misal, GPIO 27)

// Kecepatan: dari 0 (berhenti) hingga 255 (maksimal)
const int MAX_SPEED = 255; 

// Parameter PWM manual TIDAK DIPERLUKAN lagi dengan analogWrite()
// const int freq = 5000;      
// const int ledChannel = 0;   
// const int resolution = 8;   

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);
  Serial.println("ESP32 Motor Controller Siap (Mode analogWrite)!");

  // Konfigurasi Pin sebagai OUTPUT
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT); // Penting untuk analogWrite

  // Tidak perlu konfigurasi LEDC manual di sini.
  // analogWrite() akan menanganinya secara otomatis saat pertama kali dipanggil.

  Serial.println("Sistem Motor Aktif. Mulai bergerak dalam 2 detik...");
  delay(2000);
}

void loop() {
  // 1. Bergerak Maju dengan kecepatan penuh
  setMotorSpeed(MAX_SPEED); 
  moveForward();           
  Serial.println("Maju");
  delay(10000); 

  // 2. Berhenti
  stopMotor();
  Serial.println("Berhenti");
  delay(1000); 

  // 3. Bergerak Mundur dengan kecepatan penuh
  setMotorSpeed(255); // Kecepatan maksimal (255)
  moveBackward();
  Serial.println("Mundur");
  delay(5000); 

  // 4. Berhenti lagi
  stopMotor();
  delay(1000);
  // if(Serial.available()) {
  //   String command = Serial.readStringUntil('\n');

  //   if(command == "maju") {
  //     setMotorSpeed(MAX_SPEED); 
  //     moveForward();
  //   } else if(command == "mundur") {
  //     setMotorSpeed(MAX_SPEED); 
  //     moveBackward();
  //   } else if(command == "stop") {
  //     stopMotor();
  //   } else {
  //     Serial.println("Unknown command");
  //   }
  // }
}

// =========================================================
//           FUNGSI PENGENDALIAN MOTOR
// =========================================================

// FUNGSI 1: Mengatur Kecepatan Roda
void setMotorSpeed(int speed) {
  // Menggunakan analogWrite() secara langsung.
  // Core ESP32 terbaru otomatis mengonfigurasi PWM di latar belakang.
  // Rentang nilai 'speed' adalah 0 - 255 secara default.
  analogWrite(ENA_PIN, speed); 
}

// FUNGSI 2: Bergerak Maju
void moveForward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

// FUNGSI 3: Bergerak Mundur
void moveBackward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
}

// FUNGSI 4: Berhenti
void stopMotor() {
  // Matikan kecepatan motor
  analogWrite(ENA_PIN, 0); 
  
  // Opsional: Rem aktif dengan membuat kedua pin arah LOW
  // digitalWrite(IN1_PIN, LOW);
  // digitalWrite(IN2_PIN, LOW);
}