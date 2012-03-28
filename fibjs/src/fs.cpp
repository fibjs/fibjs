#include "ifs/fs.h"
#include "File.h"

namespace fibjs
{

result_t fs_base::open(const char* fname, const char* IOMode, const char* encoding, file_base*& retVal)
{
    return 0;
}

result_t fs_base::create(const char* fname, bool Overwrite, file_base*& retVal)
{
    return 0;
}

result_t fs_base::tmpFile(file_base*& retVal)
{
    retVal = new(File);
    return 0;
}

}
