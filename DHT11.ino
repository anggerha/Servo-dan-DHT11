#include "DHT.h"
#include "Servo.h"
#include "WiFi.h"
#include "MQTT.h"

const char ssid[] = "Wi-Fi";
const char pass[] = "t4np4k4bel";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("wifi check ");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("ESP32", "f191b033", "c870232af9647138")) {
    Serial.println("connecting ");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/DHTData");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

const int pinDHT = 4;
int servoPin = 25;
Servo newServo;

#define DHTTYPE DHT11   // DHT 11

DHT dht(pinDHT, DHTTYPE);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  dht.begin();
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);

  Serial.println(F("DHT11 test!"));
  newServo.attach(servoPin);
  connect();
}

void loop() {
  client.loop();
  delay(10);
  if(!client.connected()){
    connect();
  }
     
    int t = dht.readTemperature();
    
    if (isnan(t)) {
      Serial.println(F("Gagal membaca suhu !"));
      return;
    }
      if(t<35){
        newServo.write(0);
        client.publish("/DHTData/Alert", "Suhu dibawah 35 derajat celcius");
      }
      else if(t>=35 && t<45){
        newServo.write(90);
        client.publish("/DHTData/Alert", "Suhu diatas 35 derajat celcius");
      }
      else if(t>=45){
        newServo.write(180);
        client.publish("/DHTData/Alert", "Suhu diatas 45 derajat celcius");
      }
      Serial.print(t);
      Serial.println(F("°C "));
      
      char tString[10];
      itoa(t, tString, 10);
      client.publish("/DHTData/Suhu", tString);
}
