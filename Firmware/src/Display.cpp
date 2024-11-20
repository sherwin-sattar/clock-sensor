#include "Display.h"

extern NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip;

// Number of LEDs for each digit segment
const uint16_t PixelCount = 86;  


byte digits[14] = {
  0b0111111, // 0
  0b0000110, // 1
  0b1011011, // 2
  0b1001111, // 3
  0b1100110, // 4
  0b1101101, // 5
  0b1111101, // 6
  0b0000111, // 7
  0b1111111, // 8
  0b1101111, // 9
  0b0111001, // C
  0b1111001, // E
  0b1110001, // F
  0b0000000  // -
};
RgbColor black(0);

void initDisplay() {
    strip.Begin();
    strip.Show();
}

void display(int num1, int num2, int num3, int num4, int cln, RgbColor color) {
    digit1(num1, color);
    digit2(num2, color);
    digit3(num3, color);
    digit4(num4, color);
    colon(cln, color);
}

void digit1(int num1, RgbColor color) {
    for (int seg = 0; seg < 7; seg++) {
        int ledIndex = seg * 3;
        if (digits[num1] & (1 << seg)) {
            for (int i = 0; i < 3; i++) {
                strip.SetPixelColor(ledIndex + i, color);
            }
        }
    }
    strip.Show();
}

void digit2(int num2, RgbColor color) {
    for (int seg = 0; seg < 7; seg++) {
        int ledIndex = seg * 3;
        if (digits[num2] & (1 << seg)) {
            for (int i = 0; i < 3; i++) {
                strip.SetPixelColor((ledIndex + i) + 21, color);
            }
        }
    }
    strip.Show();
}

void digit3(int num3, RgbColor color) {
    for (int seg = 0; seg < 7; seg++) {
        int ledIndex = seg * 3;
        if (digits[num3] & (1 << seg)) {
            for (int i = 0; i < 3; i++) {
                strip.SetPixelColor((ledIndex + i) + 44, color);
            }
        }
    }
    strip.Show();
}

void digit4(int num4, RgbColor color) {
    for (int seg = 0; seg < 7; seg++) {
        int ledIndex = seg * 3;
        if (digits[num4] & (1 << seg)) {
            for (int i = 0; i < 3; i++) {
                strip.SetPixelColor((ledIndex + i) + 65, color);
            }
        }
    }
    strip.Show();
}

void colon(int cln, RgbColor color) {
    if (cln == 1) {
        strip.SetPixelColor(42, color);
    } else if (cln == 2) {
        strip.SetPixelColor(42, color);
        strip.SetPixelColor(43, color);
    } else {
        // Handle error for invalid cln value
    }
    strip.Show();
}

void clearDisplay() {
    for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, black);
    }
    strip.Show(); 
}