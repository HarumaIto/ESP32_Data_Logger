# ESP32データロガー

# 概要
ESP32のsoftAPを使ったデータロガーです。ESP32のアクセスポイントにIPアドレスでWebページを展開します。そして、Pinから取得したセンサのデータやI2C通信のデータをグラフとして表示します。ここで得られたデータは、CSV形式のファイルとして出力することができます。

# 使い方
## 準備
ご自身の開発環境に合わせて不必要なものは適宜に読み飛ばしてください。

### ESP32の設定
1. 「ファイル」->「環境設定」->「追加のボードマネージャのURL」の隣のボタンをクリック。   
    その空欄に`https://dl.espressif.com/dl/package_esp32_index.json`を追加。  
    ※ すでに他のアドレスが入力されている場合、「;」で区切って入力してください
2. 「ツール」->「ボード:」->「ボードマネージャ」をクリック。  
    「検索をフィルタ…」に「ESP32」と入力すると、「esp32 by Espressif Systems」が表示されるので、「インストール」をクリックします。
3. インストールができたら、「ツール」->「ボード:」->「ESP32 Dev Module」をクリックします。
    これで設定は完了です。

### SPIFFSを追加
SPIFFS（SPI Flash File System）という、ESP32のフラッシュメモリのユーザー領域にファイルを保存するライブラリ。  
1. [こちら](https://github.com/me-no-dev/arduino-esp32fs-plugin)から**releases**をクリックし**ESP32FS-1.0.zip**をダウンロード  。
2. Arduino IDEのスケッチブックの保存場所に**tools**フォルダを作成し、zipファイルを解凍したものを配置する。
3. Arduino IDEを開いている場合は再起動し、「ツール」->「ESP32 Sketch Data Upload」が追加されていることを確認する。

### ESPAsyncWebServerを追加
Webサーバーを動かすためのライブラリ。
1. [こちら](https://github.com/me-no-dev/ESPAsyncWebServer)からzipファイルをダウンロード。
2. 解凍し、フォルダ名を**ESPAsyncWebServer**に変更し、Arduino IDEのスケッチブックの保存場所の**libraries**フォルダに配置。

### AsyncTCPを追加
ESPAsyncWebServerが依存しているライブラリ。
1. [こちら](https://github.com/me-no-dev/AsyncTCP)からzipファイルをダウンロード。
2. 解凍し、フォルダ名を**AsyncTCP**に変更し、Arduino IDEのスケッチブックの保存場所の**libraries**フォルダに配置。

### ESP32のフラッシュに書き込み
プログラムのフォルダ内に**data**フォルダを作成済みです。このフォルダの中を変更することでフラッシュに書き込むファイルを変更することができます。
1. 「ツール」->「ESP32 Sketch Data Upload」を実行して書き込む。

### プログラムの書き込み
**ESP32_Data_Logger.ino**を開きESP32に書き込みます。  
この時（ESP32 Sketch Data Upload時も)書き込みエラーが発生する場合は、ESP32の**BOOT**ボタンを長押しで解決できる。

## 実行
1. まず、書き込んで電源をつけたESP32を用意します。
2. 無線LANに接続できるデバイスのWiFi設定ページで、*ESP32APWIFI*を選択し*esp32apwifi*をパスワードに入力します。  
    この時パスワード入力フォームがなければ、「セキュリティ」->「WPA/WPA2/WPA3-Personal」を選択してからパスワードを入力します。
3. アクセスポイントに接続できたら、WebサーバーのIPアドレスである「*192.168.0.32*」をブラウザで入力しページにアクセスします。

## Webページの使い方


# I2C関連のプログラムの変更方法