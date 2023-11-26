#include <WiFi.h>          
#include <WebServer.h>     
#include <AutoConnect.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
        
LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer Server;          
bool first = false;

AutoConnect      Portal(Server);

void rootPage() {
  char content[] = "T";
  Server.send(200, "text/plain", content);
}
void lcdinit(){
  lcd.init();
  lcd.backlight();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("WiFi connected: " + WiFi.localIP().toString());
  Server.on("/", rootPage);
  if (Portal.begin()) {
    // Step 1 Connect Internet
    first = false;
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
}

void loop() {
    Portal.handleClient();
    if (!first){
      lcdinit();
       first = true;
    }
    Serial.println("Re.");
    delay(1000);
     if (WiFi.status() != WL_CONNECTED) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Reconnecting to WiFi...");
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();

    }else{
        Serial.println("Internet Connected");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("IP :" + WiFi.localIP().toString());
    }
}
