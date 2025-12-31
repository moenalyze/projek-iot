#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200); // serial monitor pc
  SerialBT.begin("esp32_khatama"); // serial monitor hp
  Serial.print("Bluetooh siap digunakan!");
  pinMode(2, OUTPUT); // GPIO2 terhubung ke LED bawaan (LED_BUILTIN)
}

void loop() {
  if(SerialBT.available()) {
    String pesan_hp = SerialBT.readStringUntil('\n');
    pesan_hp.trim(); // HAPUS SEMUA KARAKTER SPASI/KONTROL
    Serial.print("Pesan dari HP: ");
    Serial.println(pesan_hp);
    
    if(pesan_hp == "nyalain") {
      digitalWrite(2, HIGH);

      // delay(500);

      // digitalWrite(2, LOW);

      // delay(500);
    } else if (pesan_hp == "matiin") {
      digitalWrite(2, LOW);
    }
    
    else {
      SerialBT.print("salah command");
    }
  }

  if(Serial.available()) {
    String pesan_pc = Serial.readStringUntil('\n');
    SerialBT.print("Pesan dari PC: ");
    SerialBT.println(pesan_pc);

    if(pesan_pc == "nyalain") {
      digitalWrite(2, HIGH);

      delay(500);

      digitalWrite(2, LOW);

      delay(500);
    } else {
        Serial.print("salah command");
    }
  }
}