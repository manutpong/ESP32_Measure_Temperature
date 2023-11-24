# ESP32Autoconnect01
1. Load Library Autoconnect
2. Default Code (WIFI Password default 12345678)
            ที่มา = https://meetjoeblog.com/2021/05/04/autoconnect-wifi-manager/
-------------------------------------------
      #include <WiFi.h>          
      #include <WebServer.h>     
      #include <AutoConnect.h>
      WebServer Server;          
      AutoConnect      Portal(Server);
      
      void rootPage() {
        char content[] = "Hello, world";
        Server.send(200, "text/plain", content);
      }
      void setup() {
        delay(1000);
        Serial.begin(115200);
        Serial.println();
      
        Server.on("/", rootPage);
        if (Portal.begin()) {
          Serial.println("WiFi connected: " + WiFi.localIP().toString());
        }
      }
      void loop() {
          Portal.handleClient();
      }
-----------------------------------------------
3.ติดตั้ง DHT11 for ESP32
![image](https://github.com/manutpong/ESP32Autoconnect01/assets/4971040/272ce1e7-50de-4f02-bf68-1629af1b0a57)
4.ติดตั้ง Interfacing I2C LCD with ESP32 | ESP32 I2C LCD Tutorial
link = https://www.electronicshub.org/esp32-i2c-lcd/
            4.1 ติดตั้ง LiquidCrystal I2C
            4.2 ให้ทำการ Finding the Slave Address of I2C LCD Module
            4.3 เขียนคำสั่งแสดง             /
-----------------------------------------------
            #include <Wire.h> 
            #include <LiquidCrystal_I2C.h>
            LiquidCrystal_I2C lcd(0x3F, 16, 2);
            void setup()
            {
              lcd.init();
              lcd.backlight();
              lcd.setCursor(0,0);
              lcd.print("  I2C LCD with ");
              lcd.setCursor(0,1);
              lcd.print("  ESP32 DevKit ");
              //delay(2000);
            }
            void loop()
            {
            }
-----------------------------------------------

