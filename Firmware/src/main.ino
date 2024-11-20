#include "Display.h"
#include "Sensor.h"
#include <HTTPClient.h>
#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NeoPixelBus.h>
#include <ClosedCube_OPT3001.h>
#include "ClosedCube_HDC1080.h"
#include <Preferences.h>

#define OPT3001_ADDRESS 0x44                       // Ambient sensor address
#define hdc1080_ADDRESS 0x40                       // temp sensoe address
const char* serverUrl = "***";                     // Replace with domain address
const char *ssid = "***";                          // WiFi SSID
const char *password = "***";                      // WiFi Password
const uint16_t PixelCount = 86;                    // Total number of LEDs
const uint8_t PixelPin = 23;                       // Display Din
const int button1 = 2;                             // GPIO2 for SW1
const int button2 = 19;                            // GPIO2 for SW2
const int button3 = 17; 
int lastButton1State = HIGH;                       // Button is pulled HIGH when not pressed
int lastButton2State = HIGH;                       // Button is pulled HIGH when not pressed
int lastButton3State = HIGH;
int DST = 0;                                       // Daylight Saving Time: DST=0(UTC-5), DST=1(UTC-4)
float brioff = 1;                                  // Brightness offset
int colorIndex = 1;                                // color change
int lastcolorIndex = 1;                            
int mode = 0;                                      // Defult mode: 0=clock, 1=Temperature, 2= Humidity
int lastmode = -1;
unsigned long lastHutime = 0;                      // Last time humidity changed
unsigned long lastTetime = 0;                      // Last time temperature changed
const unsigned long interval = 2000; 
int A = 0, B = 0, C = 0, D = 0, brt = 200;
int Te1 = 0, Te2=0, Te3 = 0;
int H1 = 0, H2 = 0, H3 = 0, H4 = 0; 

Preferences preferences;
RTC_DS3231 rtc;                                    // RTC
ClosedCube_OPT3001 opt3001;                        // Ambient sensor
ClosedCube_HDC1080 hdc1080;                        // Temp sensor
WiFiUDP udp;                                       // NTP setup
NTPClient ntp(udp, "pool.ntp.org", -18000, 60000); // Montreal
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

RgbColor CLB;
RgbColor color[] = {                               // Define colors
  RgbColor(1, 0, 0),                               // Red
  RgbColor(0, 1, 0),                               // Green
  RgbColor(0, 0, 1),                               // Blue
  RgbColor(1, 0, 1),                               // Magenta
  RgbColor(1, 1, 0),                               // Yellow
  RgbColor(0, 1, 1),                               // Light blue
  RgbColor(1, 1, 1)                                // White
};

void setup() {
  Serial.begin(115200);
  Wire.begin();
  rtc.begin();
  pinMode(button1, INPUT_PULLUP);                  // Set button1 pin as input with pull-up
  pinMode(button2, INPUT_PULLUP);                  // Set button2 pin as input with pull-up
  pinMode(button3, INPUT_PULLUP);                  // Set button3 pin as input with pull-up

  initSensors();                                   // initialize hdc1080, opt3001, RTC
  preferences.begin("settings", false);
  DST = preferences.getInt("DST", 0);              // Default to 0 if no value is stored
  connectWiFi();                                   // Connect to WiFi and fetch NTPClient time
  syncRTC();                                       // Sync RTC with NTPClient time
  initDisplay();
  receive("mode", 0, 2, mode);
  receive("colorIndex", 0, 6, colorIndex);
  receive("brioff", 0.5, 10, brioff);
  receive("DST", 0, 1, DST);
}


void loop() {
  static unsigned long lastOffsetFetchTime1 = 0;
  static unsigned long lastOffsetFetchTime2 = 0;
  static unsigned long lastOffsetFetchTime3 = 0;
  unsigned long currentMillis1 = millis();
  unsigned long currentMillis2 = millis();
  unsigned long currentMillis3 = millis();
  unsigned long doublePress = 0; 
  const unsigned long delayPress = 4000;  
  static float lastbrioff = brioff;
  bool flagB = false;
  int button1State = digitalRead(button1);
  int button2State = digitalRead(button2);
  int button3State = digitalRead(button3); 

  static unsigned long lastsend = 0;
  unsigned long nowsend = millis();
  if (nowsend - lastsend > 10000) { 
    temp_hu_server ();
    lastsend = nowsend;
  }

  if (brioff != lastbrioff) {
    send("brioff", brioff);
    lastbrioff = brioff;
  }

//SWITCH 1
  if (lastButton1State == HIGH && button1State == LOW) {             
    flagB = true;
    colorIndex = (colorIndex + 1) % 7;
    send("colorIndex", colorIndex); 
    flagB = true; 
    doublePress = millis();
  }
  lastButton1State = button1State;

//SWITCH 2
  if (lastButton2State == HIGH && button2State == LOW) {
    flagB = true;
    mode = (mode + 1) % 3;
    send("mode",mode);
    flagB = true;
    doublePress = millis();
  }
  lastButton2State = button2State;

//SWITCH 3
  if (lastButton3State == HIGH && button3State == LOW) {
    flagB = true;
    DST = (DST + 1) % 2;
    send("DST", DST);
    preferences.putInt("DST", DST);
    if (WiFi.status() == WL_CONNECTED) {
      ntp.update();  
      syncRTC();
    }
  }
  lastButton3State = button3State;


// Fetch from server
  if (!flagB && currentMillis1 - lastOffsetFetchTime1 > 2000) { 
    if (millis() - doublePress > delayPress) {
      receive("colorIndex", 0, 6, colorIndex); 
      receive("mode",0, 2, mode);  
      lastOffsetFetchTime1 = currentMillis1;
    }
  }

  if (!flagB && currentMillis2 - lastOffsetFetchTime2 > 800) { 
    if (millis() - doublePress > delayPress) {
      receive("brioff", 0.5, 10, brioff);
      lastOffsetFetchTime2 = currentMillis2;
    }
  }
  
  if (!flagB && currentMillis3 - lastOffsetFetchTime3 > 30000) { 
    receive("DST", 0, 1, DST);
    lastOffsetFetchTime3 = currentMillis3;
  }



// Mode select
  if (mode == 0) {
    timeMode();
  }
  if (mode == 1) {
    tempMode();
  }
  if (mode == 2) {
    humidityMode();
  }
  lastmode = mode;
  lastcolorIndex = colorIndex;
  //delay(10); 
}


