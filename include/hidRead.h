#pragma once
#include <3ds.h>

typedef struct HIDStruct {
    struct {
        u32 held;
        u32 pressed;
        u32 repeated;
        u32 released;
    } buttons;
    u32             touchTimer;
    u32             touchTimerOld;
    touchPosition   touch;
    touchPosition   touchOld;
    circlePosition  circle;
} HIDStruct;

extern HIDStruct    hidData;

void    hidRead();
void    hidBlockUntilButton(u32 keys);
bool    hidTouchedBox(u32 x, u32 y, u32 w, u32 h);
bool    hidTouchedBoxBefore(u32 x, u32 y, u32 w, u32 h);
bool    hidTouchedArea(u32 x1, u32 y1, u32 x2, u32 y2);
bool    hidTouchedAreaBefore(u32 x1, u32 y1, u32 x2, u32 y2);

#define HID_BTNHELD         hidData.buttons.held
#define HID_BTNPRESSED      hidData.buttons.pressed
#define HID_BTNREPEATED     hidData.buttons.repeated
#define HID_BTNRELEASED     hidData.buttons.released
#define HID_TOUCHTIME       hidData.touchTimer
#define HID_TOUCHTIMEOLD    hidData.touchTimerOld
#define HID_TOUCH           hidData.touch
#define HID_TOUCHOLD        hidData.touchOld
#define HID_CIRCLE          hidData.circle
