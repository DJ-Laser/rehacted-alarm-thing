#include <Wire.h>

const int trigPin = 9;
const int echoPin = 10;

const int buzzerPin = 12;

const int i2cAddress = 55;

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Wire.begin(i2cAddress);
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

bool alarmTripped = false;

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
