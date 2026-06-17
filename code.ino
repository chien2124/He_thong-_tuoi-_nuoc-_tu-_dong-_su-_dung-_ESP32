#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================== KHAI BAO CHAN ==================
#define SOIL_PIN   34
#define RELAY_PIN  26
#define LED_PIN    18

// Nếu relay kích mức LOW thì đổi lại:
// #define RELAY_ON   LOW
// #define RELAY_OFF  HIGH

#define RELAY_ON   HIGH
#define RELAY_OFF  LOW

// ================== NGUONG DO AM ==================
int nguongKho = 2500;
int nguongUot = 1650;

// ================== BIEN TRANG THAI ==================
bool bomDangChay = false;
bool cheDoTuDong = true;

int value = 0;

// ================== WIFI ==================
const char* ssid = "Tên WiFi";
const char* password = "Mật ";

WiFiServer server(80);

// ================== LCD I2C ==================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================== SETUP ==================
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("DANG KET NOI");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  delay(3000);
  lcd.clear();
}

// ================== LOOP ==================
void loop() {
  value = analogRead(SOIL_PIN);

  webServer();

  // Che do tu dong
  if (cheDoTuDong == true) {
    if (value > nguongKho) {
      bomDangChay = true;
    }

    if (value < nguongUot) {
      bomDangChay = false;
    }
  }

  // Dieu khien bom va LED
  if (bomDangChay == true) {
    digitalWrite(RELAY_PIN, RELAY_ON);
    digitalWrite(LED_PIN, HIGH);

    lcd.setCursor(0, 1);
    lcd.print("BOM DANG BAT   ");
  } else {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    digitalWrite(LED_PIN, LOW);

    lcd.setCursor(0, 1);
    lcd.print("BOM DANG TAT   ");
  }

  // Hien thi ADC len LCD
  lcd.setCursor(0, 0);
  lcd.print("ADC:");
  lcd.print(value);
  lcd.print("     ");

  delay(300);
}

// ================== HAM QUAY VE TRANG CHINH ==================
void redirectHome(WiFiClient client) {
  client.println("HTTP/1.1 303 See Other");
  client.println("Location: /");
  client.println("Connection: close");
  client.println();
  client.stop();
}