void timeMode() {
  static int lastA = -1, lastB = -1, lastC = -1, lastD = -1, lastBrt = -1;
  static float lastbrioff= -1;
  OPT3001 result = opt3001.readResult();
  int lux = result.lux;
  int brt = determine_brt(lux);
  updateTime();
  if (brt != lastBrt || brioff != lastbrioff || A != lastA || B != lastB || C != lastC || D != lastD || mode!= lastmode || lastcolorIndex != colorIndex) {
    clearDisplay();
    updateBrightness();
    updateTime();
    display(A, B, C, D, 2, CLB);
    lastBrt = brt; 
    lastbrioff= brioff;
    lastA = A;
    lastB = B;
    lastC = C;
    lastD = D;
  }
}

void tempMode() {
  static int lastTe1 = -1, lastTe2 = -1, lastTe3 = -1, lastBrt = -1;
  static float lastbrioff= -1;
  OPT3001 result = opt3001.readResult();
  int lux = result.lux;
  int brt = determine_brt(lux);

  if (millis() - lastTetime >= interval) {
    updateTemp();
    lastTetime = millis();
    if (Te1 != lastTe1 || Te2 != lastTe2 || Te3 != lastTe3) {
      lastTe1 = Te1;
      lastTe2 = Te2;
      lastTe3 = Te3;
      clearDisplay();
      display(Te1, Te2, Te3, 10, 1, CLB);
    }
  }
  if (brt != lastBrt || brioff != lastbrioff || mode != lastmode || lastcolorIndex != colorIndex) {
    updateBrightness();
    lastBrt = brt; 
    lastbrioff= brioff;
    clearDisplay();
    display(Te1, Te2, Te3, 10, 1, CLB);
  }
}

void humidityMode() {
  static int lastH1 = -1, lastH2 = -1, lastH3 = -1, lastH4 = -1, lastBrt = -1;
  static float lastbrioff= -1;
  OPT3001 result = opt3001.readResult();
  int lux = result.lux;
  int brt = determine_brt(lux);

  if (millis() - lastHutime >= interval) {
    updateHumidity ();
    lastHutime = millis();
    if (H1 != lastH1 || H2 != lastH2 || H3 != lastH3 || H4 != lastH4) {
      lastH1 = H1;
      lastH2 = H2;
      lastH3 = H3;
      lastH4 = H4;
      clearDisplay();
      display(H1, H2, H3, H4, 1, CLB);
    }
  }
  if (brt != lastBrt || brioff != lastbrioff || mode != lastmode || lastcolorIndex != colorIndex) {
    updateBrightness();
    lastBrt = brt; 
    lastbrioff= brioff;
    clearDisplay();
    display(H1, H2, H3, H4, 1, CLB);
  }
}

void send(String parameterName, int value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverUrl) + parameterName + ".php" + "?" + parameterName + "=" + String(value);
    http.begin(url);
    int httpResponseCode = http.GET();
    http.end();
  } 
}

void send(String parameterName, float value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverUrl) + parameterName + ".php" + "?" + parameterName + "=" + String(value, 2);
    http.begin(url);
    int httpResponseCode = http.GET();
    http.end();
  } 
}

void receive(String parameterName, int minVal, int maxVal, int &outputValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverUrl) + parameterName + ".php";
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      int receivedValue = payload.toInt();  
      if (receivedValue >= minVal && receivedValue <= maxVal) {
        outputValue = receivedValue;
      } 
    }
    http.end();
  } 
}

void receive(String parameterName, float minVal, float maxVal, float &outputValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverUrl) + parameterName + ".php";
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      float receivedValue = payload.toFloat();  
      if (receivedValue >= minVal && receivedValue <= maxVal) {
        outputValue = receivedValue;
      } 
    }
    http.end();
  } 
}

void connectWiFi() {
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(200);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    ntp.begin();
    ntp.update();
  } 
}

void syncRTC() {
  if (WiFi.status() == WL_CONNECTED) {
    ntp.update();
    DateTime now = DateTime(ntp.getEpochTime());
    rtc.adjust(now);
  }
}


void temp_hu_server () {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(serverUrl) + "sensor_data.php";
    http.begin(url);
    float temperature = hdc1080.readTemperature();
    float humidity = hdc1080.readHumidity();

    String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);
    http.end();
  }
}
