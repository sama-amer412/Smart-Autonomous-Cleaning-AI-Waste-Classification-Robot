#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// ==================== Configuration ====================
const char* ssid = "your wifi";
const char* password = "your passward";

const char* mqtt_server = "YOUR_BROKER";  
const int   mqtt_port   = 8883;
const char* mqtt_user   = "YOUR_USERNAME";
const char* mqtt_pass   = "YOUR_PASSWORD";

const char* mqtt_topic  = "robot/move";

const int ledPin = 2;   // GPIO2

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\n=== ESP8266 MQTT Test ===");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  espClient.setInsecure();           // للتجربة (HiveMQ Cloud)
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();

  Serial.print(" Received on ");
  Serial.print(topic);
  Serial.print(" → ");
  Serial.println(msg);

  if (msg.equalsIgnoreCase("forward")) {
    Serial.println("F");
  }
  else if (msg.equalsIgnoreCase("back") || msg.equalsIgnoreCase("backward")) {
    Serial.println("B");
  }
  else if (msg.equalsIgnoreCase("left")) {
    Serial.println("L");
  }
  else if (msg.equalsIgnoreCase("right")) {
    Serial.println("R");
  }
  else if (msg.equalsIgnoreCase("stop")) {
    Serial.println("S");
  }
  else if (msg.equalsIgnoreCase("manual")) {
    Serial.println("M");
  }
  else if (msg.equalsIgnoreCase("auto")) {
    Serial.println("A");
  }
  
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT Broker...");
    
    String clientId = "ESP8266_Robot_" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println(" Connected!");
      client.subscribe(mqtt_topic);
      Serial.print("Subscribed to: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" → retry in 5s");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}