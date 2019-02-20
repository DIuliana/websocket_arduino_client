
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
//#include <TimeLib.h>
#include <Time.h>
#include <Timer.h>

#define PING_TIME 120

const char* ssid     = "TP-LINK_BE92";
const char* password = "53342140";

char path[] = "/verde/socket/arduino_123";
char host[] = "2cece6ee.ngrok.io";

WebSocketClient webSocketClient;
WiFiClient client;
  Timer timer;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  if (client.connect(host, 80)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }

  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }

  //PING
  int tickEvent = timer.every(PING_TIME*1000, sendPing);

}

void loop() {
  String data;
  time_t t = now();

  if (client.connected()) {

    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
      Serial.println(String(minute(t)) + ":" + String(second(t)));
      //TODO messaging protocol
      if (data.endsWith("status!")) {
        webSocketClient.sendData("humidity over 9000");
      }
    }
  } else {
    // TODO
  }
timer.update();
}


void sendPing()
{
  if (client.connected()) {
    webSocketClient.sendData("", WS_OPCODE_PING);
    Serial.println("ping...");
  }
}
