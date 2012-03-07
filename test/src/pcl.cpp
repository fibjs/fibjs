/*
 *  pcl.cpp
 *  Created on: Mar 4, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pcl/pcl.h>
#include "test.h"


#define MIN_MEASURE_TIME 2000000ULL
#define CO_STACK_SIZE (8 * 1024)

static unsigned long long getustime(void) {
	struct timeval tm;

	gettimeofday(&tm, NULL);

	return tm.tv_sec * 1000000ULL + tm.tv_usec;
}

static void switch_bench(void *data) {
	volatile unsigned long *sw_counter = (unsigned long *) data;

	for (;;) {
		(*sw_counter)--;
		co_resume();
	}
}

static void *run_test(void *data) {
	int i, ntimes;
	coroutine_t coro;
	unsigned long nswitches, sw_counter;
	unsigned long long ts, te;

	log4cpp::Category& pcl = log4cpp::Category::getInstance(std::string("pcl"));

	pcl.debug("measuring co_create+co_delete performance ...");

	ntimes = 10000;
	do {
		ts = getustime();
		for (i = 0; i < ntimes; i++) {
			if ((coro = co_create(switch_bench, &sw_counter, NULL,
					CO_STACK_SIZE)) != NULL)
				co_delete(coro);
		}
		te = getustime();
		ntimes *= 4;
	} while ((te - ts) < MIN_MEASURE_TIME);

	pcl.debug("%g usec", (double) (te - ts) / (double) ntimes);

	if ((coro = co_create(switch_bench, &sw_counter, NULL, CO_STACK_SIZE))
			!= NULL) {
		pcl.debug("measuring switch performance ...");

		sw_counter = nswitches = 10000;
		do {
			ts = getustime();
			while (sw_counter)
				co_call(coro);
			te = getustime();
			sw_counter = (nswitches *= 4);
		} while ((te - ts) < MIN_MEASURE_TIME);

		pcl.debug("%g usec",
				(double) (te - ts) / (double) (2 * nswitches));

		co_delete(coro);
	}

	return NULL;
}

static void *thread_proc(void *data) {
	void *result;

	co_thread_init();
	result = run_test(data);
	co_thread_cleanup();

	return result;
}

void pcl_main() {
    log4cpp::Category& pcl = log4cpp::Category::getInstance(std::string("pcl"));

    pcl.debug("--------- pcl performance --------");
	run_test(NULL);
	pcl.debug("--------- pcl performance --------");
}
