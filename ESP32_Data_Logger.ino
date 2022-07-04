#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

// 定数
#define CHART_SZ 600

// Wifiで使う変数の初期化
char ssid[] = "ESP32APWIFI";
char pass[] = "esp32apwifi";
const IPAddress ip(192,168,11, 2);
const IPAddress subnet(255, 255, 255, 0);

// webで使う
String header = "";
boolean recording = false;
String startDisabled = "";
String pauseDisabled = "disabled";
String stopDisabled = "disabled";

// グラフに表示するデータ
float cData;
float chartData[CHART_SZ];
// センサのデータ(JSON形式)
const char SENSOR_JSON[] PROGMEM = R"=====({"val1":%.1f})=====";

// サーバーを初期化
AsyncWebServer webServer(80);

String readSensorData() {
  int data = analogRead(A4);
  if (isnan(data)) {
    Serial.println("正しく数値が取れていません");
    return "";
  } else {
    Serial.println(data);
    return String(data);
  }
}

void setup() {
  // シリアル
  Serial.begin(115200);
  while (!Serial) {
    ;// wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("wifi setup start");
  WiFi.disconnect(true);

  // softAPを設定
  WiFi.softAP(ssid, pass);
  WiFi.softAPConfig(ip, ip, subnet);

  if(!SPIFFS.begin(true)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }

  // wait 1 seconds for connection:
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

void loop() {
}
