/*
  AulaConectada IA - Prueba rapida ESP32

  Sirve para validar el montaje fisico sin depender todavia de beacons BLE.
  Comprueba WiFi, servidor local, LED, buzzer y llegada de datos al panel.
*/

#include <HTTPClient.h>
#include <WiFi.h>

const char* WIFI_SSID = "NOMBRE_DE_LA_RED";
const char* WIFI_PASSWORD = "CAMBIAR_CLAVE";
const char* API_BASE_URL = "http://192.168.2.4:8080";

const char* GATEWAY_ID = "ESP32-AULA-10-PRUEBA";
const char* DEFAULT_ZONE = "Aula 10";
const char* FIRMWARE_VERSION = "prueba-wifi-1.0.0";

const int LED_PIN = 2;
const int RED_LED_PIN = 4;
const int BUZZER_PIN = 15;
const int LCD_RS_PIN = 21;
const int LCD_ENABLE_PIN = 20;
const int LCD_D4_PIN = 19;
const int LCD_D5_PIN = 18;
const int LCD_D6_PIN = 17;
const int LCD_D7_PIN = 16;
const int LCD_COLS = 16;
const unsigned long WIFI_TIMEOUT_MS = 20000;
const unsigned long SEND_INTERVAL_MS = 15000;

unsigned long lastSend = 0;
int sampleIndex = 0;
String currentZone = DEFAULT_ZONE;
bool lcdAvailable = false;

const bool LCD_COMMAND = false;
const bool LCD_DATA = true;

void lcdPulseEnable() {
  digitalWrite(LCD_ENABLE_PIN, LOW);
  delayMicroseconds(1);
  digitalWrite(LCD_ENABLE_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_ENABLE_PIN, LOW);
  delayMicroseconds(100);
}

void lcdWrite4Bits(byte value) {
  digitalWrite(LCD_D4_PIN, (value >> 0) & 0x01);
  digitalWrite(LCD_D5_PIN, (value >> 1) & 0x01);
  digitalWrite(LCD_D6_PIN, (value >> 2) & 0x01);
  digitalWrite(LCD_D7_PIN, (value >> 3) & 0x01);
  lcdPulseEnable();
}

void lcdSend(byte value, bool mode) {
  digitalWrite(LCD_RS_PIN, mode);
  lcdWrite4Bits(value >> 4);
  lcdWrite4Bits(value & 0x0F);
}

void lcdCommand(byte value) {
  if (!lcdAvailable) return;
  lcdSend(value, LCD_COMMAND);
  if (value == 0x01 || value == 0x02) delay(2);
}

void lcdWriteChar(char value) {
  if (!lcdAvailable) return;
  lcdSend((byte)value, LCD_DATA);
}

String lcdFit(const String& text) {
  String out = text.substring(0, min((int)text.length(), LCD_COLS));
  while (out.length() < LCD_COLS) out += " ";
  return out;
}

void lcdPrintLine(int line, const String& text) {
  if (!lcdAvailable) return;
  lcdCommand(0x80 + (line == 0 ? 0x00 : 0x40));
  String out = lcdFit(text);
  for (int i = 0; i < LCD_COLS; i++) {
    lcdWriteChar(out.charAt(i));
  }
}

void lcdShow(const String& line1, const String& line2) {
  if (!lcdAvailable) return;
  lcdPrintLine(0, line1);
  lcdPrintLine(1, line2);
}

void lcdBegin() {
  pinMode(LCD_RS_PIN, OUTPUT);
  pinMode(LCD_ENABLE_PIN, OUTPUT);
  pinMode(LCD_D4_PIN, OUTPUT);
  pinMode(LCD_D5_PIN, OUTPUT);
  pinMode(LCD_D6_PIN, OUTPUT);
  pinMode(LCD_D7_PIN, OUTPUT);
  lcdAvailable = true;

  delay(50);
  digitalWrite(LCD_RS_PIN, LOW);
  digitalWrite(LCD_ENABLE_PIN, LOW);

  lcdWrite4Bits(0x03);
  delay(5);
  lcdWrite4Bits(0x03);
  delayMicroseconds(150);
  lcdWrite4Bits(0x03);
  lcdWrite4Bits(0x02);
  lcdCommand(0x28);
  lcdCommand(0x08);
  lcdCommand(0x01);
  lcdCommand(0x06);
  lcdCommand(0x0C);
  lcdShow("AulaConectada", "Iniciando...");
}

String escapeJson(const String& value) {
  String out = "";
  for (unsigned int i = 0; i < value.length(); i++) {
    char c = value.charAt(i);
    if (c == '"' || c == '\\') out += '\\';
    out += c;
  }
  return out;
}

