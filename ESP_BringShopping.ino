#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define MAX_CONNECT_ATTEMPTS 20
#define SLEEP_MINS 5
const char* LIST_ID = "e0cb9303-d682-4fb2-a537-e66aea5706de";
const char* LOCALE = "en-GB";

// WiFi Parameters
// TODO: Fetch these from SPIFFS as per my other projects
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

String listUrlBase = "http://api.getbring.com/rest/bringlists/";
String articleUrlBase = "http://web.getbring.com/locale/articles.";
String listUrl = "";
String articlesUrl = "";


JsonObject& fetchJsonData(String url) {
    HTTPClient http;  
    http.begin(url);
    int httpCode = http.GET();
    //Check the returning code                                                                  
    if (httpCode > 0) {
      // Get the request response payload      
      DynamicJsonBuffer jsonBuffer(2000);
      JsonObject &data = jsonBuffer.parseObject(http.getString());
      http.end();   //Close connection
      return data;
    }

}

void setup() {
  Serial.begin(115200);
  listUrl = String(listUrlBase + LIST_ID);
  articlesUrl = String(articleUrlBase + LOCALE + ".json");

  WiFi.begin(ssid, password);

  /* Connect to Wifi */
  Serial.println("Connecting...");
  int failCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    failCount++;
    if (failCount == MAX_CONNECT_ATTEMPTS) {
      Serial.println("Wifi connection failed. Giving up after too many attempts");
      fail();
    }
  }

  /* Fetch Data */
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {
    JsonObject &list = fetchJsonData(listUrl);
    JsonObject &articles = fetchJsonData(articlesUrl);
    
    Serial.println("*** ARTICLES ***");
    for (JsonPair& article : articles) {
      Serial.print(article.key);
      Serial.println(article.value.as<char*>());
    }
    
    Serial.println("*** LIST ***");
    JsonArray& items = list["purchase"];
    for(JsonObject& item : items) {
      Serial.println(item["name"].as<char*>());
    }
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void blink(int onMillis, int offMillis) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(onMillis);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(offMillis);
}

void gotoSleep() {
  ESP.deepSleep(SLEEP_MINS * 60 * 1000000);
}

void fail() {
  //something has gone wrong, blink an SOS on the LED then sleep the ESP
  for (int i=0; i < 3; i++) {
    blink(250, 250);
  }    
  for (int i=0; i < 3; i++) {
    blink(500, 250);
  }
  for (int i=0; i < 3; i++) {
    blink(250, 250);
  }
  gotoSleep();
}