// ================== WEB SERVER ==================
void webServer() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // ================== XU LY NUT BAT BOM ==================
    if (request.indexOf("GET /ON") != -1) {
      cheDoTuDong = false;
      bomDangChay = true;
      redirectHome(client);
      return;
    }

    // ================== XU LY NUT TAT BOM ==================
    if (request.indexOf("GET /OFF") != -1) {
      cheDoTuDong = false;
      bomDangChay = false;
      redirectHome(client);
      return;
    }

    // ================== XU LY NUT AUTO ==================
    if (request.indexOf("GET /AUTO") != -1) {
      cheDoTuDong = true;
      redirectHome(client);
      return;
    }

    // ================== TAO TRANG THAI HIEN THI ==================
    String trangThaiBom = bomDangChay ? "BƠM ĐANG BẬT" : "BƠM ĐANG TẮT";
    String trangThaiCheDo = cheDoTuDong ? "TỰ ĐỘNG" : "ĐIỀU KHIỂN TAY";

    String trangThaiDat;
    if (value > nguongKho) {
      trangThaiDat = "ĐẤT KHÔ";
    } else if (value < nguongUot) {
      trangThaiDat = "ĐẤT ĐỦ ẨM";
    } else {
      trangThaiDat = "ĐỘ ẨM TRUNG BÌNH";
    }

    String mauBom = bomDangChay ? "#ef4444" : "#2563eb";
    String mauDat = value > nguongKho ? "#f97316" : "#22c55e";

    String html = "";

    html += "<!DOCTYPE html>";
    html += "<html lang='vi'>";
    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";

    // Web tu cap nhat moi 1 giay
    html += "<meta http-equiv='refresh' content='1'>";

    html += "<title>Hệ thống tưới cây</title>";

    html += "<style>";

    html += "*{box-sizing:border-box;}";

    html += "body{";
    html += "margin:0;";
    html += "font-family:Arial,Helvetica,sans-serif;";
    html += "min-height:100vh;";
    html += "display:flex;";
    html += "justify-content:center;";
    html += "align-items:center;";
    html += "background:linear-gradient(135deg,#bbf7d0,#86efac,#22c55e);";
    html += "}";

    html += ".container{";
    html += "width:92%;";
    html += "max-width:430px;";
    html += "background:#ffffff;";
    html += "border-radius:28px;";
    html += "padding:25px;";
    html += "box-shadow:0 20px 45px rgba(0,0,0,0.25);";
    html += "text-align:center;";
    html += "}";

    html += ".title{";
    html += "font-size:26px;";
    html += "font-weight:bold;";
    html += "color:#166534;";
    html += "margin-bottom:5px;";
    html += "}";

    html += ".subtitle{";
    html += "font-size:14px;";
    html += "color:#64748b;";
    html += "margin-bottom:20px;";
    html += "}";

    html += ".adc-box{";
    html += "background:linear-gradient(135deg,#eff6ff,#dbeafe);";
    html += "border-radius:22px;";
    html += "padding:20px;";
    html += "margin-bottom:18px;";
    html += "border:1px solid #bfdbfe;";
    html += "}";

    html += ".adc-label{";
    html += "font-size:15px;";
    html += "color:#475569;";
    html += "margin-bottom:8px;";
    html += "}";

    html += ".adc-value{";
    html += "font-size:44px;";
    html += "font-weight:bold;";
    html += "color:#1d4ed8;";
    html += "}";

    html += ".grid{";
    html += "display:grid;";
    html += "grid-template-columns:1fr 1fr;";
    html += "gap:12px;";
    html += "margin-bottom:18px;";
    html += "}";

    html += ".status-card{";
    html += "background:#f8fafc;";
    html += "border-radius:18px;";
    html += "padding:15px 10px;";
    html += "border:1px solid #e2e8f0;";
    html += "}";

    html += ".status-title{";
    html += "font-size:13px;";
    html += "color:#64748b;";
    html += "margin-bottom:8px;";
    html += "}";

    html += ".status-value{";
    html += "font-size:16px;";
    html += "font-weight:bold;";
    html += "}";

    html += ".pump-box{";
    html += "border-radius:22px;";
    html += "padding:18px;";
    html += "margin-bottom:20px;";
    html += "background:#f8fafc;";
    html += "border:2px solid " + mauBom + ";";
    html += "}";

    html += ".pump-text{";
    html += "font-size:22px;";
    html += "font-weight:bold;";
    html += "color:" + mauBom + ";";
    html += "}";

    html += ".btn-group{";
    html += "display:flex;";
    html += "gap:10px;";
    html += "justify-content:center;";
    html += "flex-wrap:wrap;";
    html += "}";

    html += "button{";
    html += "border:none;";
    html += "border-radius:15px;";
    html += "padding:14px 18px;";
    html += "font-size:16px;";
    html += "font-weight:bold;";
    html += "color:white;";
    html += "cursor:pointer;";
    html += "box-shadow:0 6px 14px rgba(0,0,0,0.18);";
    html += "min-width:110px;";
    html += "}";

    html += ".btn-on{background:#ef4444;}";
    html += ".btn-off{background:#2563eb;}";
    html += ".btn-auto{background:#16a34a;}";

    html += "button:active{";
    html += "transform:scale(0.96);";
    html += "}";

    html += ".footer{";
    html += "margin-top:18px;";
    html += "font-size:13px;";
    html += "color:#64748b;";
    html += "}";

    html += ".dot{";
    html += "display:inline-block;";
    html += "width:10px;";
    html += "height:10px;";
    html += "background:#22c55e;";
    html += "border-radius:50%;";
    html += "margin-right:6px;";
    html += "}";

    html += "</style>";
    html += "</head>";

    html += "<body>";
    html += "<div class='container'>";

    html += "<div class='title'>HỆ THỐNG TƯỚI CÂY</div>";
    html += "<div class='subtitle'>Giám sát và điều khiển bằng ESP32 Web Server</div>";

    html += "<div class='adc-box'>";
    html += "<div class='adc-label'>Giá trị độ ẩm đất</div>";
    html += "<div class='adc-value'>" + String(value) + "</div>";
    html += "</div>";

    html += "<div class='grid'>";

    html += "<div class='status-card'>";
    html += "<div class='status-title'>Trạng thái đất</div>";
    html += "<div class='status-value' style='color:" + mauDat + ";'>" + trangThaiDat + "</div>";
    html += "</div>";

    html += "<div class='status-card'>";
    html += "<div class='status-title'>Chế độ</div>";
    html += "<div class='status-value' style='color:#16a34a;'>" + trangThaiCheDo + "</div>";
    html += "</div>";

    html += "</div>";

    html += "<div class='pump-box'>";
    html += "<div class='status-title'>Trạng thái máy bơm</div>";
    html += "<div class='pump-text'>" + trangThaiBom + "</div>";
    html += "</div>";

    html += "<div class='btn-group'>";
    html += "<a href='/ON'><button class='btn-on'>BẬT BƠM</button></a>";
    html += "<a href='/OFF'><button class='btn-off'>TẮT BƠM</button></a>";
    html += "<a href='/AUTO'><button class='btn-auto'>AUTO</button></a>";
    html += "</div>";

    html += "<div class='footer'>";
    html += "<span class='dot'></span>";
    html += "Trang tự động cập nhật mỗi 1 giây";
    html += "</div>";

    html += "</div>";
    html += "</body>";
    html += "</html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html; charset=utf-8");
    client.println("Connection: close");
    client.println();
    client.println(html);
    client.stop();
  }
}