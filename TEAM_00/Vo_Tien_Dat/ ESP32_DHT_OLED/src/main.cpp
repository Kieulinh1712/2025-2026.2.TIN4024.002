#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 16 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Định nghĩa chân LED
const int LED_CYAN   = 15;  
const int LED_YELLOW = 2;  
const int LED_RED    = 4;  

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 DHT OLED...");

  pinMode(LED_CYAN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  dht.begin();
  Wire.begin(); // Khởi tạo I2C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    for(;;);
  }
  Serial.println("OLED initialized");
  display.clearDisplay();
  delay(100); // Đợi OLED ổn định
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Sensor read failed!");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C, Hum: ");
  Serial.print(hum);
  Serial.println(" %");

  String statusText = "";
  int activeLED = -1;

  // Xác định trạng thái và đèn LED dựa trên bảng yêu cầu
  if (temp < 13.0) { statusText = "TOO COLD"; activeLED = LED_CYAN; }
  else if (temp < 20.0) { statusText = "COLD"; activeLED = LED_CYAN; }
  else if (temp < 25.0) { statusText = "COOL"; activeLED = LED_YELLOW; }
  else if (temp < 30.0) { statusText = "WARM"; activeLED = LED_YELLOW; }
  else if (temp < 35.0) { statusText = "HOT"; activeLED = LED_RED; }
  else { statusText = "TOO HOT"; activeLED = LED_RED; }

  // --- HIỂN THỊ OLED GIỐNG HÌNH MẪU ---
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Dòng 1: Temperature và Trạng thái (Size 1)
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.println(statusText);

  // Dòng 2: Giá trị nhiệt độ (Size 2)
  display.setTextSize(2);
  display.setCursor(0, 12);
  display.print(temp, 2); 
  display.println(" °C");

  // Dòng 3: Humidity (Size 1)
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.println("Humidity:");

  // Dòng 4: Giá trị độ ẩm (Size 2)
  display.setTextSize(2);
  display.setCursor(0, 47);
  display.print(hum, 2);
  display.println(" %");

  display.display();

  // --- HIỆU ỨNG NHẤP NHÁY LED ---
  digitalWrite(LED_CYAN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  if (activeLED != -1) {
    digitalWrite(activeLED, HIGH);
    delay(500); 
    digitalWrite(activeLED, LOW);
    delay(500); 
  }

  delay(1000); // Cập nhật mỗi 1 giây
}