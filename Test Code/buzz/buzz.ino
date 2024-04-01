int buzzerpin1 = 5;
int buzzerpin2 = 6;

void setup() {
  pinMode(buzzerpin1, OUTPUT);
  pinMode(buzzerpin2, OUTPUT);
}

void loop() {
  tone(buzzerpin1, 500);
  delay(100);
  noTone(buzzerpin1);  // Stop the tone on buzzerpin1
  delay(100);
  
  tone(buzzerpin2, 500);
  delay(100);
  noTone(buzzerpin2);  // Stop the tone on buzzerpin2
  delay(100);
}
