#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define MAX_CONNECT_ATTEMPTS 20
#define SLEEP_MINS 5
const char* LIST_ID = "e0cb9303-d682-4fb2-a537-e66aea5706de";

// WiFi Parameters
// TODO: Fetch these from SPIFFS as per my other projects
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

String listUrlBase = "http://api.getbring.com/rest/bringlists/";
String articlesUrl = "http://api.myjson.com/bins/19ggxd";
String listUrl = "";


JsonObject& fetchJsonData(String url, int bufferSize) {
    HTTPClient http;  
    http.begin(url);
    int httpCode = http.GET();
    //Check the returning code
    Serial.print("Response: "); 
    Serial.println(httpCode);                                                                 
    if (httpCode > 0) {
      // Get the request response payload      
      DynamicJsonBuffer jsonBuffer(bufferSize);
      JsonObject &data = jsonBuffer.parseObject(http.getString());
      jsonBuffer.clear();
      http.end();   //Close connection
      return data;
    }

}

void setup() {
  Serial.begin(115200);
  listUrl = String(listUrlBase + LIST_ID);

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
  Serial.println("Connected!");

  /* Fetch Data */
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(listUrl);
    JsonObject &list = fetchJsonData(listUrl, 1500);
    Serial.println(articlesUrl);
    JsonObject &articles = fetchJsonData(articlesUrl, 20000 );

    Serial.println("*** ARTICLES ***");
    Serial.println(articles.size());
    for (JsonPair& article : articles) {
      Serial.print(article.key);
      Serial.println(article.value.as<char*>());
    }
    
    //Serial.println(articles["Ingwer"].as<char*>());
    
    Serial.println("*** LIST ***");
    JsonArray& items = list["purchase"];
    Serial.print("List size: ");
    Serial.println(list.size());
    Serial.print("Item size: ");
    Serial.println(items.size());
    
    for(JsonObject& item : items) {
      Serial.println(item["name"].as<char*>());
      Serial.println(articles[item["name"].as<char*>()].as<char*>());
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
//  ESP.deepSleep(SLEEP_MINS * 60 * 1000000);
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
