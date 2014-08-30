#include "date.h"

namespace v8
{
namespace internal
{

class _date_cache
{
public:
    _date_cache();
    ~_date_cache();

public:
    int64_t ToLocal(int64_t time_ms);
    int64_t ToUTC(int64_t time_ms);
    int32_t LocalOffset();

private:
    void *m_pdc;
};

_date_cache::_date_cache()
{
    m_pdc = new DateCache();
}

_date_cache::~_date_cache()
{
    delete (DateCache *)m_pdc;
}

int64_t _date_cache::ToLocal(int64_t time_ms)
{
    return ((DateCache *)m_pdc)->ToLocal(time_ms);
}

int64_t _date_cache::ToUTC(int64_t time_ms)
{
    return ((DateCache *)m_pdc)->ToUTC(time_ms);
}

int32_t _date_cache::LocalOffset()
{
    return ((DateCache *)m_pdc)->GetLocalOffsetFromOS() / 36000;
}

}
}  // namespace v8::internal
