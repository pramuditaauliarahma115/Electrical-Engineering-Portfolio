
/*
 RGB Conveyor Sorting System
 Board : Arduino UNO
 Sensor : LDR / Photodiode on A0
 LEDs : Red(D3), Green(D4), Blue(D5)
 Conveyor : L298N
 ENA=D6, IN1=D7, IN2=D8
 Servo1=D9
 Servo2=D10

 NOTE:
 Thresholds MUST be calibrated for your hardware.
*/

#include <Servo.h>

Servo servo1;
Servo servo2;

// ---------------- Pins ----------------
const byte RED_LED   = 3;
const byte GREEN_LED = 4;
const byte BLUE_LED  = 5;

const byte ENA = 6;
const byte IN1 = 7;
const byte IN2 = 8;

const byte SERVO1_PIN = 9;
const byte SERVO2_PIN = 10;

const byte SENSOR = A0;

// ---------- Servo Positions ----------
const int S1_LEFT   = 40;
const int S1_CENTER = 90;

const int S2_LEFT   = 40;
const int S2_RIGHT  = 140;

// ---------- Thresholds (edit after calibration) ----------
int RED_LIMIT   = 550;
int GREEN_LIMIT = 550;
int BLUE_LIMIT  = 550;

// ---------- Variables ----------
int redValue, greenValue, blueValue;

enum ColorType
{
  RED_COLOR,
  GREEN_COLOR,
  BLUE_COLOR,
  UNKNOWN
};

// ---------------- Utility ----------------
void conveyorStart(int pwm)
{
  analogWrite(ENA, pwm);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void conveyorStop()
{
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void ledsOff()
{
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

int averageADC()
{
  long sum = 0;

  for (int i = 0; i < 20; i++)
  {
    sum += analogRead(SENSOR);
    delay(2);
  }

  return sum / 20;
}

int readRed()
{
  ledsOff();
  digitalWrite(RED_LED, HIGH);
  delay(40);
  return averageADC();
}

int readGreen()
{
  ledsOff();
  digitalWrite(GREEN_LED, HIGH);
  delay(40);
  return averageADC();
}

int readBlue()
{
  ledsOff();
  digitalWrite(BLUE_LED, HIGH);
  delay(40);
  return averageADC();
}

void readRGB()
{
  redValue   = readRed();
  greenValue = readGreen();
  blueValue  = readBlue();

  ledsOff();

  Serial.print("R : ");
  Serial.print(redValue);

  Serial.print("   G : ");
  Serial.print(greenValue);

  Serial.print("   B : ");
  Serial.println(blueValue);
}

ColorType detectColor()
{
  if (redValue > greenValue &&
      redValue > blueValue &&
      redValue > RED_LIMIT)
      return RED_COLOR;

  if (greenValue > redValue &&
      greenValue > blueValue &&
      greenValue > GREEN_LIMIT)
      return GREEN_COLOR;

  if (blueValue > redValue &&
      blueValue > greenValue &&
      blueValue > BLUE_LIMIT)
      return BLUE_COLOR;

  return UNKNOWN;
}

void sortRed()
{
  Serial.println("Detected : RED");

  servo1.write(S1_LEFT);

  delay(900);

  servo1.write(S1_CENTER);

  delay(400);
}

void sortGreen()
{
  Serial.println("Detected : GREEN");

  servo1.write(S1_CENTER);

  delay(700);

  servo2.write(S2_LEFT);

  delay(900);

  servo2.write(90);

  delay(400);
}

void sortBlue()
{
  Serial.println("Detected : BLUE");

  servo1.write(S1_CENTER);

  delay(700);

  servo2.write(S2_RIGHT);

  delay(900);

  servo2.write(90);

  delay(400);
}

void setup()
{
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  servo1.write(S1_CENTER);
  servo2.write(90);

  Serial.begin(9600);

  conveyorStart(180);

  Serial.println("RGB Conveyor Sorting Ready");
  Serial.println("--------------------------------");
}

void loop()
{
  readRGB();

  ColorType result = detectColor();

  switch (result)
  {
    case RED_COLOR:
      conveyorStop();
      delay(400);
      sortRed();
      conveyorStart(180);
      break;

    case GREEN_COLOR:
      conveyorStop();
      delay(400);
      sortGreen();
      conveyorStart(180);
      break;

    case BLUE_COLOR:
      conveyorStop();
      delay(400);
      sortBlue();
      conveyorStart(180);
      break;

    default:
      Serial.println("Unknown");
      break;
  }

  delay(300);
}

/*
======================
CALIBRATION
======================

1. Upload program.
2. Open Serial Monitor.
3. Place RED object.
4. Record R,G,B.
5. Repeat 20-30 times.
6. Do same for GREEN and BLUE.
7. Replace:

RED_LIMIT
GREEN_LIMIT
BLUE_LIMIT

with values from your experiment.

Example:

Red Object
R=812
G=402
B=301

Green Object
R=410
G=845
B=390

Blue Object
R=320
G=420
B=830

*/
