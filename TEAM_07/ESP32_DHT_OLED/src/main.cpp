/*
Team 07
ESP32 DHT OLED Project
1. Lê Công Mẫn

*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Cấu hình màn hình OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Cấu hình cảm biến DHT22 ---
#define DHTPIN 15     // Chân data của DHT22 nối vào D15
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

// --- Cấu hình đèn LED ---
#define LED_GREEN  5  // Đèn bên trái (trên mạch là màu xanh ngọc/Blue)
#define LED_YELLOW 18 // Đèn giữa
#define LED_RED    19 // Đèn bên phải

void setup() {
  Serial.begin(115200);
  delay(1000); // Chờ Serial khởi động

  Serial.println("\n\n=== ESP32 DHT OLED Project ===");
  Serial.println("Khởi động...");

  // 1. Khởi động cảm biến DHT
  dht.begin();
  Serial.println("DHT sensor initialized");
  delay(2000); // Chờ cảm biến ổn định (rất quan trọng!)

  // 2. Khởi động màn hình OLED
  // Địa chỉ I2C thường là 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("Không tìm thấy màn hình OLED"));
    for(;;); // Dừng chương trình nếu lỗi màn hình
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  Serial.println("OLED display initialized");
  
  // 3. Cấu hình chân LED là Output
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  Serial.println("LED pins configured");
  Serial.println("Setup complete!\n");
}

void loop() {
  // --- Bước 1: Đọc dữ liệu ---
  float t = dht.readTemperature(); // Đọc nhiệt độ
  float h = dht.readHumidity();    // Đọc độ ẩm

  // Kiểm tra lỗi cảm biến
  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi đọc cảm biến DHT!");
    Serial.print("Temp: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    
    // Hiển thị lỗi trên OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("DHT READ ERROR!");
    display.println("Check:");
    display.println("- Pin connection");
    display.println("- Power supply");
    display.println("- Pull-up resistor");
    display.display();
    
    delay(2000); // Chờ 2 giây trước khi thử lại
    return;
  }

  // --- Bước 2: Xử lý Logic theo bảng yêu cầu ---
  String textStatus = "";
  int activeLed = -1; // Biến lưu chân đèn cần nháy

  if (t < 13) {
    textStatus = "TOO COLD";
    activeLed = LED_GREEN;
  } 
  else if (t >= 13 && t < 20) {
    textStatus = "COLD";
    activeLed = LED_GREEN;
  }
  else if (t >= 20 && t < 25) {
    textStatus = "COOL";
    activeLed = LED_YELLOW;
  }
  else if (t >= 25 && t < 30) {
    textStatus = "WARM";
    activeLed = LED_YELLOW;
  }
  else if (t >= 30 && t < 35) {
    textStatus = "HOT";
    activeLed = LED_RED;
  }
  else { // t >= 35
    textStatus = "TOO HOT";
    activeLed = LED_RED;
  }

  // --- Bước 3: Hiển thị lên OLED ---
  display.clearDisplay();
  
  // Hiển thị Nhiệt độ & Độ ẩm
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");
  
  display.setCursor(0, 15);
  display.print("Hum:  ");
  display.print(h);
  display.print(" %");

  // Hiển thị trạng thái (TOO COLD, WARM, v.v.)
  display.setTextSize(2); // Chữ to hơn cho trạng thái
  display.setCursor(0, 40);
  display.print(textStatus);
  
  display.display();

  // --- Bước 4: Điều khiển LED (steady) ---
  // Tắt tất cả đèn trước để đảm bảo chỉ đèn đúng điều kiện mới sáng
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // Debug: in thông tin đọc và trạng thái
  Serial.print("Temp: "); Serial.print(t); Serial.print(" C, ");
  Serial.print("Hum: "); Serial.print(h); Serial.print(" %, ");
  Serial.print("Status: "); Serial.print(textStatus); Serial.print(", ");
  Serial.print("ActiveLed pin: "); Serial.println(activeLed);

  // Bật LED tương ứng ở trạng thái ON (liên tục) — dễ kiểm tra hơn
  if (activeLed == LED_GREEN) {
    digitalWrite(LED_GREEN, HIGH);
  } else if (activeLed == LED_YELLOW) {
    digitalWrite(LED_YELLOW, HIGH);
  } else if (activeLed == LED_RED) {
    digitalWrite(LED_RED, HIGH);
  }

  // Chờ 1s trước lần đọc tiếp theo
  delay(1000);
}