// Import Library LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Import Library DHT
#include "DHT.h"
// Inport Line
#include <WiFi.h>
#include <HTTPClient.h>

// Define WIFI
#define SSID        "Wokwi-GUEST"     // บรรทัดที่ 11 ให้ใส่ ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD    ""     // บรรทัดที่ 12 ใส่ รหัส Wifi
#define LINE_TOKEN  "J8zlEXAVHmuWpm0C2gdjyEzNfTqGhBmsMU29XDOeHVT"   // บรรทัดที่ 13 ใส่ รหัส TOKEN ที่ได้มาจากข้างบน
// Define DHT
#define DHTPIN 17     
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// Define Button
#define but 2


//******* Variable 
// For DHT
DHT dht(DHTPIN, DHTTYPE);
// For LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// For LED
int LED16 = 16; // Pin D16
int LED4 = 4;   // Pin  D4
// Count time 150 = 150
int istime = 0;


void setup() {
  Serial.begin(9600);
  // For Clear
  istime = 0;
  // For DHT
  dht.begin();
  // For LCD
  lcd.init();
  lcd.backlight();
  // For LED
  pinMode(LED16, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED16, LOW);
  digitalWrite(LED4, LOW);
  // For Button
  pinMode(but,INPUT); // Pull down
  // For Conenct WIFI


   WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcd.setCursor(0,0);
    lcd.print("   Connecting ..    ");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
   Serial.println(WiFi.localIP());

}

void loop() {
  delay(2000);
if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0,0);
    lcd.print("IP : "+String(WiFi.localIP()));
 if(digitalRead(but) == HIGH){
      //******** Read Temperature
      float t = dht.readTemperature();
       
      if (!isnan(t) ) { //0 = number

          lcd.setCursor(0,1);
          lcd.print("Temp :" + String(t) + " C");
          Serial.println("Temp :" +  String(t) + " C");

          if (t > 50){
            if (istime == 0){
              NotifyLine("ตรวจพบอุณหภูมิมากกว่าที่กำหนด : " + String(t) + " C");
              istime++;
            }else{
              istime++;
              if(istime ==150)istime = 0;
            }
            digitalWrite(LED16, HIGH);
            digitalWrite(LED4, LOW);
          }else{
            digitalWrite(LED16, LOW);
            digitalWrite(LED4, HIGH);
          }

      }else{
           lcd.setCursor(0,1);
           lcd.print("Failed read DHT !");
         Serial.println(F("Failed to read from DHT sensor!"));
      }
 }else{
  // Pressed Test
    lcd.setCursor(0,1);
    lcd.print("   TEST !!!    ");
    NotifyLine(" Test ");
    digitalWrite(LED16, HIGH);
    digitalWrite(LED4, HIGH);
 }
}else{
      lcd.setCursor(0,0);
    lcd.print("Disconnect !!");
    Serial.println(F("Disconnect !!"));
}
}


void NotifyLine(String message) {
  HTTPClient http;

  http.begin("https://notify-api.line.me/api/notify");
  http.addHeader("Authorization", "Bearer " + String(LINE_TOKEN));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String encodedMessage = "message=" + urlEncode(message);
  int httpCode = http.POST(encodedMessage);

  if (httpCode > 0) {
    String response = http.getString();
  } else {
    Serial.println("....");
  }

  http.end();
}

String urlEncode(String value) {
  String encodedValue = "";
  char c;
  for (size_t i = 0; i < value.length(); i++) {
    c = value.charAt(i);
    if (isAlphaNumeric(c)) {
      encodedValue += c;
    } else {
      encodedValue += String('%');
      encodedValue += String(c, HEX);
    }
  }
  return encodedValue;
}

