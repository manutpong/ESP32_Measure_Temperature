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
// #include <HTTPClient.h>
// import MQTT
#include <PubSubClient.h>
// Define WIFI
#define SSID "Private"                                           //  ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD ""                                                  //  รหัส Wifi
//#define LINE_TOKEN "J8zlEXAVHmuWpm0C2gdjyEzNfTqGhBmsMU29XDOeHVT"     // รหัส TOKEN 
//#define LINE_TOKEN_ME "fKoGOjHC4tebUKn07Y2EiW1f5RqphiqjdxLz37EB3hd"  // รหัส Token ทดสอบ send me
// Define MQTT
const char* mqtt_server = "maechanhospital.dyndns.org"; // แทนด้วย IP หรือ hostname ของ MQTT Broker
const int   mqtt_port = 1883;
const char* mqtt_user = "";  // ถ้าไม่มี ไม่ต้องใส่
const char* mqtt_password = "";
const char* mqtt_topic = "room/temp1";
const char* mqtt_topic2 = "room/dashboard";
// Define DHT
#define DHTPIN 17 // get temp
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
// Define Button
#define but 2
//#define but 15
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
int isupdate = 0;
int istimedht = 0;
int istimehr = 0;  // 1 hour
// check status
boolean isover = false;
boolean isoverdht = false;
boolean isstart = false;
// Temperature
float Fixtemp = 0;
// for mqtt
WiFiClient espClient;
PubSubClient mqttClient(espClient);

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
  isupdate = 0;
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
  //pinMode(but, INPUT);  // Pull down
  pinMode(but, INPUT_PULLUP);
  // Initial LCD
  lcd.init();
  lcd.backlight();
  // Initial Conenct WIFI
  //Serial.println();
  clearRow(0);
  lcd.setCursor(0, 0);
  lcd.print(" Connecting ..   ");
  Server.on("/", rootPage);
  if (Portal.begin()) {
    // Step 1 Connect Internet
    //Serial.println("WiFi connected: " + WiFi.localIP().toString());
    clearRow(0);
    lcd.setCursor(0, 0);
    lcd.print("IP:" + WiFi.localIP().toString());

    // Step 2 MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();
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
      if (digitalRead(but) == 0) {
        befor_str = 0;
        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print(" Let's Start     ");
        //Serial.println("       Let's Start       ");
      } else {
        befor_str--;
      }
      clearRow(1);
      lcd.setCursor(0, 1);
      lcd.print("Delay :" + String(befor_str));
      //Serial.println("Delay :" + String(befor_str));
      delay(1000);
      // Delay Befor start
    } else {
      if (digitalRead(but) == 1) { // ไม่ได้กด
    
        //******** Read Temperature
        float t = dht.readTemperature();
    

         
      if (befor_str == 0 && isstart == false) {
        ////NotifyLine("Device 02 : เริ่มต้นตรวจจับอุณหภูมิ !!");
        NotifyMQTT("d1,Device : เริ่มต้นตรวจจับอุณหภูมิ !!");
        isstart = true;
      }

        clearRow(1);
        lcd.setCursor(0, 1);
        lcd.print("Temp :" + String(t) + " C");
        if (!isnan(t))  //0 = number
        {

          istimehr++;  // for noti 1 hour
                       //******** Count 1 HR

        if ( t != Fixtemp ){
          NotifyMQTTDashboard("d1," + String(t));
          Fixtemp = t;
        }else if (isupdate >= 150){
          // ประมาณ 5 นาที
          NotifyMQTTDashboard("d1," + String(t));
          isupdate = 0;
        }
        isupdate++;

        
          if (istimehr >= 1800) {
            istimehr = 0;
            NotifyMQTT("d1,อุณหภูมิที่ตรวจพบในปัจจุบัน : " + String(t) + " C");
//            NotifyMQTT("d1,c," + String(t));
            
          }else if(istimehr == 1) {
              NotifyMQTT("d1,อุณหภูมิที่ตรวจพบในปัจจุบัน : " + String(t) + " C");
//            NotifyMQTT("d1,c," + String(t));
          }
          //********
          if (t > 25) {
            istimedht = 0;
            if (istime == 0) {
             NotifyMQTT("d1,ตรวจพบอุณหภูมิมากกว่าที่กำหนด : " + String(t) + " C");
//              NotifyMQTT("d1,mc," + String(t));
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
              NotifyMQTT("d1, อุณหภูมิกลับมาปกติ : " + String(t) + " C");
//              NotifyMQTT("d1,nc," + String(t));
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
            //Serial.println("Failed to read from DHT sensor!");
            //NotifyLine("Device: ไม่สามารถเชือมต่อกับ Sensor ได้");
            NotifyMQTT("d1,Device : ไม่สามารถเชือมต่อกับ Sensor ได้");
            istimedht++;
          } else {
            clearRow(1);
            lcd.setCursor(0, 1);
            lcd.print("Failed read DHT!");
            //Serial.println("Failed to read from DHT sensor!");
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
        //NotifyLine("Device 02 :  Test ");
        NotifyMQTT("d1,Device :  Test ");
        digitalWrite(LED16, HIGH);
        digitalWrite(LED4, HIGH);
      }
    }
  } else {
    clearRow(0);
    clearRow(1);
    lcd.setCursor(0, 0);
    lcd.print("Disconnect !!");
    //Serial.println("Disconnect !!");
    WiFi.disconnect();
    WiFi.reconnect();
    //Serial.println("Reconnect !!");
    isstart = true;
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client", mqtt_user, mqtt_password)) {
      //Serial.println("connected");
    } else {
      clearRow(0);
      clearRow(1);
      lcd.setCursor(0, 0);
      lcd.print("MQTT Failed!!");
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      //Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void NotifyMQTT(String message) {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  mqttClient.publish(mqtt_topic, message.c_str());
}

void NotifyMQTTDashboard(String message) {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  mqttClient.publish(mqtt_topic2, message.c_str());
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
