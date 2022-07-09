# ESP32データロガー

# 概要
ESP32のsoftAPを使ったデータロガーです。ESP32のアクセスポイントにIPアドレスでWebページを展開します。そして、Pinから取得したセンサのデータやI2C通信のデータをグラフとして表示します。ここで得られたデータは、CSV形式のファイルとして出力することができます。

# 使い方
## 準備
ご自身の開発環境に合わせて不必要なものは適宜に読み飛ばしてください。

### ESP32の設定
1. 「ファイル」->「環境設定」->「追加のボードマネージャのURL」の隣のボタンをクリック    
    その空欄に`https://dl.espressif.com/dl/package_esp32_index.json`を追加  
    ※ すでに他のアドレスが入力されている場合、「;」で区切って入力してください
2. 「ツール」->「ボード:」->「ボードマネージャ」をクリック  
    「検索をフィルタ…」に「ESP32」と入力すると、「esp32 by Espressif Systems」が表示されるので、「インストール」をクリック
3. インストールができたら、「ツール」->「ボード:」->「ESP32 Dev Module」をクリック  
    これで設定は完了です。

### SPIFFSを追加
SPIFFS（SPI Flash File System）という、ESP32のフラッシュメモリのユーザー領域にファイルを保存するライブラリです。  
1. [こちら](https://github.com/me-no-dev/arduino-esp32fs-plugin)から**releases**をクリックし**ESP32FS-1.0.zip**をダウンロード 
2. Arduino IDEのスケッチブックの保存場所に**tools**フォルダを作成し、zipファイルを解凍したものを配置
3. Arduino IDEを開いている場合は再起動し、「ツール」->「ESP32 Sketch Data Upload」が追加されていることを確認

### ESPAsyncWebServerを追加
Webサーバーを動かすためのライブラリ
1. [こちら](https://github.com/me-no-dev/ESPAsyncWebServer)からzipファイルをダウンロード
2. 解凍し、フォルダ名を**ESPAsyncWebServer**に変更し、Arduino IDEのスケッチブックの保存場所の**libraries**フォルダに配置

### AsyncTCPを追加
ESPAsyncWebServerが依存しているライブラリ
1. [こちら](https://github.com/me-no-dev/AsyncTCP)からzipファイルをダウンロード。
2. 解凍し、フォルダ名を**AsyncTCP**に変更し、Arduino IDEのスケッチブックの保存場所の**libraries**フォルダに配置

### ESP32のフラッシュに書き込み
プログラムのフォルダ内に**data**フォルダを作成済みです。このフォルダの中を変更することでフラッシュに書き込むファイルを変更することができます。
1. 「ツール」->「ESP32 Sketch Data Upload」を実行して書き込む

### プログラムの書き込み
**ESP32_Data_Logger.ino**を開きESP32に書き込みます。  
この時（ESP32 Sketch Data Upload時も)書き込みエラーが発生する場合は、ESP32の**BOOT**ボタンを長押しで解決できます。

## 実行
1. まず、書き込んで電源をつけたESP32を用意
2. 無線LANに接続できるデバイスのWiFi設定ページで、*ESP32APWIFI*を選択し*esp32apwifi*をパスワードに入力  
    この時パスワード入力フォームがなければ、「セキュリティ」->「WPA/WPA2/WPA3-Personal」を選択してからパスワードを入力
3. アクセスポイントに接続できたら、WebサーバーのIPアドレスである「*192.168.0.32*」をブラウザで入力しページにアクセス

## Webページの使い方

# I2C関連と編集箇所の説明
40~47行目で定数を宣言  
別のセンサを使う場合には必要ないので**削除**します
```cpp
// I2Cのアドレス指定
#define ADXL345_ADDR 0x53
#define DATA_FORMAT 0x31
#define POWER_CTL 0x2D
#define DATAX0 0x32
#define FULL_RES_16G 0x0B
#define BIT10_16G 0x03
#define MEASURE 0x08
```
67~97行目はI2Cからデータを取得する関数  
データの取得プログラムなので任意に編集してください
ただし**戻り値の形式**は変更しないでください
```cpp
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
```
105~115行目でI2Cを扱うためのWireライブラリを初期化  
アドレスを変更して使用されるセンサに対しての**初期化処理**を行ってください
ここでは定義してある定数を引数として使用しています
```cpp
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
```
145~148行目でHTTPリクエストに反応する処理  
基本的には変更しないでください
```cpp
webServer.on("/i2c", HTTP_GET, [](AsyncWebServerRequest *request) {
  request->send_P(200, "text/plain", getI2CData().c_str());
  Serial.println("send I2C data");
});
```