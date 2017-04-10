/*
 * date_cache.cpp
 *
 *  Created on: Jun 5, 2016
 *      Author: lion
 */

#include "DateCache.h"
#include <stdint.h>
#include <limits>
#include <cmath>
#include <time.h>

#ifdef _WIN32
#include <stdio.h>
#include <chrono>
#include <windows.h>
#endif

namespace fibjs {

const int kMaxInt = 0x7FFFFFFF;
const int msPerSecond = 1000;

#ifdef _WIN32

class TimezoneCache {
public:
    TimezoneCache()
        : initialized_(false)
    {
    }

    void Clear()
    {
        initialized_ = false;
    }

    // Initialize timezone information. The timezone information is obtained from
    // windows. If we cannot get the timezone information we fall back to CET.
    void InitializeIfNeeded()
    {
        // Just return if timezone information has already been initialized.
        if (initialized_)
            return;

        // Initialize POSIX time zone data.
        _tzset();
        // Obtain timezone information from operating system.
        memset(&tzinfo_, 0, sizeof(tzinfo_));
        if (GetTimeZoneInformation(&tzinfo_) == TIME_ZONE_ID_INVALID) {
            // If we cannot get timezone information we fall back to CET.
            tzinfo_.Bias = -60;
            tzinfo_.StandardDate.wMonth = 10;
            tzinfo_.StandardDate.wDay = 5;
            tzinfo_.StandardDate.wHour = 3;
            tzinfo_.StandardBias = 0;
            tzinfo_.DaylightDate.wMonth = 3;
            tzinfo_.DaylightDate.wDay = 5;
            tzinfo_.DaylightDate.wHour = 2;
            tzinfo_.DaylightBias = -60;
        }

        // Timezone information initialized.
        initialized_ = true;
    }

private:
    static const int kTzNameSize = 128;
    bool initialized_;
    TIME_ZONE_INFORMATION tzinfo_;
    friend class Win32Time;
};

class Win32Time {
public:
    // Constructors.
    Win32Time();
    explicit Win32Time(double jstime);
    Win32Time(int year, int mon, int day, int hour, int min, int sec);

    // Convert timestamp to JavaScript representation.
    double ToJSTime();

    // Returns the local timezone offset in milliseconds east of UTC. This is
    // the number of milliseconds you must add to UTC to get local time, i.e.
    // LocalOffset(CET) = 3600000 and LocalOffset(PST) = -28800000. This
    // routine also takes into account whether daylight saving is effect
    // at the time.
    int64_t LocalOffset(TimezoneCache* cache);

    // Returns the daylight savings time offset for the time in milliseconds.
    int64_t DaylightSavingsOffset(TimezoneCache* cache);

private:
    // Constants for time conversion.
    static const int64_t kTimeEpoc = 116444736000000000LL;
    static const int64_t kTimeScaler = 10000;
    static const int64_t kMsPerMinute = 60000;

    // Constants for timezone information.
    static const bool kShortTzNames = false;

    // Return whether or not daylight savings time is in effect at this time.
    bool InDST(TimezoneCache* cache);

    // Accessor for FILETIME representation.
    FILETIME& ft() { return time_.ft_; }

    // Accessor for integer representation.
    int64_t& t() { return time_.t_; }

    // Although win32 uses 64-bit integers for representing timestamps,
    // these are packed into a FILETIME structure. The FILETIME structure
    // is just a struct representing a 64-bit integer. The TimeStamp union
    // allows access to both a FILETIME and an integer representation of
    // the timestamp.
    union TimeStamp {
        FILETIME ft_;
        int64_t t_;
    };

