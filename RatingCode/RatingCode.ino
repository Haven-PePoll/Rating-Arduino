#include <AceButton.h>
using namespace ace_button;

#include "time.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h" //Provide the real-time database payload printing info and other helper functions.

#define WIFI_SSID "SuryasPhone"

#define WIFI_PASSWORD "suryam0801"

#define API_KEY "AIzaSyCOI3KIs3b3JvAY4u4ayqxiZXBbOuiSpBU"

#define USER_EMAIL "creamery01@peopoll.com"
#define USER_PASSWORD "creamery"

#define FIREBASE_PROJECT_ID "thepollingboothmvp"

#define DATABASE_URL "https://thepollingboothmvp-default-rtdb.asia-southeast1.firebasedatabase.app"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String Question = "";

const char* ntpServer = "pool.ntp.org";
unsigned long epochTime;


String pollId;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

const int BUTTON1_PIN = 13;
const int BUTTON2_PIN = 25;
const int BUTTON3_PIN = 23; 
const int BUTTON4_PIN = 18;
const int BUTTON5_PIN = 5;

#ifdef ESP32
// Different ESP32 boards use different pins
const int LED1_PIN = 32;
const int LED2_PIN = 27;
const int LED3_PIN = 14; 
const int LED4_PIN = 19;
const int LED5_PIN = 2;
#else
const int LED_PIN = LED_BUILTIN;
#endif

const int LED_ON = HIGH;
const int LED_OFF = LOW;

AceButton button1(BUTTON1_PIN);
AceButton button2(BUTTON2_PIN);
AceButton button3(BUTTON3_PIN);
AceButton button4(BUTTON4_PIN);
AceButton button5(BUTTON5_PIN);

void handleEvent(AceButton*, uint8_t, uint8_t);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(LED5_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  pinMode(BUTTON5_PIN, INPUT_PULLUP);

  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);

  connectToWifi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop() {

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

  // put your main code here, to run repeatedly:
  button1.check();
  button2.check();
  button3.check();
  button4.check();
  button5.check();
}

void connectToWifi () {

  uint32_t notConnectedCounter = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED1_PIN, (millis() / 500) % 2);
    delay(100);
    Serial.println("Wifi connecting...");
    notConnectedCounter++;
    if (notConnectedCounter > 150) { // Reset board if not connected after 5s
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
  }

  Serial.println("Wifi connected");
  Serial.println(WiFi.localIP());

  digitalWrite(LED1_PIN, LED_ON);
  delay(3000);
  digitalWrite(LED1_PIN, LED_OFF);
  
  configTime(0, 0, ntpServer);
  //setup firebase

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;


  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);


  Serial.println("Getting User POllId");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  pollId = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(pollId);

  readFirebase();
}

// The event handler for both buttons.
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      if (button->getPin() == BUTTON1_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        epochTime = getTime();
        updateFirebase(1);
      } else if (button->getPin() == BUTTON2_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        epochTime = getTime();
        updateFirebase(2);
      } else if (button->getPin() == BUTTON3_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        digitalWrite(LED3_PIN, LED_ON);
        epochTime = getTime();
        updateFirebase(3);
      } else if (button->getPin() == BUTTON4_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        digitalWrite(LED3_PIN, LED_ON);
        digitalWrite(LED4_PIN, LED_ON);
        epochTime = getTime();
        updateFirebase(4);
      } else if (button->getPin() == BUTTON5_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        digitalWrite(LED3_PIN, LED_ON);
        digitalWrite(LED4_PIN, LED_ON);
        digitalWrite(LED5_PIN, LED_ON);
        epochTime = getTime();
        updateFirebase(5);
      }
      break;
    case AceButton::kEventReleased:
      digitalWrite(LED1_PIN, LED_OFF);
      digitalWrite(LED2_PIN, LED_OFF);
      digitalWrite(LED3_PIN, LED_OFF);
      digitalWrite(LED4_PIN, LED_OFF);
      digitalWrite(LED5_PIN, LED_OFF);
      break;
    case AceButton::kEventLongPressed:
      digitalWrite(LED1_PIN, LED_ON);
      break;
  }
}

void updateFirebase(int rating) {
  if (Firebase.ready())
  {
    String documentPath = "pollData/";
    FirebaseJson content;

    content.set("fields/rating/integerValue", rating);
    content.set("fields/location/stringValue", "creamery");
    content.set("fields/question/stringValue", String(Question).c_str());
    content.set("fields/timestamp/integerValue", epochTime);

    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      Serial.println("New Write");
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "")) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      Serial.println("UPdate Write");
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }

  }
}

void readFirebase () {
  if (Firebase.RTDB.getString(&fbdo, "/Location-Question/Creamery")) {

    if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_string) {
      Question = fbdo.to<String>();
      Serial.println(fbdo.to<String>());
    }

  } else {
    Serial.println(fbdo.errorReason());
  }
}


unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}
