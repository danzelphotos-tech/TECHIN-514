#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include "esp_sleep.h"

// ====== YOUR CREDS ======
static const char* WIFI_SSID     = "UW MPSK";
static const char* WIFI_PASS     = "mNhrgsTrPPCEV9in";

static const char* API_KEY       = "AIzaSyCasXZTdURHJAUWsKq6-FCuQYGg_Q892nI";
static const char* USER_EMAIL    = "Danzel@uw.edu";
static const char* USER_PASSWORD = "Firebase!";
static const char* RTDB_URL      = "https://danzel-techin514-lab5-default-rtdb.firebaseio.com/";
// ========================

// HC-SR04 pins (set these to what you wired)
static const int TRIG_PIN = 2;
static const int ECHO_PIN = 3;

// Strategy knobs
static const uint32_t SLEEP_SECONDS = 10;           // deep sleep stage length
static const float MOVE_THRESHOLD_CM = 10.0;        // movement threshold
static const uint32_t HEARTBEAT_SECONDS = 600;      // upload at least every 10 min

// RTC memory persists across deep sleep resets
RTC_DATA_ATTR float lastDistanceCm = -1.0;
RTC_DATA_ATTR uint32_t secondsSinceLastUpload = 0;

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
RealtimeDatabase Database;

WiFiClientSecure ssl;
using AsyncClient = AsyncClientClass;
AsyncClient aclient(ssl);

// ---------- Ultrasonic helpers ----------
float readDistanceOnceCm() {
  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo (timeout to avoid hangs)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000UL); // 25ms timeout ~ 4m
  if (duration == 0) return -1.0;

  // speed of sound: 343 m/s => 0.0343 cm/us
  // distance = (duration * 0.0343) / 2
  return (duration * 0.0343f) / 2.0f;
}

float readDistanceAvgCm(int samples = 3) {
  float sum = 0;
  int good = 0;
  for (int i = 0; i < samples; i++) {
    float d = readDistanceOnceCm();
    if (d > 0) {
      sum += d;
      good++;
    }
    delay(20);
  }
  if (good == 0) return -1.0;
  return sum / good;
}

// ---------- WiFi/Firebase ----------
bool connectWiFi(uint32_t timeoutMs = 8000) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(200);
  }
  return WiFi.status() == WL_CONNECTED;
}

bool firebaseInit(uint32_t timeoutMs = 10000) {
  // Skip cert validation (OK for lab)
  ssl.setInsecure();
  ssl.setHandshakeTimeout(5);

  // Blocking init with timeout
  initializeApp(aclient, app, getAuth(user_auth), timeoutMs);

  app.getApp<RealtimeDatabase>(Database);
  Database.url(RTDB_URL);
  return true;
}

bool uploadEvent(float distanceCm, bool moved) {
  // You can choose any paths you want
  bool ok1 = Database.set<float>(aclient, "/lab5/distance_cm", distanceCm);
  bool ok2 = Database.set<bool>(aclient, "/lab5/moved", moved);
  bool ok3 = Database.set<uint32_t>(aclient, "/lab5/uptime_since_last_upload_s", 0);

  return (aclient.lastError().code() == 0) && ok1 && ok2 && ok3;
}

// ---------- Main cycle ----------
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // ----- Stage 1: Ultrasonic reading -----
  float dist = readDistanceAvgCm(3);
  bool moved = false;

  if (dist > 0 && lastDistanceCm > 0) {
    moved = fabs(dist - lastDistanceCm) >= MOVE_THRESHOLD_CM;
  }

  // Update baseline
  if (dist > 0) lastDistanceCm = dist;

  // Track time since last upload
  secondsSinceLastUpload += SLEEP_SECONDS;

  bool shouldUpload = moved || (secondsSinceLastUpload >= HEARTBEAT_SECONDS);

  // ----- Stage 2: WiFi + Firebase (only if needed) -----
  if (shouldUpload) {
    bool wifiOk = connectWiFi();
    if (wifiOk) {
      firebaseInit();
      // allow background auth tasks a moment
      uint32_t start = millis();
      while (!app.ready() && (millis() - start) < 3000) {
        app.loop();
        delay(10);
      }

      if (app.ready()) {
        bool ok = uploadEvent(dist, moved);
        (void)ok;
        secondsSinceLastUpload = 0;
      }
    }
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }

  // ----- Stage 3: Deep Sleep -----
  esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_SECONDS * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {
  // never reached (deep sleep resets)
}