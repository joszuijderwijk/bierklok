/* 
  BIERKLOK - Displays the current beer deals on an analog clock.
  @author: Jos Zuijderwijk
*/

#define NUM_STEPS 720
#define STEPS_PER_HOUR 60
#define STEPS_PER_MINUTE 12


// Beer positions
#define HERTOG_JAN 0
#define GROLSCH 1
#define KORNUIT 2
#define BRAND 3
#define WARSTEINER 4
#define BUD 5
#define HEINEKEN 6
#define JUPILER 7
#define ALFA 8
#define BAVARIA 9
#define AMSTEL 10
#define GULPENER 11

// supermarkets
enum Supermarket{
  ALBERTHEIJN, COOP, PLUS, SPAR, BOONS, HOOGVLIET
};

// priorities
const int priorities[12] = {HERTOG_JAN, GROLSCH, BRAND, HEINEKEN, KORNUIT, AMSTEL, JUPILER, WARSTEINER, BUD, ALFA, GULPENER, BAVARIA};

#define EEPROM_SIZE 8
#define HOUR_ADDRESS 0
#define MIN_ADDRESS 4

#include "config.h"             // mqtt credentials

#include "AccelStepper.h"

#include <ESP8266WiFi.h>        // Wifi library
#include <PubSubClient.h>       // MQTT library

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic 

#include <EEPROM.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;               // WiFi
PubSubClient client(wifiClient);     // MQTT

AccelStepper minuteStepper(AccelStepper::FULL4WIRE, D4, D5, D6, D7);
AccelStepper hourStepper(AccelStepper::FULL4WIRE, D0,D1,D2,D3); 


bool isConnected = false;
Supermarket supermarket = ALBERTHEIJN;

// saved data
String albertheijn_data = "";
String coop_data = "";
String plus_data = "";
String spar_data = "";
String boons_data = "";
String hoogvliet_data = "";

void setup() {

   Serial.begin(9600);

    WiFiManager wifiManager;
    WiFiManagerParameter custom_text("<p>(c) 2021 by <a href=\"maito:hoi@joszuijderwijk.nl\">Jos Zuijderwijk</a></p>");
    wifiManager.addParameter(&custom_text);

    if (wifiManager.autoConnect("Bierklok", "")){
      isConnected = true;
    }
  
  // MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);


  // Calibrate
  // Get positions from EEPROM

  int hourPos = 0;
  int minutePos = 0;
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(HOUR_ADDRESS, hourPos);
  EEPROM.get(MIN_ADDRESS, minutePos);
  EEPROM.end();

  hourStepper.setCurrentPosition(hourPos * STEPS_PER_HOUR);
  minuteStepper.setCurrentPosition(minutePos * STEPS_PER_HOUR);

  Serial.print("Hour:");
  Serial.println(hourPos);
  Serial.print("Minute:");
  Serial.println(minutePos);

}

// Try restore the MQTT connection
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(MQTT_CLIENT_NAME, MQTT_USER, MQTT_PASS, "beerclock/connection", 0, 1, "0")) {
      // Stuur Hello World!
      client.publish("beerclock/connection", "1", 1);
      client.subscribe("beerclock/input/#");
      client.subscribe("kratjes/#");
    }
  }
}

// Handle incoming messages
void callback(char* topic, byte* payload, unsigned int len) {
    
    String msg = ""; // payload
    for (int i = 0; i < len; i++) {
      msg += ((char)payload[i]);
    }


  if ( strcmp(topic, "beerclock/input") == 0 ){
    parseInput(msg);
  } else if (strcmp(topic, "beerclock/input/minute") == 0){
    int t = msg.toInt();
    moveMinute(t);
  } else if (strcmp(topic, "beerclock/input/hour") == 0){
    int t = msg.toInt();
    moveHour(t);
  } else if (strcmp(topic, "beerclock/input/supermarket") == 0){
    msg.toLowerCase();

    // set supermarket
    if (msg == "albertheijn")
      supermarket = ALBERTHEIJN;
    else if (msg == "coop")
      supermarket = COOP;
    else if (msg == "plus")
      supermarket = PLUS;
    else if (msg == "spar")
      supermarket = SPAR;
    else if (msg == "boons")
      supermarket = BOONS;
    else if (msg == "hoogvliet")
      supermarket = HOOGVLIET;

    Serial.println(supermarket);

    refresh();
  }else if (strcmp(topic, "beerclock/input/calibrate/hour")==0){
    int t = msg.toInt();
    calibrateHour(t);
  }else if(strcmp(topic, "beerclock/input/calibrate/minute")==0){
    int t = msg.toInt();
    calibrateMinute(t);
 }else if (strcmp(topic, "kratjes/albertheijn") == 0){
    albertheijn_data = msg;
    if (supermarket == ALBERTHEIJN) refresh();
  }else if (strcmp(topic, "kratjes/coop") == 0){
    coop_data = msg;
    if (supermarket == COOP) refresh();
  }else if (strcmp(topic, "kratjes/plus") == 0){
    plus_data = msg;
    if (supermarket == PLUS) refresh();
  }else if (strcmp(topic, "kratjes/spar") == 0){
    spar_data = msg;
    if (supermarket == SPAR) refresh();
  }else if (strcmp(topic, "kratjes/boons") == 0){
    boons_data = msg;
    if (supermarket == BOONS) refresh();
  }else if (strcmp(topic, "kratjes/hoogvliet") == 0){
    hoogvliet_data = msg;
    if (supermarket == HOOGVLIET) refresh();
  }
   
}

