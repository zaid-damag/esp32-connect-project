#include <WiFi.h>
#include <WiFiUdp.h>

// Motor control pins
int ENA = 32;  // Pin for motor A speed control (PWM)
int ENB = 33;  // Pin for motor B speed control (PWM)
int MotorA1 = 25; // Motor A direction pin 1
int MotorA2 = 26; // Motor A direction pin 2
int MotorB1 = 27; // Motor B direction pin 1
int MotorB2 = 14; // Motor B direction pin 2

// WiFi settings
const char* ssid = "ESP32_AP";         // SSID الخاص بنقطة الوصول
const char* password = "123456789";    // كلمة المرور الخاصة بنقطة الوصول

// UDP settings
WiFiUDP udp;
const int listenPort = 8888;  // المنفذ الذي سيتم الاستماع إليه

// Structure to store received data
struct data {
  int xAxis;
  int yAxis;
};
data received_data;

void setup() {
  Serial.begin(115200);

  // الاتصال بشبكة WiFi (ESP32 الأول)
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // إعداد UDP للاستماع على المنفذ 8888
  udp.begin(listenPort);
  Serial.println("Listening for UDP packets...");

  // إعداد مداخل المحركات للتحكم عبر PWM
  pinMode(MotorA1, OUTPUT);
  pinMode(MotorA2, OUTPUT);
  pinMode(MotorB1, OUTPUT);
  pinMode(MotorB2, OUTPUT);
  
  // تفعيل PWM للمحركات
  ledcSetup(0, 5000, 8);  // تحديد التردد و دقة PWM (تردد 5kHz و دقة 8 بت)
  ledcAttachPin(ENA, 0);  // توصيل ENA إلى القناة 0
  ledcAttachPin(ENB, 1);  // توصيل ENB إلى القناة 1
}

void loop() {
  int packetSize = udp.parsePacket();  // قراءة الحزمة الواردة
  if (packetSize) {
    udp.read((uint8_t*)&received_data, sizeof(data));  // قراءة البيانات من الحزمة

    // طباعة البيانات المستقبلة
    Serial.print("Received: X = ");
    Serial.print(received_data.xAxis);
    Serial.print(" | Y = ");
    Serial.println(received_data.yAxis);

    // التحكم في المحركات بناءً على القيم المستلمة
    if (received_data.xAxis > 1900) {  // Move forward
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, HIGH);
      digitalWrite(MotorB1, HIGH);
      digitalWrite(MotorB2, LOW);
      ledcWrite(0, 128);  // تعيين قيمة PWM (دورة عمل 50%)
      ledcWrite(1, 128);  // تعيين قيمة PWM (دورة عمل 50%)
    } else if (received_data.xAxis < 1700) {  // Move backward
      digitalWrite(MotorA1, HIGH);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      ledcWrite(0, 128);  // تعيين قيمة PWM (دورة عمل 50%)
      ledcWrite(1, 128);  // تعيين قيمة PWM (دورة عمل 50%)
    } else if (received_data.yAxis > 1900) {  // Turn left
      digitalWrite(MotorA1, HIGH);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, HIGH);
      digitalWrite(MotorB2, LOW);
      ledcWrite(0, 128);  // تعيين قيمة PWM (دورة عمل 50%)
      ledcWrite(1, 0);    // إيقاف المحرك الثاني
    } else if (received_data.yAxis < 1700) {  // Turn right
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, HIGH);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      ledcWrite(0, 0);    // إيقاف المحرك الأول
      ledcWrite(1, 128);  // تعيين قيمة PWM (دورة عمل 50%)
    } else {  // Stop
      digitalWrite(MotorA1, LOW);
      digitalWrite(MotorA2, LOW);
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, LOW);
      ledcWrite(0, 0);    // إيقاف المحرك الأول
      ledcWrite(1, 0);    // إيقاف المحرك الثاني
    }
  }

  delay(100);  // تأخير بين استقبال البيانات
}
