#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/lockfree/fifo.hpp>
#include "test.h"

boost::atomic_int producer_count(0);
boost::atomic_int consumer_count(0);

boost::lockfree::fifo<int> fifo;

const int iterations = 10000000;
const int producer_thread_count = 4;
const int consumer_thread_count = 4;

static unsigned long long getustime(void) {
	struct timeval tm;

	gettimeofday(&tm, NULL);

	return tm.tv_sec * 1000000ULL + tm.tv_usec;
}

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        while (!fifo.enqueue(value))
            ;
    }
}

boost::atomic<bool> done (false);
void consumer(void)
{
    int value;
    while (!done) {
        while (fifo.dequeue(value))
            ++consumer_count;
    }

    while (fifo.dequeue(value))
        ++consumer_count;
}

void lockfree_main()
{
    unsigned long long ts;
    log4cpp::Category& log = log4cpp::Category::getInstance(std::string("lockfree"));

    log.debug("--------- lockfree performance --------");
    log.debug("boost::lockfree::fifo is %slockfree", fifo.is_lock_free() ? "" : "not ");

    boost::thread_group producer_threads, consumer_threads;

    ts = getustime();
    for (int i = 0; i != producer_thread_count; ++i)
        producer_threads.create_thread(producer);

    for (int i = 0; i != consumer_thread_count; ++i)
        consumer_threads.create_thread(consumer);

    producer_threads.join_all();
    done = true;

    consumer_threads.join_all();

    log.debug("produced %d objects.", (int)producer_count);
    log.debug("consumed %d objects.", (int)consumer_count);
    log.debug("%g usec", (double) (getustime() - ts) / (int)consumer_count);
    log.debug("--------- lockfree performance --------");
}
