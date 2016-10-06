/*
 * console_mouse.cpp
 *
 *  Created on: Oct 1, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#ifdef _WIN32

namespace fibjs
{

#define MOUSE_COORD_TO_ABS(coord, width_or_height) (((65536 * coord) / width_or_height) + (coord < 0 ? -1 : 1))

result_t console_base::moveMouse(int32_t x, int32_t y)
{
	SetCursorPos(x, y);
	return 0;
}

result_t console_base::mouseUp(exlib::string button)
{
	DWORD ev = 0;

	if (button == "left")
		ev = MOUSEEVENTF_LEFTUP;
	else if (button == "right")
		ev = MOUSEEVENTF_RIGHTUP;
	else if (button == "middle")
		ev = MOUSEEVENTF_MIDDLEUP;
	else
		return CHECK_ERROR(CALL_E_INVALIDARG);

	mouse_event(ev, 0, 0, 0, 0);

	return 0;
}

result_t console_base::mouseDown(exlib::string button)
{
	DWORD ev = 0;

	if (button == "left")
		ev = MOUSEEVENTF_LEFTDOWN;
	else if (button == "right")
		ev = MOUSEEVENTF_RIGHTDOWN;
	else if (button == "middle")
		ev = MOUSEEVENTF_MIDDLEDOWN;
	else
		return CHECK_ERROR(CALL_E_INVALIDARG);

	mouse_event(ev, 0, 0, 0, 0);

	return 0;
}

result_t console_base::clickMouse(exlib::string button, bool dbclick)
{
	result_t hr;

	hr = mouseDown(button);
	if (hr < 0)
		return hr;

	hr = mouseUp(button);
	if (hr < 0)
		return hr;

	if (dbclick)
	{
		hr = mouseDown(button);
		if (hr < 0)
			return hr;

		hr = mouseUp(button);
		if (hr < 0)
			return hr;
	}

	return 0;
}

}

#endif
