/*
 * SubProcess.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#include "object.h"
#include "SubProcess.h"
#include "BufferedStream.h"

namespace fibjs {

#ifdef _WIN32
extern HANDLE s_hIocp;
#endif

void SubProcess::wrap_pipe(intptr_t fd, obj_ptr<BufferedStream_base>& bs)
{
#ifdef _WIN32
    CreateIoCompletionPort((HANDLE)fd, s_hIocp, 0, 0);
#endif

    obj_ptr<Stream_base> stm = new Pipe(fd);
    bs = new BufferedStream(stm);
}

result_t SubProcess::get_stdin(obj_ptr<BufferedStream_base>& retVal)
{
    if (m_stdin == NULL)
        return CHECK_ERROR(CALL_RETURN_NULL);

    retVal = m_stdin;

    return 0;
}

result_t SubProcess::get_stdout(obj_ptr<BufferedStream_base>& retVal)
{
    if (m_stdout == NULL)
        return CHECK_ERROR(CALL_RETURN_NULL);

    retVal = m_stdout;

    return 0;
}
}
