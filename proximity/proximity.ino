#include <Wire.h>

enum I2cDataMode
{
  I2C_MODE_ALARM_STATUS,
  I2C_MODE_CHECK_PASSCODE,
  I2C_MODE_CHECK_RFID,
  I2C_MODE_INVALID_MODE,
};

const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 12;

const int i2cAddress = 55;
const String passcode = "1234";

bool alarmTripped = false;

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Wire.begin(i2cAddress);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

I2cDataMode currentI2cMode = I2C_MODE_ALARM_STATUS;
bool lastI2cRequestStatus = false;

I2cDataMode getI2cMode(int firstByte)
{
  switch (firstByte)
  {
  case 0:
    return I2C_MODE_ALARM_STATUS;

  case 1:
    return I2C_MODE_CHECK_PASSCODE;

  case 2:
    return I2C_MODE_CHECK_RFID;

  default:
    return I2C_MODE_INVALID_MODE;
  }
}

// Recieve data to verify and prepare data to return
void receiveEvent(int bytes)
{
  if (bytes == 0)
  {
    return;
  }

  int modeByte = Wire.read();
  bytes -= 1;

  currentI2cMode = getI2cMode(modeByte);

  switch (currentI2cMode)
  {
  case I2C_MODE_ALARM_STATUS:
    break;

  default:
    break;
  }
}

void requestEvent()
{
  switch (currentI2cMode)
  {
  case I2C_MODE_ALARM_STATUS:
    if (alarmTripped)
    {
      Wire.write(0);
    }
    else
    {
      Wire.write(1);
    }
    break;

  default:
    break;
  }
}

float getSensorDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * .0343) / 2;

  return distance;
}

void beepBuzzer(int onMilis, int offMilis)
{
  int period = onMilis + offMilis;

  int currentCycleTime = millis() % period;

  if (currentCycleTime < onMilis)
  {
    digitalWrite(buzzerPin, HIGH);
  }
  else
  {
    digitalWrite(buzzerPin, LOW);
  }
}

void buzzerOff()
{
  digitalWrite(buzzerPin, LOW);
}

void loop()
{
  if (!alarmTripped)
  {

    buzzerOff();
    float distance = getSensorDistance();

    if (distance < 25)
    {
      alarmTripped = true;
    }
  }
  else
  {
    beepBuzzer(100, 200);
  }

  delay(100);
}
