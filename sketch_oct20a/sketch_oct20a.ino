void setup() {

  pinMode(2, OUTPUT); // GPIO2 terhubung ke LED bawaan (LED_BUILTIN)

}



void loop() {

  digitalWrite(2, HIGH);

  delay(500);

  digitalWrite(2, LOW);

  delay(500);

}