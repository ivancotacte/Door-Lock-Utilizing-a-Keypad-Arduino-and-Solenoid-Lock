#define BLYNK_TEMPLATE_ID "TMPL6Ilo-KDJc"
#define BLYNK_TEMPLATE_NAME "ESP8266 Noti"
#define BLYNK_AUTH_TOKEN "F5BM-Ls8Eg0hfqtA4LALYSRTTasMhYko"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>

const char* serverName = "ducusin.replit.app";
String ServerPath = "/email";

const int port = 443;

char ssid[32];
char password[32];

WiFiManager wifiManager;
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  wifiManager.autoConnect("Group 7 - AutoConnectAP");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

void loop() {
  Blynk.run();

  if (Serial.available() > 0) {
    String serialDoor = Serial.readStringUntil('\n');
    serialDoor.trim();

    if (serialDoor == "INCORRECT") {
      Blynk.logEvent("incorrect_passcode", "Alert: Possible intruder detected due to multiple failed attempts to unlock the door.");
      sendEmail("incorrectDoor");
    } else if (serialDoor == "OPENDOOR") {
      Blynk.logEvent("open_door", "Alert: The door has been opened");
      sendEmail("openDoor");
    } else if (serialDoor == "CLOSEDOOR") {
      Blynk.logEvent("close_door", "Alert: The door has been closed");
      sendEmail("closedDoor");
    } else if (serialDoor == "SOMEONEPASSCODE") {
      Blynk.logEvent("someonechangepasscode", "Alert: Someone has changing the passcode.");
      sendEmail("someoneChangepasscode");
    } else if (serialDoor == "CHANGESUCCESS") {
      Blynk.logEvent("successChangepasscode", "Alert: Passcode has been successfully changed.");
      sendEmail("successChangepasscode");
    } 
  }
}


void sendEmail(String Mode) {
  client.setInsecure();
  
  if (client.connect(serverName, port)) {
    Serial.println("Connection Successful");

    String postData = "apiKey=twdKxJoi1l6MZR5tPvWrN7slDr7YRgdfjtLdSUL37PAw0Q6IYr&" + Mode + "=true";

    client.println("POST " + ServerPath + " HTTP/1.1");
    client.println("Host: " + String(serverName));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  }
}
