#include <AceButton.h>
using namespace ace_button;

#ifdef ESP32
// Different ESP32 boards use different pins
const int LED1_PIN = 2;
const int LED2_PIN = 19;
const int LED3_PIN = 14;
const int LED4_PIN = 13;
#else
const int LED_PIN = LED_BUILTIN;
#endif

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h" //Provide the token generation process info.
#include "addons/RTDBHelper.h" //Provide the real-time database payload printing info and other helper functions.

#define WIFI_SSID "moto"
#define WIFI_PASSWORD "1234567890"

#define API_KEY "AIzaSyCOI3KIs3b3JvAY4u4ayqxiZXBbOuiSpBU"

#define USER_EMAIL "creamery01@peopoll.com"
#define USER_PASSWORD "creamery"

#define FIREBASE_PROJECT_ID "thepollingboothmvp"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String pollId;
unsigned long previousMillis = 0;
unsigned long interval = 30000;

const int BUTTON1_PIN = 5;//23
const int BUTTON2_PIN = 18;
const int BUTTON3_PIN = 23;
const int BUTTON4_PIN = 25;

const int LED_ON = HIGH;
const int LED_OFF = LOW;

AceButton button1(BUTTON1_PIN);
AceButton button2(BUTTON2_PIN);
AceButton button3(BUTTON3_PIN);
AceButton button4(BUTTON4_PIN);

void handleEvent(AceButton*, uint8_t, uint8_t);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);

  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);

  connectToWifi();
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
}

void connectToWifi () {
  long st = millis();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(WiFi.status());
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  long tm = millis() - st;
  Serial.print(F("\nConnected to AP in "));
  Serial.print(tm, DEC);
  Serial.println();

  Serial.print("Connected to… ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //setup firebase

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);


  //  Serial.println("Getting User POllId");
  //  while ((auth.token.uid) == "") {
  //    Serial.print('.');
  //    delay(1000);
  //  }
  //
  //  pollId = auth.token.uid.c_str();
  //  Serial.print("User UID: ");
  //  Serial.println(pollId);
}

// The event handler for both buttons.
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      if (button->getPin() == BUTTON1_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        updateFirebase("1");
      } else if (button->getPin() == BUTTON2_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        updateFirebase("2");
      } else if (button->getPin() == BUTTON3_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        digitalWrite(LED3_PIN, LED_ON);
        updateFirebase("3");
      } else if (button->getPin() == BUTTON4_PIN) {
        digitalWrite(LED1_PIN, LED_ON);
        digitalWrite(LED2_PIN, LED_ON);
        digitalWrite(LED3_PIN, LED_ON);
        digitalWrite(LED4_PIN, LED_ON);
        updateFirebase("4");
      }
      break;
    case AceButton::kEventReleased:
      digitalWrite(LED1_PIN, LED_OFF);
      digitalWrite(LED2_PIN, LED_OFF);
      digitalWrite(LED3_PIN, LED_OFF);
      digitalWrite(LED4_PIN, LED_OFF);
      break;
  }
}

void updateFirebase(String option) {
  if (Firebase.ready())
  {
    String documentPath = "pollData/" + pollId + "";
    FirebaseJson content;

    content.set("fields/rating/stringValue", String(option).c_str());
    content.set("fields/location/stringValue", "creamery");
    content.set("fields/question/stringValue", "how was the food?");

    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      Serial.println("New Write");
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}
