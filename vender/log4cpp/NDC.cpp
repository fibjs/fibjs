/*
 * NDC.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"
#include <log4cpp/NDC.hh>
#include <log4cpp/threading/Threading.hh>

namespace log4cpp {

    NDC::DiagnosticContext::DiagnosticContext(const std::string& message) :
        message(message),
        fullMessage(message) {
    }

    NDC::DiagnosticContext::DiagnosticContext(const std::string& message, 
            const DiagnosticContext& parent) :
        message(message),
        fullMessage(parent.fullMessage + " " + message) {
    }

    namespace {
        threading::ThreadLocalDataHolder<NDC> _nDC;
    }

    void NDC::clear() {
        getNDC()._clear();
    }

    NDC::ContextStack* NDC::cloneStack() {
        return getNDC()._cloneStack();
    }

    const std::string& NDC::get() {
        return getNDC()._get();
    }

    size_t NDC::getDepth() {
        return getNDC()._getDepth();
    }

    void NDC::inherit(NDC::ContextStack* stack) {
        getNDC()._inherit(stack);
    }

    std::string NDC::pop() {
        return getNDC()._pop();
    }

    void NDC::push(const std::string& message) {
        getNDC()._push(message);
    }

    void NDC::setMaxDepth(int maxDepth) {
        getNDC()._setMaxDepth(maxDepth);
    }

    NDC& NDC::getNDC() {
        NDC* nDC = _nDC.get();

        if (!nDC) {
            nDC = new NDC();
            _nDC.reset(nDC);
        }

        return *nDC;
    }

    NDC::NDC() {
    }

    NDC::~NDC() {
    }

    void NDC::_clear() {
        _stack.clear();
    }

    NDC::ContextStack* NDC::_cloneStack() {
        return new ContextStack(_stack);
    }

    const std::string& NDC::_get() const {
        static std::string empty = "";

        return (_stack.empty() ? empty : _stack.back().fullMessage);
    }

    size_t NDC::_getDepth() const {
        return _stack.size();
    }

    void NDC::_inherit(NDC::ContextStack* stack) {
        _stack = *stack;
    }

    std::string NDC::_pop() {
        std::string result = _stack.back().message;
        _stack.pop_back();
        return result;
    }

    void NDC::_push(const std::string& message) {
        if (_stack.empty()) {
            _stack.push_back(DiagnosticContext(message));
        } else {            
            _stack.push_back(DiagnosticContext(message, _stack.back()));
        }
    }

    void NDC::_setMaxDepth(int maxDepth) {
        // XXX no maximum
    }

}
