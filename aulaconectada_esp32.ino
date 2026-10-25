/*
  Aula Conectada
  ESP32-S3 + LCD 16x2 paralela + LED verde + pagina web

  Version preparada para presentar sin buzzer y sin LED rojo.

  Flujo:
  ESP32 -> WiFi -> servidor local -> pagina web
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>

// WiFi del celular o de la red que estan usando
const char* WIFI_SSID = "NOMBRE_DE_LA_RED";
const char* WIFI_PASSWORD = "CAMBIAR_CLAVE";

// IP actual del computador
const char* API_BASE_URL = "http://10.109.231.5:8080";

// Datos del gateway
const char* GATEWAY_ID = "ESP32-S3-801";
const char* DEFAULT_ZONE = "Aula 10";
const char* FIRMWARE_VERSION = "2.1.0";

// Componentes conectados
const bool USAR_LED_VERDE = true;
const bool USAR_LED_ROJO = false;
const bool USAR_BUZZER = false;

// Pines
const int LED_GREEN_PIN = 2;
const int LED_RED_PIN = 4;
const int BUZZER_PIN = 15;
const int BOOT_BUTTON_PIN = 0;

// LCD paralela
const int LCD_RS = 21;
const int LCD_E  = 20;
const int LCD_D4 = 19;
const int LCD_D5 = 18;
const int LCD_D6 = 17;
const int LCD_D7 = 16;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// true = envia eventos automaticamente cada 10 segundos.
// false = envia solo cuando presionas BOOT.
const bool AUTO_DEMO = true;

unsigned long lastHeartbeat = 0;
unsigned long lastDemoEvent = 0;
unsigned long lastButtonPress = 0;

const unsigned long HEARTBEAT_INTERVAL_MS = 30000;
const unsigned long DEMO_EVENT_INTERVAL_MS = 10000;

int eventIndex = 0;

struct Student {
  const char* id;
  const char* name;
};

Student students[] = {
  {"EST-001", "Hary Sofia Banqueth Arias"},
  {"EST-002", "Isabel Sofia Cattaneo Contreras"},
  {"EST-003", "Dariana Moreno Hurtado"},
  {"EST-004", "Alejandro Maecha Mantilla"},
  {"EST-005", "Greysis Nicole Torres Alvarado"}
};

const int STUDENT_COUNT = sizeof(students) / sizeof(students[0]);

const char* statuses[] = {
  "Presente en clase",
  "En zonas comunes",
  "Ausente"
};

String lcdFit(String text) {
  if (text.length() > 16) {
    return text.substring(0, 16);
  }

  while (text.length() < 16) {
    text += " ";
  }

  return text;
}

void lcdShow(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdFit(line1));
  lcd.setCursor(0, 1);
  lcd.print(lcdFit(line2));
}

String escapeJson(String value) {
  String out = "";

  for (unsigned int i = 0; i < value.length(); i++) {
    char c = value.charAt(i);

    if (c == '"' || c == '\\') {
      out += '\\';
    }

    out += c;
  }

  return out;
}

void beep(int times, int durationMs) {
  if (!USAR_BUZZER) {
    return;
  }

  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(durationMs);
    digitalWrite(BUZZER_PIN, LOW);
    delay(120);
  }
}

void ledGreen(bool state) {
  if (USAR_LED_VERDE) {
    digitalWrite(LED_GREEN_PIN, state ? HIGH : LOW);
  }
}

void ledRed(bool state) {
  if (USAR_LED_ROJO) {
    digitalWrite(LED_RED_PIN, state ? HIGH : LOW);
  }
}

void signalStatus(String status) {
  if (status == "Presente en clase") {
    ledGreen(true);
    ledRed(false);
    beep(2, 100);
    delay(400);
    ledGreen(false);
  } else if (status == "En zonas comunes") {
    ledGreen(true);
    ledRed(true);
    beep(1, 150);
    delay(400);
    ledGreen(false);
    ledRed(false);
  } else {
    ledGreen(false);
    ledRed(true);
    beep(3, 90);
    delay(400);
    ledRed(false);
  }
}

bool connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando WiFi");
  lcdShow("Conectando WiFi", WIFI_SSID);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi fallo");
    lcdShow("WiFi fallo", "Revise clave");
    ledRed(true);
    beep(2, 250);
    return false;
  }

  Serial.print("WiFi conectado. IP ESP32: ");
  Serial.println(WiFi.localIP());

  lcdShow("WiFi conectado", WiFi.localIP().toString());

  ledGreen(true);
  beep(1, 200);
  delay(600);
  ledGreen(false);

  return true;
}

bool postJson(String path, String payload) {
  if (WiFi.status() != WL_CONNECTED) {
    if (!connectWifi()) {
      return false;
    }
  }

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
    return true;
  }

  lcdShow("Servidor fallo", String(statusCode));
  ledRed(true);
  beep(2, 150);
  delay(500);
  ledRed(false);

  return false;
}

void sendHeartbeat() {
  String payload = "{";
  payload += "\"gateway_id\":\"" + String(GATEWAY_ID) + "\",";
  payload += "\"zone\":\"" + String(DEFAULT_ZONE) + "\",";
  payload += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  payload += "\"wifi_rssi\":" + String(WiFi.RSSI()) + ",";
  payload += "\"firmware_version\":\"" + String(FIRMWARE_VERSION) + "\"";
  payload += "}";

  postJson("/api/heartbeat", payload);
}

void sendAttendanceEvent() {
  Student student = students[eventIndex % STUDENT_COUNT];
  String status = String(statuses[eventIndex % 3]);

  int rssi = -55;
  float confidence = 0.92;

  if (status == "En zonas comunes") {
    rssi = -76;
    confidence = 0.68;
  }

  if (status == "Ausente") {
    rssi = -95;
    confidence = 0.35;
  }

  String payload = "{";
  payload += "\"student_id\":\"" + String(student.id) + "\",";
  payload += "\"student_name\":\"" + escapeJson(String(student.name)) + "\",";
  payload += "\"status\":\"" + status + "\",";
  payload += "\"zone\":\"" + String(DEFAULT_ZONE) + "\",";
  payload += "\"rssi\":" + String(rssi) + ",";
  payload += "\"confidence\":" + String(confidence, 2) + ",";
  payload += "\"source\":\"ESP32 circuito real\",";
  payload += "\"gateway_id\":\"" + String(GATEWAY_ID) + "\"";
  payload += "}";

  Serial.print("Enviando evento: ");
  Serial.print(student.name);
  Serial.print(" - ");
  Serial.println(status);

  lcdShow(String(student.name), status);

  bool ok = postJson("/api/events", payload);

  if (ok) {
    signalStatus(status);
  } else {
    ledRed(true);
    beep(2, 200);
    delay(400);
    ledRed(false);
  }

  eventIndex++;
}

void setup() {
  Serial.begin(115200);

  if (USAR_LED_VERDE) {
    pinMode(LED_GREEN_PIN, OUTPUT);
    digitalWrite(LED_GREEN_PIN, LOW);
  }

  if (USAR_LED_ROJO) {
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, LOW);
  }

  if (USAR_BUZZER) {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
  }

  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcdShow("Aula Conectada", "Iniciando...");

  delay(1000);

  Serial.println();
  Serial.println("Aula Conectada - ESP32-S3");
  Serial.print("Servidor: ");
  Serial.println(API_BASE_URL);

  if (connectWifi()) {
    sendHeartbeat();
    lcdShow("Sistema listo", DEFAULT_ZONE);
  } else {
    lcdShow("Sin WiFi", "Revisar red");
  }
}

void loop() {
  if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL_MS) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }

  if (AUTO_DEMO && millis() - lastDemoEvent > DEMO_EVENT_INTERVAL_MS) {
    sendAttendanceEvent();
    lastDemoEvent = millis();
  }

  if (digitalRead(BOOT_BUTTON_PIN) == LOW && millis() - lastButtonPress > 1000) {
    sendAttendanceEvent();
    lastButtonPress = millis();
  }

  delay(50);
}
