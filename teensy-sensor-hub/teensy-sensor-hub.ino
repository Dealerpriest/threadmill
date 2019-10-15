#include "c:/dev/threadmill/messageStruct.h"

SerialMessage outMsg;
// SerialMessage inMsg;

const int touchPins[] = {0,1,15,16,17,18,19,22,23};
const int nrOfTouchPins = sizeof(touchPins)/sizeof(int);
int touchValues[nrOfTouchPins];
int touchReferenceValues[nrOfTouchPins];

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);

  for (int i = 0; i < nrOfTouchPins; i++)
  {
    touchReferenceValues[i] = 0;
    for (int k = 0; k < 100; k++)
    {
      touchReferenceValues[i] += touchRead(touchPins[i]);
    }
    touchReferenceValues[i] /= 100;
    
  }
  
}

unsigned long printStamp = 0;
unsigned long printInterval = 100;

void loop(){
  unsigned long now = millis();

  for (int i = 0; i < serialMessageNrOfTouchValues; i++)
  {
    touchValues[i] = touchRead(touchPins[i]);
    outMsg.touchValues[i] = touchValues[i] - touchReferenceValues[i];
  }

  if(Serial2.available()){
    uint8_t inByte = Serial2.read();
    if(inByte == '.'){
      Serial2.write((const char*) &outMsg, sizeof(SerialMessage));
    }else if(inByte == '!'){
      Serial.println("----------- Some error with serial comms!");
    }else{
      Serial.printf("received: %c", inByte);
    }
  }

  if(now - printStamp > printInterval){
    printStamp = now;
    printMessage(outMsg);
  }
  
}

void printMessage(SerialMessage msg){
  Serial.printf("msgPrint: \n");
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