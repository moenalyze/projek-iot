int pinPir = 7;
int buzzerPin = 13;

void setup() {
  // put your setup code here, to run once:
  noTone(buzzerPin);
  pinMode(pinPir, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sistem alarm siap digunakan");
}

void loop() {
  // menangkap sensor pir
  int motion = digitalRead(pinPir); // low (0) atau high (1)

  if(motion == 1) {
    Serial.println("Gerakan terdeteksi");
    tone(buzzerPin, 500);
  } else {
    Serial.println("Tidak ada gerakan");
    noTone(buzzerPin);
  }

  delay(100);
}
