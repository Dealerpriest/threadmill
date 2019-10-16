#include "c:/dev/threadmill/messageStruct.h"

// serialMessage outMsg;
SerialMessage inMsg;

// https://github.com/gilmaimon/ArduinoWebsockets
#include <ArduinoWebsockets.h>
#include <WiFi.h>

const char *ssid = "Interactive";      // Enter SSID
const char *password = "wecanbuildit"; // Enter Password

// const char *url = "ws://echo.websocket.org:80/";
const char *url = "ws://threadmill.herokuapp.com:80/";

bool websocketReady = false;

using namespace websockets;

WebsocketsClient client;
void onEventsCallback(WebsocketsEvent event, String data)
{
  if (event == WebsocketsEvent::ConnectionOpened)
  {
    Serial.println("Connnection Opened");
  }
  else if (event == WebsocketsEvent::ConnectionClosed)
  {
    Serial.println("Connnection Closed");
    delay(3000);
    client.connect(url);
  }
  else if (event == WebsocketsEvent::GotPing)
  {
    Serial.println("Got a Ping!");
  }
  else if (event == WebsocketsEvent::GotPong)
  {
    Serial.println("Got a Pong!");
  }
}

void setup()
{
  Serial2.begin(115200);
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
  {
    Serial.print(".");
    delay(1000);
  }

  // Check if connected to wifi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("No Wifi!");
    return;
  }

  Serial.println("Connected to Wifi, Connecting to server.");
  // try to connect to Websockets server
  bool connected = client.connect(url);
  // client.connect(websockets_server_host, websockets_server_port, "/");

  if (connected)
  {
    Serial.println("Connected!");
    client.send("Hello Server");
  }
  else
  {
    Serial.println("Not Connected!");
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {
    // Serial.print("Got Message: ");
    // Serial.println(message.data());

    // Serial.print("Sending: ");
    // Serial.println(nowString);
    // client.send(nowString);
    websocketReady = true;
    // client.sendBinary((const char *)&inMsg, sizeof(SerialMessage));
  });
  client.onEvent(onEventsCallback);

  sendSerialRequest();
}
unsigned long sendStamp = 0;
unsigned long sendInterval = 30;

unsigned long serialRequestStamp = 0;
unsigned long serialRequestTimeOut = 500;

void loop()
{
  unsigned long now = millis();

  if (Serial2.available() >= sizeof(SerialMessage))
  {
    int nrOfReceivedBytes = Serial2.readBytes((uint8_t *)&inMsg, sizeof(SerialMessage));
    //Check for corrupt msg!
    if (nrOfReceivedBytes != sizeof(SerialMessage) || inMsg.startChar != '<' || inMsg.endChar != '>')
    {
      Serial2.print('!');
      Serial.println("FUUUUCK!!!! CORRUPT MESSAGE ON SERIAL BUS!");
      Serial.print("startChar: ");
      Serial.println(inMsg.startChar);
      Serial.print("endChar: ");
      Serial.println(inMsg.endChar);
      while (Serial2.available())
      {
        Serial2.read();
      }
    }
    else
    {
      // Serial.print("SERIAL RECEIVED -->  ");
      // for (int i = 0; i < serialMessageNrOfTouchValues; i++)
      // {
      //   Serial.printf("touchValue %i: %i \t", i, inMsg.touchValues[i]);
      // }
      // Serial.println();
      if (websocketReady)
      {
        client.sendBinary((const char *)&inMsg, sizeof(SerialMessage));
        websocketReady = false;
      }
      sendSerialRequest();
    }
  }
  else if (now - serialRequestStamp > serialRequestTimeOut)
  {
    sendSerialRequest();
  }

  // let the websockets client check for incoming messages
  if (client.available())
  {
    // if (now - sendStamp > sendInterval)
    // {
    //   sendStamp = now;
    //   // Serial.print("Sending: ");
    //   // Serial.println(nowString);
    //   // client.send(nowString);
    // }

    client.poll();
  }
  else
  {
    // client.connect(url);
    Serial.println("client not available. RESTARTING");
    ESP.restart();
  }
}

void sendSerialRequest()
{
  serialRequestStamp = millis();
  // Serial.printf("requesting serial\n");
  Serial2.print('.');
}