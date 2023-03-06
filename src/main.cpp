#include <M5StickCPlus.h>
#include <TinyGPS++.h>
#include <WiFi.h>

#include <ArduinoJson.h>
#include <HTTPClient.h>



// Wifi udaje
const char* ssid = "name";         //nazev wifi
const char* password = "passwd";  //heslo wifi

//server address
String serverName = "https://192.168.0.135:443/Rest/PostJSON";

// The TinyGPS++ object
TinyGPSPlus gps;

//http klient
HTTPClient http;

// The serial connection to the GPS device
HardwareSerial Uart = HardwareSerial(2);


static void smartDelay(unsigned long ms);

void setup() {

  //Pozadi a text na displeji
  M5.begin();
  M5.Lcd.fillScreen(TFT_PURPLE);
  M5.Lcd.setTextColor(TFT_GREEN,TFT_BLACK);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);

  //Pripojeni k WIFI
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED)
    


  // Serial connection on baud rate 115200
  Serial.begin(115200);
  
  // GPS connection on baud rate 9600
  // PIN 33 = TX of AT6558 GPS 
  // PIN 32 = RX of AT6558 GPS
  Uart.begin(9600, SERIAL_8N1, 33, 32);
}

void loop() {

  // Displej data
  M5.Lcd.setCursor(80, 15);
  M5.Lcd.println("GPS Tracker");

  M5.Lcd.setCursor(30, 50);
  M5.Lcd.printf("Lng: %5.5f", gps.location.lng());

  M5.Lcd.setCursor(30, 70); 
  M5.Lcd.printf("Lat: %5.5f",  gps.location.lat());

  M5.Lcd.setCursor(30, 90); 
  M5.Lcd.printf("Sat: %%%i", gps.satellites.value());

  DynamicJsonDocument doc(1024);
  doc["lng"] = gps.location.lng();
  doc["lat"] = gps.location.lat();

  String dataToPost;
  serializeJson(doc, dataToPost);

  http.begin(serverName.c_str());
  http.addHeader("Content-Type", "application/json");

  int httpResponse = http.POST(dataToPost);

  Serial.println(httpResponse);

  smartDelay(5000);
}
//Update informaci na displeji
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Uart.available())
      gps.encode(Uart.read());

  } while (millis() - start < ms);
}
