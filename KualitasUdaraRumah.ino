#include <Arduino.h>
#include <SimpleDHT.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Konfigurasi Blynk
#define BLYNK_TEMPLATE_ID "TMPLvslh7XCZ"
#define BLYNK_DEVICE_NAME "KualitasUdara"
#define BLYNK_AUTH_TOKEN "vybp8et9Mnkawk0FRWa8S3QtyI5LxQ_M"
#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Kecap sedap";
char pass[] = "22oscar22";

BlynkTimer timer;

//Konfigurasi Pin pada board nodemcu
#define mq2 A0
#define LEDMerah D2
#define LEDHijau D3
#define LEDBiru D4
#define buzzer 12
DHT dht(D1, DHT11);

int GasSmoke = A0; //Sensor MQ-2
int dht11 = D1; // Sensor DHT11

void setup(){
  Serial.begin(115200);
  pinMode(LEDMerah, OUTPUT);
  pinMode(LEDHijau, OUTPUT);
  pinMode(LEDBiru, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(GasSmoke, INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, polusi);
  dht.begin();
}

BLYNK_WRITE(V0){
  GasSmoke = param.asInt();
}

void polusi(){
  int sensor = analogRead(GasSmoke);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  sensor = map(sensor, 0, 1024, 0, 1024);
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(3000);
    return;
  }
  if (t <= 33 && t >= 21){
    if (h >=60 && h <= 100){
      if (sensor <= 260){
        
        //Menampilkan peringatan LED fisik
        digitalWrite(LEDHijau, HIGH);
        digitalWrite(LEDMerah, LOW);
        digitalWrite(LEDBiru, LOW);
        
        //Menampilkan peringatan LED pada Blynk
        WidgetLED LEDHijau(V4);
        LEDHijau.on();
        WidgetLED LEDBiru(V5);
        LEDBiru.off();
        WidgetLED LEDMerah(V3);
        LEDMerah.off();
        
        //Mengirim notifikasi melalui event Blynk
        Blynk.logEvent("aman_alert");
        Serial.print("Status : NORMAL -> ");
      }
    }
  }
  else if (t <= 36 && t >= 34 ){
    if (h >=45 && h <=59){
      if (sensor >=261 && sensor <= 700){
        
        //Menampilkan peringatan LED fisik
        digitalWrite(LEDBiru, HIGH);
        digitalWrite(LEDMerah, LOW);
        digitalWrite(LEDHijau, LOW);

        //Menampilkan peringatan LED pada Blynk
        WidgetLED LEDBiru(V5);
        LEDBiru.on();
        WidgetLED LEDHijau(V4);
        LEDHijau.off();
        WidgetLED LEDMerah(V3);
        LEDMerah.off();

        //Mengirim notifikasi melalui event Blynk
        Blynk.logEvent("waspada_alert");
        Serial.print("Status : WASPADA -> ");
      }
    }
  }
  else if (t >= 37){
    if (h <= 44 && h >= 1){
      if (sensor >= 701 && sensor <= 1024){
        //Menampilkan peringatan LED fisik
        digitalWrite(LEDMerah, HIGH);
        digitalWrite(LEDBiru, LOW);
        digitalWrite(LEDHijau, LOW);
  
        //Menampilkan peringatan LED pada Blynk
        WidgetLED LEDMerah(V3);
        LEDMerah.on();
        WidgetLED LEDHijau(V4);
        LEDHijau.off();
        WidgetLED LEDBiru(V5);
        LEDBiru.off();
  
        //Mengirim notifikasi melalui event Blynk
        Blynk.logEvent("bahaya_alert");
        Serial.print("Status : BAHAYA -> ");
      }
    }  
  } 

  //Menampilkan output pada serial monitor
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.print("C, ");
  Serial.print("Kelembaban: ");
  Serial.print(h);
  Serial.print("H, ");
  Serial.print("Polusi : ");
  Serial.println(sensor);

  //Konfigurasi virtual pin pada Blynk
  Blynk.virtualWrite(V0, sensor);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);

  delay(1000);
}

void loop(){
  Blynk.run();
  timer.run();
}
