#pragma once

#include <3ds.h>
#include <citro2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

/**
 * @brief Function type declaration for buttonRender()
 * @param x Horizontal offset of the button
 * @param y Vertical offset of the button
 * @param w Button width
 * @param h Button height
 * @param selected Whether the button is selected
 * @param touched Whether the button is touched
 * @param disabled Whether the button is disabled
 */
typedef void (*ButtonRenderCB)(float x, float y, float w, float h, bool selected, bool touched, bool disabled);

typedef struct Button_s {
    struct {
        float x, y, w, h;
    } pos;
    bool selected;          // Whether the button is selected
    bool inArea;            // Whether the button is physically being touched
    bool disabled;          // Whether the button is disabled
    bool renderBox;         // Whether to render the default box around the button
    ButtonRenderCB render;  // Render callback used to draw the button content
} Button;

#define BUTTON_STRUCTSIZE   sizeof(Button)

extern float buttonAlpha;

/**
 * @brief Setup variables of a button
 * 
 * @param self Button struct
 * @param x Horizontal offset
 * @param y Vertical offset
 * @param width Button width
 * @param height Button height
 */
void buttonSetup(Button* self, float x, float y, float width, float height);

/**
 * @brief Setup variables of a button (with callback)
 * 
 * @param self Button struct
 * @param x Horizontal offset
 * @param y Vertical offset
 * @param width Button width
 * @param height Button height
 * @param render Callback function to render button with
 */
void buttonSetupCB(Button* self, float x, float y, float width, float height, ButtonRenderCB render);

/**
 * @brief Set the position of a button
 * 
 * @param self Button struct
 * @param x Horizontal offset
 * @param y Vertical offset
 */
void buttonSetPosition(Button* self, float x, float y);

/**
 * @brief Set the size of a button
 * 
 * @param self Button struct
 * @param width Button width
 * @param height Button height
 */
void buttonSetSize(Button* self, float width, float height);

/**
 * @brief Sets the render callback of a button
 * 
 * @param self Button struct
 * @param render Callback function to render button with
 */
void buttonSetCallback(Button* self, ButtonRenderCB render);

/**
 * @brief Set whether a button is enabled or not
 * 
 * @param self Button struct
 * @param enabled `true` to enable, `false` to disable
 */
void buttonSetEnabled(Button* self, bool enabled);

/**
 * @brief Set whether to render the default box around a button (regardless of callback)
 * 
 * @param self Button struct
 * @param enabled `true` to enable, `false` to disable
 */
void buttonSetDrawBox(Button* self, bool enabled);

/**
 * @brief Run input logic for the button
 * 
 * @param self Button struct
 * @return `true`, if the button was actuated, `false` otherwise
 */
bool buttonTick(Button* self);

/**
 * @brief Render the button
 * 
 * @param self Button struct
 */
void buttonRender(Button* self);