void pulse(int count, int frequency) {
  int activeLedPin = frequency >= 1500 ? LED_PIN : RED_LED_PIN;
  for (int i = 0; i < count; i++) {
    digitalWrite(activeLedPin, HIGH);
    tone(BUZZER_PIN, frequency, 120);
    delay(160);
    digitalWrite(activeLedPin, LOW);
    delay(100);
  }
}

bool connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando WiFi");
  lcdShow("Conectando WiFi", WIFI_SSID);

  unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("No se pudo conectar. Revisa SSID, clave y red 2.4 GHz.");
    lcdShow("WiFi fallo", "Revise clave");
    pulse(1, 700);
    return false;
  }

  Serial.println();
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());
  lcdShow("WiFi conectado", WiFi.localIP().toString());
  pulse(2, 1500);
  return true;
}

bool postJson(const String& path, const String& payload) {
  if (WiFi.status() != WL_CONNECTED && !connectWifi()) return false;

  HTTPClient http;
  String url = String(API_BASE_URL) + path;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int statusCode = http.POST(payload);
  String response = http.getString();
  http.end();

  Serial.print("POST ");
  Serial.print(url);
  Serial.print(" -> ");
  Serial.println(statusCode);
  Serial.println(response);

  if (statusCode >= 200 && statusCode < 300) {
    lcdShow("Servidor OK", path);
    pulse(2, 1800);
    return true;
  }

  lcdShow("Servidor fallo", String(statusCode));
  pulse(1, 900);
  return false;
}

String getJson(const String& path) {
  if (WiFi.status() != WL_CONNECTED && !connectWifi()) return "";

  HTTPClient http;
  String url = String(API_BASE_URL) + path;
  http.begin(url);
  int statusCode = http.GET();
  String response = http.getString();
  http.end();

  Serial.print("GET ");
  Serial.print(url);
  Serial.print(" -> ");
  Serial.println(statusCode);

  if (statusCode >= 200 && statusCode < 300) return response;
  return "";
}

String extractJsonString(const String& json, const String& key) {
  String pattern = "\"" + key + "\":";
  int keyIndex = json.indexOf(pattern);
  if (keyIndex < 0) return "";

  int firstQuote = json.indexOf("\"", keyIndex + pattern.length());
  if (firstQuote < 0) return "";

  int secondQuote = json.indexOf("\"", firstQuote + 1);
  if (secondQuote < 0) return "";

  return json.substring(firstQuote + 1, secondQuote);
}

void updateActiveClassroom() {
  String response = getJson("/api/settings");
  String apiZone = extractJsonString(response, "active_classroom");
  if (apiZone.length() > 0) {
    currentZone = apiZone;
  }

  Serial.print("Aula activa: ");
  Serial.println(currentZone);
  lcdShow("Aula activa", currentZone);
}

void sendHeartbeat() {
  String payload = "{";
  payload += "\"gateway_id\":\"" + String(GATEWAY_ID) + "\",";
  payload += "\"zone\":\"" + escapeJson(currentZone) + "\",";
  payload += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  payload += "\"wifi_rssi\":" + String(WiFi.RSSI()) + ",";
  payload += "\"firmware_version\":\"" + String(FIRMWARE_VERSION) + "\"";
  payload += "}";

  postJson("/api/heartbeat", payload);
}

void sendTestEvent() {
  int rssiSamples[] = {-61, -78, -91};
  const char* statuses[] = {"Presente en clase", "En zonas comunes", "Ausente"};
  float confidenceSamples[] = {0.92, 0.68, 0.35};

  int index = sampleIndex % 3;
  sampleIndex++;

  String payload = "{";
  payload += "\"student_id\":\"EST-001\",";
  payload += "\"student_name\":\"Hary Sofia Banqueth Arias\",";
  payload += "\"status\":\"" + String(statuses[index]) + "\",";
  payload += "\"zone\":\"" + escapeJson(currentZone) + "\",";
  payload += "\"rssi\":" + String(rssiSamples[index]) + ",";
  payload += "\"confidence\":" + String(confidenceSamples[index], 2) + ",";
  payload += "\"source\":\"Prueba ESP32 WiFi\",";
  payload += "\"gateway_id\":\"" + String(GATEWAY_ID) + "\"";
  payload += "}";

  bool ok = postJson("/api/events", payload);
  lcdShow(ok ? String(statuses[index]) : String("Error evento"), currentZone);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  lcdBegin();

  Serial.println();
  Serial.println("AulaConectada IA - prueba rapida ESP32");
  Serial.println("Servidor: " + String(API_BASE_URL));

  if (connectWifi()) {
    updateActiveClassroom();
    sendHeartbeat();
    sendTestEvent();
    lastSend = millis();
  }
}

void loop() {
  if (millis() - lastSend >= SEND_INTERVAL_MS) {
    updateActiveClassroom();
    sendHeartbeat();
    sendTestEvent();
    lastSend = millis();
  }
}
