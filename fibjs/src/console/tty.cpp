#include "ifs/console.h"
#include "ifs/tty.h"
#include "AsyncUV.h"
#include "TTYStream.h"
#include "console.h"

#ifdef _WIN32
#include <io.h>
#endif

namespace fibjs {

DECLARE_MODULE(tty);

result_t tty_base::isatty(int32_t fd, bool& retVal)
{
#ifndef _WIN32
    if (fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t hr = ::isatty(fd);
#else
    if (fd == -2) {
        retVal = true;
        return 0;
    }

    if (fd < 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t hr = _isatty(fd);
    if (hr == FALSE && _lseek(fd, 0, SEEK_CUR) < 0)
        hr = TRUE;
#endif
    if (hr < 0)
        return CHECK_ERROR(LastError());

    retVal = !!hr;
    return 0;
}

result_t TTYInputStream::get_isRaw(bool& retVal)
{
    retVal = m_isRaw;
    return 0;
}

result_t TTYInputStream::setRawMode(bool isRawMode, obj_ptr<TTYInputStream_base>& retVal)
{
    uv_call([&] {
        return uv_tty_set_mode(&this->m_tty, isRawMode ? UV_TTY_MODE_RAW : UV_TTY_MODE_NORMAL);
    });
    m_isRaw = isRawMode;

    retVal = this;

    return 0;
}

const char* TTYOutputStream::kClearToLineBeginning = "\x1b[1K";
const char* TTYOutputStream::kClearToLineEnd = "\x1b[0K";
const char* TTYOutputStream::kClearLine = "\x1b[2K";
const char* TTYOutputStream::kClearScreenDown = "\x1b[0J";

result_t TTYOutputStream::clearLine(int32_t dir)
{
    switch (dir) {
    case -1:
        outLog(console_base::C_PRINT, kClearToLineBeginning);
        break;
    case 1:
        outLog(console_base::C_PRINT, kClearToLineEnd);
        break;
    case 0:
        outLog(console_base::C_PRINT, kClearLine);
        break;
    default:
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    return 0;
}

result_t TTYOutputStream::clearScreenDown()
{
    outLog(console_base::C_PRINT, kClearScreenDown);

    return 0;
}

result_t TTYOutputStream::cursorTo(int32_t x, int32_t y, AsyncEvent* ac)
{
    if (x < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    char numStr[64];

    if (y >= 0)
        snprintf(numStr, sizeof(numStr), "\x1b[%d;%dH", y + 1, x + 1);
    else
        snprintf(numStr, sizeof(numStr), "\x1b[%dG", x + 1);

    outLog(console_base::C_PRINT, numStr);

    return 0;
}

result_t TTYOutputStream::moveCursor(int32_t dx, int32_t dy, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    char numStr[64];
    char* data = numStr;

    if (dx < 0)
        data += snprintf(data, sizeof(numStr) / 2, "\x1b[%dD", -dx);
    else if (dx > 0)
        data += snprintf(data, sizeof(numStr) / 2, "\x1b[%dC", dx);

    if (dy < 0)
        data += snprintf(data, sizeof(numStr) / 2, "\x1b[%dA", -dy);
    else if (dy > 0)
        data += snprintf(data, sizeof(numStr) / 2, "\x1b[%dB", dy);

    outLog(console_base::C_PRINT, numStr);

    return 0;
}

result_t TTYOutputStream::getWindowSize(obj_ptr<NArray>& retVal)
{
    int32_t width, height;
    uv_tty_get_winsize(&m_tty, &width, &height);

    retVal = new NArray();
    retVal->append(width);
    retVal->append(height);

    return 0;
}

result_t TTYOutputStream::get_columns(int32_t& retVal)
{
    int32_t height;
    uv_tty_get_winsize(&m_tty, &retVal, &height);

    return 0;
}

result_t TTYOutputStream::get_rows(int32_t& retVal)
{
    int32_t width;
    uv_tty_get_winsize(&m_tty, &width, &retVal);

    return 0;
}

}