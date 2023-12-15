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
#define SSID "Private"                                           //  ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD ""                                                  //  รหัส Wifi
#define LINE_TOKEN "J8zlEXAVHmuWpm0C2gdjyEzNfTqGhBmsMU29XDOeHVT"     // รหัส TOKEN 
#define LINE_TOKEN_ME "fKoGOjHC4tebUKn07Y2EiW1f5RqphiqjdxLz37EB3hd"  // รหัส Token ทดสอบ send me
// Define DHT
#define DHTPIN 17
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
// Define Button
#define but 2
//******* Variable*************************************
// Autoconnect
WebServer Server;
AutoConnect Portal(Server);
// For DHT
DHT dht(DHTPIN, DHTTYPE);
// For LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// For LED
int LED16 = 16;  // Pin D16
int LED4 = 4;    // Pin  D4
// Count time 150 = 150
int befor_str = 300;
int istime = 0;
int istimedht = 0;
int istimehr = 0;  // 1 hour
// check status
boolean isover = false;
boolean isoverdht = false;
boolean isstart = false;


void rootPage() {
  char content[] = "T";
  Server.send(200, "text/plain", content);
}

void clearRow(byte rowToClear) {
  lcd.setCursor(0, rowToClear);
  lcd.print("                ");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  // For Clear
  befor_str = 300;
  istime = 0;
  istimehr = 0;
  isover = false;
  istimedht = 0;
  isoverdht = false;
  isstart = false;
  // Initial DHT
  dht.begin();
  // Initial LED
  pinMode(LED16, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED16, LOW);
  digitalWrite(LED4, LOW);
  // Initial Button
  pinMode(but, INPUT);  // Pull down
  // Initial LCD
  lcd.init();
  lcd.backlight();
  // Initial Conenct WIFI
  Serial.println();
  clearRow(0);
  lcd.setCursor(0, 0);
  lcd.print(" Connecting ..   ");
  Server.on("/", rootPage);
  if (Portal.begin()) {
    // Step 1 Connect Internet
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    clearRow(0);
    lcd.setCursor(0, 0);
    lcd.print("IP:" + WiFi.localIP().toString());
  }
}

void loop() {
  Portal.handleClient();
  delay(2000); // DHT22 Support Delay 2 sec
  if (WiFi.status() == WL_CONNECTED) {
    clearRow(0);
    lcd.setCursor(0, 0);
    lcd.print("IP:" + WiFi.localIP().toString());
    // Delay befor start*****************
    if (befor_str > 0) {
      if (digitalRead(but) == LOW) {
        befor_str = 0;
        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print(" Let's Start     ");
        Serial.println("       Let's Start       ");
      } else {
        befor_str--;
      }
      clearRow(1);
      lcd.setCursor(0, 1);
      lcd.print("Delay :" + String(befor_str));
      Serial.println("Delay :" + String(befor_str));
      delay(1000);
      // Delay Befor start
    } else {
      if (digitalRead(but) == HIGH) {
        //******** Read Temperature
        float t = dht.readTemperature();

      if (befor_str == 0 && isstart == false) {
        NotifyLine("Device 02 : เริ่มต้นตรวจจับอุณหภูมิ !!");
        isstart = true;
      }

        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print("Temp :" + String(t) + " C");
        Serial.println("Temp :" + String(t) + " C");
        Serial.println("istime" + String(istime));
        if (!isnan(t))  //0 = number
        {
          istimehr++;  // for noti 1 hour
                       //******** Count 1 HR

          if (istimehr >= 1800) {
            istimehr = 0;
            NotifyLine("Device 02 : อุณหภูมิที่ตรวจพบในปัจจุบัน : " + String(t) + " C");
          }else if(istimehr == 1) {
            NotifyLine("Device 02 : อุณหภูมิที่ตรวจพบในปัจจุบัน : " + String(t) + " C");
          }
          //********
          if (t > 25) {
            istimedht = 0;
            if (istime == 0) {
              NotifyLine("Device 02 : ตรวจพบอุณหภูมิมากกว่าที่กำหนด : " + String(t) + " C");
              istime++;
            } else {
              istime++;
              if (istime == 75) istime = 0;
            }
            isover = true;
            digitalWrite(LED16, HIGH);  //red
            digitalWrite(LED4, LOW);    //blue
          } else {
            if (isover) {
              NotifyLine("Device 02 : อุณหภูมิกลับมาปกติ : " + String(t) + " C");
              isover = false;
              istime = 0;
            }
            digitalWrite(LED16, LOW);  //red
            digitalWrite(LED4, HIGH);  //blue
          }
        } else {
          if (istimedht == 0) {
            clearRow(1);
            lcd.setCursor(0, 1);
            lcd.print("Failed read DHT!");
            Serial.println("Failed to read from DHT sensor!");
            NotifyLine("Device 02 : ไม่สามารถเชือมต่อกับ Sensor ได้");
            istimedht++;
          } else {
            clearRow(1);
            lcd.setCursor(0, 1);
            lcd.print("Failed read DHT!");
            Serial.println("Failed to read from DHT sensor!");
            istimedht++;
            if (istimedht == 75) istimedht = 0;
          }
          digitalWrite(LED16, HIGH);  //red
          digitalWrite(LED4, LOW);    //blue
        }
      } else {
        // Pressed Test
        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print("   TEST !!!  ");
        NotifyLine("Device 02 :  Test ");
        digitalWrite(LED16, HIGH);
        digitalWrite(LED4, HIGH);
      }
    }
  } else {
    clearRow(0);
    clearRow(1);
    lcd.setCursor(0, 0);
    lcd.print("Disconnect !!");
    Serial.println("Disconnect !!");
    WiFi.disconnect();
    WiFi.reconnect();
    Serial.println("Reconnect !!");
    isstart = true;
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
    Serial.println(response);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Disconnect Line Server !!");
    Serial.println("Cannect not connect Line Server");
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
