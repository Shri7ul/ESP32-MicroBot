#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <UniversalTelegramBot.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include <math.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ==================================================
// 1. ASSETS & CONFIG
// ==================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SDA_PIN 8
#define SCL_PIN 9
#define BUZZER_PIN 4

#ifndef MICROBOT_AI_API_URL
#define MICROBOT_AI_API_URL ""
#endif
#ifndef MICROBOT_AI_API_KEY
#define MICROBOT_AI_API_KEY ""
#endif
#ifndef MICROBOT_AI_MODEL
#define MICROBOT_AI_MODEL ""
#endif


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// --- WEATHER ICONS ---
const unsigned char bmp_clear[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0xc0, 0x80, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x3e, 0xff, 0xff, 0x7c, 0x00, 0xff, 0xff, 0x00, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x06, 0xff, 0xff, 0x60, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x01, 0x0f, 0xf0, 0x80, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_clouds[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xfc, 0x0f, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xff, 0xf8, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x00, 0xe0, 0x1c, 0x00, 0x00, 0xc0, 0x18, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x80, 0x70, 0x00, 0x03, 0x00, 0x60, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_sun[] PROGMEM = { 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x10, 0x08, 0x04, 0x20, 0x03, 0xc0, 0x27, 0xe4, 0x07, 0xe0, 0x07, 0xe0, 0x27, 0xe4, 0x03, 0xc0, 0x04, 0x20, 0x10, 0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00 };
const unsigned char mini_cloud[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char mini_rain[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x1f, 0xf8, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfe, 0x3f, 0xfe, 0x1f, 0xfc, 0x00, 0x00, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11 };
const unsigned char bmp_tiny_drop[] PROGMEM = { 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x7c, 0x38, 0x00 };

// --- EMOTION PARTICLES (16x16) ---
const unsigned char bmp_heart[] PROGMEM = { 0x00, 0x00, 0x0c, 0x60, 0x1e, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_zzz[] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x0c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00 };
const unsigned char bmp_anger[] PROGMEM = { 0x00, 0x00, 0x11, 0x10, 0x2a, 0x90, 0x44, 0x40, 0x80, 0x20, 0x80, 0x20, 0x44, 0x40, 0x2a, 0x90, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// --- GLOBALS ---
enum DisplayMode {
  MODE_FACE,
  MODE_CLOCK,
  MODE_WEATHER,
  MODE_FORECAST,
  MODE_WORLD
};

DisplayMode currentMode = MODE_FACE;
DisplayMode previousMode = MODE_FACE;
bool notificationActive = false;
unsigned long notificationStart = 0;
String notifTitle = "";
String notifMsg = "";
portMUX_TYPE displayStateMux = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t notificationMutex;
SemaphoreHandle_t buzzerMutex;
SemaphoreHandle_t reminderMutex;
SemaphoreHandle_t aiStatusMutex;
SemaphoreHandle_t telegramMutex;
TaskHandle_t displayTaskHandle = NULL;
bool faceResetRequested = true;
bool displayRedrawRequested = true;
bool weatherRefreshRequested = false;
uint32_t displayStateVersion = 0;

#define MAX_REMINDERS 10
#define REMINDER_TITLE_LENGTH 96
#define REMINDER_CHAT_LENGTH 48
#define AI_STATUS_LENGTH 24
#define AI_PROMPT_LENGTH 512
#define AI_REQUEST_BODY_LENGTH 1024
#define AI_RESPONSE_JSON_LENGTH 6144
#define AI_ANSWER_LENGTH 3072
#define AI_TASK_STACK_SIZE 24576
#define TELEGRAM_TASK_STACK_SIZE 16384
#define IDLE_INACTIVITY_MS 180000UL

struct Reminder {
  bool active;
  uint8_t id;
  unsigned long dueAt;
  char title[REMINDER_TITLE_LENGTH];
  char chatId[REMINDER_CHAT_LENGTH];
};

struct ReminderTelegramNotice {
  char chatId[REMINDER_CHAT_LENGTH];
  char title[REMINDER_TITLE_LENGTH];
};

Reminder reminders[MAX_REMINDERS];
uint8_t nextReminderId = 1;
QueueHandle_t reminderNoticeQueue = NULL;
QueueHandle_t reminderPopupQueue = NULL;

bool aiThinking = false;
char aiStatus[AI_STATUS_LENGTH] = "";
unsigned long aiStatusUntil = 0;

struct AIRequest {
  char chatId[REMINDER_CHAT_LENGTH];
  char prompt[AI_PROMPT_LENGTH];
};

QueueHandle_t aiRequestQueue = NULL;
TaskHandle_t aiTaskHandle = NULL;
portMUX_TYPE aiRequestMux = portMUX_INITIALIZER_UNLOCKED;
bool aiRequestBusy = false;
HTTPClient aiHttpClient;
JsonDocument aiJsonDocument;
char aiRequestBody[AI_REQUEST_BODY_LENGTH];
char aiResponseJson[AI_RESPONSE_JSON_LENGTH];
char aiAnswer[AI_ANSWER_LENGTH];
long lastProcessedTelegramUpdate = 0;

#define PARTICLE_COUNT 6

struct Particle {
  int x;
  int y;
  int speed;
};

Particle particles[PARTICLE_COUNT];

// MOODS
#define MOOD_NORMAL 0
#define MOOD_HAPPY 1
#define MOOD_SURPRISED 2
#define MOOD_SLEEPY 3
#define MOOD_ANGRY 4
#define MOOD_SAD 5
#define MOOD_EXCITED 6
#define MOOD_LOVE 7
#define MOOD_SUSPICIOUS 8
#define MOOD_CRY 9
#define MOOD_WINK 10
#define MOOD_LAUGH 11
#define MOOD_SHOCK 12
#define MOOD_EVIL 13
#define MOOD_TIRED 14
#define MOOD_FOCUS 15
#define MOOD_CONFUSED 16
#define MOOD_COOL 17
int currentMood = MOOD_NORMAL;
bool manualMood = false;

enum IdleAction {
  IDLE_NONE,
  IDLE_LOOK_LEFT,
  IDLE_LOOK_RIGHT,
  IDLE_LOOK_UP,
  IDLE_LOOK_DOWN,
  IDLE_BLINK,
  IDLE_DOUBLE_BLINK,
  IDLE_LONG_BLINK,
  IDLE_SLEEPY_BLINK,
  IDLE_STRETCH,
  IDLE_YAWN,
  IDLE_SLEEP,
  IDLE_WAKE,
  IDLE_SCAN,
  IDLE_ZZZ
};

IdleAction idleAction = IDLE_NONE;
unsigned long idleActionStarted = 0;
unsigned long idleActionUntil = 0;
unsigned long idleNextActionAt = 0;
uint8_t idleBlinkCount = 0;
bool idleResetRequested = false;
unsigned long lastUserActivity = 0;

String city;         // Loaded from Preferences
String countryCode;  // Loaded from Preferences
String apiKey;       // Loaded from Preferences or defaults
String wifiSsid;     // Loaded from Preferences
String wifiPass;     // Loaded from Preferences
String telegramBotToken;  // Loaded from Preferences
String aiApiUrl;
String aiApiKey;
String aiModel;
unsigned long lastWeatherUpdate = 0;
float temperature = 0.0;
float feelsLike = 0.0;
int humidity = 0;
String weatherMain = "Loading";
String weatherDesc = "Wait...";
bool weatherDataAvailable = false;
bool forecastDataAvailable = false;
const char* TELEGRAM_OK = "\xE2\x9C\x85 ";

struct ForecastDay {
  String dayName;
  int temp;
  String iconType;
};
ForecastDay fcast[3];
const char* ntpServer = "pool.ntp.org";
String tzString;  // Loaded from Preferences (e.g. "IST-5:30")

// ==================================================
// 2. ULTRA PRO PHYSICS ENGINE
// ==================================================

struct Eye {
  float x, y;  // Current position (Top-Left)
  float w, h;  // Current size
  float targetX, targetY, targetW, targetH;

  // Pupil Physics (Secondary Motion)
  float pupilX, pupilY;
  float targetPupilX, targetPupilY;

  // Physics constants
  float velX, velY, velW, velH;
  float pVelX, pVelY;
  float k = 0.12;   // Eye Spring
  float d = 0.60;   // Eye Damping (Heavier feel)
  float pk = 0.08;  // Pupil Spring (Softer/Laggier)
  float pd = 0.50;  // Pupil Damping

  bool blinking;
  unsigned long lastBlink;
  unsigned long nextBlinkTime;

  void reset(float _x, float _y, float _w, float _h, unsigned long now) {
    x = targetX = _x;
    y = targetY = _y;
    w = targetW = _w;
    h = targetH = _h;
    pupilX = targetPupilX = 0;
    pupilY = targetPupilY = 0;
    velX = velY = velW = velH = 0;
    pVelX = pVelY = 0;
    blinking = false;
    lastBlink = now;
    nextBlinkTime = now + random(1000, 4000);
  }

  void init(float _x, float _y, float _w, float _h) {
    reset(_x, _y, _w, _h, millis());
  }

  void update() {
    // 1. Main Eye Physics
    float ax = (targetX - x) * k;
    float ay = (targetY - y) * k;
    float aw = (targetW - w) * k;
    float ah = (targetH - h) * k;

    velX = (velX + ax) * d;
    velY = (velY + ay) * d;
    velW = (velW + aw) * d;
    velH = (velH + ah) * d;

    x += velX;
    y += velY;
    w += velW;
    h += velH;

    // 2. Pupil Physics (Dragging behind)
    float pax = (targetPupilX - pupilX) * pk;
    float pay = (targetPupilY - pupilY) * pk;
    pVelX = (pVelX + pax) * pd;
    pVelY = (pVelY + pay) * pd;
    pupilX += pVelX;
    pupilY += pVelY;
  }
};

Eye leftEye, rightEye;
unsigned long lastSaccade = 0;
unsigned long saccadeInterval = 3000;
float breathVal = 0;
unsigned long breathStart = 0;

void resetEyeAnimationState(int mood) {
  float leftW = 36;
  float leftH = 36;
  float rightW = 36;
  float rightH = 36;

  switch (mood) {
    case MOOD_HAPPY:
    case MOOD_LOVE:
    case MOOD_EXCITED:
      leftW = rightW = 40;
      leftH = rightH = 32;
      break;
    case MOOD_SURPRISED:
      leftW = rightW = 30;
      leftH = rightH = 45;
      break;
    case MOOD_SLEEPY:
      leftW = rightW = 38;
      leftH = rightH = 30;
      break;
    case MOOD_ANGRY:
      leftW = rightW = 34;
      leftH = rightH = 32;
      break;
    case MOOD_SAD:
      leftW = rightW = 34;
      leftH = rightH = 40;
      break;
    case MOOD_SUSPICIOUS:
      leftW = rightW = 36;
      leftH = 20;
      rightH = 42;
      break;
    case MOOD_CRY:
      leftW = rightW = 34;
      leftH = rightH = 38;
      break;
    case MOOD_WINK:
      leftW = rightW = 36;
      leftH = rightH = 36;
      break;
    case MOOD_LAUGH:
      leftW = rightW = 40;
      leftH = rightH = 30;
      break;
    case MOOD_SHOCK:
      leftW = rightW = 28;
      leftH = rightH = 46;
      break;
    case MOOD_EVIL:
      leftW = rightW = 36;
      leftH = rightH = 30;
      break;
    case MOOD_TIRED:
      leftW = rightW = 38;
      leftH = rightH = 24;
      break;
    case MOOD_FOCUS:
      leftW = rightW = 32;
      leftH = rightH = 28;
      break;
    case MOOD_CONFUSED:
      leftW = 34;
      leftH = 38;
      rightW = 38;
      rightH = 30;
      break;
    case MOOD_COOL:
      leftW = rightW = 36;
      leftH = rightH = 34;
      break;
  }

  unsigned long now = millis();
  leftEye.reset(18, 14, leftW, leftH, now);
  rightEye.reset(74, 14, rightW, rightH, now);
  rightEye.nextBlinkTime = leftEye.nextBlinkTime;
  lastSaccade = now;
  saccadeInterval = 3000;
  breathStart = now;
  breathVal = 0;
}

enum BuzzerDriveMode {
  BUZZER_DRIVE_ACTIVE_HIGH,
  BUZZER_DRIVE_ACTIVE_LOW,
  BUZZER_DRIVE_PASSIVE
};
BuzzerDriveMode buzzerDriveMode = BUZZER_DRIVE_PASSIVE;
bool buzzerPulseActive = false;
bool buzzerPulsePassive = false;
unsigned long buzzerPulseEndsAt = 0;

// ==================================================
// 2b. CONFIG PORTAL (WiFi + API Key + Telegram via local web)
// ==================================================
#define CONFIG_AP_SSID "ESP32-MicroBot" //AP Mobe Wifi Name
#define CONFIG_AP_PASS "123456789" //AP Mobe Wifi password

Preferences prefs;
WebServer configServer(80);
bool inConfigMode = false;
bool configRoutesRegistered = false;
WiFiClientSecure telegramClient;
WiFiClientSecure aiClient;
UniversalTelegramBot telegramBot("", telegramClient);

void handleNotify();
void beep(int duration = 150);
String askAI(String prompt);
void noteUserActivity();

void loadConfig() {
  prefs.begin("microbot", true);
  wifiSsid = prefs.getString("ssid", "");
  wifiPass = prefs.getString("pass", "");
  apiKey = prefs.getString("apikey", "");
  city = prefs.getString("city", "");
  countryCode = prefs.getString("country", "");
  tzString = prefs.getString("tz", "");
  telegramBotToken = prefs.getString("bottoken", "");
  aiApiUrl = prefs.getString("aiurl", MICROBOT_AI_API_URL);
  aiApiKey = prefs.getString("aikey", MICROBOT_AI_API_KEY);
  aiModel = prefs.getString("aimodel", MICROBOT_AI_MODEL);
  prefs.end();
  if (wifiSsid.isEmpty()) {
    wifiSsid = "asdggrgf)"; //local Wifi name
    wifiPass = "45542654645"; //local wifi password
    apiKey = "2c904"; //weather API Key
    city = "Dhaka"; //enter your city
    countryCode = "BD";
    tzString = "BST-6";
    telegramBotToken= "86QNzeoDzdI";
  } else {  
    if (apiKey.isEmpty()) apiKey = "21c904"; //weather API Key
    if (city.isEmpty()) city = "Dhaka"; //enter your city
    if (countryCode.isEmpty()) countryCode = "BD";
    if (tzString.isEmpty()) tzString = "BST-6";
    if (telegramBotToken.isEmpty()) telegramBotToken = "862oDzdI";
  }
}

void saveConfig(const String& s, const String& p, const String& ak,
                const String& cty, const String& ctry, const String& tz,
                const String& botToken, const String& aiUrl,
                const String& aiKey, const String& aiModelName) {
  prefs.begin("microbot", false);
  prefs.putString("ssid", s);
  prefs.putString("pass", p);
  prefs.putString("apikey", ak);
  prefs.putString("city", cty);
  prefs.putString("country", ctry);
  prefs.putString("tz", tz);
  prefs.putString("bottoken", botToken);
  prefs.putString("aiurl", aiUrl);
  prefs.putString("aikey", aiKey);
  prefs.putString("aimodel", aiModelName);
  prefs.end();
}

void handleConfigRoot() {
  prefs.begin("microbot", true);
  String sSsid = prefs.getString("ssid", "");
  String sApik = prefs.getString("apikey", "");
  String sCity = prefs.getString("city", "Idukki");
  String sCtry = prefs.getString("country", "IN");
  String sTz = prefs.getString("tz", "IST-5:30");
  String sBotToken = prefs.getString("bottoken", "");
  String sAiUrl = prefs.getString("aiurl", MICROBOT_AI_API_URL);
  String sAiKey = prefs.getString("aikey", MICROBOT_AI_API_KEY);
  String sAiModel = prefs.getString("aimodel", MICROBOT_AI_MODEL);
  prefs.end();

  String html = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>microbot Config</title>
<style>
body{font-family:sans-serif;max-width:420px;margin:30px auto;padding:24px;background:#0c1929;color:#e8f4fc;}
h1{color:#5ba3f5;margin-bottom:8px;}
input{width:100%;padding:10px;margin:6px 0;border:1px solid #2d4a6f;border-radius:6px;box-sizing:border-box;background:#1a2d47;color:#e8f4fc;}
input:focus{outline:none;border-color:#5ba3f5;}
button{width:100%;padding:12px;background:#3498db;color:#fff;border:none;border-radius:6px;font-size:16px;cursor:pointer;margin-top:16px;}
button:hover{background:#2980b9;}
label{display:block;margin-top:14px;color:#8ab4e8;font-size:14px;}
.section{margin-top:20px;padding-top:16px;border-top:1px solid #1e3a5f;}
.section-title{color:#5ba3f5;font-size:13px;margin-bottom:8px;}
</style></head><body>
<h1>microbot Setup</h1>
<form action="/save" method="POST">
<label>WiFi SSID</label><input name="ssid" placeholder="Your WiFi name" value=")rawliteral";
  html += sSsid;
  html += R"rawliteral(">
<label>WiFi Password</label><input name="pass" type="password" placeholder="WiFi password">
<div class="section"><div class="section-title">Weather (OpenWeatherMap)</div>
<label>API Key</label><input name="apikey" placeholder="API key" value=")rawliteral";
  html += sApik;
  html += R"rawliteral(">
<label>City</label><input name="city" placeholder="e.g. London" value=")rawliteral";
  html += sCity;
  html += R"rawliteral(">
<label>Country Code</label><input name="country" placeholder="e.g. IN, US, GB" value=")rawliteral";
  html += sCtry;
  html += R"rawliteral(">
</div>
<div class="section"><div class="section-title">Time</div>
<label>Timezone</label><input name="tz" placeholder="e.g. IST-5:30, EST5EDT" value=")rawliteral";
  html += sTz;
  html += R"rawliteral(">
</div>
<div class="section"><div class="section-title">Telegram</div>
<label>Bot Token</label><input name="bottoken" type="password" placeholder="123456789:ABC..." value=")rawliteral";
  html += sBotToken;
  html += R"rawliteral(">
</div>
<div class="section"><div class="section-title">AI (OpenAI-compatible API)</div>
<label>Endpoint URL</label><input name="aiurl" placeholder="https://provider.example/v1/chat/completions" value=")rawliteral";
  html += sAiUrl;
  html += R"rawliteral(">
<label>API Key</label><input name="aikey" type="password" placeholder="Provider API key" value=")rawliteral";
  html += sAiKey;
  html += R"rawliteral(">
<label>Model</label><input name="aimodel" placeholder="Provider model name" value=")rawliteral";
  html += sAiModel;
  html += R"rawliteral(">
</div>
<button type="submit">Save &amp; Reboot</button>
</form></body></html>)rawliteral";
  configServer.send(200, "text/html", html);
}

void handleConfigSave() {
  if (!configServer.hasArg("ssid") || configServer.arg("ssid").length() == 0) {
    configServer.send(400, "text/plain", "SSID required");
    return;
  }
  String s = configServer.arg("ssid");
  String p = configServer.arg("pass");
  String ak = configServer.arg("apikey");
  String cty = configServer.arg("city");
  String ctr = configServer.arg("country");
  String tz = configServer.arg("tz");
  String botToken = configServer.arg("bottoken");
  String aiUrl = configServer.arg("aiurl");
  String aiKey = configServer.arg("aikey");
  String aiModelName = configServer.arg("aimodel");
  prefs.begin("microbot", true);
  if (p.isEmpty()) p = prefs.getString("pass", "");
  if (ak.isEmpty()) ak = prefs.getString("apikey", "45fcf5807a5920e2006c2b8a077d423f");
  if (cty.isEmpty()) cty = prefs.getString("city", "Idukki");
  if (ctr.isEmpty()) ctr = prefs.getString("country", "IN");
  if (tz.isEmpty()) tz = prefs.getString("tz", "IST-5:30");
  if (botToken.isEmpty()) botToken = prefs.getString("bottoken", "");
  if (aiUrl.isEmpty()) aiUrl = prefs.getString("aiurl", MICROBOT_AI_API_URL);
  if (aiKey.isEmpty()) aiKey = prefs.getString("aikey", MICROBOT_AI_API_KEY);
  if (aiModelName.isEmpty()) aiModelName = prefs.getString("aimodel", MICROBOT_AI_MODEL);
  prefs.end();
  saveConfig(s, p, ak, cty, ctr, tz, botToken, aiUrl, aiKey, aiModelName);
  configServer.send(200, "text/html",
                    "<html><body style='font-family:sans-serif;background:#0c1929;color:#e8f4fc;padding:40px;'>"
                    "<h2 style='color:#5ba3f5'>Saved!</h2><p>Rebooting in 2 seconds...</p></body></html>");
  delay(2000);
  ESP.restart();
}

void setupConfigServerRoutes() {
  if (configRoutesRegistered) return;
  configServer.on("/", handleConfigRoot);
  configServer.on("/save", HTTP_POST, handleConfigSave);
  configServer.on("/notify", handleNotify);
  configRoutesRegistered = true;
}

void startConfigPortal() {
  inConfigMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(CONFIG_AP_SSID, CONFIG_AP_PASS);
  setupConfigServerRoutes();
  configServer.begin();
  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(0, 0);
  display.print("Config mode\n\nConnect to:\n");
  display.print(CONFIG_AP_SSID);
  display.print("\n\nThen open:\n192.168.4.1");
  display.display();
}

// ==================================================
// 3. LOGIC & NETWORK
// ==================================================
const char* displayModeName(DisplayMode mode) {
  switch (mode) {
    case MODE_FACE: return "MODE_FACE";
    case MODE_CLOCK: return "MODE_CLOCK";
    case MODE_WEATHER: return "MODE_WEATHER";
    case MODE_FORECAST: return "MODE_FORECAST";
    case MODE_WORLD: return "MODE_WORLD";
  }
  return "MODE_UNKNOWN";
}

const char* moodName(int mood) {
  switch (mood) {
    case MOOD_HAPPY: return "happy";
    case MOOD_SAD: return "sad";
    case MOOD_ANGRY: return "angry";
    case MOOD_LOVE: return "love";
    case MOOD_SLEEPY: return "sleepy";
    case MOOD_EXCITED: return "excited";
    case MOOD_SUSPICIOUS: return "suspicious";
    case MOOD_SURPRISED: return "surprised";
    case MOOD_CRY: return "cry";
    case MOOD_WINK: return "wink";
    case MOOD_LAUGH: return "laugh";
    case MOOD_SHOCK: return "shock";
    case MOOD_EVIL: return "evil";
    case MOOD_TIRED: return "tired";
    case MOOD_FOCUS: return "focus";
    case MOOD_CONFUSED: return "confused";
    case MOOD_COOL: return "cool";
    case MOOD_NORMAL: return "normal";
  }
  return "unknown";
}

void wakeDisplayTask() {
  if (displayTaskHandle != NULL) {
    xTaskNotifyGive(displayTaskHandle);
  }
}

void markDisplayChangedLocked(bool resetFace) {
  displayRedrawRequested = true;
  if (resetFace) faceResetRequested = true;
  displayStateVersion++;
}

void requestCompleteRedraw(bool resetFace = false) {
  portENTER_CRITICAL(&displayStateMux);
  markDisplayChangedLocked(resetFace);
  portEXIT_CRITICAL(&displayStateMux);
  wakeDisplayTask();
}

void requestWeatherRefresh() {
  portENTER_CRITICAL(&displayStateMux);
  weatherRefreshRequested = true;
  portEXIT_CRITICAL(&displayStateMux);
  wakeDisplayTask();
}

bool takeWeatherRefreshRequest() {
  portENTER_CRITICAL(&displayStateMux);
  bool requested = weatherRefreshRequested;
  weatherRefreshRequested = false;
  portEXIT_CRITICAL(&displayStateMux);
  return requested;
}

void detectBuzzerDriveMode() {
  pinMode(BUZZER_PIN, INPUT_PULLUP);
  delay(2);
  int pullUpReading = digitalRead(BUZZER_PIN);

  pinMode(BUZZER_PIN, INPUT_PULLDOWN);
  delay(2);
  int pullDownReading = digitalRead(BUZZER_PIN);

  if (pullUpReading == pullDownReading) {
    buzzerDriveMode = pullUpReading == HIGH
      ? BUZZER_DRIVE_ACTIVE_LOW
      : BUZZER_DRIVE_ACTIVE_HIGH;
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN,
      buzzerDriveMode == BUZZER_DRIVE_ACTIVE_LOW ? HIGH : LOW);
  } else {
    buzzerDriveMode = BUZZER_DRIVE_PASSIVE;
    pinMode(BUZZER_PIN, INPUT);
  }

  Serial.print("Buzzer detected: ");
  if (buzzerDriveMode == BUZZER_DRIVE_ACTIVE_LOW) Serial.println("Active LOW");
  else if (buzzerDriveMode == BUZZER_DRIVE_ACTIVE_HIGH) Serial.println("Active HIGH");
  else Serial.println("Passive");
}

void beep(int duration) {
  if (duration <= 0) return;
  if (buzzerMutex != NULL) xSemaphoreTake(buzzerMutex, portMAX_DELAY);

  if (buzzerPulseActive && buzzerPulsePassive) {
    noTone(BUZZER_PIN);
  }

  if (buzzerDriveMode == BUZZER_DRIVE_PASSIVE) {
    tone(BUZZER_PIN, 2000, duration);
    buzzerPulsePassive = true;
  } else {
    int onLevel = buzzerDriveMode == BUZZER_DRIVE_ACTIVE_LOW ? LOW : HIGH;
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, onLevel);
    buzzerPulsePassive = false;
  }
  buzzerPulseEndsAt = millis() + duration;
  buzzerPulseActive = true;

  if (buzzerMutex != NULL) xSemaphoreGive(buzzerMutex);
}

void updateBuzzer() {
  if (buzzerMutex != NULL) xSemaphoreTake(buzzerMutex, portMAX_DELAY);
  if (buzzerPulseActive && (long)(millis() - buzzerPulseEndsAt) >= 0) {
    if (buzzerPulsePassive) {
      noTone(BUZZER_PIN);
      pinMode(BUZZER_PIN, INPUT);
    } else {
      int offLevel = buzzerDriveMode == BUZZER_DRIVE_ACTIVE_LOW ? HIGH : LOW;
      digitalWrite(BUZZER_PIN, offLevel);
    }
    buzzerPulseActive = false;
  }
  if (buzzerMutex != NULL) xSemaphoreGive(buzzerMutex);
}

void selectDisplayMode(DisplayMode mode) {
  portENTER_CRITICAL(&displayStateMux);
  currentMode = mode;
  if (notificationActive) previousMode = mode;
  markDisplayChangedLocked(mode == MODE_FACE);
  portEXIT_CRITICAL(&displayStateMux);
  Serial.print("Mode changed: ");
  Serial.println(displayModeName(mode));
  wakeDisplayTask();
}

void selectMood(int mood) {
  portENTER_CRITICAL(&displayStateMux);
  currentMood = mood;
  manualMood = true;
  currentMode = MODE_FACE;
  if (notificationActive) previousMode = MODE_FACE;
  markDisplayChangedLocked(true);
  portEXIT_CRITICAL(&displayStateMux);
  Serial.print("Mood changed: ");
  Serial.println(moodName(mood));
  Serial.println("Mode changed: MODE_FACE");
  wakeDisplayTask();
}

void showNotification(const String& title, const String& message) {
  if (notificationMutex != NULL) xSemaphoreTake(notificationMutex, portMAX_DELAY);
  notifTitle = title;
  notifMsg = message;
  if (notificationMutex != NULL) xSemaphoreGive(notificationMutex);

  portENTER_CRITICAL(&displayStateMux);
  previousMode = currentMode;
  notificationActive = true;
  notificationStart = millis();
  markDisplayChangedLocked(false);
  portEXIT_CRITICAL(&displayStateMux);
  Serial.println("Notification shown");
  wakeDisplayTask();
  beep();
}

void handleNotify() {
  if (configServer.hasArg("title") && configServer.hasArg("msg")) {
    showNotification(configServer.arg("title"), configServer.arg("msg"));
    configServer.send(200, "text/plain", "Notification Received");
  } else {
    configServer.send(400, "text/plain", "Missing Parameters");
  }
}


const unsigned char* getBigIcon(String w) {
  if (w == "Clear") return bmp_clear;
  if (w == "Clouds") return bmp_clouds;
  if (w == "Rain" || w == "Drizzle") return bmp_rain;
  return bmp_clouds;
}
const unsigned char* getMiniIcon(String w) {
  if (w == "Clear") return mini_sun;
  if (w == "Rain" || w == "Drizzle" || w == "Thunderstorm") return mini_rain;
  return mini_cloud;
}

void updateMoodBasedOnWeather() {
  int m = MOOD_NORMAL;
  if (weatherMain == "Clear") m = MOOD_HAPPY;
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") m = MOOD_SAD;
  else if (weatherMain == "Thunderstorm") m = MOOD_SURPRISED;
  else if (weatherMain == "Clouds") m = MOOD_NORMAL;
  else if (temperature > 25) m = MOOD_EXCITED;
  else if (temperature < 5) m = MOOD_SLEEPY;

  bool changed = false;
  portENTER_CRITICAL(&displayStateMux);
  if (manualMood) {
    portEXIT_CRITICAL(&displayStateMux);
    return;
  }
  if (currentMood != m) {
    currentMood = m;
    changed = true;
    markDisplayChangedLocked(currentMode == MODE_FACE);
  }
  portEXIT_CRITICAL(&displayStateMux);
  if (changed) {
    Serial.print("Mood changed: ");
    Serial.println(moodName(m));
    wakeDisplayTask();
  }
}

void noteUserActivity() {
  unsigned long now = millis();
  portENTER_CRITICAL(&displayStateMux);
  lastUserActivity = now;
  idleResetRequested = true;
  markDisplayChangedLocked(false);
  portEXIT_CRITICAL(&displayStateMux);
  wakeDisplayTask();
}

void setAIStatus(bool thinking, const char* status, unsigned long visibleForMs = 0) {
  if (aiStatusMutex != NULL) xSemaphoreTake(aiStatusMutex, portMAX_DELAY);
  aiThinking = thinking;
  snprintf(aiStatus, sizeof(aiStatus), "%s", status);
  aiStatusUntil = thinking ? 0 : millis() + visibleForMs;
  if (aiStatusMutex != NULL) xSemaphoreGive(aiStatusMutex);
  requestCompleteRedraw(false);
}

bool getAIStatusSnapshot(unsigned long now, bool& thinking, char* status, size_t statusLength) {
  if (aiStatusMutex != NULL) xSemaphoreTake(aiStatusMutex, portMAX_DELAY);
  if (!aiThinking && aiStatus[0] != '\0'
      && (long)(now - aiStatusUntil) >= 0) {
    aiStatus[0] = '\0';
  }
  thinking = aiThinking;
  snprintf(status, statusLength, "%s", aiStatus);
  bool active = thinking || status[0] != '\0';
  if (aiStatusMutex != NULL) xSemaphoreGive(aiStatusMutex);
  return active;
}

bool parseReminderArguments(const String& arguments, String& title,
                            unsigned long& durationMs) {
  int lastSpace = arguments.lastIndexOf(' ');
  if (lastSpace <= 0) return false;

  title = arguments.substring(0, lastSpace);
  String durationText = arguments.substring(lastSpace + 1);
  title.trim();
  durationText.trim();
  if (title.isEmpty() || title.length() >= REMINDER_TITLE_LENGTH
      || durationText.length() < 2) {
    return false;
  }

  char unit = tolower(durationText[durationText.length() - 1]);
  if (unit != 'm' && unit != 'h') return false;

  String amountText = durationText.substring(0, durationText.length() - 1);
  for (unsigned int i = 0; i < amountText.length(); i++) {
    if (!isDigit(amountText[i])) return false;
  }

  unsigned long amount = strtoul(amountText.c_str(), NULL, 10);
  if (amount == 0) return false;

  uint64_t multiplier = unit == 'h' ? 3600000ULL : 60000ULL;
  uint64_t requestedDuration = (uint64_t)amount * multiplier;
  if (requestedDuration > 0x7FFFFFFFULL) return false;

  durationMs = (unsigned long)requestedDuration;
  return true;
}

bool reminderIdInUse(uint8_t id) {
  for (int i = 0; i < MAX_REMINDERS; i++) {
    if (reminders[i].active && reminders[i].id == id) return true;
  }
  return false;
}

int createReminder(const String& title, const String& chatId,
                   unsigned long durationMs) {
  if (reminderMutex != NULL) xSemaphoreTake(reminderMutex, portMAX_DELAY);

  int freeIndex = -1;
  for (int i = 0; i < MAX_REMINDERS; i++) {
    if (!reminders[i].active) {
      freeIndex = i;
      break;
    }
  }

  uint8_t reminderId = 0;
  if (freeIndex >= 0) {
    for (int attempt = 0; attempt < 255; attempt++) {
      reminderId = nextReminderId++;
      if (nextReminderId == 0) nextReminderId = 1;
      if (reminderId != 0 && !reminderIdInUse(reminderId)) break;
      reminderId = 0;
    }
  }

  if (freeIndex >= 0 && reminderId != 0) {
    Reminder& reminder = reminders[freeIndex];
    reminder.active = true;
    reminder.id = reminderId;
    reminder.dueAt = millis() + durationMs;
    title.toCharArray(reminder.title, sizeof(reminder.title));
    chatId.toCharArray(reminder.chatId, sizeof(reminder.chatId));
  }

  if (reminderMutex != NULL) xSemaphoreGive(reminderMutex);
  return freeIndex >= 0 && reminderId != 0 ? reminderId : -1;
}

bool cancelReminder(uint8_t id) {
  bool cancelled = false;
  if (reminderMutex != NULL) xSemaphoreTake(reminderMutex, portMAX_DELAY);
  for (int i = 0; i < MAX_REMINDERS; i++) {
    if (reminders[i].active && reminders[i].id == id) {
      reminders[i].active = false;
      cancelled = true;
      break;
    }
  }
  if (reminderMutex != NULL) xSemaphoreGive(reminderMutex);
  return cancelled;
}

String formatRemainingTime(unsigned long remainingMs) {
  unsigned long totalMinutes = (remainingMs + 59999UL) / 60000UL;
  if (totalMinutes == 0) return "<1m";
  if (totalMinutes < 60) return String(totalMinutes) + "m";

  unsigned long hours = totalMinutes / 60;
  unsigned long minutes = totalMinutes % 60;
  if (minutes == 0) return String(hours) + "h";
  return String(hours) + "h " + String(minutes) + "m";
}

String listReminders() {
  String result = "Active reminders:";
  int count = 0;
  unsigned long now = millis();

  if (reminderMutex != NULL) xSemaphoreTake(reminderMutex, portMAX_DELAY);
  for (int i = 0; i < MAX_REMINDERS; i++) {
    if (!reminders[i].active) continue;
    unsigned long remaining = (long)(reminders[i].dueAt - now) > 0
      ? reminders[i].dueAt - now
      : 0;
    result += "\n";
    result += reminders[i].id;
    result += ". ";
    result += reminders[i].title;
    result += " (";
    result += formatRemainingTime(remaining);
    result += ")";
    count++;
  }
  if (reminderMutex != NULL) xSemaphoreGive(reminderMutex);

  return count == 0 ? "No active reminders." : result;
}

void checkReminders(unsigned long now) {
  static unsigned long lastReminderCheck = 0;
  if (now - lastReminderCheck < 250) return;
  lastReminderCheck = now;

  ReminderTelegramNotice expired[MAX_REMINDERS];
  int expiredCount = 0;

  if (reminderMutex != NULL) xSemaphoreTake(reminderMutex, portMAX_DELAY);
  for (int i = 0; i < MAX_REMINDERS; i++) {
    if (!reminders[i].active
        || (long)(now - reminders[i].dueAt) < 0) {
      continue;
    }

    snprintf(expired[expiredCount].chatId,
             sizeof(expired[expiredCount].chatId), "%s", reminders[i].chatId);
    snprintf(expired[expiredCount].title,
             sizeof(expired[expiredCount].title), "%s", reminders[i].title);
    reminders[i].active = false;
    expiredCount++;
  }
  if (reminderMutex != NULL) xSemaphoreGive(reminderMutex);

  for (int i = 0; i < expiredCount; i++) {
    if (reminderNoticeQueue != NULL
        && xQueueSend(reminderNoticeQueue, &expired[i], 0) != pdTRUE) {
      Serial.println("Reminder Telegram queue full");
    }
    if (reminderPopupQueue != NULL
        && xQueueSend(reminderPopupQueue, &expired[i], 0) != pdTRUE) {
      Serial.println("Reminder popup queue full");
    }
  }
}

void showNextReminderPopup() {
  bool notificationShowing;
  portENTER_CRITICAL(&displayStateMux);
  notificationShowing = notificationActive;
  portEXIT_CRITICAL(&displayStateMux);
  if (notificationShowing || reminderPopupQueue == NULL) return;

  ReminderTelegramNotice reminderNotice;
  if (xQueueReceive(reminderPopupQueue, &reminderNotice, 0) == pdTRUE) {
    showNotification("Reminder", reminderNotice.title);
  }
}

void sendPendingReminderNotices() {
  if (reminderNoticeQueue == NULL) return;

  ReminderTelegramNotice reminderNotice;
  while (xQueueReceive(reminderNoticeQueue, &reminderNotice, 0) == pdTRUE) {
    String message = "Reminder:\n";
    message += reminderNotice.title;
    if (!telegramBot.sendMessage(reminderNotice.chatId, message, "")) {
      xQueueSendToFront(reminderNoticeQueue, &reminderNotice, 0);
      break;
    }
  }
}

bool sendTelegramLongMessage(const String& chatId, const String& message) {
  const unsigned int chunkSize = 1200;
  unsigned int start = 0;
  bool sentAll = true;

  while (start < message.length()) {
    unsigned int end = min(start + chunkSize, message.length());
    while (end < message.length() && end > start
           && (((uint8_t)message[end] & 0xC0) == 0x80)) {
      end--;
    }
    if (end == start) end = min(start + chunkSize, message.length());
    if (!telegramBot.sendMessage(chatId, message.substring(start, end), "")) {
      sentAll = false;
      break;
    }
    start = end;
  }
  return sentAll;
}

String askAI(String prompt) {
  if (WiFi.status() != WL_CONNECTED || aiApiUrl.isEmpty()
      || aiModel.isEmpty()) {
    return "";
  }

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(20000);

  bool started = false;
  if (aiApiUrl.startsWith("https://")) {
    aiClient.setInsecure();
    started = http.begin(aiClient, aiApiUrl);
  } else {
    started = http.begin(aiApiUrl);
  }
  if (!started) return "";

  http.addHeader("Content-Type", "application/json");
  if (!aiApiKey.isEmpty()) {
    http.addHeader("Authorization", "Bearer " + aiApiKey);
  }

  JSONVar message;
  message["role"] = "user";
  message["content"] = prompt;
  JSONVar messages;
  messages[0] = message;
  JSONVar request;
  request["model"] = aiModel;
  request["messages"] = messages;

  int httpCode = http.POST(JSON.stringify(request));
  if (httpCode < 200 || httpCode >= 300) {
    Serial.print("AI request failed, HTTP ");
    Serial.println(httpCode);
    http.end();
    return "";
  }

  String payload = http.getString();
  http.end();
  JSONVar response = JSON.parse(payload);
  bool validResponse = JSON.typeof(response) == "object"
    && response.hasOwnProperty("choices")
    && JSON.typeof(response["choices"]) == "array"
    && response["choices"].length() > 0
    && JSON.typeof(response["choices"][0]) == "object"
    && response["choices"][0].hasOwnProperty("message")
    && JSON.typeof(response["choices"][0]["message"]) == "object"
    && response["choices"][0]["message"].hasOwnProperty("content")
    && JSON.typeof(response["choices"][0]["message"]["content"]) == "string";
  if (!validResponse) {
    Serial.println("AI response format was not OpenAI-compatible");
    return "";
  }

  String answer = (const char*)response["choices"][0]["message"]["content"];
  answer.trim();
  return answer;
}

void handleTelegramMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chatId = telegramBot.messages[i].chat_id;
    String text = telegramBot.messages[i].text;
    text.trim();
    noteUserActivity();

    int firstSpace = text.indexOf(' ');
    String command = firstSpace >= 0 ? text.substring(0, firstSpace) : text;
    String arguments = firstSpace >= 0 ? text.substring(firstSpace + 1) : "";
    int mentionIndex = command.indexOf('@');
    if (mentionIndex >= 0) command = command.substring(0, mentionIndex);
    command.toLowerCase();
    arguments.trim();
    Serial.print("Telegram command received: ");
    Serial.println(command);

    if (command == "/start") {
      telegramBot.sendMessage(chatId,
        "ESP32 Weather Bot commands:\n"
        "/face - Display animated eyes\n"
        "/clock - Display digital clock\n"
        "/weather - Display weather\n"
        "/forecast - Display forecast\n"
        "/world - Display world clock\n"
        "/notify <title> | <message> - Show an alert\n"
        "/remind <text> <duration> - Create a reminder\n"
        "/reminders - List active reminders\n"
        "/cancelreminder <id> - Cancel a reminder\n"
        "/ask <question> - Ask the configured AI\n"
        "/mood <name> - Change the face mood\n"
        "Moods: happy, sad, angry, sleepy, excited, love, suspicious, normal, "
        "cry, wink, laugh, shock, evil, tired, focus, confused, cool\n"
        "/config - Show the configuration page address", "");
    } else if (command == "/face") {
      selectDisplayMode(MODE_FACE);
      telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "Face screen opened.", "");
    } else if (command == "/clock") {
      selectDisplayMode(MODE_CLOCK);
      telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "Clock screen opened.", "");
    } else if (command == "/weather") {
      selectDisplayMode(MODE_WEATHER);
      requestWeatherRefresh();
      telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "Weather screen opened.", "");
    } else if (command == "/forecast") {
      selectDisplayMode(MODE_FORECAST);
      requestWeatherRefresh();
      telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "Forecast screen opened.", "");
    } else if (command == "/world") {
      selectDisplayMode(MODE_WORLD);
      telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "World clock screen opened.", "");
    } else if (command == "/notify") {
      int separator = arguments.indexOf('|');
      String title = separator >= 0 ? arguments.substring(0, separator) : "";
      String message = separator >= 0 ? arguments.substring(separator + 1) : "";
      title.trim();
      message.trim();
      if (title.isEmpty() || message.isEmpty()) {
        telegramBot.sendMessage(chatId, "Usage: /notify <title> | <message>", "");
      } else {
        showNotification(title, message);
        telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "Notification displayed.", "");
      }
    } else if (command == "/remind") {
      String reminderTitle;
      unsigned long durationMs = 0;
      if (!parseReminderArguments(arguments, reminderTitle, durationMs)) {
        telegramBot.sendMessage(chatId,
          "Usage: /remind <text> <duration>\nSupported durations: 15m, 2h", "");
      } else {
        int reminderId = createReminder(reminderTitle, chatId, durationMs);
        if (reminderId < 0) {
          telegramBot.sendMessage(chatId,
            "Reminder limit reached. Cancel one of the 10 active reminders first.", "");
        } else {
          telegramBot.sendMessage(chatId,
            String(TELEGRAM_OK) + "Reminder created.\nID: " + reminderId, "");
        }
      }
    } else if (command == "/reminders") {
      telegramBot.sendMessage(chatId, listReminders(), "");
    } else if (command == "/cancelreminder") {
      bool validId = !arguments.isEmpty();
      for (unsigned int j = 0; j < arguments.length(); j++) {
        if (!isDigit(arguments[j])) {
          validId = false;
          break;
        }
      }
      int reminderId = validId ? arguments.toInt() : 0;
      if (reminderId <= 0 || reminderId > 255) {
        telegramBot.sendMessage(chatId, "Usage: /cancelreminder <id>", "");
      } else if (cancelReminder((uint8_t)reminderId)) {
        telegramBot.sendMessage(chatId,
          String(TELEGRAM_OK) + "Reminder cancelled.", "");
      } else {
        telegramBot.sendMessage(chatId, "Reminder not found.", "");
      }
    } else if (command == "/ask") {
      if (arguments.isEmpty()) {
        telegramBot.sendMessage(chatId, "Usage: /ask <question>", "");
      } else {
        setAIStatus(true, "Thinking...");
        String answer = askAI(arguments);
        if (answer.isEmpty()) {
          setAIStatus(false, "AI unavailable", 3000);
          telegramBot.sendMessage(chatId, "AI unavailable.", "");
        } else {
          setAIStatus(false, "Done!", 3000);
          if (!sendTelegramLongMessage(chatId, answer)) {
            Serial.println("Failed to send the complete AI response");
          }
        }
      }
    } else if (command == "/mood") {
      String mood = arguments;
      mood.toLowerCase();
      int moodValue = -1;
      if (mood == "happy") moodValue = MOOD_HAPPY;
      else if (mood == "sad") moodValue = MOOD_SAD;
      else if (mood == "angry") moodValue = MOOD_ANGRY;
      else if (mood == "sleepy") moodValue = MOOD_SLEEPY;
      else if (mood == "excited") moodValue = MOOD_EXCITED;
      else if (mood == "love") moodValue = MOOD_LOVE;
      else if (mood == "suspicious") moodValue = MOOD_SUSPICIOUS;
      else if (mood == "normal") moodValue = MOOD_NORMAL;
      else if (mood == "cry") moodValue = MOOD_CRY;
      else if (mood == "wink") moodValue = MOOD_WINK;
      else if (mood == "laugh") moodValue = MOOD_LAUGH;
      else if (mood == "shock") moodValue = MOOD_SHOCK;
      else if (mood == "evil") moodValue = MOOD_EVIL;
      else if (mood == "tired") moodValue = MOOD_TIRED;
      else if (mood == "focus") moodValue = MOOD_FOCUS;
      else if (mood == "confused") moodValue = MOOD_CONFUSED;
      else if (mood == "cool") moodValue = MOOD_COOL;

      if (moodValue < 0) {
        telegramBot.sendMessage(chatId,
          "Usage: /mood happy|sad|angry|sleepy|excited|love|suspicious|normal|"
          "cry|wink|laugh|shock|evil|tired|focus|confused|cool", "");
      } else {
        selectMood(moodValue);
        telegramBot.sendMessage(chatId, String(TELEGRAM_OK) + "Mood changed to " + mood + ".", "");
      }
    } else if (command == "/config") {
      String ipAddress = WiFi.localIP().toString();
      telegramBot.sendMessage(chatId,
        "ESP IP address: " + ipAddress +
        "\nOpen http://" + ipAddress +
        "/ in a browser connected to the same WiFi network.", "");
    } else {
      telegramBot.sendMessage(chatId, "Unknown command. Send /start for the command list.", "");
    }
  }
}

void telegramTask(void* parameter) {
  (void)parameter;
  for (;;) {
    if (WiFi.status() == WL_CONNECTED && !telegramBotToken.isEmpty()) {
      sendPendingReminderNotices();
      int numNewMessages = telegramBot.getUpdates(telegramBot.last_message_received + 1);
      while (numNewMessages > 0) {
        handleTelegramMessages(numNewMessages);
        numNewMessages = telegramBot.getUpdates(telegramBot.last_message_received + 1);
      }
      sendPendingReminderNotices();
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void getWeatherAndForecast() {
  if (WiFi.status() != WL_CONNECTED) return;
  bool weatherUpdated = false;
  bool forecastUpdated = false;
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar myObject = JSON.parse(payload);
    bool validWeather = JSON.typeof(myObject) == "object"
      && myObject.hasOwnProperty("main")
      && myObject.hasOwnProperty("weather")
      && JSON.typeof(myObject["main"]) == "object"
      && myObject["main"].hasOwnProperty("temp")
      && myObject["main"].hasOwnProperty("feels_like")
      && myObject["main"].hasOwnProperty("humidity")
      && JSON.typeof(myObject["weather"]) == "array"
      && myObject["weather"].length() > 0
      && JSON.typeof(myObject["weather"][0]) == "object"
      && myObject["weather"][0].hasOwnProperty("main")
      && myObject["weather"][0].hasOwnProperty("description");
    if (validWeather) {
      float updatedTemperature = double(myObject["main"]["temp"]);
      float updatedFeelsLike = double(myObject["main"]["feels_like"]);
      int updatedHumidity = int(myObject["main"]["humidity"]);
      String updatedWeatherMain = (const char*)myObject["weather"][0]["main"];
      String updatedWeatherDesc = (const char*)myObject["weather"][0]["description"];
      if (!updatedWeatherDesc.isEmpty()) {
        updatedWeatherDesc[0] = toupper(updatedWeatherDesc[0]);
      }

      temperature = updatedTemperature;
      feelsLike = updatedFeelsLike;
      humidity = updatedHumidity;
      weatherMain = updatedWeatherMain;
      weatherDesc = updatedWeatherDesc;
      weatherDataAvailable = true;
      weatherUpdated = true;
      updateMoodBasedOnWeather();
    }
  }
  http.end();
  url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar fo = JSON.parse(payload);
    bool validForecast = JSON.typeof(fo) == "object"
      && fo.hasOwnProperty("list")
      && JSON.typeof(fo["list"]) == "array"
      && fo["list"].length() > 23;
    if (validForecast) {
      struct tm t = {};
      getLocalTime(&t);
      int today = t.tm_wday;
      const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
      int indices[3] = { 7, 15, 23 };
      for (int i = 0; i < 3; i++) {
        int idx = indices[i];
        fcast[i].temp = (int)double(fo["list"][idx]["main"]["temp"]);
        fcast[i].iconType = (const char*)fo["list"][idx]["weather"][0]["main"];
        int nextDayIndex = (today + i + 1) % 7;
        fcast[i].dayName = days[nextDayIndex];
      }
      forecastDataAvailable = true;
      forecastUpdated = true;
    }
  }
  http.end();
  if (weatherUpdated) Serial.println("Weather updated successfully");
  if (weatherUpdated || forecastUpdated) requestCompleteRedraw();
}

void scheduleNextIdleAction(unsigned long fromTime) {
  idleNextActionAt = fromTime + random(30000UL, 120001UL);
}

void startIdleAction(IdleAction action, unsigned long now) {
  idleAction = action;
  idleActionStarted = now;
  idleBlinkCount = 0;

  switch (action) {
    case IDLE_LOOK_LEFT:
    case IDLE_LOOK_RIGHT:
    case IDLE_LOOK_UP:
    case IDLE_LOOK_DOWN:
      idleActionUntil = now + random(1200UL, 2401UL);
      break;
    case IDLE_BLINK:
      idleActionUntil = now + 400;
      break;
    case IDLE_DOUBLE_BLINK:
      idleActionUntil = now + 700;
      break;
    case IDLE_LONG_BLINK:
      idleActionUntil = now + 1000;
      break;
    case IDLE_SLEEPY_BLINK:
      idleActionUntil = now + 1400;
      break;
    case IDLE_STRETCH:
      idleActionUntil = now + 1600;
      break;
    case IDLE_YAWN:
      idleActionUntil = now + 2400;
      break;
    case IDLE_SLEEP:
      idleActionUntil = now + random(5000UL, 9001UL);
      break;
    case IDLE_WAKE:
      idleActionUntil = now + 1200;
      break;
    case IDLE_SCAN:
      idleActionUntil = now + 2800;
      break;
    case IDLE_ZZZ:
      idleActionUntil = now + 3500;
      break;
    case IDLE_NONE:
      idleActionUntil = now;
      break;
  }
}

void finishIdleAction(unsigned long now) {
  idleAction = IDLE_NONE;
  idleActionStarted = 0;
  idleActionUntil = 0;
  leftEye.blinking = false;
  rightEye.blinking = false;
  leftEye.targetX = 18;
  rightEye.targetX = 74;
  leftEye.targetY = 14;
  rightEye.targetY = 14;
  leftEye.targetPupilX = rightEye.targetPupilX = 0;
  leftEye.targetPupilY = rightEye.targetPupilY = 0;
  lastSaccade = now;
  saccadeInterval = random(1200UL, 3001UL);
  leftEye.nextBlinkTime = now + random(1200UL, 3501UL);
  rightEye.nextBlinkTime = leftEye.nextBlinkTime;
  scheduleNextIdleAction(now);
}

void updateIdleBehavior(unsigned long now, bool aiStatusActive) {
  DisplayMode mode;
  bool notificationShowing;
  bool resetRequested;
  unsigned long activityTime;

  portENTER_CRITICAL(&displayStateMux);
  mode = currentMode;
  notificationShowing = notificationActive;
  activityTime = lastUserActivity;
  resetRequested = idleResetRequested;
  idleResetRequested = false;
  portEXIT_CRITICAL(&displayStateMux);

  if (resetRequested) {
    finishIdleAction(now);
    idleNextActionAt = activityTime + IDLE_INACTIVITY_MS
      + random(30000UL, 120001UL);
  }

  if (mode != MODE_FACE || notificationShowing || aiStatusActive) {
    if (idleAction != IDLE_NONE) finishIdleAction(now);
    return;
  }

  if (now - activityTime < IDLE_INACTIVITY_MS) {
    if (idleAction != IDLE_NONE) finishIdleAction(now);
    if ((long)(idleNextActionAt - activityTime) < 0) {
      idleNextActionAt = activityTime + IDLE_INACTIVITY_MS
        + random(30000UL, 120001UL);
    }
    return;
  }

  if (idleAction != IDLE_NONE) {
    if ((long)(now - idleActionUntil) >= 0) {
      if (idleAction == IDLE_SLEEP) {
        startIdleAction(IDLE_WAKE, now);
      } else {
        finishIdleAction(now);
      }
    }
    return;
  }

  if (idleNextActionAt == 0) {
    scheduleNextIdleAction(now);
    return;
  }
  if ((long)(now - idleNextActionAt) < 0) return;

  static const IdleAction naturalActions[] = {
    IDLE_LOOK_LEFT, IDLE_LOOK_RIGHT, IDLE_LOOK_UP, IDLE_LOOK_DOWN,
    IDLE_BLINK, IDLE_DOUBLE_BLINK, IDLE_LONG_BLINK, IDLE_SLEEPY_BLINK,
    IDLE_STRETCH, IDLE_YAWN, IDLE_SLEEP, IDLE_SCAN, IDLE_ZZZ
  };
  int actionIndex = random(0, sizeof(naturalActions) / sizeof(naturalActions[0]));
  startIdleAction(naturalActions[actionIndex], now);
}

void applyIdleActionTargets(unsigned long now) {
  if (idleAction == IDLE_NONE) return;

  unsigned long elapsed = now - idleActionStarted;
  float progress = idleActionUntil > idleActionStarted
    ? (float)elapsed / (float)(idleActionUntil - idleActionStarted)
    : 1.0f;
  if (progress > 1.0f) progress = 1.0f;

  switch (idleAction) {
    case IDLE_LOOK_LEFT:
      leftEye.targetPupilX = rightEye.targetPupilX = -9;
      leftEye.targetX = 15;
      rightEye.targetX = 71;
      break;
    case IDLE_LOOK_RIGHT:
      leftEye.targetPupilX = rightEye.targetPupilX = 9;
      leftEye.targetX = 21;
      rightEye.targetX = 77;
      break;
    case IDLE_LOOK_UP:
      leftEye.targetPupilY = rightEye.targetPupilY = -6;
      leftEye.targetY = rightEye.targetY = 12;
      break;
    case IDLE_LOOK_DOWN:
      leftEye.targetPupilY = rightEye.targetPupilY = 6;
      leftEye.targetY = rightEye.targetY = 16;
      break;
    case IDLE_BLINK:
      if (elapsed >= 80 && elapsed < 260) {
        leftEye.targetH = rightEye.targetH = 2;
      }
      break;
    case IDLE_DOUBLE_BLINK:
      if ((elapsed >= 60 && elapsed < 170)
          || (elapsed >= 300 && elapsed < 430)) {
        leftEye.targetH = rightEye.targetH = 2;
      }
      break;
    case IDLE_LONG_BLINK:
      if (elapsed >= 120 && elapsed < 820) {
        leftEye.targetH = rightEye.targetH = 2;
      }
      break;
    case IDLE_SLEEPY_BLINK:
      leftEye.targetPupilY = rightEye.targetPupilY = 4;
      if (elapsed >= 180 && elapsed < 1050) {
        leftEye.targetH = rightEye.targetH = 5;
      }
      break;
    case IDLE_STRETCH: {
      float stretch = sin(progress * PI);
      leftEye.targetW += 8 * stretch;
      rightEye.targetW += 8 * stretch;
      leftEye.targetH -= 6 * stretch;
      rightEye.targetH -= 6 * stretch;
      break;
    }
    case IDLE_YAWN: {
      float yawn = sin(progress * PI);
      leftEye.targetH = rightEye.targetH = 30 - 18 * yawn;
      leftEye.targetPupilY = rightEye.targetPupilY = 5;
      break;
    }
    case IDLE_SLEEP:
      leftEye.targetH = rightEye.targetH = 2;
      leftEye.targetPupilX = rightEye.targetPupilX = 0;
      break;
    case IDLE_WAKE: {
      float wake = sin(progress * PI);
      leftEye.targetH += 12 * wake;
      rightEye.targetH += 12 * wake;
      leftEye.targetW += 4 * wake;
      rightEye.targetW += 4 * wake;
      break;
    }
    case IDLE_SCAN: {
      float scan = sin(progress * TWO_PI * 2.0f) * 10;
      leftEye.targetPupilX = rightEye.targetPupilX = scan;
      leftEye.targetX = 18 + scan * 0.25f;
      rightEye.targetX = 74 + scan * 0.25f;
      break;
    }
    case IDLE_ZZZ:
      leftEye.targetH = rightEye.targetH = 2;
      break;
    case IDLE_NONE:
      break;
  }
}

void drawIdleOverlay(unsigned long now) {
  if (idleAction == IDLE_YAWN) {
    display.fillRoundRect(57, 49, 14, 13, 6, SSD1306_WHITE);
    display.fillRoundRect(61, 52, 6, 8, 3, SSD1306_BLACK);
  } else if (idleAction == IDLE_ZZZ || idleAction == IDLE_SLEEP) {
    int y = 1 + ((now / 350) % 3);
    display.drawBitmap(108, y, bmp_zzz, 16, 16, SSD1306_WHITE);
  }
}

// ==================================================
// 4. DRAWING & ANIMATION
// ==================================================

void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft) {
  int ix = (int)x;
  int iy = (int)y;
  int iw = (int)w;
  int ih = (int)h;
  if (iw < 4 || ih < 4) return;

  int right = ix + iw - 1;
  int bottom = iy + ih - 1;
  int lidDepth = ih / 5;
  if (lidDepth < 2) lidDepth = 2;
  if (lidDepth > 6) lidDepth = 6;
  if (lidDepth >= ih) lidDepth = ih - 1;

  // Keep expression masks shallow and entirely inside the eye bounds.
  if (mood == MOOD_ANGRY) {
    if (isLeft) {
      display.fillTriangle(ix, iy, right, iy, right, iy + lidDepth, SSD1306_BLACK);
    } else {
      display.fillTriangle(ix, iy, right, iy, ix, iy + lidDepth, SSD1306_BLACK);
    }
  }
  else if (mood == MOOD_SAD) {
    if (isLeft) {
      display.fillTriangle(ix, iy, right, iy, ix, iy + lidDepth, SSD1306_BLACK);
    } else {
      display.fillTriangle(ix, iy, right, iy, right, iy + lidDepth, SSD1306_BLACK);
    }
  }
  else if (mood == MOOD_HAPPY || mood == MOOD_LOVE || mood == MOOD_EXCITED) {
    display.fillTriangle(
      ix, bottom,
      right, bottom,
      ix + iw / 2, bottom - lidDepth,
      SSD1306_BLACK);
  }
  else if (mood == MOOD_SLEEPY) {
    display.fillRect(ix, iy, iw, lidDepth, SSD1306_BLACK);
  }
  else if (mood == MOOD_SUSPICIOUS) {
    if (isLeft) {
      display.fillRect(ix, iy, iw, lidDepth, SSD1306_BLACK);
    } else {
      int lowerLidDepth = lidDepth > 2 ? 2 : lidDepth;
      display.fillRect(ix, bottom - lowerLidDepth + 1, iw, lowerLidDepth, SSD1306_BLACK);
    }
  }
  else if (mood == MOOD_CRY) {
    if (isLeft) {
      display.fillTriangle(ix, iy, right, iy, ix, iy + lidDepth, SSD1306_BLACK);
    } else {
      display.fillTriangle(ix, iy, right, iy, right, iy + lidDepth, SSD1306_BLACK);
    }
    display.fillTriangle(ix, bottom, right, bottom,
                         ix + iw / 2, bottom - 2, SSD1306_BLACK);
  }
  else if (mood == MOOD_LAUGH) {
    display.fillRect(ix, iy, iw, ih / 2, SSD1306_BLACK);
    display.fillTriangle(ix, bottom, ix + iw / 3, bottom,
                         ix, bottom - lidDepth, SSD1306_BLACK);
    display.fillTriangle(right, bottom, right - iw / 3, bottom,
                         right, bottom - lidDepth, SSD1306_BLACK);
  }
  else if (mood == MOOD_EVIL) {
    if (isLeft) {
      display.fillTriangle(ix, iy, right, iy, ix, iy + lidDepth + 2, SSD1306_BLACK);
    } else {
      display.fillTriangle(ix, iy, right, iy, right, iy + lidDepth + 2, SSD1306_BLACK);
    }
  }
  else if (mood == MOOD_TIRED) {
    display.fillRect(ix, iy, iw, min(ih / 3, 8), SSD1306_BLACK);
  }
  else if (mood == MOOD_FOCUS) {
    int focusDepth = min(ih / 4, 5);
    display.fillRect(ix, iy, iw, focusDepth, SSD1306_BLACK);
    display.fillRect(ix, bottom - focusDepth + 1, iw, focusDepth, SSD1306_BLACK);
  }
  else if (mood == MOOD_CONFUSED) {
    if (isLeft) {
      display.fillRect(ix, iy, iw, lidDepth, SSD1306_BLACK);
    } else {
      display.fillRect(ix, bottom - 2, iw, 2, SSD1306_BLACK);
    }
  }
  else if (mood == MOOD_COOL) {
    display.fillRect(ix, iy, iw, min(ih / 4, 6), SSD1306_BLACK);
  }
}

void drawUltraProEye(Eye& e, bool isLeft, int mood) {
  int ix = (int)e.x;
  int iy = (int)e.y;
  int iw = (int)e.w;
  int ih = (int)e.h;
  if (iw <= 0 || ih <= 0) return;

  if (mood == MOOD_WINK && isLeft) {
    int lineY = iy + max(0, ih / 2 - 1);
    display.fillRoundRect(ix, lineY, iw, 3, 1, SSD1306_WHITE);
    return;
  }

  // 1. Draw Sclera (White base)
  int r = min(8, min(iw, ih) / 2);
  if (r > 0) {
    display.fillRoundRect(ix, iy, iw, ih, r, SSD1306_WHITE);
  } else {
    display.fillRect(ix, iy, iw, ih, SSD1306_WHITE);
  }

  // 2. Draw Pupil (Black inner eye) - Constrained to be inside
  // Calculate center of eye
  int cx = ix + iw / 2;
  int cy = iy + ih / 2;

  // Pupil size is proportional to eye size
  float pupilDivisor = 2.2f;
  if (mood == MOOD_CRY) pupilDivisor = 3.0f;
  else if (mood == MOOD_SHOCK) pupilDivisor = 3.2f;
  else if (mood == MOOD_EVIL) pupilDivisor = 3.0f;
  else if (mood == MOOD_FOCUS) pupilDivisor = 2.8f;
  else if (mood == MOOD_TIRED || mood == MOOD_COOL) pupilDivisor = 2.5f;

  int pw = max(1, (int)(iw / pupilDivisor));
  int ph = max(1, (int)(ih / pupilDivisor));
  if (pw > iw) pw = iw;
  if (ph > ih) ph = ih;

  // Apply pupil offset (Gaze)
  int px = cx + (int)e.pupilX - (pw / 2);
  int py = cy + (int)e.pupilY - (ph / 2);

  // Clamp pupil to ensure it doesn't leave the eye completely
  if (px < ix) px = ix;
  if (px + pw > ix + iw) px = ix + iw - pw;
  if (py < iy) py = iy;
  if (py + ph > iy + ih) py = iy + ih - ph;

  int pupilRadius = min(r / 2, min(pw, ph) / 2);
  if (pupilRadius > 0) {
    display.fillRoundRect(px, py, pw, ph, pupilRadius, SSD1306_BLACK);
  } else {
    display.fillRect(px, py, pw, ph, SSD1306_BLACK);
  }

  // 3. Draw Specular Highlight (The "Glint" of life)
  // A tiny white dot in the top-right of the pupil
  if (iw > 15 && ih > 15) {
    display.fillCircle(px + pw - 4, py + 4, 2, SSD1306_WHITE);
  }

  // 4. Apply Eyelid Masks (Expressions)
  drawEyelidMask(e.x, e.y, e.w, e.h, mood, isLeft);
}

unsigned long blinkDurationForMood(int mood) {
  switch (mood) {
    case MOOD_SHOCK: return 70;
    case MOOD_EVIL:
    case MOOD_FOCUS: return 90;
    case MOOD_CRY:
    case MOOD_WINK: return 180;
    case MOOD_TIRED:
    case MOOD_COOL: return 240;
    default: return 120;
  }
}

unsigned long nextBlinkDelayForMood(int mood) {
  switch (mood) {
    case MOOD_SHOCK: return random(600UL, 1401UL);
    case MOOD_LAUGH:
    case MOOD_CONFUSED: return random(1600UL, 3501UL);
    case MOOD_CRY:
    case MOOD_WINK:
    case MOOD_EVIL: return random(2500UL, 5001UL);
    case MOOD_TIRED:
    case MOOD_COOL: return random(3500UL, 7001UL);
    case MOOD_FOCUS: return random(4500UL, 7501UL);
    default: return random(2000UL, 6000UL);
  }
}

void updatePhysicsAndMood(int mood) {
  unsigned long now = millis();
  breathVal = sin((now - breathStart) / 800.0) * 1.5;  // Breathing effect

  // --- BLINK LOGIC ---
  if (idleAction == IDLE_NONE) {
    if (now > leftEye.nextBlinkTime) {
      leftEye.blinking = true;
      leftEye.lastBlink = now;
      rightEye.blinking = true;
      rightEye.lastBlink = now;
      leftEye.nextBlinkTime = now + nextBlinkDelayForMood(mood);
      rightEye.nextBlinkTime = leftEye.nextBlinkTime;
    }
    if (leftEye.blinking) {
      leftEye.targetH = 2;
      rightEye.targetH = 2;  // Close
      if (now - leftEye.lastBlink > blinkDurationForMood(mood)) {
        leftEye.blinking = false;
        rightEye.blinking = false;
      }
    }
  } else {
    leftEye.blinking = false;
    rightEye.blinking = false;
  }

  // --- SACCADE (Gaze) LOGIC ---
  if (idleAction == IDLE_NONE && !leftEye.blinking
      && now - lastSaccade > saccadeInterval) {
    lastSaccade = now;
    saccadeInterval = random(500, 3000);

    // Pick direction
    int dir = random(0, 10);
    float lx = 0, ly = 0;

    if (dir < 4) {
      lx = 0;
      ly = 0;
    }  // Center
    else if (dir == 4) {
      lx = -6;
      ly = -4;
    }  // TL
    else if (dir == 5) {
      lx = 6;
      ly = -4;
    }  // TR
    else if (dir == 6) {
      lx = -6;
      ly = 4;
    }  // BL
    else if (dir == 7) {
      lx = 6;
      ly = 4;
    }  // BR
    else if (dir == 8) {
      lx = 8;
      ly = 0;
    }  // R
    else if (dir == 9) {
      lx = -8;
      ly = 0;
    }  // L

    // Move pupil target relative to center
    leftEye.targetPupilX = lx;
    leftEye.targetPupilY = ly;
    rightEye.targetPupilX = lx;
    rightEye.targetPupilY = ly;

    // Move eye container slightly (Head follow)
    leftEye.targetX = 18 + (lx * 0.3);
    leftEye.targetY = 14 + (ly * 0.3);
    rightEye.targetX = 74 + (lx * 0.3);
    rightEye.targetY = 14 + (ly * 0.3);
  }

  // --- MOOD SHAPES (Overrides targets) ---
  if (!leftEye.blinking) {
    float baseW = 36;
    float baseH = 36;

    // Breathing effect applied to height
    baseH += breathVal;

    switch (mood) {
      case MOOD_NORMAL:
        leftEye.targetW = baseW;
        leftEye.targetH = baseH;
        rightEye.targetW = baseW;
        rightEye.targetH = baseH;
        break;
      case MOOD_HAPPY:
      case MOOD_LOVE:
      case MOOD_EXCITED:
        leftEye.targetW = 40;
        leftEye.targetH = 32;
        rightEye.targetW = 40;
        rightEye.targetH = 32;
        break;
      case MOOD_SURPRISED:
        leftEye.targetW = 30;
        leftEye.targetH = 45;
        rightEye.targetW = 30;
        rightEye.targetH = 45;
        // Jitter pupil
        leftEye.targetPupilX += random(-1, 2);
        break;
      case MOOD_SLEEPY:
        leftEye.targetW = 38;
        leftEye.targetH = 30;
        rightEye.targetW = 38;
        rightEye.targetH = 30;
        break;
      case MOOD_ANGRY:
        leftEye.targetW = 34;
        leftEye.targetH = 32;
        rightEye.targetW = 34;
        rightEye.targetH = 32;
        break;
      case MOOD_SAD:
        leftEye.targetW = 34;
        leftEye.targetH = 40;
        rightEye.targetW = 34;
        rightEye.targetH = 40;
        break;
      case MOOD_SUSPICIOUS:
        leftEye.targetW = 36;
        leftEye.targetH = 20;  // Left Squint
        rightEye.targetW = 36;
        rightEye.targetH = 42;  // Right Wide
        break;
      case MOOD_CRY: {
        float sway = sin(now * 0.002f) * 2;
        leftEye.targetW = rightEye.targetW = 34;
        leftEye.targetH = rightEye.targetH = 38;
        leftEye.targetPupilX = rightEye.targetPupilX = sway;
        leftEye.targetPupilY = rightEye.targetPupilY = 4;
        break;
      }
      case MOOD_WINK:
        leftEye.targetW = 36;
        leftEye.targetH = 2;
        rightEye.targetW = 36;
        rightEye.targetH = 36 + breathVal;
        rightEye.targetPupilX = sin(now * 0.003f) * 4;
        rightEye.targetPupilY = 0;
        break;
      case MOOD_LAUGH: {
        float bounce = sin(now * 0.012f) * 2;
        leftEye.targetW = rightEye.targetW = 40;
        leftEye.targetH = rightEye.targetH = 28;
        leftEye.targetY = rightEye.targetY = 14 + bounce;
        leftEye.targetPupilY = rightEye.targetPupilY = bounce;
        break;
      }
      case MOOD_SHOCK: {
        float jitterX = random(-2, 3);
        float jitterY = random(-1, 2);
        leftEye.targetW = rightEye.targetW = 28;
        leftEye.targetH = rightEye.targetH = 46;
        leftEye.targetPupilX = rightEye.targetPupilX = jitterX;
        leftEye.targetPupilY = rightEye.targetPupilY = jitterY;
        break;
      }
      case MOOD_EVIL: {
        float evilScan = sin(now * 0.006f) * 7;
        leftEye.targetW = rightEye.targetW = 36;
        leftEye.targetH = rightEye.targetH = 30;
        leftEye.targetPupilX = rightEye.targetPupilX = evilScan;
        leftEye.targetPupilY = rightEye.targetPupilY = 1;
        break;
      }
      case MOOD_TIRED: {
        float tiredScan = sin(now * 0.0015f) * 2;
        leftEye.targetW = rightEye.targetW = 38;
        leftEye.targetH = rightEye.targetH = 24;
        leftEye.targetY = rightEye.targetY = 18;
        leftEye.targetPupilX = rightEye.targetPupilX = tiredScan;
        leftEye.targetPupilY = rightEye.targetPupilY = 3;
        break;
      }
      case MOOD_FOCUS: {
        float focusMove = sin(now * 0.001f);
        leftEye.targetW = rightEye.targetW = 34;
        leftEye.targetH = rightEye.targetH = 22;
        leftEye.targetX = 19;
        rightEye.targetX = 73;
        leftEye.targetY = rightEye.targetY = 20;
        leftEye.targetPupilX = rightEye.targetPupilX = focusMove;
        leftEye.targetPupilY = rightEye.targetPupilY = 0;
        break;
      }
      case MOOD_CONFUSED: {
        float alternate = sin(now * 0.004f);
        leftEye.targetW = 34 + alternate * 4;
        leftEye.targetH = 36 + alternate * 5;
        rightEye.targetW = 36 - alternate * 4;
        rightEye.targetH = 34 - alternate * 5;
        leftEye.targetPupilX = sin(now * 0.007f) * 3;
        rightEye.targetPupilX = cos(now * 0.006f) * 3;
        leftEye.targetPupilY = -alternate * 2;
        rightEye.targetPupilY = alternate * 2;
        break;
      }
      case MOOD_COOL: {
        float coolScan = sin(now * 0.0018f) * 5;
        leftEye.targetW = rightEye.targetW = 36;
        leftEye.targetH = rightEye.targetH = 31;
        leftEye.targetPupilX = rightEye.targetPupilX = coolScan;
        leftEye.targetPupilY = rightEye.targetPupilY = 1;
        break;
      }
    }
  }

  applyIdleActionTargets(now);
  leftEye.update();
  rightEye.update();

  if(mood == MOOD_SUSPICIOUS){

  float scan = sin(millis()*0.005)*10;

  leftEye.targetPupilX = scan;
  rightEye.targetPupilX = scan;

}
}

void updateParticles(int mood) {

  for(int i=0;i<PARTICLE_COUNT;i++){

    particles[i].y += particles[i].speed;

    if(particles[i].y > 64){
      particles[i].y = 0;
      particles[i].x = random(0,120);
    }

    if(mood == MOOD_SAD){
      display.drawBitmap(particles[i].x, particles[i].y, bmp_tiny_drop, 8, 8, SSD1306_WHITE);
    }

    if(mood == MOOD_LOVE){
      display.drawBitmap(particles[i].x, particles[i].y, bmp_heart, 16, 16, SSD1306_WHITE);
    }

  }

  if (mood == MOOD_CRY) {
    unsigned long tearFrame = millis() / 70;
    for (int i = 0; i < 4; i++) {
      int tearX = (i % 2 == 0 ? 38 : 94) + (i / 2) * 3;
      int tearY = 38 + ((tearFrame + i * 9) % 25);
      display.drawBitmap(tearX, tearY, bmp_tiny_drop, 8, 8, SSD1306_WHITE);
    }
  }
}

void drawEmoPage(int mood) {

  updatePhysicsAndMood(mood);

  updateParticles(mood);

  drawUltraProEye(leftEye, true, mood);
  drawUltraProEye(rightEye, false, mood);
  drawIdleOverlay(millis());
}
void drawNotification() {
  // Notification হেডার আঁকা
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE); // হেডারের ব্যাকগ্রাউন্ড
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(20, 4);
  display.print("NEW ALERT"); // হেডারে লেখা

  // হেডার নিচে লাইন
  display.drawLine(0, 18, 128, 18, SSD1306_WHITE);

  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.setFont(NULL);

  // Title এবং Message অনেক বড় হলে কেটে দাও
  if (notificationMutex != NULL) xSemaphoreTake(notificationMutex, portMAX_DELAY);
  String titleToShow = notifTitle;
  String msgToShow = notifMsg;
  if (notificationMutex != NULL) xSemaphoreGive(notificationMutex);

  if (titleToShow.length() > 16) titleToShow = titleToShow.substring(0, 16) + "...";
  if (msgToShow.length() > 32) msgToShow = msgToShow.substring(0, 32) + "...";

  display.print(titleToShow);
  display.setCursor(0, 44);
  display.print(msgToShow);
}

void drawAIStatus(bool thinking, const char* status) {
  display.setFont(NULL);
  display.setTextSize(1);

  if (thinking) {
    display.fillRect(0, 0, SCREEN_WIDTH, 12, SSD1306_BLACK);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(42, 2);
    display.print("Thinking...");
    return;
  }

  display.setTextColor(SSD1306_WHITE);
  if (strcmp(status, "Done!") == 0) {
    display.setCursor(50, 24);
    display.print("Done!");
    display.setCursor(37, 40);
    display.print("Check Telegram");
  } else {
    display.setCursor(27, 31);
    display.print(status);
  }
}

// --- STANDARD PAGES ---
void drawForecastPage() {
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(20, 4);
  display.print("3-DAY FORECAST");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(42, 16, 42, 64, SSD1306_WHITE);
  display.drawLine(85, 16, 85, 64, SSD1306_WHITE);
  for (int i = 0; i < 3; i++) {
    int xStart = i * 43;
    int centerX = xStart + 21;
    display.setFont(NULL);
    String d = fcast[i].dayName;
    if (d == "") d = "Wait";
    display.setCursor(centerX - (d.length() * 3), 20);
    display.print(d);
    display.drawBitmap(centerX - 8, 28, getMiniIcon(fcast[i].iconType), 16, 16, SSD1306_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(String(fcast[i].temp).c_str(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor(centerX - (w / 2) - 2, 60);
    display.print(fcast[i].temp);
    display.fillCircle(centerX + (w / 2) + 1, 52, 2, SSD1306_WHITE);
  }
}
void drawClock() {
  struct tm t;
  if (!getLocalTime(&t)) {
    display.setFont(NULL);
    display.setCursor(30, 30);
    display.print("Syncing...");
    return;
  }
  String ampm = (t.tm_hour >= 12) ? "PM" : "AM";
  int h12 = t.tm_hour % 12;
  if (h12 == 0) h12 = 12;
  display.setTextColor(SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setCursor(114, 0);
  display.print(ampm);
  display.setFont(&FreeSansBold18pt7b);
  char timeStr[8];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", h12, t.tm_min);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 42);
  display.print(timeStr);
  display.setFont(&FreeSans9pt7b);
  char dateStr[20];
  strftime(dateStr, 20, "%a, %b %d", &t);
  display.getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 62);
  display.print(dateStr);
}

void drawWeatherAnimation() {
  unsigned long now = millis();
  uint16_t frame = now / 80;

  if (weatherMain == "Clear") {
    int phase = (frame / 2) % 4;
    display.drawLine(91 - phase, 4, 94 - phase, 7, SSD1306_WHITE);
    display.drawLine(88, 15 + phase, 93, 15 + phase, SSD1306_WHITE);
    display.drawLine(91 + phase, 27, 94 + phase, 24, SSD1306_WHITE);
  }
  else if (weatherMain == "Clouds") {
    int cloudX = 73 + ((frame / 4) % 18);
    display.drawBitmap(cloudX, 16, mini_cloud, 16, 16, SSD1306_WHITE);
    display.drawBitmap(58 + ((frame / 7) % 13), 34,
                       mini_cloud, 16, 16, SSD1306_WHITE);
  }
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") {
    int dropCount = weatherMain == "Drizzle" ? 4 : 8;
    int travel = weatherMain == "Drizzle" ? 22 : 34;
    int speed = weatherMain == "Drizzle" ? 1 : 3;
    for (int i = 0; i < dropCount; i++) {
      int x = 58 + ((i * 13 + (frame / 5)) % 34);
      int y = 12 + ((frame * speed + i * 9) % travel);
      if (weatherMain == "Drizzle") {
        display.drawPixel(x, y, SSD1306_WHITE);
      } else {
        display.drawLine(x, y, x - 1, y + 3, SSD1306_WHITE);
      }
    }
  }
  else if (weatherMain == "Thunderstorm") {
    bool flash = (now % 2400UL) < 120;
    if (flash) {
      display.drawRoundRect(93, 0, 35, 33, 6, SSD1306_WHITE);
      display.fillTriangle(81, 18, 91, 18, 85, 31, SSD1306_WHITE);
      display.fillTriangle(85, 29, 91, 27, 82, 44, SSD1306_WHITE);
    } else {
      display.drawLine(87, 24, 83, 34, SSD1306_WHITE);
      display.drawLine(83, 34, 88, 32, SSD1306_WHITE);
      display.drawLine(88, 32, 84, 43, SSD1306_WHITE);
    }
  }
  else if (weatherMain == "Snow") {
    for (int i = 0; i < 10; i++) {
      int x = 53 + ((i * 17 + frame / 3) % 42);
      int y = (frame + i * 11) % 51;
      display.drawPixel(x, y, SSD1306_WHITE);
      if ((i & 1) == 0) display.drawPixel(x + 1, y, SSD1306_WHITE);
    }
  }
  else if (weatherMain == "Mist") {
    for (int i = 0; i < 8; i++) {
      int x = 52 + ((i * 19 + frame) % 43);
      int y = 6 + ((i * 13 + frame / 6) % 40);
      display.drawPixel(x, y, SSD1306_WHITE);
    }
  }
  else if (weatherMain == "Fog") {
    int offset = (frame / 3) % 12;
    display.drawLine(50 + offset, 12, 78 + offset, 12, SSD1306_WHITE);
    display.drawLine(57 - offset, 26, 91 - offset, 26, SSD1306_WHITE);
    display.drawLine(49 + offset, 41, 82 + offset, 41, SSD1306_WHITE);
  }
}

void drawWeatherCard() {
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  if (WiFi.status() != WL_CONNECTED) {
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.print("No WiFi\n\nWeather unavailable");
    return;
  }
  if (!weatherDataAvailable) {
    display.setFont(NULL);
    display.setCursor(12, 24);
    display.print("Loading weather...");
    display.setCursor(30, 40);
    display.print("Please wait");
    return;
  }
  drawWeatherAnimation();
  display.drawBitmap(96, 0, getBigIcon(weatherMain), 32, 32, SSD1306_WHITE);
  display.setFont(&FreeSansBold9pt7b);
  String c = city;
  c.toUpperCase();
  display.setCursor(0, 14);
  if (c.length() > 9) c = c.substring(0, 8) + ".";
  display.print(c);
  display.setFont(&FreeSansBold18pt7b);
  int tempInt = (int)temperature;
  display.setCursor(0, 48);
  display.print(tempInt);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(String(tempInt).c_str(), 0, 48, &x1, &y1, &w, &h);
  display.fillCircle(x1 + w + 5, 26, 4, SSD1306_WHITE);
  display.setFont(NULL);
  display.drawBitmap(88, 32, bmp_tiny_drop, 8, 8, SSD1306_WHITE);
  display.setCursor(100, 32);
  display.print(humidity);
  display.print("%");
  display.setCursor(88, 45);
  display.print("~");
  display.print((int)feelsLike);
  display.drawLine(0, 52, 128, 52, SSD1306_WHITE);
  display.setCursor(0, 55);
  display.print(weatherDesc);
}
void drawWorldClock() {
  time_t now;
  time(&now);
  time_t indiaEpoch = now + (5 * 3600) + (30 * 60);
  time_t sydneyEpoch = now + (11 * 3600);
  struct tm* indiatm = gmtime(&indiaEpoch);
  int i_h = indiatm->tm_hour;
  int i_m = indiatm->tm_min;
  struct tm* sydneytm = gmtime(&sydneyEpoch);
  int s_h = sydneytm->tm_hour;
  int s_m = sydneytm->tm_min;
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setFont(NULL);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(32, 4);
  display.print("WORLD CLOCK");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(64, 18, 64, 54, SSD1306_WHITE);
  display.setFont(NULL);
  display.setCursor(16, 22);
  display.print("INDIA");
  display.setFont(&FreeSansBold9pt7b);
  char iStr[10];
  sprintf(iStr, "%02d:%02d", i_h, i_m);
  display.setCursor(5, 46);
  display.print(iStr);
  display.setFont(NULL);
  display.setCursor(78, 22);
  display.print("SYDNEY");
  display.setFont(&FreeSansBold9pt7b);
  char sStr[10];
  sprintf(sStr, "%02d:%02d", s_h, s_m);
  display.setCursor(69, 46);
  display.print(sStr);
  display.setFont(NULL);
  display.setCursor(28, 56);
  display.print("Use Telegram");
}

// ==================================================
// 5. BOOT & MAIN
// ==================================================

void playBootAnimation() {
display.clearDisplay();
  display.setFont(&FreeSansBold9pt7b);

  String bootText = "Micro Bot";

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(bootText, 0, 0, &x1, &y1, &w, &h);

  display.setCursor((SCREEN_WIDTH - w) / 2, 28);
  display.print(bootText);

  display.setFont(NULL);
  display.setCursor(36, 45);
  display.print("Starting...");

  display.display();

  // Loading animation
  for (int i = 0; i < 30; i++) {

    int x = 44 + (i % 6) * 8;

    display.fillCircle(x, 55, 2, SSD1306_WHITE);
    display.display();

    delay(120);

    display.fillCircle(x, 55, 2, SSD1306_BLACK);
  }
}

void setup() {
Serial.begin(115200);
Wire.begin(SDA_PIN, SCL_PIN);

displayTaskHandle = xTaskGetCurrentTaskHandle();
notificationMutex = xSemaphoreCreateMutex();
buzzerMutex = xSemaphoreCreateMutex();
reminderMutex = xSemaphoreCreateMutex();
aiStatusMutex = xSemaphoreCreateMutex();
reminderNoticeQueue = xQueueCreate(MAX_REMINDERS, sizeof(ReminderTelegramNotice));
reminderPopupQueue = xQueueCreate(MAX_REMINDERS, sizeof(ReminderTelegramNotice));
randomSeed(esp_random());
lastUserActivity = millis();
idleNextActionAt = lastUserActivity + IDLE_INACTIVITY_MS
  + random(30000UL, 120001UL);
detectBuzzerDriveMode();
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);

  loadConfig();

  // Init Eyes Center
  resetEyeAnimationState(MOOD_NORMAL);

  playBootAnimation();

  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(40, 30);
  display.print("connecting");
  display.display();
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart < 15000)) {
    delay(200);
  }
if (WiFi.status() != WL_CONNECTED) {
  startConfigPortal();
  return;
}

Serial.println("WiFi Connected!");
Serial.print("ESP32 IP Address: ");
Serial.println(WiFi.localIP());

display.clearDisplay();
display.setFont(NULL);
display.setTextSize(1);

display.setCursor(0,0);
display.println("WiFi Connected!");

display.setCursor(0,20);
display.println("IP Address:");

display.setCursor(0,35);
display.println(WiFi.localIP());

display.display();
delay(4000);

  setupConfigServerRoutes();
  configServer.begin();

  configTime(0, 0, ntpServer);
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  telegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  telegramBot.updateToken(telegramBotToken);
  telegramBot.waitForResponse = 1000;
  getWeatherAndForecast();
  lastWeatherUpdate = millis();

  if (!telegramBotToken.isEmpty()) {
    BaseType_t taskResult = xTaskCreatePinnedToCore(
      telegramTask, "telegramBot", 16384, NULL, 1, NULL, 0);
    if (taskResult != pdPASS) {
      Serial.println("Failed to start Telegram task");
    }
  } else {
    Serial.println("Telegram bot token is not configured");
  }

for(int i=0;i<PARTICLE_COUNT;i++){
  particles[i].x = random(0,120);
  particles[i].y = random(0,64);
  particles[i].speed = random(1,3);
}
}

