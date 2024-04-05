#include "PCH_Engine.h"
#include "Input.h"
#include "Event.h"
#include "Memory.h"

static Input_System* input_system_ptr = 0;
static IEngine* int_enginePtr = 0;

// NOTE!!!: This string array MUST stay in parody with e_KeyTable!!
const char* e_KeyTable_Strings[MAX_INPUT_KEYS] = {
	"UNDEFINED KEY!",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
};

const char* e_MouseButtonTable_Strings[MAX_MOUSE_BUTTONS] = {
	"UNDEFINED",

	"LEFT",
	"LEFT",
	"RIGHT",
	"RIGHT",
	"MIDDLE",
	"MIDDLE",
};

bool Input_System::InputSystem_Initialize(IEngine* enginePtr)
{
	int_enginePtr = enginePtr;

	input_system_ptr = (Input_System*)enginePtr->m_memorySystem->Mem_Allocate(sizeof(Input_System));

	int_enginePtr->m_inputSystem = input_system_ptr;

	// setup m_KeyState table
	for (char i = 0; i < MAX_INPUT_KEYS; i++)
	{
		input_system_ptr->m_keyState[i].m_char = i + 65; // +65 so that array 0 lines up with 'A'
		input_system_ptr->m_keyState[i].m_pressed = false;

		input_system_ptr->m_prevKeyState[i].m_char = i + 65; // +65 so that array 0 lines up with 'A'
		input_system_ptr->m_prevKeyState[i].m_pressed = false;
	}

	// setup m_MouseState table
	input_system_ptr->m_mouseState.m_XPos = 0.0f;
	input_system_ptr->m_mouseState.m_YPos = 0.0f;
	for (int i = 0; i < MAX_MOUSE_BUTTONS / 2; i++)
	{
		input_system_ptr->m_mouseState.m_MouseButtons[i] = false;
	}

	LOG_INFO("Input System Initialized");

	return true;
}

void Input_System::InputSystem_Shutdown(IEngine* enginePtr)
{
	LOG_VERBOSE("InputSystem_Shutdown called");

	int_enginePtr->m_memorySystem->Mem_Free(input_system_ptr);

	int_enginePtr->m_inputSystem = 0;
	int_enginePtr = 0;
}

void Input_System::InputSystem_Update()
{
	// TODO: Is this the cleanest/fastest way to handle this?
	for (char i = 0; i < MAX_INPUT_KEYS; i++)
	{
		m_prevKeyState[i].m_pressed = m_keyState[i].m_pressed;
	}
}

void Input_System::ProcessKeyInput(const int processedKeyInput, bool pressed)
{
	if (processedKeyInput != 0)
		if (pressed)
		{
			input_system_ptr->m_keyState[processedKeyInput - 65].m_pressed = true;
			int_enginePtr->m_eventSystem->EventFire(EVENT_TYPE_INPUT_KEY_PRESSED, (void*)processedKeyInput);
		}
		else
		{
			input_system_ptr->m_keyState[processedKeyInput - 65].m_pressed = false;
			int_enginePtr->m_eventSystem->EventFire(EVENT_TYPE_INPUT_KEY_RELEASED, (void*)processedKeyInput);
		}
	
	else
	{
		LOG_ERROR("Input System: Tried to process undefined keycode!");
	}
}

void Input_System::ProcessMouseInput(float mouseXPos, float mouseYPos, unsigned int mouseButtonFlags, bool pressed)
{
	if (mouseXPos != NULL && mouseYPos != NULL)
	{
		input_system_ptr->m_mouseState.m_XPos = mouseXPos;
		input_system_ptr->m_mouseState.m_YPos = mouseYPos;

		// LOG_DEBUG("Mouse Pos: X:%f  Y:%f", mouseXPos, mouseYPos);
	}

	if (mouseButtonFlags != NULL && pressed)
	{
		switch (mouseButtonFlags)
		{
		case 1:
			input_system_ptr->m_mouseState.m_MouseButtons[0] = true;
			break;
		case 3:
			input_system_ptr->m_mouseState.m_MouseButtons[1] = true;
			break;
		case 5:
			input_system_ptr->m_mouseState.m_MouseButtons[2] = true;
			break;
		default:
			LOG_ERROR("Error processing mouse button input!");
			break;
		}


		LOG_DEBUG("%s Mouse Button Pressed", e_MouseButtonTable_Strings[mouseButtonFlags]);

		int_enginePtr->m_eventSystem->EventFire(EVENT_TYPE_INPUT_MOUSE_BPRESSED, (void*)mouseButtonFlags);

	}

	if (mouseButtonFlags != NULL && !pressed)
	{
		switch (mouseButtonFlags)
		{
		case 2:
			input_system_ptr->m_mouseState.m_MouseButtons[0] = false;
			break;
		case 4:
			input_system_ptr->m_mouseState.m_MouseButtons[1] = false;
			break;
		case 6:
			input_system_ptr->m_mouseState.m_MouseButtons[2] = false;
			break;
		default:
			LOG_ERROR("Error processing mouse button input!");
			break;
		}

		LOG_DEBUG("%s Mouse Button Released", e_MouseButtonTable_Strings[mouseButtonFlags]);

		int_enginePtr->m_eventSystem->EventFire(EVENT_TYPE_INPUT_MOUSE_BRELEASED, (void*)mouseButtonFlags);
	}
}

bool Input_System::IsKeyDown(char key)
{
	if (m_keyState[key - 65].m_pressed == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Input_System::WasKeyDown(char key)
{
	if (m_prevKeyState[key - 65].m_pressed == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}