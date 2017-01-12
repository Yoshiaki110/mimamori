# mimamori
## 使用するもの
- ESPr
- 人感センサ
 - xxに接続
- サウンドセンサ
 - A0ピンに接続
- Microsoft Azure
## 仕様
- １秒おきにセンサ値を読む
 - 人感センサは、ONをカウント
 - サウンドセンサは、最大値を記録
- １時間おきに送信
 - 送信エラーはリセット

- デバッグ機能
 - 起動時LEDチカチカ10秒後にSWが押されていたらデバッグモード
 - デバッグモードは、１分毎に送信
## コンパイル
- private.hを作成し各自の環境に合わせる
 #define SSID "WiFiのSSID"
 #define PASSWORD "WiFiのパスワード"
 #define SERVER "Azure上のサーバ"
 #define PATH "Azure上のテーブルへのパス"
