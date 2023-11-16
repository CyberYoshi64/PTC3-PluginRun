#include "hidRead.h"

HIDStruct hidData;

extern bool blockHOME;

void hidRead() {
    hidScanInput();
    hidData.buttons.held        = hidKeysHeld();
    hidData.buttons.pressed     = hidKeysDown();
    hidData.buttons.repeated    = hidKeysDownRepeat();
    hidData.buttons.released    = hidKeysUp();
    hidData.touchOld = hidData.touch;
    hidTouchRead(&hidData.touch);
    hidCircleRead(&hidData.circle);
    hidData.touchTimerOld = hidData.touchTimer;
    hidData.touchTimer++;
    if (!hidData.touch.px) hidData.touchTimer = 0;
}

void hidBlockUntilButton(u32 keys) {
    bool s = false;
    u32 held;
    while (!aptShouldClose() && (blockHOME || aptMainLoop())) {
        hidScanInput();
        held = hidKeysHeld();
        if (!s) {
            if (!(held & keys)) s = true;
        } else {
            if ((held & keys) == keys) break;
        }
        gspWaitForVBlank();
    }
    svcSleepThread(250000000);
}

bool hidTouchedBox(u32 x, u32 y, u32 w, u32 h) {
    return (
        (HID_TOUCH.px >= x) &&
        (HID_TOUCH.py >= y) &&
        (HID_TOUCH.px < x+w) &&
        (HID_TOUCH.py < y+h)
    );
}
bool hidTouchedBoxBefore(u32 x, u32 y, u32 w, u32 h) {
    return (
        (HID_TOUCHOLD.px >= x) &&
        (HID_TOUCHOLD.py >= y) &&
        (HID_TOUCHOLD.px < x+w) &&
        (HID_TOUCHOLD.py < y+h)
    );
}
bool hidTouchedArea(u32 x1, u32 y1, u32 x2, u32 y2) {
    return (
        (HID_TOUCH.px >= x1) &&
        (HID_TOUCH.py >= y1) &&
        (HID_TOUCH.px <= x2) &&
        (HID_TOUCH.py <= y2)
    );
}
bool hidTouchedAreaBefore(u32 x1, u32 y1, u32 x2, u32 y2) {
    return (
        (HID_TOUCHOLD.px >= x1) &&
        (HID_TOUCHOLD.py >= y1) &&
        (HID_TOUCHOLD.px <= x2) &&
        (HID_TOUCHOLD.py <= y2)
    );
}
