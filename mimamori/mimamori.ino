#include <ESP8266WiFi.h>
#include "private.h"

#define SW  0
#define LED  14
#define MOTION  16
#define SOUND  A0

int _interval = 0;
int _motion = 0;
int _sound = 0;
unsigned long _time = 0;
WiFiClient _client;

void setup() {
  //delay(5000);
  Serial.begin(9600);
  pinMode(SW, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(MOTION, INPUT);
  pinMode(SOUND, INPUT);

  // Lチカ10秒
  for (int i = 0; i < 5; i++) {  
    Serial.println("loop");
    digitalWrite(LED, 1);
    delay(1000);
    digitalWrite(LED, 0);
    delay(1000);
  }
  // スイッチが押されていたら１分間隔で送信
  int sw = digitalRead(SW);
  if (sw == 0) {
    _interval = 60 * 1000;
    Serial.println("debug mode");
  } else {
    _interval = 60 * 60 * 1000;
    Serial.println("normal mode");
  }
  Serial.print("dbpath : ");
  Serial.println(PATH);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed. Rebooting...");
    delay(5000);
    ESP.restart();
  }
  delay(2000);
  _time = millis();
}

void send() {
  if (_client.connect(SERVER, 80)) {
    Serial.print("Connect ");
    Serial.println(SERVER);
    String data = "{\"motion\":" + String(_motion) + ", \"sound\":" + String(_sound) + ", \"millis\":" + String(millis()) + "}";
    Serial.println(data);
    _client.print("POST ");
    _client.print(PATH);
    _client.print(" HTTP/1.1\n");
    _client.print("Host: ");
    _client.print(SERVER);
    _client.print("\n");
    //_client.print("Host: api.thingspeak.com\n");
    //_client.print("Connection: close\n");
    _client.print("Content-Type: application/json\n");
    _client.print("Content-Length: ");
    _client.print(data.length());
    _client.print("\n\n");
    _client.print(data);
    //_client.stop();
    int timeout = millis() + 5000;
    while (_client.available() == 0) {
      if (timeout - millis() < 0) {
        Serial.println(">>> Client Timeout !");
        _client.stop();
        return;
      }
    }
    while(_client.available()){
      String line = _client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
    _client.stop();
  } else {
    Serial.println("Connection failed.");
    delay(5000);
    ESP.restart();
  }
}

int getMotion(){
  int motion = digitalRead(MOTION);
  delay(25);
  motion += digitalRead(MOTION);
  delay(25);
  motion += digitalRead(MOTION);
  if (motion >= 2) {
    return 1;
  }
  return 0;
}
int getSound(){
  int sound1 = analogRead(SOUND);
  delay(25);
  int sound2 = analogRead(SOUND);
  delay(25);
  int sound3 = analogRead(SOUND);
  int ret = sound1 > sound1 ? sound1 : sound2; // 大きいのとって
  ret = ret < sound3 ? ret : sound3;           // 小さいのをとる
  return ret;
}

void loop() {
  long time = millis();
//  int motion = digitalRead(MOTION);//getMotion();
  int motion = getMotion();
  digitalWrite(LED, motion); // 人感センサONでLEDをON
  int sound = getSound();
  Serial.print(time);
  Serial.print(" motion:");
  Serial.print(motion);
  Serial.print(" sound:");
  Serial.println(sound);
  _motion += motion;  // 人感センサはONの累計
  _sound = sound > _sound ? sound : _sound; // 音量は最大値
  int sw = digitalRead(SW);
  //Serial.print("sw:");
  //Serial.println(sw);
  if (sw == 0) {  // スイッチが押されていたら送信
    //ESP.restart();
    _time = 0;
  }
  //Serial.print(_time);
  //Serial.print(" + ");
  //Serial.print(_interval);
  //Serial.print(" < ");
  //Serial.println(time);
  if (_time + _interval < time || _time == 0 || _time > time) {
    Serial.print("send motion:");
    Serial.print(_motion);
    Serial.print(" sound:");
    Serial.println(_sound);
    send();                     // 送信
    _time = time;
    _motion = 0;
    _sound = 0;
  }
  delay(900);
}