    TimeStamp time_;
};

// Initialize timestamp to start of epoc.
Win32Time::Win32Time()
{
    t() = 0;
}

// Initialize timestamp from a JavaScript timestamp.
Win32Time::Win32Time(double jstime)
{
    t() = static_cast<int64_t>(jstime) * kTimeScaler + kTimeEpoc;
}

// Initialize timestamp from date/time components.
Win32Time::Win32Time(int year, int mon, int day, int hour, int min, int sec)
{
    SYSTEMTIME st;
    st.wYear = year;
    st.wMonth = mon;
    st.wDay = day;
    st.wHour = hour;
    st.wMinute = min;
    st.wSecond = sec;
    st.wMilliseconds = 0;
    SystemTimeToFileTime(&st, &ft());
}

// Convert timestamp to JavaScript timestamp.
double Win32Time::ToJSTime()
{
    return static_cast<double>((t() - kTimeEpoc) / kTimeScaler);
}

// Return the local timezone offset in milliseconds east of UTC. This
// takes into account whether daylight saving is in effect at the time.
// Only times in the 32-bit Unix range may be passed to this function.
// Also, adding the time-zone offset to the input must not overflow.
// The function EquivalentTime() in date.js guarantees this.
int64_t Win32Time::LocalOffset(TimezoneCache* cache)
{
    cache->InitializeIfNeeded();

    Win32Time rounded_to_second(*this);
    rounded_to_second.t() = rounded_to_second.t() / 1000 / kTimeScaler * 1000 * kTimeScaler;
    // Convert to local time using POSIX localtime function.
    // Windows XP Service Pack 3 made SystemTimeToTzSpecificLocalTime()
    // very slow.  Other browsers use localtime().

    // Convert from JavaScript milliseconds past 1/1/1970 0:00:00 to
    // POSIX seconds past 1/1/1970 0:00:00.
    double unchecked_posix_time = rounded_to_second.ToJSTime() / 1000;
    if (unchecked_posix_time > INT_MAX || unchecked_posix_time < 0) {
        return 0;
    }
    // Because _USE_32BIT_TIME_T is defined, time_t is a 32-bit int.
    time_t posix_time = static_cast<time_t>(unchecked_posix_time);

    // Convert to local time, as struct with fields for day, hour, year, etc.
    tm posix_local_time_struct;
    if (localtime_s(&posix_local_time_struct, &posix_time))
        return 0;

    if (posix_local_time_struct.tm_isdst > 0) {
        return (cache->tzinfo_.Bias + cache->tzinfo_.DaylightBias) * -kMsPerMinute;
    } else if (posix_local_time_struct.tm_isdst == 0) {
        return (cache->tzinfo_.Bias + cache->tzinfo_.StandardBias) * -kMsPerMinute;
    } else {
        return cache->tzinfo_.Bias * -kMsPerMinute;
    }
}

// Return whether or not daylight savings time is in effect at this time.
bool Win32Time::InDST(TimezoneCache* cache)
{
    cache->InitializeIfNeeded();

    // Determine if DST is in effect at the specified time.
    bool in_dst = false;
    if (cache->tzinfo_.StandardDate.wMonth != 0 || cache->tzinfo_.DaylightDate.wMonth != 0) {
        // Get the local timezone offset for the timestamp in milliseconds.
        int64_t offset = LocalOffset(cache);

        // Compute the offset for DST. The bias parameters in the timezone info
        // are specified in minutes. These must be converted to milliseconds.
        int64_t dstofs = -(cache->tzinfo_.Bias + cache->tzinfo_.DaylightBias) * kMsPerMinute;

        // If the local time offset equals the timezone bias plus the daylight
        // bias then DST is in effect.
        in_dst = offset == dstofs;
    }

    return in_dst;
}

// Return the daylight savings time offset for this time.
int64_t Win32Time::DaylightSavingsOffset(TimezoneCache* cache)
{
    return InDST(cache) ? 60 * kMsPerMinute : 0;
}

TimezoneCache* CreateTimezoneCache()
{
    return new TimezoneCache();
}

void DisposeTimezoneCache(TimezoneCache* cache)
{
    delete cache;
}

void ClearTimezoneCache(TimezoneCache* cache)
{
    cache->Clear();
}

double DaylightSavingsOffset(double time, TimezoneCache* cache)
{
    int64_t offset = Win32Time(time).DaylightSavingsOffset(cache);
    return static_cast<double>(offset);
}

double LocalTimeOffset(TimezoneCache* cache)
{
    // Use current time, rounded to the millisecond.
    Win32Time t((double)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
                    .count());
    // Time::LocalOffset inlcudes any daylight savings offset, so subtract it.
    return static_cast<double>(t.LocalOffset(cache) - t.DaylightSavingsOffset(cache));
}

#else

class TimezoneCache {
};

TimezoneCache* CreateTimezoneCache()
{
    return 0;
}

void DisposeTimezoneCache(TimezoneCache* cache)
{
}

void ClearTimezoneCache(TimezoneCache* cache)
{
}

double DaylightSavingsOffset(double time, TimezoneCache*)
{
    if (std::isnan(time))
        return std::numeric_limits<double>::quiet_NaN();
    time_t tv = static_cast<time_t>(std::floor(time / msPerSecond));
    struct tm* t = localtime(&tv);
    if (0 == t)
        return std::numeric_limits<double>::quiet_NaN();
    return t->tm_isdst > 0 ? 3600 * msPerSecond : 0;
}

double LocalTimeOffset(TimezoneCache* cache)
{
    time_t tv = time(0);
    struct tm* t = localtime(&tv);
    return static_cast<double>(t->tm_gmtoff * msPerSecond - (t->tm_isdst > 0 ? 3600 * msPerSecond : 0));
}

#endif

inline int BoolToInt(bool b)
{
    return b ? 1 : 0;
}

class DateCacheX {
public:
    static const int kMsPerMin = 60 * 1000;
    static const int kSecPerDay = 24 * 60 * 60;
    static const int64_t kMsPerDay = kSecPerDay * 1000;
    static const int64_t kMsPerMonth = kMsPerDay * 30;

