
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketClient.h>
#include <Time.h>
#include <ArduinoJson.h>
#include <Timer.h>

#define PING_TIME 120
//
const char* ssid     = "lala";
const char* password = "53342140";

char path[] = "/verde/socket/arduino_123";
char host[] = "c6431f4a.ngrok.io";

WebSocketClient webSocketClient;
WiFiClient client;
Timer timer;
ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);

  if (createSoftAccessPoint()) {
      connectToNetwork();
  }

    WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
     server.handleClient();
    Serial.println(WiFi.status());
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
  int tickEvent = timer.every(PING_TIME * 1000, sendPing);

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

boolean createSoftAccessPoint() {

//TODO stop access point after connection
  Serial.println("Creating Soft Access Point...");
  boolean ready = WiFi.softAP("arduino_123", "123456789");
  Serial.println(ready);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  return ready;
}

void connectToNetwork(){
  server.on("/add_network", handleNetwork);
  server.begin();
  Serial.println("HTTP server started");
}


//Handler for the body path
void handleNetwork() {
  const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);

  //Check if body received
  if (server.hasArg("plain") == false) {
    server.send(500, "text/plain", "Body not received");
    return;

  }

  JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));

  ssid = root["ssid"];
  password = root["pass"];

  Serial.println(ssid);
  Serial.println(password);

  WiFi.begin(ssid, password);



  server.send(200, "text/json", createResponse());

}



  String createResponse(){

    StaticJsonBuffer<200> jsonBufferResp;
    JsonObject& response = jsonBufferResp.createObject();
    response["status"] = "OK";

    String json;
    response.prettyPrintTo(json);
    return json;

}
