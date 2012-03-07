/*
 * Filter.hh
 *
 * Copyright 2001, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2001, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_FILTER_HH
#define _LOG4CPP_FILTER_HH

#include <log4cpp/Portability.hh>
#include <log4cpp/LoggingEvent.hh>

namespace log4cpp {

    /**
       Users should extend this class to implement customized logging
       event filtering. Note that log4cpp::Category and
       lof4cpp::Appender have built-in filtering rules. It is suggested 
       that you first use and understand the built-in rules before rushing 
       to write your own custom filters.

       <p>This abstract class assumes and also imposes that filters be
       organized in a linear chain. The <code>decide(LoggingEvent)</code> 
       method of each filter is called sequentially, in the order of their 
       addition to the chain.
       
       <p>The <code>decide(LoggingEvent)</code> method must return a
       Decision value, either DENY, NEUTRAL or ACCCEPT.

       <p>If the value DENY is returned, then the log event is
       dropped immediately without consulting with the remaining
       filters. 
       
       <p>If the value NEUTRAL is returned, then the next filter
       in the chain is consulted. If there are no more filters in the
       chain, then the log event is logged. Thus, in the presence of no
       filters, the default behaviour is to log all logging events.
       
       <p>If the value ACCEPT is returned, then the log
       event is logged without consulting the remaining filters. 
       
       <p>The philosophy of log4cpp filters is largely inspired from the
       Linux ipchains. 
    **/

    class LOG4CPP_EXPORT Filter {
        public:
        
        typedef enum { DENY = -1,
                       NEUTRAL = 0,
                       ACCEPT = 1
        } Decision;

        /**
         * Default Constructor for Filter
         **/
        Filter();

        /**
         * Destructor for Filter
         **/
        virtual ~Filter();

        /**
         * Set the next Filter in the Filter chain
         * @param filter The filter to chain
         **/
        virtual void setChainedFilter(Filter* filter);

        /**
         * Get the next Filter in the Filter chain
         * @return The next Filter or NULL if the current filter is the last
         * in the chain
         **/
        virtual Filter* getChainedFilter();

        /**
         * Get the last Filter in the Filter chain
         * @return The last Filter in the Filter chain
         **/
        virtual Filter* getEndOfChain();

        /**
         * Add a Filter to the end of the Filter chain. Convience method for
         * getEndOfChain()->setChainedFilter(filter).
         * @param filter The filter to add to the end of the chain.
         **/
        virtual void appendChainedFilter(Filter* filter);

        /**
         * Decide whether to accept or deny a LoggingEvent. This method will
         * walk the entire chain until a non neutral decision has been made
         * or the end of the chain has been reached.
         * @param event The LoggingEvent to decide on.
         * @return The Decision
         **/
        virtual Decision decide(const LoggingEvent& event);

        protected:
        /**
         * Decide whether <b>this</b> Filter accepts or denies the given 
         * LoggingEvent. Actual implementation of Filter should override this
         * method and not <code>decide(LoggingEvent&)</code>.
         * @param event The LoggingEvent to decide on.
         * @return The Decision
         **/
        virtual Decision _decide(const LoggingEvent& event) = 0;

        private:
        Filter* _chainedFilter;

    }; 

}

#endif // _LOG4CPP_FILTER_HH
