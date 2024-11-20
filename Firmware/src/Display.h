#ifndef DISPLAY_H
#define DISPLAY_H

#include <NeoPixelBus.h>

void initDisplay();
void display(int num1, int num2, int num3, int num4, int cln, RgbColor color);
void clearDisplay();
void digit1(int num1, RgbColor color);
void digit2(int num2, RgbColor color);
void digit3(int num3, RgbColor color);
void digit4(int num4, RgbColor color);
void colon(int cln, RgbColor color);

#endif