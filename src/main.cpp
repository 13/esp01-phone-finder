#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define SENSOR_PIN      0  // LED Pin
#define SENSOR_TYPE     ""
#define SENSOR_IP       53 // IP Address

#define WLAN_SSID       ""
#define WLAN_PASS       ""

#define AIO_SERVER      "192.168.1.1"
#define AIO_SERVERPORT  1883 // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""
#define AIO_FEED        "phonefinder/alert"
#define MSG             "benphone"
#define DS_L            0; // DeepSleep Long minutes
#define DS_S            0; // DeepSleep Short minutes

ADC_MODE(ADC_VCC);

// MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish phonefinder = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME AIO_FEED);

// WiFi Static
IPAddress ip_static(192, 168, 22, SENSOR_IP);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 22, 2);
IPAddress dns1(192, 168, 22, 9);

String sensor_id = ""; // sensor id afterwards generated
String hn_prefix = "ESP-"; // hostname with sensor_id
boolean running = false; // flag to check if running

// platformio fix
void MQTT_connect(const char *message);
unsigned int mac2int(const uint8_t *macAddr);
void sleepDeep(bool a);

void setup() {
  // Start Serial Output
  Serial.begin(9600);
  delay(10);

  // LED status
  pinMode(SENSOR_PIN, OUTPUT);
  digitalWrite(SENSOR_PIN, HIGH);

  // Start Boot
  Serial.printf("\n\nBooting... Compiled: %s", __TIMESTAMP__);

  // Start WIFI
  uint8_t macAddr[6];
  sensor_id = mac2int(WiFi.macAddress(macAddr));
  hn_prefix += sensor_id;
  WiFi.hostname(hn_prefix);
  WiFi.config(ip_static, gateway, subnet, dns1); // set static ip
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.printf("\n%s %s\nSSID: %s connecting", WiFi.macAddress().c_str(),WiFi.hostname().c_str(), WLAN_SSID);
  int count = 0; int timeout = 30;
  while (count < timeout) {
    if (WiFi.status() == WL_CONNECTED) {
      count = timeout;
      Serial.printf(" OK\n");
      Serial.printf("IP: %u.%u.%u.%u ", WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
      Serial.printf("SM: %u.%u.%u.%u ", WiFi.subnetMask()[0],WiFi.subnetMask()[1],WiFi.subnetMask()[2],WiFi.subnetMask()[3]);
      Serial.printf("GW: %u.%u.%u.%u ", WiFi.gatewayIP()[0],WiFi.gatewayIP()[1],WiFi.gatewayIP()[2],WiFi.gatewayIP()[3]);
      Serial.printf("DNS: %u.%u.%u.%u\n", WiFi.dnsIP()[0],WiFi.dnsIP()[1],WiFi.dnsIP()[2],WiFi.dnsIP()[3]);
    } else {
      delay(500);
      Serial.printf(".");
      ++count;
    }
  }
  if (WiFi.status() != WL_CONNECTED){
    Serial.printf(" ERR\n");
  }
}

void loop() {

  if (!running) {
    MQTT_connect(MSG);
    //sleepDeep(true);
    running = true;
  }
}

void MQTT_connect(const char *message) {
  int8_t ret;
  uint8_t retries = 3;

  if (mqtt.connected()) {
    return;
  }
  Serial.printf("[MQTT] Connecting... ");
  while ((ret = mqtt.connect()) != 0) { 
    Serial.printf("\n[MQTT] %s RETRY...", mqtt.connectErrorString(ret));
    mqtt.disconnect();
    delay(1000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      Serial.printf("TIMEOUT\n");
      sleepDeep(false);
    }
  }
  Serial.printf("OK\n");

  Serial.printf("[MQTT] %s %s %s\n[MQTT] Sending... ", AIO_SERVER, AIO_FEED, message);
  if (! phonefinder.publish(message)) {
    Serial.printf("ERR\n");
  } else {
    Serial.printf("OK\n");
  }
  delay(250); // debounce
}

unsigned int mac2int(const uint8_t *macAddr) {
  char hexAddr[3];
  sprintf(hexAddr,"%02x%02x%02x\n",macAddr[3], macAddr[4], macAddr[5]);
  return (int)strtol(hexAddr, 0, 16);
}

void sleepDeep(bool a) {
  int ds_t;
  if (a) {
    ds_t = DS_L;
  } else {
    ds_t = DS_S;
  }
  Serial.printf("Deep Sleep: %dm\n", ds_t);
  ESP.deepSleep(ds_t * 60 * 1000000);
}