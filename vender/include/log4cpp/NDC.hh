/*
 * NDC.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_NDC_HH
#define _LOG4CPP_NDC_HH

#include <log4cpp/Portability.hh>
#include <string>
#include <vector>

namespace log4cpp {
    /**
       The NDC class implements <i>nested diagnostic contexts</i> as
       defined by Neil Harrison in the article "Patterns for Logging
       Diagnostic Messages" part of the book "<i>Pattern Languages of
       Program Design 3</i>" edited by Martin et al.
       
       <p>A Nested Diagnostic Context, or NDC in short, is an instrument
       to distinguish interleaved log output from different sources. Log
       output is typically interleaved when a server handles multiple
       clients near-simulatanously.
       
       <p>Interleaved log output can still be meaningful if each log entry
       from different contexts had a distinctive stamp. This is where NDCs
       come into play.
       
       <p><em><b>Note that NDCs are managed on a per thread
       basis</b></em>. NDC operations such as <code>push</code>, <code>
       pop</code>, <code>clear</code>, <code>getDepth</code> and <code>
       setMaxDepth</code> affect the NDC of the <em>current</em> thread only.
       NDCs of other threads remain unaffected.
       
       <p>To build an NDC one uses the <code>push</code> operation. 
       Simply put,

       <p><ul>
       <li>Contexts can be nested.
       
       <p><li>When entering a context, call <code>NDC.push</code>. As a
       side effect, if there is no nested diagnostic context for the
       current thread, this method will create it.
       
       <p><li>When leaving a context, call <code>NDC.pop</code>.
       </ul>

       <p>There is no penalty for forgetting to match each
       <code>push</code> operation with a corresponding <code>pop</code>,
       except the obvious mismatch between the real application context
       and the context set in the NDC.

       <p>Custom Layouts may include the nested diagnostic context for the 
       current thread in log messages, without any user intervention.
       Hence, even if a server is serving multiple clients
       simultaneously, the logs emanating from the same code (belonging to
       the same category) can still be distinguished because each client
       request will have a different NDC tag.

       <p><em>Unfortunately, unlike Java, C++ does not have platform 
       independent multithreading support. Therefore, currently log4cpp is 
       not multithread aware, it implicitly assumes only one thread exists, 
       the main process thread. </em>        
    **/
    class LOG4CPP_EXPORT NDC {
        public:

        struct DiagnosticContext {
            DiagnosticContext(const std::string& message);
            DiagnosticContext(const std::string& message, 
                              const DiagnosticContext& parent);

            std::string message;
            std::string fullMessage;
        };

        typedef std::vector<DiagnosticContext> ContextStack;

        /**
           Clear any nested disgnostic information if any. This method is
           useful in cases where the same thread can be potentially used
           over and over in different unrelated contexts.
           
           <p>This method is equivalent to calling the <code>setMaxDepth</code>
           method with a zero <code>maxDepth</code> argument.
        **/
        static void clear();

        /**
           Clone the diagnostic context for the current thread.
           
           <p>Internally a diagnostic context is represented as a stack.  A
           given thread can supply the stack (i.e. diagnostic context) to a
           child thread so that the child can inherit the parent thread's
           diagnostic context.
           
           <p>The child thread uses the <code>inherit</code> method to
           inherit the parent's diagnostic context.
           
           @return Stack A clone of the current thread's  diagnostic context.
        **/
        static ContextStack* cloneStack();

        /**
           Get the current diagnostic context string.
           @return the context string.
        **/
        static const std::string& get();

        /**
           Get the current nesting depth of this diagnostic context.
           @return the nesting depth
        **/
        static size_t getDepth();

        static void inherit(ContextStack* stack);

        /**
           Clients should call this method before leaving a diagnostic
           context.
           
           <p>The returned value is the value that was pushed last. If no
           context is available, then the empty string "" is returned.
           
           @return String The innermost diagnostic context.
        **/
 
        static std::string pop();

        /**
           Push new diagnostic context information for the current thread.
           
           <p>The contents of the <code>message</code> parameter is
           determined solely by the client.  
           
           @param message The new diagnostic context information.
        **/
        static void push(const std::string& message);

        /**
           Set the maximum nesting depth for the current NDC. Curently NDCs 
           do not enforce a maximum depth and consequentially this method
           has no effect.
           @param maxDepth the maximum nesting depth
        **/
        static void setMaxDepth(int maxDepth);

        /**
           Return the NDC for the current thread.
           @return the NDC for the current thread
        **/
        static NDC& getNDC();

        NDC();
        virtual ~NDC();

        public:
        virtual void _clear();
        virtual ContextStack* _cloneStack();
        virtual const std::string& _get() const;
        virtual size_t _getDepth() const;
        virtual void _inherit(ContextStack* stack);
        virtual std::string _pop();
        virtual void _push(const std::string& message);
        virtual void _setMaxDepth(int maxDepth);

        ContextStack _stack;
    };        
}

#endif // _LOG4CPP_NDC_HH
