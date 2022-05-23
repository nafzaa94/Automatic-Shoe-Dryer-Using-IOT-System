#define BLYNK_TEMPLATE_ID "TMPLGiWBaUZ9"
#define BLYNK_DEVICE_NAME "project kering kasut"
#define BLYNK_AUTH_TOKEN "vmauzuHZVMtg6iF8O3ZUXMQ7Bcp38qYo"

#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// or Software Serial on Uno, Nano...
// #include <SoftwareSerial.h>
// SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

BlynkTimer timer;

//DHT sensor 
#include "DHT.h"

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  
#define DHTPIN 8 

DHT dht(DHTPIN, DHTTYPE);


//RTC timer
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

// lcd 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int ButtonManualOn = 9;
int ButtonManualOff = 10;

int ValueButtonManualOn = 0;
int ValueButtonManualOff = 0;

int ValuePowerOnOff = 0;

int datastart = 0;
int Var = 0;

int relaylamp1 = 2;
int relaylamp2 = 3;
int relaylamp3 = 4;
int relaykipas = 5;
int signalmotor1 = 6;
int signalmotor2 = 7;

float hum = 0;
float temp = 0;

int statetimer = 0;

int SetTimer = 50;

int Hour = 0;
int Min = 0;

int HourOff = 0;
int MinOff = 0;

int TimerDisplay = 0;

BLYNK_WRITE(V0)
{ 
  ValuePowerOnOff = param.asInt();

  if (ValuePowerOnOff == 1){
    datastart = 1;
    } 
}

void myTimerEvent(){
  tmElements_t tm;

  RTC.read(tm);

  ValueButtonManualOn = digitalRead(ButtonManualOn);
  ValueButtonManualOff = digitalRead(ButtonManualOff);

  if (ValueButtonManualOn == LOW){
    datastart = 1;
    digitalWrite(relaylamp2, LOW);
    digitalWrite(relaylamp3, HIGH);
    }

  if (ValueButtonManualOff == LOW){
    datastart = 0;
    digitalWrite(relaylamp2, HIGH);
    digitalWrite(relaylamp3, LOW);
    }

  if (datastart == 1){
    Var = 1;
    }
  else if (datastart == 0){
    Var = 0;
    }

  hum = dht.readHumidity();
  temp = dht.readTemperature();

  switch (Var) {
    case 1:
      digitalWrite(relaylamp1, LOW);
      digitalWrite(relaykipas, LOW);
      digitalWrite(signalmotor1, HIGH);
      digitalWrite(signalmotor2, LOW);
      Var = 2;
      break;
    case 2:
      Hour = tm.Hour;
      Min =  tm.Minute; 
      
      if (statetimer == 0){
        HourOff = Hour;
        MinOff = Min + SetTimer;
  
        if (MinOff >= 60){
          HourOff = HourOff + 1;
          MinOff = MinOff - 60;
          }
        statetimer = 1;
      }

      TimerDisplay = (HourOff * 60 + MinOff) - (Hour * 60 + Min);
      
      lcd.setCursor(0, 9);
      lcd.print("     ");
      lcd.setCursor(0, 9);
      lcd.print(temp);

      lcd.setCursor(1, 9);
      lcd.print("     ");
      lcd.setCursor(1, 9);
      lcd.print(TimerDisplay);
      

      if (Hour == HourOff && Min == MinOff){
        digitalWrite(relaylamp1, HIGH);
        digitalWrite(relaykipas, HIGH);
        digitalWrite(signalmotor1, LOW);
        digitalWrite(signalmotor2, LOW);
        Blynk.logEvent("notification", "Sudah siap....");
        Var = 3;
        }
      break;
    case 3:
      statetimer = 0;
      Blynk.virtualWrite(V0, 0);
      datastart = 0;
      Var = 0;
      break;
  }
  
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(relaylamp1, OUTPUT);
  pinMode(relaylamp2, OUTPUT);
  pinMode(relaylamp3, OUTPUT);
  pinMode(relaykipas, OUTPUT);
  pinMode(signalmotor1, OUTPUT);
  pinMode(signalmotor2, OUTPUT);
  pinMode(ButtonManualOn, INPUT_PULLUP);
  pinMode(ButtonManualOff, INPUT_PULLUP);

  digitalWrite(relaylamp1, HIGH);
  digitalWrite(relaylamp2, HIGH);
  digitalWrite(relaylamp3, HIGH);
  digitalWrite(relaykipas, HIGH);

  dht.begin();

  lcd.begin(); // kalu error tukar begin jadi init()
  lcd.backlight();

  lcd.setCursor(0, 1);
  lcd.print("Temp  = ");
  lcd.setCursor(1, 1);
  lcd.print("Timer = ");
  
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
}
