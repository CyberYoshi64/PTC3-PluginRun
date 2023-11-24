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

/**
 * @brief Read HID data
 */
void    hidRead();

/**
 * @brief Block current thread until a keymask is pressed
 * 
 * @note
 * This function will also unblock when the application is forced to close (i.e. through APT)
 * 
 * @param keys Key mask (0=Ignore)
 */
void    hidBlockUntilButton(u32 keys);

/**
 * @brief Wrapper to check if the touch position is within a box
 * 
 * @param x Box horizontal offset
 * @param y Box vertical offsert
 * @param w Box width
 * @param h Box height
 * @return `true` if the box is touched, `false` otherwise
 */
bool    hidTouchedBox(u32 x, u32 y, u32 w, u32 h);

/**
 * @brief Wrapper to check if the previous frame's touch position was within a box
 * 
 * @param x Box horizontal offset
 * @param y Box vertical offsert
 * @param w Box width
 * @param h Box height
 * @return `true` if the box was touched on the previous frame, `false` otherwise
 */
bool    hidTouchedBoxBefore(u32 x, u32 y, u32 w, u32 h);

/**
 * @brief Wrapper to check if the touch position is within an area
 * 
 * @param x1 Left end of the area
 * @param y1 Top end of the area
 * @param x2 Right end of the area
 * @param y2 Bottom end of the area
 * @return `true` if the area is touched, `false` otherwise
 */
bool    hidTouchedArea(u32 x1, u32 y1, u32 x2, u32 y2);

/**
 * @brief Wrapper to check if the previous frame's touch position was within an area
 * 
 * @param x1 Left end of the area
 * @param y1 Top end of the area
 * @param x2 Right end of the area
 * @param y2 Bottom end of the area
 * @return `true` if the area was touched on the previous frame, `false` otherwise
 */
bool    hidTouchedAreaBefore(u32 x1, u32 y1, u32 x2, u32 y2);

/// Macros

#define HID_BTNHELD         hidData.buttons.held
#define HID_BTNPRESSED      hidData.buttons.pressed
#define HID_BTNREPEATED     hidData.buttons.repeated
#define HID_BTNRELEASED     hidData.buttons.released
#define HID_TOUCHTIME       hidData.touchTimer
#define HID_TOUCHTIMEOLD    hidData.touchTimerOld
#define HID_TOUCH           hidData.touch
#define HID_TOUCHOLD        hidData.touchOld
#define HID_CIRCLE          hidData.circle
