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

const int i2cAddress = 3;
const String passcode = "1234";

bool alarmTripped = false;

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Wire.begin(i2cAddress);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);
  Serial.println("Started Alarm System");
}

I2cDataMode currentI2cMode = I2C_MODE_ALARM_STATUS;
int lastI2cRequestReturn = false;

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

  lastI2cRequestReturn = 0;

  switch (currentI2cMode)
  {
  case I2C_MODE_ALARM_STATUS:
    if (!alarmTripped)
    {
      lastI2cRequestReturn = 1;
    }
    break;

  default:
    break;
  }
}

void requestEvent()
{
  Wire.write(lastI2cRequestReturn);
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

int loopsTripped = 0;
int debounceLoops = 5;

void loop()
{
  if (!alarmTripped)
  {
    buzzerOff();

    float distance = getSensorDistance();
    Serial.print("Alarm Ok: ");
    Serial.println(distance);

    if (loopsTripped > debounceLoops)
    {
      Serial.println("Alarm Tripped!");
      alarmTripped = true;
    }
    else if (distance < 25)
    {
      loopsTripped += 1;
    }
    else
    {
      loopsTripped = 0;
    }
  }
  else
  {
    beepBuzzer(100, 200);
  }

  delay(100);
}
