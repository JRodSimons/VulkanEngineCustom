#pragma once

#include "Core/Event.h"

class IEngine;

enum e_KeyTable
{
	KEY_UNDEFINED = 0,

	KEY_A = 65,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	MAX_INPUT_KEYS
};

enum e_MouseButton_Table
{
	MOUSE_BUTTON_UNDEFINED = -1,

	MOUSE_BUTTON_LEFTDOWN = 1,
	MOUSE_BUTTON_LEFTUP = 2,
	MOUSE_BUTTON_RIGHTDOWN = 3,
	MOUSE_BUTTON_RIGHTUP = 4,
	MOUSE_BUTTON_MIDDLEDOWN = 5,
	MOUSE_BUTTON_MIDDLEUP = 6,

	MAX_MOUSE_BUTTONS
};

typedef struct Key_t
{
	bool m_pressed;
	char m_char;
};

struct Mouse_t
{
	float m_XPos;
	float m_YPos;
	bool m_MouseButtons[MAX_MOUSE_BUTTONS / 2];	// Divide by 2 to account for button up/down	TODO: Maybe pack these into an 8 bit integer? THIS WILL CAUSE AN ERROR!!!
};

class Input_System
{
public:
	static bool InputSystem_Initialize(IEngine* enginePtr);
	void InputSystem_Shutdown(IEngine* enginePtr);
	void InputSystem_Update();

	void ProcessKeyInput(const int processedKeyInput, bool pressed);
	void ProcessMouseInput(const float mouseXPos, const float mouseYPos, unsigned int mouseButtonFlags, bool pressed);

	bool IsKeyDown(char key);
	bool WasKeyDown(char key);

	
private:
	Key_t	  m_keyState[256];
	Key_t	  m_prevKeyState[256];

	Mouse_t m_mouseState;

	Event Event_Key_Pressed;
	Event Event_Key_Released;
};
