#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN A0

MFRC522 rfid(SS_PIN, RST_PIN);

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String password = "";
bool awaitingPassword = false;

byte authorizedUID[4] = {0x21, 0x38, 0xC0, 0x01};

int alarmUnoAddress = 3;

bool isAlarmOk()
{
  Wire.beginTransmission(alarmUnoAddress);
  Wire.write(0); // Request alarm check
  Wire.endTransmission();

  Wire.requestFrom(alarmUnoAddress, 1);

  int alarmOk = Wire.read();

  return alarmOk == 1;
}

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop()
{
  bool alarmOk = isAlarmOk();
  Serial.print("Alarm: ");
  Serial.println(alarmOk);

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
  {
    bool authorized = true;

    if (rfid.uid.size != 4)
      authorized = false;
    else
    {
      for (byte i = 0; i < 4; i++)
      {
        if (rfid.uid.uidByte[i] != authorizedUID[i])
        {
          authorized = false;
          break;
        }
      }
    }

    lcd.clear();
    if (authorized)
    {
      lcd.setCursor(0, 0);
      lcd.print("Valid");
      delay(2000);
      lcd.clear();
      lcd.print("Enter Password:");
      lcd.setCursor(0, 1);
      awaitingPassword = true;
      password = "";
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Invalid");
      delay(2000);
      lcd.clear();
      awaitingPassword = false;
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  if (awaitingPassword)
  {
    char key = keypad.getKey();

    if (key)
    {
      if (key == '#')
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Password:");
        lcd.setCursor(0, 1);
        lcd.print(password);
        delay(3000);
        lcd.clear();
        awaitingPassword = false;
        password = "";
      }
      else if (key == '*')
      {
        password = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
      }
      else
      {
        if (password.length() < 16)
        {
          password += key;
          lcd.print(key);
        }
      }
    }
  }
}