    // The largest time that can be passed to OS date-time library functions.
    static const int kMaxEpochTimeInSec = kMaxInt;
    static const int64_t kMaxEpochTimeInMs = static_cast<int64_t>(kMaxInt) * 1000;

    // The largest time that can be stored in JSDate.
    static const int64_t kMaxTimeInMs = static_cast<int64_t>(864000000) * 10000000;

    // Conservative upper bound on time that can be stored in JSDate
    // before UTC conversion.
    static const int64_t kMaxTimeBeforeUTCInMs = kMaxTimeInMs + kMsPerMonth;

    // Sentinel that denotes an invalid local offset.
    static const int kInvalidLocalOffsetInMs = kMaxInt;
    // Sentinel that denotes an invalid cache stamp.
    // It is an invariant of DateCacheX that cache stamp is non-negative.
    static const int kInvalidStamp = -1;

    DateCacheX()
        : tz_cache_(CreateTimezoneCache())
    {
        ResetDateCacheX();
    }

    virtual ~DateCacheX()
    {
        DisposeTimezoneCache(tz_cache_);
        tz_cache_ = 0;
    }

    // Clears cached timezone information and increments the cache stamp.
    void ResetDateCacheX();

    // Computes floor(time_ms / kMsPerDay).
    static int DaysFromTime(int64_t time_ms)
    {
        if (time_ms < 0)
            time_ms -= (kMsPerDay - 1);
        return static_cast<int>(time_ms / kMsPerDay);
    }

    // Computes modulo(time_ms, kMsPerDay) given that
    // days = floor(time_ms / kMsPerDay).
    static int TimeInDay(int64_t time_ms, int days)
    {
        return static_cast<int>(time_ms - days * kMsPerDay);
    }

    // Given the number of days since the epoch, computes the weekday.
    // ECMA 262 - 15.9.1.6.
    int Weekday(int days)
    {
        int result = (days + 4) % 7;
        return result >= 0 ? result : result + 7;
    }

