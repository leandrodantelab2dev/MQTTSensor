/***************************************************************************
  This is a library for the AMG88xx GridEYE 8x8 IR camera

  This sketch tries to read the pixels from the sensor

  Designed specifically to work with the Adafruit AMG88 breakout
  ----> http://www.adafruit.com/products/3538

  These sensors use I2C to communicate. The device's I2C address is 0x69

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid     = "LIVE TIM_CD84_2G";
const char* password = "4un36ccdbx";
const char* mqtt_server = "34.135.186.196";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

void setup() {
    Serial.begin(9600);
    Serial.println(F("AMG88xx pixels"));

    bool status;
    
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Pixels Test --");

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    delay(100); // let sensor boot up
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() { 
    //read all the pixels
    amg.readPixels(pixels);  
    String mapTemp = "[";
    //Serial.print("[");
    float topValue = 0;
    for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
    //  Serial.print(pixels[i-1]);
        mapTemp += pixels[i-1];
        if(topValue < float(pixels[i-1])){
          topValue = float(pixels[i-1]);
        }
    //  Serial.print(", ");
        mapTemp += ",";
    //  if( i%8 == 0 ) Serial.println();
    }
    mapTemp += "]";
    //Serial.println("]");
    //Serial.println();
    //Serial.println(String(amg.readThermistor()).c_str());
    Serial.println(String(topValue).c_str());

    if (!client.connected()) {
      reconnect();
    }
    //Serial.println(mapTemp);
    client.publish("Thermistor1", String(topValue).c_str());
    //client.publish("MapTemperature", mapTemp.c_str());
    //delay a second
    delay(1000);
}