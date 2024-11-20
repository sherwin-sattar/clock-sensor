#include "Sensor.h"
extern float brioff;                               // Brightness offset
extern int colorIndex; 
extern int DST;

extern RTC_DS3231 rtc;
extern ClosedCube_OPT3001 opt3001;
extern ClosedCube_HDC1080 hdc1080;
#define OPT3001_ADDRESS 0x44                       // Ambient sensor address
#define hdc1080_ADDRESS 0x40                       // temp sensoe address

extern RgbColor CLB;
extern RgbColor color[];
extern int A, B, C, D;
extern int Te1, Te2, Te3;
extern int H1, H2, H3, H4;

void initSensors() {
  Wire.begin();                                    // Initialize I2C for RTC
  hdc1080.begin(hdc1080_ADDRESS);
  opt3001.begin(OPT3001_ADDRESS);
  configureSensor();

  if (!rtc.begin()) {                              // Initialize RTC
      Serial.println("RTC not found!");
      while (1);                                 
  }
}

void configureSensor() {
  OPT3001_Config newConfig;
  newConfig.RangeNumber = B1100;    
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;
  OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
}

void updateBrightness () {
  OPT3001 result = opt3001.readResult();
  int lux = result.lux;
  int brt = determine_brt(lux);
  int CB = (brt / brioff);
  if (CB < 2) {
    CB = 2;
  }
  CLB = color[colorIndex];
  CLB = RgbColor(
    (CLB.R * CB), (CLB.G * CB), (CLB.B * CB)
  );
}

int determine_brt(int lux) {
  if (0 <= lux && lux < 10) {
    return 2;
  } else if (10 <= lux && lux < 30) {
    return 10;
  } else if (30 <= lux && lux < 65) {
    return 15;
  } else if (65 <= lux && lux < 100) {
    return 20;       
  } else if (100 <= lux && lux < 200 ) {
    return 25;
  } else if (200 <= lux && lux < 300 ) {
    return 30;
  } else if (300 <= lux && lux < 400 ) {
    return 35;
  } else if (400 <= lux && lux < 500 ) {
    return 40;
  } else if (500 <= lux && lux < 600 ) {
    return 45;
  } else if (600 <= lux && lux < 700 ) {
    return 50;
  } else if (700 <= lux && lux < 800 ) {
    return 55;
  } else if (800 <= lux && lux < 1000) {
    return 60;
  } else if (1000 <= lux && lux < 2000) {
    return 80;
  } else if (2000 <= lux && lux < 3000) {
    return 100;
  } else if (10000 <= lux ) {
    return 115;
  } else {
    return -1; 
  }
}

void updateTime () {
  DateTime now = rtc.now();
  int adjustedHour = now.hour();
  if (DST == 1) {
    adjustedHour = (adjustedHour + 1) % 24;
  }
  A = adjustedHour / 10;
  B = adjustedHour % 10;
  C = now.minute() / 10;
  D = now.minute() % 10;
}

void updateTemp () {
  float temperature = hdc1080.readTemperature();
  Te1 = (int)temperature / 10;
  Te2 = (int)temperature % 10;
  Te3 = (int)(temperature * 10) % 10;
}

void updateHumidity () {
  float humidity = hdc1080.readHumidity();
  H1 = (int)humidity / 10;
  H2 = (int)humidity % 10;
  H3 = (int)(humidity * 10) % 10;
  H4 = (int)(humidity * 100) % 10;
}