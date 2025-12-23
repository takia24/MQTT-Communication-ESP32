#include <WiFi.h>
#include <PubSubClient.h>

// =====================
// WiFi Credentials
// =====================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// =====================
// MQTT Broker Settings
// =====================
const char* mqtt_broker = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "wokwi/esp32/demo";

// (Public broker, so username/password optional)
const char* mqtt_username = "Takia";
const char* mqtt_password = "2532467";

// =====================
// Objects
// =====================
WiFiClient espClient;
PubSubClient client(espClient);

// =====================
// MQTT Callback Function
// =====================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

// =====================
// Connect to WiFi
// =====================
void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// =====================
// Connect to MQTT Broker
// =====================
void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT Broker... ");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected!");
      client.subscribe(mqtt_topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("Failed (rc=");
      Serial.print(client.state());
      Serial.println("). Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// =====================
// Setup Function
// =====================
void setup() {
  Serial.begin(115200);
  delay(1000);

  connectToWiFi();

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqttCallback);

  connectToMQTT();
}

// =====================
// Loop Function
// =====================
void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }

  client.loop();

  static unsigned long lastPublishTime = 0;

  if (millis() - lastPublishTime >= 5000) {
    lastPublishTime = millis();

    client.publish(mqtt_topic, "Hello from ESP32 in Wokwi!");
    Serial.println("Message Published!");
  }
}

