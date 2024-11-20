#ifndef SENSOR_H
#define SENSOR_H

#include <NeoPixelBus.h>
#include <RTClib.h>
#include <ClosedCube_OPT3001.h>
#include "ClosedCube_HDC1080.h"

void initSensors();
void configureSensor();
void updateBrightness ();
int determine_brt(int lux);
void updateTime ();
void updateTemp ();
void updateHumidity ();

#endif // SENSOR_H