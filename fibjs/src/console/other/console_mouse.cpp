/*
 * console_mouse.cpp
 *
 *  Created on: Oct 1, 2016
 *      Author: lion
 */

#include "object.h"
#include "ifs/console.h"

#ifndef _WIN32

namespace fibjs
{

result_t console_base::moveMouse(int32_t x, int32_t y)
{
	return CALL_E_INVALID_CALL;
}

result_t console_base::mouseUp(exlib::string button)
{
	return CALL_E_INVALID_CALL;
}

result_t console_base::mouseDown(exlib::string button)
{
	return CALL_E_INVALID_CALL;
}

result_t console_base::clickMouse(exlib::string button, bool dbclick)
{
	return CALL_E_INVALID_CALL;
}

}

#endif
