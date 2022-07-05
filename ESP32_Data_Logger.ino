#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

/*  
 *  Pinを宣言
 *  GPIO0 A11 Serial系   ADC2
 *  GPIO2 A12 Serial系   ADC2
 *  GPIO4 A10            ADC2
 *  GPIO12 A15 Serial系  ADC2
 *  GPIO13 A14           ADC2
 *  GPIO14 A16           ADC2
 *  GPIO15 A13 Serial系  ADC2
 *  GPIO25 A18           ADC2
 *  GPIO26 A19           ADC2
 *  GPIO27 A17           ADC2
 *  
 *  GPIO32 A4            ADC1
 *  GPIO33 A5            ADC1
 *  GPIO34 A6            ADC1
 *  GPIO35 A7            ADC1
 *  GPIO36 A0            ADC1
 *  GPIO39 A3            ADC1
 *  
 *  "Serial系"はコンパイル時にPinを接続していない状態にする必要がある
 *  "ADC2"はWiFi使用時に使えない
 *  "ADC1"はWiFi使用時に使える
 */

// Wifiで使う変数の初期化
char ssid[] = "ESP32APWIFI";
char pass[] = "esp32apwifi";
const IPAddress ip(192,168,11, 2);
const IPAddress subnet(255, 255, 255, 0);

// サーバーを初期化
AsyncWebServer webServer(80);

// センサーからデータ取得
String readSensorData() {
  int sensorData = analogRead(A4);
  if (isnan(sensorData)) {
    Serial.println("正しく数値が取れていません");
    return "";
  } else {
    return String(sensorData);
  }
}

void setup() {
  // シリアル
  Serial.begin(115200);
  // シリアルポートの接続を待つ
  while (!Serial) {}

  Serial.println("wifi setup start");

  // softAPを設定
  WiFi.softAP(ssid, pass);
  WiFi.softAPConfig(ip, ip, subnet);

  // SPIFFSを設定する
  if(!SPIFFS.begin(true)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }

  // アクセスポイントのセットアップを待つ
  delay(1000);

  // serverを設定
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Fixed IP addr= ");
  Serial.println(myIP);
  webServer.serveStatic("/chart.min.js", SPIFFS, "/chart.min.js");
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/web_page.html");
    Serial.println ("send html");
  });
  webServer.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readSensorData().c_str());
    Serial.println("send data");
  });
  webServer.begin();
  Serial.println("Server starting!");
}

void loop() {}
