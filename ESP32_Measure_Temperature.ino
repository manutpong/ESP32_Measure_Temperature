// Import Library LCD
#include <Wire.h>   
#include <LiquidCrystal_I2C.h>
// Import Library DHT
#include "DHT.h"
// Import Auto connect
#include <WebServer.h>     
#include <AutoConnect.h>
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
// Autoconnect
WebServer Server;          
AutoConnect  Portal(Server);
// For DHT
DHT dht(DHTPIN, DHTTYPE);
// For LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// For LED
int LED16 = 16; // Pin D16
int LED4 = 4;   // Pin  D4
// Count time 150 = 150
int istime = 0;
// check status
boolean isover= false;

void rootPage() {
  char content[] = "T";
  Server.send(200, "text/plain", content);
}


void setup() {
  Serial.begin(115200);
  delay(1000);
   // For Clear
  istime = 0;
  isover = false;
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
  Serial.println();
  lcd.setCursor(0,0);
  lcd.print("   Connecting ..    ");
  Server.on("/", rootPage);
  if (Portal.begin()) {
    // Step 1 Connect Internet
  Serial.println("WiFi connected: " + WiFi.localIP().toString());
  lcd.setCursor(0,0);
  lcd.print("IP:"+WiFi.localIP().toString());
  }
}

void loop() {
    Portal.handleClient();
    delay(2000);
    if (WiFi.status() == WL_CONNECTED) {
          lcd.setCursor(0,0);
          lcd.print("IP:"+WiFi.localIP().toString());
            if(digitalRead(but) == LOW){
               //******** Read Temperature
                float t = dht.readTemperature();
                //stest ****************************
                t = 52.0;
                if(istime == 50) t = 34.00;
                 //stest ****************************
                  lcd.setCursor(0,1);
                  lcd.print("Temp :" + String(t) + " C");
                  Serial.println("Temp :" +  String(t) + " C");
                  Serial.println("istime"+String(istime));
        if (!isnan(t) ) //0 = number
        { 
          if (t > 50)
          {
                  if (istime == 0)
                  {
                      NotifyLine("ตรวจพบอุณหภูมิมากกว่าที่กำหนด : " + String(t) + " C");
                      istime++;
                  }
                  else
                  {
                      istime++;
                      if(istime ==150)istime = 0;
                  }
                        isover = true;
                          digitalWrite(LED16, HIGH);
                          digitalWrite(LED4, LOW);
          }
          else
          {          
            if(isover)
            {
                  NotifyLine("อุณหภูมิกลับมาปกติ : " + String(t) + " C");
                  isover = false;
            }
                  digitalWrite(LED16, LOW);
                  digitalWrite(LED4, HIGH);
          }
        }
        else
        {
           lcd.setCursor(0,1);
           lcd.print("Failed read DHT !");
           Serial.println("Failed to read from DHT sensor!");
        }
    }
    else
    {
        // Pressed Test
          lcd.setCursor(0,1);
          lcd.print("   TEST !!!    ");
          NotifyLine(" Test ");
          digitalWrite(LED16, HIGH);
          digitalWrite(LED4, HIGH);
    }
  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print("Disconnect !!");
    Serial.println("Disconnect !!");
    WiFi.disconnect();
    WiFi.reconnect();
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
  for (size_t i = 0;   i < value.length(); i++) {
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



























