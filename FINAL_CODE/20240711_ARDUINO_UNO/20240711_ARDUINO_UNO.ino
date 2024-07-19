#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

const int solenoidPin = 10;
const int buzzerPin = 11;
const int redLed = 12;
const int greenLed = 13;

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {9, 8, 7, 6};
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

String password;
String input_password;
String new_password;
int incorrect_attempts = 0;

enum State { ENTER_PASSWORD, CHANGE_PASSWORD_OLD, CHANGE_PASSWORD_NEW, CONFIRM_PASSWORD };
State currentState = ENTER_PASSWORD;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  input_password.reserve(32);

  pinMode(solenoidPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  password = readPasswordFromEEPROM();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    lcd.setCursor(0, 1);
    lcd.print(input_password);

    if (key == '*') {
      input_password = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
    } else if (key == '#') {
      handleHashKey();
    } else if (key == 'A') {
      currentState = CHANGE_PASSWORD_OLD;
      input_password = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Current:");
    } else if (key == 'B') {
      if (currentState == CHANGE_PASSWORD_OLD || currentState == CHANGE_PASSWORD_NEW || currentState == CONFIRM_PASSWORD) {
        resetToEnterPassword();
      }
    } else {
      input_password += key;
      lcd.setCursor(0, 1);
      lcd.print(input_password);
    }
  }
}

void handleHashKey() {
  switch (currentState) {
    case ENTER_PASSWORD:
      if (password == input_password) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Correct Password");

        digitalWrite(greenLed, HIGH);
        digitalWrite(solenoidPin, HIGH);
        Serial.println("OPENDOOR");
        delay(2000);
        digitalWrite(greenLed, LOW);
        digitalWrite(solenoidPin, LOW);
        Serial.println("CLOSEDOOR");

        incorrect_attempts = 0;
      } else {
        handleIncorrectPassword();
      }
      break;

    case CHANGE_PASSWORD_OLD:
      if (password == input_password) {
        currentState = CHANGE_PASSWORD_NEW;
        input_password = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter New:");
        Serial.println("SOMEONEPASSCODE");
      } else {
        handleIncorrectPassword();
        lcd.setCursor(0, 0);
        lcd.print("Enter Current:");
      }
      break;

    case CHANGE_PASSWORD_NEW:
      new_password = input_password;
      currentState = CONFIRM_PASSWORD;
      input_password = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Confirm New:");
      break;

    case CONFIRM_PASSWORD:
      if (input_password == new_password) {
        password = new_password;
        writePasswordToEEPROM(password);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Password Changed");
        Serial.println("CHANGESUCCESS");
        delay(2000);
        resetToEnterPassword();
      } else {
        handleIncorrectPassword();
        lcd.setCursor(0, 0);
        lcd.print("Enter New:");
      }
      break;
  }

  input_password = "";
  delay(1000);
  if (currentState == ENTER_PASSWORD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
  }
}

void handleIncorrectPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Incorrect Pass!");

  digitalWrite(redLed, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(redLed, LOW);
  digitalWrite(buzzerPin, LOW);

  incorrect_attempts++;

  if (incorrect_attempts >= 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wait 5 seconds");
    Serial.println("INCORRECT");
    for (int i = 0; i < 5; i++) {
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(redLed, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(redLed, LOW);
      delay(500);
    }
    incorrect_attempts = 0;
    lcd.clear();
  }
}

void resetToEnterPassword() {
  currentState = ENTER_PASSWORD;
  input_password = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void writePasswordToEEPROM(String pass) {
  for (int i = 0; i < pass.length(); i++) {
    EEPROM.write(i, pass[i]);
  }
  EEPROM.write(pass.length(), '\0');
}

String readPasswordFromEEPROM() {
  char stored_pass[32];
  int i = 0;
  char ch = EEPROM.read(i);
  while (ch != '\0' && i < sizeof(stored_pass) - 1) {
    stored_pass[i] = ch;
    i++;
    ch = EEPROM.read(i);
  }
  stored_pass[i] = '\0';
  return String(stored_pass);
}