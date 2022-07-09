#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>

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

// I2Cのアドレス指定
#define ADXL345_ADDR 0x53
#define DATA_FORMAT 0x31
#define POWER_CTL 0x2D
#define DATAX0 0x32
#define FULL_RES_16G 0x0B
#define BIT10_16G 0x03
#define MEASURE 0x08

// センサーからデータ取得
String getSensorData() {
  int a4 = analogRead(A4);
  int a5 = analogRead(A5);
  int a6 = analogRead(A6);
  int a7 = analogRead(A7);
  int a0 = analogRead(A0);
  int a3 = analogRead(A4);
  if (isnan(a4)) {
    Serial.println("正しく数値が取れていません");
    return "";
  } else {
    String result = String(a4)+","+String(a5)+","+String(a6)+","+String(a7)+","+String(a0)+","+String(a3);
    Serial.println(result);
    return result;
  }
}

// I2Cからデータを取得
// ADXL345（3軸加速度センサ）を使ったサンプル
// ご自身のI2Cのデータの取得方法に変更してください
String getI2CData() {
  // 変数を宣言
  String result;
  unsigned int dac[6];
  unsigned int i, x, y, z;
  float xAxis, yAxis, zAxis;

  Wire.beginTransmission(ADXL345_ADDR);// I2Cスレーブのデータ送信開始
  Wire.write(DATAX0);                  // 出力データバイトを「X軸データ0」のアドレスに指定
  Wire.endTransmission();              // I2Cスレーブのデータ送信終了
  
  Wire.requestFrom(ADXL345_ADDR, 6);   // I2Cデバイス「ADXL345」に6Byteのデータ要求
  for (i=0; i<6; i++){
    while (Wire.available() == 0 ){}
    dac[i] = Wire.read();              // dacにI2Cデバイス「ADXL345」のデータ読み込み
  }

  x = (dac[1] << 8) | dac[0];     // 2Byte目のデータを8bit左にシフト、OR演算子で1Byte目のデータを結合して、xに代入
  xAxis = float(x) * 0.0392266;   // 加速度の計算、xAxisに代入 ※0.0392266=(4/1000*9.80665)
  y = (dac[3] << 8) | dac[2];     // 2Byte目のデータを8bit左にシフト、OR演算子で1Byte目のデータを結合して、xに代入
  yAxis = float(y) * 0.0392266;   // 加速度の計算、yAxisに代入
  z = (dac[5] << 8) | dac[4];     // 2Byte目のデータを8bit左にシフト、OR演算子で1Byte目のデータを結合して、xに代入
  zAxis = float(z) * 0.0392266;   // 加速度の計算、zAxisに代入

  // "a,b,c"という形式で送信
  result = String(xAxis)+","+String(yAxis)+","+String(zAxis);
  return result;
}

void setup() {
  // シリアル
  Serial.begin(115200);
  // シリアルポートの接続を待つ
  while (!Serial) {}

  // I2Cを初期化
  Wire.begin();
  Wire.beginTransmission(ADXL345_ADDR);// I2Cスレーブのデータ送信開始
  Wire.write(DATA_FORMAT);             // データ・フォーマット・コントロール
  Wire.write(FULL_RES_16G);            // 「最大分解能モード」、「±16g」
  Wire.endTransmission();              // I2Cスレーブのデータ送信終了
 
  Wire.beginTransmission(ADXL345_ADDR);// I2Cスレーブのデータ送信開始
  Wire.write(POWER_CTL);               // 省電力機能コントロール
  Wire.write(MEASURE);                 // 測定モード
  Wire.endTransmission();              // I2Cスレーブのデータ送信終了

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
    request->send_P(200, "text/plain", getSensorData().c_str());
    Serial.println("send sensor data");
  });
  webServer.on("/i2c", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", getI2CData().c_str());
    Serial.println("send I2C data");
  });
  webServer.begin();
  Serial.println("Server starting!");
}

void loop() {}
