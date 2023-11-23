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
