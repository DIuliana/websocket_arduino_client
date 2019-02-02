
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
//#include <TimeLib.h>
#include <Time.h>

 
const char* ssid     = "TP-LINK_BE92";
const char* password = "53342140";
 
char path[] = "/verde/socket/arduino_123";
char host[] = "2cece6ee.ngrok.io";
 
WebSocketClient webSocketClient;
WiFiClient client;
 
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
 
  delay(5000);
 
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
      if(data.endsWith("status!")){
        webSocketClient.sendData("humidity over 9000");
      }
    }
 
  } else {
    Serial.println("Client disconnected.");
  }
}
