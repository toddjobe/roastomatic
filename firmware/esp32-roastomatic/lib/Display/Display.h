/*
MIT License

Copyright (c) 2025 Todd Jobe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "C:/Users/toddj/Projects/roastomatic/firmware/esp32-roastomatic/.pio/libdeps/esp32doit-devkit-v1/Adafruit SSD1306/Adafruit_SSD1306.h" // OLED display library
#include <Wire.h>                                                                                                                              // I2C library

class Display : public Adafruit_SSD1306
{
public:
    Display() : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {};

    static const int SCREEN_WIDTH = 128;  // OLED display width, in pixels
    static const int SCREEN_HEIGHT = 64;  // OLED display height, in pixels
    static const int OLED_RESET = -1;     // Reset pin # (or -1 if sharing Arduino reset pin)
    static const int OLED_ADDRESS = 0x3C; // I2C address for the OLED display

    void setup()
    {
        // Initialize the OLED display
        if (!begin(SSD1306_BLACK, OLED_ADDRESS))
        {
            Serial.println(F("SSD1306 allocation failed"));
            for (;;)
                ;
        }

        // Clear the display buffer
        clearDisplay();
        setTextColor(SSD1306_WHITE);
        setTextSize(1);
    };
    char displayArray1[8][22];
    char displayArray2[4][10];
    void set_display_row(int row, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        vsnprintf(displayArray1[row], sizeof(displayArray1[row]), format, args);
        va_end(args);
    }

    void displayArray()
    {
        clearDisplay();
        setTextSize(1);
        setCursor(0, 0);

        for (int i = 0; i < sizeof(displayArray1) / sizeof(displayArray1[0]); i++)
        {
            println(displayArray1[i]);
        }
        display();
    }
};

#endif // DISPLAY_H