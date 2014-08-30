// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_PORT_PORT_H_
#define STORAGE_LEVELDB_PORT_PORT_H_

#include <string.h>
#include <stdio.h>
#include <utils.h>
#include <thread.h>
#include <snappy.h>

namespace leveldb
{
namespace port
{

#if defined(arm) || defined(arm64)
static const bool kLittleEndian = true;
#else
static const bool kLittleEndian = false;
#endif

#ifdef WIN32
#define snprintf _snprintf
#else
#define fread_unlocked fread
#define fwrite_unlocked fwrite
#define fflush_unlocked fflush
#define fdatasync fsync
#endif

typedef exlib::OSMutex Mutex;
typedef exlib::OSCondVar CondVar;

typedef int32_t OnceType;
#define LEVELDB_ONCE_INIT 0
inline void InitOnce(port::OnceType *once, void (*initializer)())
{
    exlib::InitOnce(once, initializer);
}

class AtomicPointer
{
private:
    void *rep_;
public:
    AtomicPointer() { }
    explicit AtomicPointer(void *p) : rep_(p) {}
    inline void *NoBarrier_Load() const
    {
        return rep_;
    }
    inline void NoBarrier_Store(void *v)
    {
        rep_ = v;
    }
    inline void *Acquire_Load() const
    {
        void *result = rep_;
        exlib::MemoryBarrier();
        return result;
    }
    inline void Release_Store(void *v)
    {
        exlib::MemoryBarrier();
        rep_ = v;
    }
};

inline bool Snappy_Compress(const char *input, size_t length,
                            ::std::string *output)
{
    output->resize(snappy::MaxCompressedLength(length));
    size_t outlen;
    snappy::RawCompress(input, length, &(*output)[0], &outlen);
    output->resize(outlen);
    return true;
}

inline bool Snappy_GetUncompressedLength(const char *input, size_t length,
        size_t *result)
{
    return snappy::GetUncompressedLength(input, length, result);
}

inline bool Snappy_Uncompress(const char *input, size_t length,
                              char *output)
{
    return snappy::RawUncompress(input, length, output);
}

inline bool GetHeapProfile(void (*func)(void *, const char *, int), void *arg)
{
    return false;
}

}  // namespace port
}  // namespace leveldb




#endif  // STORAGE_LEVELDB_PORT_PORT_H_
