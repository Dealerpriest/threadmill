/*
        Esp32 Websockets Client

        This sketch:
        1. Connects to a WiFi network
        2. Connects to a Websockets server
        3. Sends the websockets server a message ("Hello Server")
        4. Prints all incoming messages while the connection is open

        Hardware:
        For this sketch you only need an ESP32 board.

        Created 15/02/2019
        By Gil Maimon
        https://github.com/gilmaimon/ArduinoWebsockets

*/

// https://github.com/gilmaimon/ArduinoWebsockets
#include <ArduinoWebsockets.h>
#include <WiFi.h>

const char *ssid = "Interactive";      // Enter SSID
const char *password = "wecanbuildit"; // Enter Password
const char *websockets_server_host =
    "ws://echo.websocket.org";              // Enter server adress
const uint16_t websockets_server_port = 80; // Enter server port
const char *url = "ws://echo.websocket.org:80/";

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

String nowString;
void setup()
{
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
    Serial.print("Got Message: ");
    Serial.println(message.data());

    Serial.print("Sending: ");
    Serial.println(nowString);
    client.send(nowString);
  });
  client.onEvent(onEventsCallback);
}
unsigned long sendStamp = 0;
unsigned long sendInterval = 30;
void loop()
{
  unsigned long now = millis();
  nowString = now;

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
    client.connect(url);
    Serial.println("client not available");
  }
}