// refresh 
void refresh(){
  if (supermarket == ALBERTHEIJN)
    parseInput(albertheijn_data);
  else if (supermarket == COOP)
    parseInput(coop_data);
  else if (supermarket == PLUS)
    parseInput(plus_data);
  else if (supermarket == SPAR)
    parseInput(spar_data);
  else if (supermarket == BOONS)
    parseInput(boons_data);
  else if (supermarket == HOOGVLIET)
    parseInput(hoogvliet_data);
}

// parse specified input, i.e. array of brands to clock positions
void parseInput(String msg){
    if (msg == "") return;

  // get multiple brands
    int hour, min;

    // deserialize JSON
    const size_t CAPACITY = JSON_ARRAY_SIZE(12);
    StaticJsonDocument<CAPACITY> doc;
    deserializeJson(doc, msg);
    JsonArray array = doc.as<JsonArray>();
    int n = array.size();

    Serial.println(msg);

    // save inputs as positions into array
    int input[n];
    int i = 0;
    for(JsonVariant v : array) {
        input[i] = getBeerPos(v.as<String>());
        i++;
    }

    // go through priorities, to sort the input according to those
    int found = 0;
    for (int p : priorities){
      for (int x : input){
        // found match
        if (x == p){
          if (found == 0){
            hour = x;
            if (n == 1) min = x;
          } 
          else if (found == 1){

            min = x;
          }
          found++;
        } 
      }
    }

    moveMinute(min);
    moveHour(hour);
}

// Return the associated value
int getBeerPos(String beer){
  beer.toLowerCase();

  if (beer == "hertog jan"){
    return HERTOG_JAN;
  }else if (beer == "grolsch"){
    return GROLSCH;
  }else if (beer == "kornuit"){
    return KORNUIT;
  }else if (beer == "brand"){
    return BRAND;
  }else if (beer == "warsteiner"){
    return WARSTEINER;
  }else if (beer == "bud"){
    return BUD;
  }else if (beer == "heineken"){
    return HEINEKEN;
  }else if (beer == "jupiler"){
    return JUPILER;
  }else if (beer == "alfa"){
    return ALFA;
  }else if (beer == "bavaria"){
    return BAVARIA;
  }else if (beer == "amstel"){
    return AMSTEL;
  }else if (beer == "gulpener"){
    return GULPENER;
  }

  return -1;

}

// Move hour hand
void moveHour(int t){
  hourStepper.moveTo(STEPS_PER_HOUR * (t % 12));
  hourStepper.setMaxSpeed(200);
  hourStepper.setSpeed(200);
  
  // store into EEPROM
  save(HOUR_ADDRESS, t);

  Serial.print("Hour:");
  Serial.println(t);
}

// Move minute hand
void moveMinute(int t){
  minuteStepper.moveTo(STEPS_PER_HOUR * (t % 12));
  minuteStepper.setMaxSpeed(200);
  minuteStepper.setSpeed(200);
  
  // store into EEPROM
  save(MIN_ADDRESS, t);

  Serial.print("Minute:");
  Serial.println(t);
}

// calibrate hour
void calibrateHour(int t){
  hourStepper.move(t % NUM_STEPS);
  hourStepper.setMaxSpeed(200);
  hourStepper.setSpeed(200);

  Serial.print("Calibrate hour:");
  Serial.println(t% NUM_STEPS);
}

// calibrate minute (set to 0)
void calibrateMinute(int t){
  
  minuteStepper.move(t % NUM_STEPS);
  minuteStepper.setMaxSpeed(200);
  minuteStepper.setSpeed(200);

  Serial.print("Calibrate minute:");
  Serial.println(t% NUM_STEPS);
}


// store into EEPROm
void save(int address, int data){
   EEPROM.begin(EEPROM_SIZE);
   EEPROM.put(address, data);
   EEPROM.commit();
   EEPROM.end();
}

void loop() {
    
    hourStepper.runSpeedToPosition();
    minuteStepper.runSpeedToPosition();

    if (!client.connected() && isConnected){
     reconnect();
    }

    client.loop();
}
