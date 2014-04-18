#ifndef TCS3414_color_sensor_H
#define TCS3414_color_sensor_H

enum Color
{
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_CLEAR
};

typedef struct
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} ColorReading;

void Tcs3414Init(void);
void Tcs3414Shutdown(void);
ColorReading Tcs3414ReadAllColors(void);
uint16_t Tcs3414ReadColor(enum Color c);


#endif
