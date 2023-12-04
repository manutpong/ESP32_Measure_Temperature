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
#define SSID "Wokwi-GUEST"                                        // บรรทัดที่ 11 ให้ใส่ ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD ""                                               // บรรทัดที่ 12 ใส่ รหัส Wifi
#define LINE_TOKEN "J8zlEXAVHmuWpm0C2gdjyEzNfTqGhBmsMU29XDOeHVT"  // บรรทัดที่ 13 ใส่ รหัส TOKEN ที่ได้มาจากข้างบน
// Define DHT
#define DHTPIN 17
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
// Define Button
#define but 2




//******* Variable
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
  isover = false;
  istimedht = 0;
  isoverdht = false;
  isstart = false;
  // For DHT
  dht.begin();

  // For LED
  pinMode(LED16, OUTPUT);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED16, LOW);
  digitalWrite(LED4, LOW);
  // For Button
  pinMode(but, INPUT);  // Pull down
  // For LCD
  lcd.init();
  lcd.backlight();
  // For Conenct WIFI
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
  delay(2000);
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
        NotifyLine("เริ่มต้นตรวจจับอุณหภูมิ !!");
      }else{
        befor_str--;
      }
      clearRow(1);
      lcd.setCursor(0, 1);
      lcd.print("Delay :" + String(befor_str));
      Serial.println("Delay :" + String(befor_str));
      delay(1000);

      // Delay Befor start
    } else {
      
      if (befor_str == 0 && isstart == false) {
        NotifyLine("เริ่มต้นตรวจจับอุณหภูมิ !!");
        isstart = true;
      }
      if (digitalRead(but) == HIGH) {
        //******** Read Temperature
        float t = dht.readTemperature();
        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print("Temp :" + String(t) + " C");
        Serial.println("Temp :" + String(t) + " C");
        Serial.println("istime" + String(istime));
        if (!isnan(t))  //0 = number
        {
          if (t > 25) {
            istimedht = 0;
            if (istime == 0) {
              NotifyLine("ตรวจพบอุณหภูมิมากกว่าที่กำหนด : " + String(t) + " C");
              istime++;
            } else {
              istime++;
              if (istime == 150) istime = 0;
            }
            isover = true;
            digitalWrite(LED16, LOW);  //red
            digitalWrite(LED4, HIGH);  //blue
          } else {
            if (isover) {
              NotifyLine("อุณหภูมิกลับมาปกติ : " + String(t) + " C");
              isover = false;
              istime = 0;
            }
            digitalWrite(LED16, HIGH);  //red
            digitalWrite(LED4, LOW);    //blue
          }
        } else {
             if (istimedht == 0) {
                clearRow(1);
                lcd.setCursor(0, 1);
                lcd.print("Failed read DHT!");
                Serial.println("Failed to read from DHT sensor!");
                NotifyLine("ไม่สามารถเชือมต่อกับ Sensor ได้");
                istimedht++;
            } else {
              istimedht++;
              if (istimedht == 150) istimedht = 0;
            }
            
            digitalWrite(LED16, HIGH);  //red
            digitalWrite(LED4, LOW);    //blue
        }
      } else {
        // Pressed Test
        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print("   TEST !!!  ");
        NotifyLine(" Test ");
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
