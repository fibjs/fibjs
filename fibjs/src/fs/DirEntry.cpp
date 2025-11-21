/*
 * DirEntry.cpp
 *
 *  Created on: Nov 21, 2025
 *      Author: lion
 */

#include "ifs/fs.h"
#include "object.h"
#include "DirEntry.h"
#include "Stat.h"
#include "path.h"

namespace fibjs {

void DirEntry::fill(exlib::string _name, exlib::string parent_path, uint32_t mode)
{
    name = _name;
    parentPath = parent_path;
    m_mode = mode;
}

result_t DirEntry::get_name(exlib::string& retVal)
{
    retVal = name;
    return 0;
}

result_t DirEntry::get_parentPath(exlib::string& retVal)
{
    retVal = parentPath;
    return 0;
}

result_t DirEntry::isBlockDevice(bool& retVal)
{
    retVal = S_ISBLK(m_mode);
    return 0;
}

result_t DirEntry::isCharacterDevice(bool& retVal)
{
    retVal = S_ISCHR(m_mode);
    return 0;
}

result_t DirEntry::isDirectory(bool& retVal)
{
    retVal = S_ISDIR(m_mode);
    return 0;
}

result_t DirEntry::isFIFO(bool& retVal)
{
    retVal = S_ISFIFO(m_mode);
    return 0;
}

result_t DirEntry::isFile(bool& retVal)
{
    retVal = S_ISREG(m_mode);
    return 0;
}

result_t DirEntry::isSymbolicLink(bool& retVal)
{
    retVal = S_ISLNK(m_mode);
    return 0;
}

result_t DirEntry::isSocket(bool& retVal)
{
    retVal = S_ISSOCK(m_mode);
    return 0;
}

}
