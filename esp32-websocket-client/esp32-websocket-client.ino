#include "c:/dev/fredmill/messageStruct.h"
#include <IotWebConf.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "fredmillConfig";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "12341234";

#define CONFIG_VERSION "fred1"

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);

// serialMessage outMsg;
SerialMessage inMsg;

// https://github.com/gilmaimon/ArduinoWebsockets
#include <ArduinoWebsockets.h>
// #include <WiFi.h>

#define LED_BUILTIN 22

// const char *ssid = "IAMAIOTDEVICE";      // Enter SSID
// const char *password = "verysafepassword"; // Enter Password

// const char *url = "ws://echo.websocket.org:80/";
const char *url = "ws://fredmill.herokuapp.com:80/";
// const char *url = "ws://192.168.1.210:8800/";

bool webSocketConnected = false;
bool shouldSendSocketData = false;

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
    Serial.println("Trying to reconnect");
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial2.begin(115200);
  Serial.begin(115200);

  // -- Initializing the configuration.
  iotWebConf.init();
  // -- Set up required URL handlers on the web server.
  server.on("/", [] { iotWebConf.handleConfig(); });
  server.on("/config", [] { iotWebConf.handleConfig(); });
  server.onNotFound([]() { iotWebConf.handleNotFound(); });

  // Connect to wifi
  // WiFi.begin(ssid, password);

  // // Wait some time to connect to wifi
  // for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
  // {
  //   Serial.print(".");
  //   delay(1000);
  // }

  // // Check if connected to wifi
  // if (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.println("No Wifi!");
  //   return;
  // }

  iotWebConf.setWifiConnectionCallback(&wifiConnected);

  // sendSerialRequest();
}

void wifiConnected()
{
  Serial.println("Connected to Wifi, Connecting to server.");

  digitalWrite(LED_BUILTIN, LOW);
  // return;
  // try to connect to Websockets server
  webSocketConnected = client.connect(url);
  // client.connect(websockets_server_host, websockets_server_port, "/");

  if (webSocketConnected)
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
    shouldSendSocketData = true;
  });
  client.onEvent(onEventsCallback);
}

unsigned long printStamp = 0;
unsigned long printInterval = 300;

unsigned long socketSendStamp = 0;
unsigned long socketSendTimeOut = 250;

unsigned long serialRequestStamp = 0;
unsigned long serialRequestTimeOut = 500;

void loop()
{
  unsigned long now = millis();

  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();

  if (Serial2.available() >= sizeof(SerialMessage))
  {
    int nrOfReceivedBytes =
        Serial2.readBytes((uint8_t *)&inMsg, sizeof(SerialMessage));
    // Check for corrupt msg!
    if (nrOfReceivedBytes != sizeof(SerialMessage) ||
        inMsg.startChar[0] != '<' || inMsg.endChar[0] != '>')
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
      // printInMsg();
      if (webSocketConnected && shouldSendSocketData)
      {
        // Serial.printf("Sending to socket: \n");
        // printSocketData();
        client.sendBinary((const char *)&inMsg, sizeof(SerialMessage));
        socketSendStamp = now;
        shouldSendSocketData = false;
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      }
      sendSerialRequest();
    }
  }
  else if (now - serialRequestStamp > serialRequestTimeOut)
  {
    sendSerialRequest();
  }

  if (now - socketSendStamp > socketSendTimeOut)
  {
    shouldSendSocketData = true;
  }

  if (webSocketConnected && now - printStamp > printInterval)
  {
    printStamp = now;
    printSocketData();
  }

  // let the websockets client check for incoming messages
  if (webSocketConnected)
  {
    if (client.available())
    {
      client.poll();
    }
    else
    {
      Serial.println("client not available. Trying to reconnect");
      webSocketConnected = false;
      webSocketConnected = client.connect(url);

      // Serial.println("client not available. RESTARTING");
      // ESP.restart();
    }
  }
  else
  {
    webSocketConnected = client.connect(url);
  }
}

void printSocketData()
{
  printInMsg();
  Serial.write((const uint8_t *)&inMsg, sizeof(SerialMessage));
  Serial.println();
}

void printInMsg()
{
  for (int i = 0; i < serialMessageNrOfTouchValues; i++)
  {
    Serial.printf("touchValue %i: %i \t", i, inMsg.touchValues[i]);
  }
  Serial.println();
  for (int i = 0; i < serialMessageNrOfLoadCellValues; i++)
  {
    Serial.printf("loadCellValue %i: %i \t", i, inMsg.loadCellValues[i]);
  }
  Serial.println();
}

void sendSerialRequest()
{
  serialRequestStamp = millis();
  // Serial.printf("requesting serial\n");
  Serial2.print('.');
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}
