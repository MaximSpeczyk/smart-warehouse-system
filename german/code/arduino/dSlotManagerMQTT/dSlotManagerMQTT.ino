#include <LiquidCrystal_I2C.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "passwordMaxim.h"

#define green 7   //derzeit nicht verbunden
#define yellow 6  //derzeit nicht verbunden
#define blue 5    //derzeit nicht verbunden

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.63.200";
int port = 1883;
const char topicTest[] = "QRCODE";
const char topicSlot1[] = "SLOT/1";
const char topicSlot2[] = "SLOT/2";
const char topicSlot3[] = "SLOT/3";

LiquidCrystal_I2C lcd_1(0x27, 16, 2);

String strings[3] = {"", "", ""}; // Anfangszustand: leer
bool updated = false;

void setup() 
{
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(blue, OUTPUT);

  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, LOW);

  Serial.begin(9600);
  while (!Serial) { ; }

  // Verbindung mit WLAN herstellen
  Serial.print("Verbinde mit WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Mit Netzwerk verbunden");

  // Verbindung zum MQTT-Broker herstellen
  Serial.print("Verbinde mit MQTT-Broker: ");
  Serial.println(broker);
  if (!mqttClient.connect(broker, port)) 
  {
    Serial.print("MQTT-Verbindung fehlgeschlagen! Fehlercode = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("Mit MQTT-Broker verbunden");

  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topicTest);

  lcd_1.init();
  lcd_1.backlight();
  Serial.println("Fertig");

  lcd_1.setCursor(0, 0);
  lcd_1.print("Slo1  Slo2  Slo3");

  publishSlots(); // Anfangszustand veröffentlichen
}

void updateLEDs() 
{
  digitalWrite(green, strings[0] != "" ? HIGH : LOW);
  digitalWrite(yellow, strings[1] != "" ? HIGH : LOW);
  digitalWrite(blue, strings[2] != "" ? HIGH : LOW);
}

void updateLCD() 
{
  lcd_1.setCursor(0, 1);
  lcd_1.print("                "); // Zweite Zeile löschen

  lcd_1.setCursor(0, 1);
  lcd_1.print(strings[0]);

  lcd_1.setCursor(6, 1);
  lcd_1.print(strings[1]);

  lcd_1.setCursor(12, 1);
  lcd_1.print(strings[2]);
}

void showMessage(const String& message) 
{
  lcd_1.setCursor(0, 0);
  lcd_1.print("                ");  // Erste Zeile löschen (16 Leerzeichen)
  lcd_1.setCursor(0, 0);
  lcd_1.print(message);
  delay(2000); // 2 Sekunden warten
  lcd_1.setCursor(0, 0);
  lcd_1.print("                ");  // Erneut löschen
  lcd_1.setCursor(0, 0);
  lcd_1.print("Slo1  Slo2  Slo3");
}

void publishSlots() 
{
  mqttClient.beginMessage(topicSlot1);
  mqttClient.print(strings[0]);
  mqttClient.endMessage();

  mqttClient.beginMessage(topicSlot2);
  mqttClient.print(strings[1]);
  mqttClient.endMessage();

  mqttClient.beginMessage(topicSlot3);
  mqttClient.print(strings[2]);
  mqttClient.endMessage();
}

void onMqttMessage(int messageSize) 
{
  String message = "";
  while (mqttClient.available()) 
  {
    char c = (char)mqttClient.read();
    message += c;
  }
  message.trim();
  Serial.print("Empfangen: ");
  Serial.println(message);

  // Löschbefehle verarbeiten
  if (message == "del1") 
  {
    strings[0] = "";
    updated = true;
  } 
  else if (message == "del2") 
  {
    strings[1] = "";
    updated = true;
  } 
  else if (message == "del3") 
  {
    strings[2] = "";
    updated = true;
  } 
  else 
  {
    if (message.length() > 4) 
    {
      message = message.substring(0, 4);
    }

    // Auf Duplikate prüfen
    bool duplicate = false;
    for (int i = 0; i < 3; i++) 
    {
      if (strings[i] == message) 
      {
        duplicate = true;
        break;
      }
    }

    if (duplicate) 
    {
      showMessage("Schon vorhanden");
    } 
    else 
    {
      bool added = false;
      for (int i = 0; i < 3; i++) 
      {
        if (strings[i] == "") 
        {
          strings[i] = message;
          updated = true;
          showMessage("Zu Slot " + String(i + 1) + " hinzu.");
          added = true;
          break;
        }
      }

      if (!added) 
      {
        showMessage("Alle Slots voll");
      }
    }
  }

  updateLCD();
  updateLEDs();
  publishSlots();
}

void loop() 
{
  mqttClient.poll();  // Notwendig, um MQTT-Nachrichten zu empfangen
}
