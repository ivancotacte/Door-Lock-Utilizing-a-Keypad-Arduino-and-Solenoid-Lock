#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

const int redLed = 12;
const int greenLed = 13;
const int buzzerPin = 11;

LiquidCrystal_I2C lcd(0x27, 16, 2);

String new_password;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(buzzerPin, LOW);

  lcd.setCursor(0, 0);
  lcd.print("System reset...");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(3000);

  new_password = "1234";
  writePasswordToEEPROM(new_password);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Password saved:");
  lcd.setCursor(0, 1);
  lcd.print(new_password);

  digitalWrite(greenLed, HIGH);
  delay(3000);
  digitalWrite(greenLed, LOW);

  tone(buzzerPin, 1000, 100);
}

void loop() {
}
void writePasswordToEEPROM(String pass) {
  for (int i = 0; i < pass.length(); i++) {
    EEPROM.write(i, pass[i]);
  }
  EEPROM.write(pass.length(), '\0');
}