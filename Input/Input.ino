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

byte authorizedUID[4] = {0x21, 0x38, 0xC0, 0x01};
String correctPasscode = "1236";

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

void disableAlarm()
{
  Wire.beginTransmission(alarmUnoAddress);
  Wire.write(1); // Request alarm disable
  Wire.endTransmission();
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin();

  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void alarmOkLoop()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm Ok");

  while (isAlarmOk())
  {
    Serial.println("Alarm Ok");
    delay(300);
  }

  Serial.println("Alarm Tripped!");
}

void waitForRfid()
{
  bool authorized = false;

  while (!authorized)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tap RFID Keycard");

    while (!(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()))
    {
      delay(500);
    }

    authorized = true;

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
      lcd.print("Authorized");
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Unauthorized");
    }

    delay(2000);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void printPasswordDialog(String password)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Passcode:");
  lcd.setCursor(0, 1);
  lcd.print(password);
}

String checkPassword()
{
  String password = "";
  printPasswordDialog(password);

  while (true)
  {
    char key = keypad.getKey();

    if (key)
    {
      if (key == '#')
      {
        lcd.clear();
        if (password == correctPasscode)
        {
          lcd.setCursor(0, 0);
          lcd.print("Correct");
          delay(2000);
          break;
        }
        else
        {
          lcd.setCursor(0, 0);
          lcd.print("Incorrect");
          delay(2000);
          password = "";
          printPasswordDialog(password);
        }
      }
      else if (key == '*')
      {
        password = "";
        printPasswordDialog(password);
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

void loop()
{
  if (isAlarmOk())
  {
    alarmOkLoop();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm Tripped!");
  delay(3000);

  waitForRfid();
  checkPassword();

  disableAlarm();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Disabling Alarm");
  delay(3000);
}
