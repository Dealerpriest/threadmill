#include "c:/dev/fredmill/messageStruct.h"

SerialMessage outMsg;

const int touchPins[] = {0, 1, 15, 16, 17, 18};
const int nrOfTouchPins = sizeof(touchPins) / sizeof(int);
int touchValues[nrOfTouchPins];
int touchReferenceValues[nrOfTouchPins];

const int loadCellPins[] = {20, 21, 22, 23};
const int nrOfLoadCells = sizeof(loadCellPins) / sizeof(int);
int loadCellValues[nrOfLoadCells];
int loadCellReferenceValues[nrOfLoadCells];

void measureTouchReferenceValues()
{
  Serial.printf("CALIBRATING:\n\n\n\n\n\n");
  const int nrOfMeasures = 25;
  for (int i = nrOfTouchPins - 1; i >= 0; i--)
  {
    touchReferenceValues[i] = 0;
    delay(10);
    for (int k = 0; k < nrOfMeasures; k++)
    {
      int value = touchRead(touchPins[i]);
      Serial.printf("calibration touch %i index %i: %i\n", i, k, value);
      touchReferenceValues[i] += value;
      delay(1);
    }
    Serial.printf("sum touchPin %i: %i\t", i, touchReferenceValues[i]);
    touchReferenceValues[i] /= nrOfMeasures;
    Serial.printf("avg touchPin %i: %i\t", i, touchReferenceValues[i]);
    Serial.println();
  }
}

void measureLoadCellReferenceValues()
{
  for (int i = 0; i < nrOfLoadCells; i++)
  {
    loadCellReferenceValues[i] = 0;
    for (int k = 0; k < 100; k++)
    {
      loadCellReferenceValues[i] += analogRead(loadCellPins[i]);
      delay(1);
    }
    loadCellReferenceValues[i] /= 100;
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);

  Serial.printf("STARTING SKETCH!!!\n\n\n\n");

  for (int i = 0; i < nrOfTouchPins; i++)
  {
    touchReferenceValues[i] = 0;
  }

  // delay(1000);
  // measureTouchReferenceValues();
  // measureLoadCellReferenceValues();
}

unsigned long printStamp = 0;
unsigned long printInterval = 100;

unsigned long uBitSendStamp = 0;
unsigned long uBitSendInterval = 50;

unsigned long calibrationStamp = 0;
bool calibrated = false;

void loop()
{

  unsigned long now = millis();

  if (now - uBitSendStamp > uBitSendInterval)
  {
    uBitSendStamp = now;

    // Serial3.printf("%i#", outMsg.touchValues[0]);

    for (int i = 0; i < serialMessageNrOfTouchValues; i++)
    {
      Serial3.printf("%i,", outMsg.touchValues[i]);
      // Serial.printf("%i,", outMsg.touchValues[i]);
    }
    Serial3.printf("$");
    // Serial.printf("$");
    for (int i = 0; i < serialMessageNrOfLoadCellValues; i++)
    {
      Serial3.printf("%i,", outMsg.loadCellValues[i]);
      // Serial.printf("%i,", outMsg.loadCellValues[i]);
    }
    Serial3.printf("#");
    // Serial.printf("#");
    // Serial3.write((const char *)&outMsg, sizeof(SerialMessage));
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  for (int i = 0; i < serialMessageNrOfLoadCellValues; i++)
  {
    loadCellValues[i] = analogRead(loadCellPins[i]);
    outMsg.loadCellValues[i] = loadCellValues[i];
  }

  for (int i = 0; i < serialMessageNrOfTouchValues; i++)
  {
    touchValues[i] = touchRead(touchPins[i]);
    outMsg.touchValues[i] = touchValues[i] - touchReferenceValues[i];
  }

  if (Serial2.available())
  {
    uint8_t inByte = Serial2.read();
    if (inByte == '.')
    {
      Serial2.write((const char *)&outMsg, sizeof(SerialMessage));
    }
    else if (inByte == '!')
    {
      Serial.println("----------- Some error with serial comms!");
    }
    else
    {
      Serial.printf("received: %c", inByte);
    }
  }

  if (now - printStamp > printInterval)
  {
    printStamp = now;
    printMessage(outMsg);
  }

  if (!calibrated && (now - calibrationStamp > 3000))
  {
    calibrationStamp = now;
    calibrated = true;
    // measureTouchReferenceValues();
  }
}

void printMessage(SerialMessage msg)
{
  Serial.printf("msgPrint: \n");
  for (size_t i = 0; i < serialMessageNrOfTouchValues; i++)
  {
    Serial.printf("touch (raw) %i: %i \t", i, touchValues[i]);
  }
  Serial.println();

  for (size_t i = 0; i < serialMessageNrOfTouchValues; i++)
  {
    Serial.printf("touchValue %i: %i \t", i, msg.touchValues[i]);
  }

  Serial.println();

  for (size_t i = 0; i < serialMessageNrOfLoadCellValues; i++)
  {
    Serial.printf("loadCellValue %i: %i \t", i, msg.loadCellValues[i]);
  }

  Serial.println();
}