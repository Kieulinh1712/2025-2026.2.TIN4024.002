 #include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Cấu hình màn hình OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cấu hình cảm biến DHT22
#define DHTPIN 16          // Chân Data của DHT22 nối vào GPIO 16
#define DHTTYPE DHT11     // Thay đổi thành DHT11 nếu cảm biến là DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cấu hình chân LED (Dựa trên sơ đồ của bạn)
const int LED_CYAN   = 15;  // Màu xanh lam (Cyan) cho trạng thái Lạnh
const int LED_YELLOW = 2;  // Màu vàng cho trạng thái Mát/Ấm
const int LED_RED    = 4;  // Màu đỏ cho trạng thái Nóng

void setup() {
  Serial.begin(115200);

  // Khởi tạo các chân LED
  pinMode(LED_CYAN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Khởi tạo DHT11
  dht.begin();
  delay(2000); // Đợi cảm biến ổn định

  // Khởi tạo OLED (Địa chỉ mặc định thường là 0x3C)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Không tìm thấy màn hình OLED"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Khoi dong he thong...");
  display.display();
  delay(2000);
}

void loop() {
  // Đọc dữ liệu từ DHT22 với retry
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Nếu đọc thất bại, thử lại sau 1 giây
  if (isnan(temp) || isnan(hum)) {
    delay(1000);
    temp = dht.readTemperature();
    hum = dht.readHumidity();
  }

  // Kiểm tra lại nếu vẫn lỗi
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Loi doc cam bien!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Loi doc cam bien!");
    display.display();
    delay(2000);
    return;
  }

  // In ra Serial để debug
  Serial.print("Nhiet do: ");
  Serial.print(temp);
  Serial.print(" C, Do am: ");
  Serial.print(hum);
  Serial.println(" %");

  String statusText = "";
  int activeLED = -1;

  // Logic phân loại ngưỡng nhiệt độ theo bảng yêu cầu
  if (temp < 13.0) {
    statusText = "TOO COLD ❄️";
    activeLED = LED_CYAN;
  } else if (temp >= 13.0 && temp < 20.0) {
    statusText = "COLD ☔";
    activeLED = LED_CYAN;
  } else if (temp >= 20.0 && temp < 25.0) {
    statusText = "COOL ☁️";
    activeLED = LED_YELLOW;
  } else if (temp >= 25.0 && temp < 30.0) {
    statusText = "WARM ☀️";
    activeLED = LED_YELLOW;
  } else if (temp >= 30.0 && temp < 35.0) {
    statusText = "HOT 🌞";
    activeLED = LED_RED;
  } else { // temp >= 35.0
    statusText = "TOO HOT 🔥";
    activeLED = LED_RED;
  }

  // Cập nhật màn hình OLED
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Humidity: ");
  display.print(hum);
  display.println(" %");

  display.setTextSize(2); // Chữ to hơn cho nhiệt độ
  display.setCursor(0, 15);
  display.print("T: ");
  display.print(temp, 1);
  display.println(" C");

  display.setTextSize(2);
  display.setCursor(0, 45);
  display.println(statusText);
  
  display.display();

  // Xử lý LED nhấp nháy (không chặn code hoàn toàn)
  // Tắt tất cả các đèn trước
  digitalWrite(LED_CYAN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // Nhấp nháy LED tương ứng
  if (activeLED != -1) {
    digitalWrite(activeLED, HIGH);
    delay(500); // Sáng 0.5 giây
    digitalWrite(activeLED, LOW);
    delay(500); // Tắt 0.5 giây
  }

  delay(2000); // Đợi 2 giây trước khi đọc lại cảm biến
}