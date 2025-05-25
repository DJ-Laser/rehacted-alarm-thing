#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

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

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
}

void loop()
{
  char key = keypad.getKey();

  if (key)
  {
    if (key == '#')
    {
      lcd.clear();
      lcd.print("Password:");
      lcd.setCursor(0, 1);
      lcd.print(password);
      delay(3000);
      lcd.clear();
      password = "";
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
      lcd.setCursor(0, 1);
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