    bool IsLeap(int year)
    {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    // ECMA 262 - 15.9.1.7.
    int LocalOffsetInMs()
    {
        if (local_offset_ms_ == kInvalidLocalOffsetInMs) {
            local_offset_ms_ = GetLocalOffsetFromOS();
        }
        return local_offset_ms_;
    }

    // ECMA 262 - 15.9.5.26
    int TimezoneOffset(int64_t time_ms)
    {
        int64_t local_ms = ToLocal(time_ms);
        return static_cast<int>((time_ms - local_ms) / kMsPerMin);
    }

    // ECMA 262 - 15.9.1.9
    // LocalTime(t) = t + LocalTZA + DaylightSavingTA(t)
    int64_t ToLocal(int64_t time_ms)
    {
        return time_ms + LocalOffsetInMs() + DaylightSavingsOffsetInMs(time_ms);
    }

    // ECMA 262 - 15.9.1.9
    // UTC(t) = t - LocalTZA - DaylightSavingTA(t - LocalTZA)
    int64_t ToUTC(int64_t time_ms)
    {
        // We need to compute UTC time that corresponds to the given local time.
        // Literally following spec here leads to incorrect time computation at
        // the points were we transition to and from DST.
        //
        // The following shows that using DST for (t - LocalTZA - hour) produces
        // correct conversion.
        //
        // Consider transition to DST at local time L1.
        // Let L0 = L1 - hour, L2 = L1 + hour,
        //     U1 = UTC time that corresponds to L1,
        //     U0 = U1 - hour.
        // Transitioning to DST moves local clock one hour forward L1 => L2, so
        // U0 = UTC time that corresponds to L0 = L0 - LocalTZA,
        // U1 = UTC time that corresponds to L1 = L1 - LocalTZA,
        // U1 = UTC time that corresponds to L2 = L2 - LocalTZA - hour.
        // Note that DST(U0 - hour) = 0, DST(U0) = 0, DST(U1) = 1.
        // U0 = L0 - LocalTZA - DST(L0 - LocalTZA - hour),
        // U1 = L1 - LocalTZA - DST(L1 - LocalTZA - hour),
        // U1 = L2 - LocalTZA - DST(L2 - LocalTZA - hour).
        //
        // Consider transition from DST at local time L1.
        // Let L0 = L1 - hour,
        //     U1 = UTC time that corresponds to L1,
        //     U0 = U1 - hour, U2 = U1 + hour.
        // Transitioning from DST moves local clock one hour back L1 => L0, so
        // U0 = UTC time that corresponds to L0 (before transition)
        //    = L0 - LocalTZA - hour.
        // U1 = UTC time that corresponds to L0 (after transition)
        //    = L0 - LocalTZA = L1 - LocalTZA - hour
        // U2 = UTC time that corresponds to L1 = L1 - LocalTZA.
        // Note that DST(U0) = 1, DST(U1) = 0, DST(U2) = 0.
        // U0 = L0 - LocalTZA - DST(L0 - LocalTZA - hour) = L0 - LocalTZA - DST(U0).
        // U2 = L1 - LocalTZA - DST(L1 - LocalTZA - hour) = L1 - LocalTZA - DST(U1).
        // It is impossible to get U1 from local time.

        const int kMsPerHour = 3600 * 1000;
        time_ms -= LocalOffsetInMs();
        return time_ms - DaylightSavingsOffsetInMs(time_ms - kMsPerHour);
    }

    // Computes a time equivalent to the given time according
    // to ECMA 262 - 15.9.1.9.
    // The issue here is that some library calls don't work right for dates
    // that cannot be represented using a non-negative signed 32 bit integer
    // (measured in whole seconds based on the 1970 epoch).
    // We solve this by mapping the time to a year with same leap-year-ness
    // and same starting day for the year. The ECMAscript specification says
    // we must do this, but for compatibility with other browsers, we use
    // the actual year if it is in the range 1970..2037
    int64_t EquivalentTime(int64_t time_ms)
    {
        int days = DaysFromTime(time_ms);
        int time_within_day_ms = static_cast<int>(time_ms - days * kMsPerDay);
        int year, month, day;
        YearMonthDayFromDays(days, &year, &month, &day);
        int new_days = DaysFromYearMonth(EquivalentYear(year), month) + day - 1;
        return static_cast<int64_t>(new_days) * kMsPerDay + time_within_day_ms;
    }

    // Returns an equivalent year in the range [2008-2035] matching
    // - leap year,
    // - week day of first day.
    // ECMA 262 - 15.9.1.9.
    int EquivalentYear(int year)
    {
        int week_day = Weekday(DaysFromYearMonth(year, 0));
        int recent_year = (IsLeap(year) ? 1956 : 1967) + (week_day * 12) % 28;
        // Find the year in the range 2008..2037 that is equivalent mod 28.
        // Add 3*28 to give a positive argument to the modulus operator.
        return 2008 + (recent_year + 3 * 28 - 2008) % 28;
    }

    // Given the number of days since the epoch, computes
    // the corresponding year, month, and day.
    void YearMonthDayFromDays(int days, int* year, int* month, int* day);

    // Computes the number of days since the epoch for
    // the first day of the given month in the given year.
    int DaysFromYearMonth(int year, int month);

    // Breaks down the time value.
    void BreakDownTime(int64_t time_ms, int* year, int* month, int* day,
        int* weekday, int* hour, int* min, int* sec, int* ms);

    // These functions are virtual so that we can override them when testing.
    virtual int GetDaylightSavingsOffsetFromOS(int64_t time_sec)
    {
        double time_ms = static_cast<double>(time_sec * 1000);
        return static_cast<int>(
            DaylightSavingsOffset(time_ms, tz_cache_));
    }

    virtual int GetLocalOffsetFromOS()
    {
        double offset = LocalTimeOffset(tz_cache_);
        return static_cast<int>(offset);
    }

private:
    // The implementation relies on the fact that no time zones have
    // more than one daylight savings offset change per 19 days.
    // In Egypt in 2010 they decided to suspend DST during Ramadan. This
    // led to a short interval where DST is in effect from September 10 to
    // September 30.
    static const int kDefaultDSTDeltaInSec = 19 * kSecPerDay;

    // Size of the Daylight Savings Time cache.
    static const int kDSTSize = 32;

    // Daylight Savings Time segment stores a segment of time where
    // daylight savings offset does not change.
    struct DST {
        int start_sec;
        int end_sec;
        int offset_ms;
        int last_used;
    };

    // Computes the daylight savings offset for the given time.
    // ECMA 262 - 15.9.1.8
    int DaylightSavingsOffsetInMs(int64_t time_ms);

    // Sets the before_ and the after_ segments from the DST cache such that
    // the before_ segment starts earlier than the given time and
    // the after_ segment start later than the given time.
    // Both segments might be invalid.
    // The last_used counters of the before_ and after_ are updated.
    void ProbeDST(int time_sec);

    // Finds the least recently used segment from the DST cache that is not
    // equal to the given 'skip' segment.
    DST* LeastRecentlyUsedDST(DST* skip);

    // Extends the after_ segment with the given point or resets it
    // if it starts later than the given time + kDefaultDSTDeltaInSec.
    inline void ExtendTheAfterSegment(int time_sec, int offset_ms);

    // Makes the given segment invalid.
    inline void ClearSegment(DST* segment);

    bool InvalidSegment(DST* segment)
    {
        return segment->start_sec > segment->end_sec;
    }

    // Daylight Saving Time cache.
    DST dst_[kDSTSize];
    int dst_usage_counter_;
    DST* before_;
    DST* after_;

    int local_offset_ms_;

    // Year/Month/Day cache.
    bool ymd_valid_;
    int ymd_days_;
    int ymd_year_;
    int ymd_month_;
    int ymd_day_;

    TimezoneCache* tz_cache_;
};

static const int kDaysIn4Years = 4 * 365 + 1;
static const int kDaysIn100Years = 25 * kDaysIn4Years - 1;
static const int kDaysIn400Years = 4 * kDaysIn100Years + 1;
static const int kDays1970to2000 = 30 * 365 + 7;
static const int kDaysOffset = 1000 * kDaysIn400Years + 5 * kDaysIn400Years - kDays1970to2000;
static const int kYearsOffset = 400000;
static const char kDaysInMonths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void DateCacheX::ResetDateCacheX()
{
    for (int i = 0; i < kDSTSize; ++i) {
        ClearSegment(&dst_[i]);
    }
    dst_usage_counter_ = 0;
    before_ = &dst_[0];
    after_ = &dst_[1];
    local_offset_ms_ = kInvalidLocalOffsetInMs;
    ymd_valid_ = false;
    ClearTimezoneCache(tz_cache_);
}

void DateCacheX::ClearSegment(DST* segment)
{
    segment->start_sec = kMaxEpochTimeInSec;
    segment->end_sec = -kMaxEpochTimeInSec;
    segment->offset_ms = 0;
    segment->last_used = 0;
}

void DateCacheX::YearMonthDayFromDays(
    int days, int* year, int* month, int* day)
{
    if (ymd_valid_) {
        // Check conservatively if the given 'days' has
        // the same year and month as the cached 'days'.
        int new_day = ymd_day_ + (days - ymd_days_);
        if (new_day >= 1 && new_day <= 28) {
            ymd_day_ = new_day;
            ymd_days_ = days;
            *year = ymd_year_;
            *month = ymd_month_;
            *day = new_day;
            return;
        }
    }
    int save_days = days;

    days += kDaysOffset;
    *year = 400 * (days / kDaysIn400Years) - kYearsOffset;
    days %= kDaysIn400Years;

    days--;
    int yd1 = days / kDaysIn100Years;
    days %= kDaysIn100Years;
    *year += 100 * yd1;

    days++;
    int yd2 = days / kDaysIn4Years;
    days %= kDaysIn4Years;
    *year += 4 * yd2;

    days--;
    int yd3 = days / 365;
    days %= 365;
    *year += yd3;

    bool is_leap = (!yd1 || yd2) && !yd3;

    days += is_leap;

    // Check if the date is after February.
    if (days >= 31 + 28 + BoolToInt(is_leap)) {
        days -= 31 + 28 + BoolToInt(is_leap);
        // Find the date starting from March.
        for (int i = 2; i < 12; i++) {
            if (days < kDaysInMonths[i]) {
                *month = i;
                *day = days + 1;
                break;
            }
            days -= kDaysInMonths[i];
        }
    } else {
        // Check January and February.
        if (days < 31) {
            *month = 0;
            *day = days + 1;
        } else {
            *month = 1;
            *day = days - 31 + 1;
        }
    }
    ymd_valid_ = true;
    ymd_year_ = *year;
    ymd_month_ = *month;
    ymd_day_ = *day;
    ymd_days_ = save_days;
}

int DateCacheX::DaysFromYearMonth(int year, int month)
{
    static const int day_from_month[] = { 0, 31, 59, 90, 120, 151,
        181, 212, 243, 273, 304, 334 };
    static const int day_from_month_leap[] = { 0, 31, 60, 91, 121, 152,
        182, 213, 244, 274, 305, 335 };

    year += month / 12;
    month %= 12;
    if (month < 0) {
        year--;
        month += 12;
    }

    // year_delta is an arbitrary number such that:
    // a) year_delta = -1 (mod 400)
    // b) year + year_delta > 0 for years in the range defined by
    //    ECMA 262 - 15.9.1.1, i.e. upto 100,000,000 days on either side of
    //    Jan 1 1970. This is required so that we don't run into integer
    //    division of negative numbers.
    // c) there shouldn't be an overflow for 32-bit integers in the following
    //    operations.
    static const int year_delta = 399999;
    static const int base_day = 365 * (1970 + year_delta) + (1970 + year_delta) / 4 - (1970 + year_delta) / 100 + (1970 + year_delta) / 400;

    int year1 = year + year_delta;
    int day_from_year = 365 * year1 + year1 / 4 - year1 / 100 + year1 / 400 - base_day;

    if ((year % 4 != 0) || (year % 100 == 0 && year % 400 != 0)) {
        return day_from_year + day_from_month[month];
    }
    return day_from_year + day_from_month_leap[month];
}

void DateCacheX::BreakDownTime(int64_t time_ms, int* year, int* month, int* day,
    int* weekday, int* hour, int* min, int* sec,
    int* ms)
{
    int const days = DaysFromTime(time_ms);
    int const time_in_day_ms = TimeInDay(time_ms, days);
    YearMonthDayFromDays(days, year, month, day);
    *weekday = Weekday(days);
    *hour = time_in_day_ms / (60 * 60 * 1000);
    *min = (time_in_day_ms / (60 * 1000)) % 60;
    *sec = (time_in_day_ms / 1000) % 60;
    *ms = time_in_day_ms % 1000;
}

void DateCacheX::ExtendTheAfterSegment(int time_sec, int offset_ms)
{
    if (after_->offset_ms == offset_ms && after_->start_sec <= time_sec + kDefaultDSTDeltaInSec && time_sec <= after_->end_sec) {
        // Extend the after_ segment.
        after_->start_sec = time_sec;
    } else {
        // The after_ segment is either invalid or starts too late.
        if (after_->start_sec <= after_->end_sec) {
            // If the after_ segment is valid, replace it with a new segment.
            after_ = LeastRecentlyUsedDST(before_);
        }
        after_->start_sec = time_sec;
        after_->end_sec = time_sec;
        after_->offset_ms = offset_ms;
        after_->last_used = ++dst_usage_counter_;
    }
}

int DateCacheX::DaylightSavingsOffsetInMs(int64_t time_ms)
{
    int time_sec = (time_ms >= 0 && time_ms <= kMaxEpochTimeInMs)
        ? static_cast<int>(time_ms / 1000)
        : static_cast<int>(EquivalentTime(time_ms) / 1000);

    // Invalidate cache if the usage counter is close to overflow.
    // Note that dst_usage_counter is incremented less than ten times
    // in this function.
    if (dst_usage_counter_ >= kMaxInt - 10) {
        dst_usage_counter_ = 0;
        for (int i = 0; i < kDSTSize; ++i) {
            ClearSegment(&dst_[i]);
        }
    }

    // Optimistic fast check.
    if (before_->start_sec <= time_sec && time_sec <= before_->end_sec) {
        // Cache hit.
        before_->last_used = ++dst_usage_counter_;
        return before_->offset_ms;
    }

    ProbeDST(time_sec);

    if (InvalidSegment(before_)) {
        // Cache miss.
        before_->start_sec = time_sec;
        before_->end_sec = time_sec;
        before_->offset_ms = GetDaylightSavingsOffsetFromOS(time_sec);
        before_->last_used = ++dst_usage_counter_;
        return before_->offset_ms;
    }

    if (time_sec <= before_->end_sec) {
        // Cache hit.
        before_->last_used = ++dst_usage_counter_;
        return before_->offset_ms;
    }

    if (time_sec > before_->end_sec + kDefaultDSTDeltaInSec) {
        // If the before_ segment ends too early, then just
        // query for the offset of the time_sec
        int offset_ms = GetDaylightSavingsOffsetFromOS(time_sec);
        ExtendTheAfterSegment(time_sec, offset_ms);
        // This swap helps the optimistic fast check in subsequent invocations.
        DST* temp = before_;
        before_ = after_;
        after_ = temp;
        return offset_ms;
    }

    // Now the time_sec is between
    // before_->end_sec and before_->end_sec + default DST delta.
    // Update the usage counter of before_ since it is going to be used.
    before_->last_used = ++dst_usage_counter_;

    // Check if after_ segment is invalid or starts too late.
    // Note that start_sec of invalid segments is kMaxEpochTimeInSec.
    if (before_->end_sec + kDefaultDSTDeltaInSec <= after_->start_sec) {
        int new_after_start_sec = before_->end_sec + kDefaultDSTDeltaInSec;
        int new_offset_ms = GetDaylightSavingsOffsetFromOS(new_after_start_sec);
        ExtendTheAfterSegment(new_after_start_sec, new_offset_ms);
    } else {
        // Update the usage counter of after_ since it is going to be used.
        after_->last_used = ++dst_usage_counter_;
    }

    // Now the time_sec is between before_->end_sec and after_->start_sec.
    // Only one daylight savings offset change can occur in this interval.

    if (before_->offset_ms == after_->offset_ms) {
        // Merge two segments if they have the same offset.
        before_->end_sec = after_->end_sec;
        ClearSegment(after_);
        return before_->offset_ms;
    }

    // Binary search for daylight savings offset change point,
    // but give up if we don't find it in four iterations.
    for (int i = 4; i >= 0; --i) {
        int delta = after_->start_sec - before_->end_sec;
        int middle_sec = (i == 0) ? time_sec : before_->end_sec + delta / 2;
        int offset_ms = GetDaylightSavingsOffsetFromOS(middle_sec);
        if (before_->offset_ms == offset_ms) {
            before_->end_sec = middle_sec;
            if (time_sec <= before_->end_sec) {
                return offset_ms;
            }
        } else {
            after_->start_sec = middle_sec;
            if (time_sec >= after_->start_sec) {
                // This swap helps the optimistic fast check in subsequent invocations.
                DST* temp = before_;
                before_ = after_;
                after_ = temp;
                return offset_ms;
            }
        }
    }

    return 0;
}

void DateCacheX::ProbeDST(int time_sec)
{
    DST* before = 0;
    DST* after = 0;

    for (int i = 0; i < kDSTSize; ++i) {
        if (dst_[i].start_sec <= time_sec) {
            if (before == 0 || before->start_sec < dst_[i].start_sec) {
                before = &dst_[i];
            }
        } else if (time_sec < dst_[i].end_sec) {
            if (after == 0 || after->end_sec > dst_[i].end_sec) {
                after = &dst_[i];
            }
        }
    }

    // If before or after segments were not found,
    // then set them to any invalid segment.
    if (before == 0) {
        before = InvalidSegment(before_) ? before_ : LeastRecentlyUsedDST(after);
    }
    if (after == 0) {
        after = InvalidSegment(after_) && before != after_
            ? after_
            : LeastRecentlyUsedDST(before);
    }

    before_ = before;
    after_ = after;
}

DateCacheX::DST* DateCacheX::LeastRecentlyUsedDST(DST* skip)
{
    DST* result = 0;
    for (int i = 0; i < kDSTSize; ++i) {
        if (&dst_[i] == skip)
            continue;
        if (result == 0 || result->last_used > dst_[i].last_used) {
            result = &dst_[i];
        }
    }
    ClearSegment(result);
    return result;
}

DateCache::DateCache()
{
    m_pdc = new DateCacheX();
}

DateCache::~DateCache()
{
    delete (DateCacheX*)m_pdc;
}

int64_t DateCache::ToLocal(int64_t time_ms)
{
    return ((DateCacheX*)m_pdc)->ToLocal(time_ms);
}

int64_t DateCache::ToUTC(int64_t time_ms)
{
    return ((DateCacheX*)m_pdc)->ToUTC(time_ms);
}

int32_t DateCache::LocalOffset()
{
    return ((DateCacheX*)m_pdc)->GetLocalOffsetFromOS() / 36000;
}
}
