#include <WiFi.h>
#include <WiFiUdp.h>

// Pin assignments for ADXL335 (X and Y axes)
const int x_out = 34;  // X axis connected to GPIO34 (Analog input)
const int y_out = 35;  // Y axis connected to GPIO35 (Analog input)

// WiFi settings
const char* ssid = "ESP32_AP";         // SSID الخاص بنقطة الوصول
const char* password = "123456789";    // كلمة المرور الخاصة بنقطة الوصول

// UDP settings
WiFiUDP udp;
const char* receiverIP = "192.168.4.2";  // عنوان IP لجهاز الاستقبال ESP32
const int receiverPort = 8888;  // المنفذ لرسائل UDP

// Structure to store gesture data
struct data {
  int xAxis;
  int yAxis;
};
data send_data;

void setup() {
  Serial.begin(115200);

  // إعداد نقطة الوصول (Access Point)
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());  // طباعة عنوان IP الخاص بنقطة الوصول

  // إعداد UDP للإرسال
  udp.begin(8888);  // بدء الاتصال على المنفذ 8888
}

void loop() {
  // قراءة قيم الـ X و Y من المستشعرات (مثال: من عصا التحكم أو حساس تسارع)
  send_data.xAxis = analogRead(x_out);  // قراءة القيم من الدبوس X
  send_data.yAxis = analogRead(y_out);  // قراءة القيم من الدبوس Y

  // طباعة القيم التي تم قراءتها
  Serial.print("X = ");
  Serial.print(send_data.xAxis);
  Serial.print(" | Y = ");
  Serial.println(send_data.yAxis);

  // إرسال البيانات عبر UDP
  udp.beginPacket(receiverIP, receiverPort);
  udp.write((uint8_t*)&send_data, sizeof(data));  // إرسال البيانات
  udp.endPacket();

  // طباعة البيانات للإرسال
  Serial.println("Sending Data...");

  delay(100);  // تأخير بين كل إرسال
}
