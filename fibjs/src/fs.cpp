#include "ifs/fs.h"
#include "File.h"

namespace fibjs
{

result_t fs_base::open(const char* fname, const char* mode, obj_ptr<file_base>& retVal)
{
    obj_ptr<File> pFile = new File;
    result_t hr;

    hr = pFile->Open(fname, mode);
    if(hr < 0)
        return hr;

    retVal = pFile;

    return 0;
}

result_t fs_base::create(const char* fname, bool Overwrite, obj_ptr<file_base>& retVal)
{
    return 0;
}

result_t fs_base::tmpFile(obj_ptr<file_base>& retVal)
{
    retVal = new(File);
    return 0;
}

}