void loop() {
  configServer.handleClient();
  updateBuzzer();

  if (inConfigMode) {
    return;
  }
  unsigned long now = millis();
  checkReminders(now);
  showNextReminderPopup();
  bool notificationClosed = false;
  DisplayMode restoredMode = MODE_FACE;

  portENTER_CRITICAL(&displayStateMux);
  if (notificationActive && now - notificationStart > 5000) {
    notificationActive = false;
    currentMode = previousMode;
    restoredMode = currentMode;
    markDisplayChangedLocked(currentMode == MODE_FACE);
    notificationClosed = true;
  }
  portEXIT_CRITICAL(&displayStateMux);

  if (notificationClosed) {
    Serial.println("Notification closed");
    Serial.print("Mode changed: ");
    Serial.println(displayModeName(restoredMode));
  }

  bool showNotificationNow;
  bool resetFaceNow;
  bool forceRedrawNow;
  DisplayMode modeToDraw;
  int moodToDraw;
  uint32_t stateVersionToDraw;
  bool aiThinkingNow;
  char aiStatusNow[AI_STATUS_LENGTH];
  bool aiStatusActive = getAIStatusSnapshot(now, aiThinkingNow,
                                             aiStatusNow, sizeof(aiStatusNow));

  updateIdleBehavior(now, aiStatusActive);

  portENTER_CRITICAL(&displayStateMux);
  showNotificationNow = notificationActive;
  modeToDraw = currentMode;
  moodToDraw = currentMood;
  resetFaceNow = !showNotificationNow && modeToDraw == MODE_FACE && faceResetRequested;
  if (resetFaceNow) faceResetRequested = false;
  forceRedrawNow = displayRedrawRequested;
  displayRedrawRequested = false;
  stateVersionToDraw = displayStateVersion;
  portEXIT_CRITICAL(&displayStateMux);

  if (resetFaceNow) resetEyeAnimationState(moodToDraw);

  display.clearDisplay();

  if (showNotificationNow) {
    drawNotification();
  } else if (aiStatusActive && !aiThinkingNow) {
    drawAIStatus(false, aiStatusNow);
  } else {
    if (aiStatusActive && aiThinkingNow) {
      drawEmoPage(moodToDraw);
      drawAIStatus(true, aiStatusNow);
    } else {
      switch (modeToDraw) {
        case MODE_FACE: drawEmoPage(moodToDraw); break;
        case MODE_CLOCK: drawClock(); break;
        case MODE_WEATHER:
          if (forceRedrawNow) Serial.println("drawWeatherCard() called");
          drawWeatherCard();
          break;
        case MODE_FORECAST: drawForecastPage(); break;
        case MODE_WORLD: drawWorldClock(); break;
      }
    }
  }

  portENTER_CRITICAL(&displayStateMux);
  bool stateChangedWhileDrawing = displayStateVersion != stateVersionToDraw;
  portEXIT_CRITICAL(&displayStateMux);

  if (!stateChangedWhileDrawing) {
    display.display();
  } else {
    ulTaskNotifyTake(pdTRUE, 0);
    return;
  }

  if (showNotificationNow) {
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(15));
    return;
  }

  if (ulTaskNotifyTake(pdTRUE, 0) > 0) {
    return;
  }

  bool refreshWeatherNow = takeWeatherRefreshRequest();
  now = millis();
  if (refreshWeatherNow || now - lastWeatherUpdate > 600000) {
    getWeatherAndForecast();
    lastWeatherUpdate = millis();
  }

  ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(15));
}
