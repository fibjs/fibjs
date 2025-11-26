/*
 * rtc.cpp
 *
 *  Created on: Oct 26, 2024
 */

#include "object.h"
#include "RTCPeerConnection.h"
#include "Fiber.h"
#include "ifs/rtc.h"

#include <boost/preprocessor.hpp>

#ifndef JUICE_STATIC
#define JUICE_STATIC
#endif
#include "juice/include/juice/juice.h"

namespace fibjs {
DECLARE_MODULE(rtc);

result_t rtc_base::listen(exlib::string bind_address, int32_t local_port, v8::Local<v8::Function> cb)
{
    struct cb_data {
        exlib::string local_ufrag;
        exlib::string remote_ufrag;
        exlib::string address;
        uint16_t port;
    };

    Isolate* isolate = Isolate::current(cb);
    if (isolate->m_id != 1)
        return Runtime::setError("rtc.bind() can only be called in the main isolate");

    static v8::Global<v8::Function> s_cb_global;
    s_cb_global.Reset(isolate->m_isolate, cb);
    const char* bind_address_ = bind_address.empty() ? nullptr : bind_address.c_str();

    int ret = juice_mux_listen(bind_address_, local_port, [](const juice_mux_incoming_t* incoming_info, void* user_data) {
            cb_data* data_ = new cb_data();

            data_->local_ufrag = incoming_info->local_ufrag;
            data_->remote_ufrag = incoming_info->remote_ufrag;
            data_->address = incoming_info->address;
            data_->port = incoming_info->port;

            ((Isolate*)user_data)->sync([data_]() -> int {
                Isolate* isolate = Isolate::current();
                JSFiber::EnterJsScope s;

                v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate->m_isolate, s_cb_global);
                v8::Local<v8::Object> data = v8::Object::New(isolate->m_isolate);

                data->Set(isolate->context(), isolate->NewString("local_ufrag"), isolate->NewString(data_->local_ufrag)).Check();
                data->Set(isolate->context(), isolate->NewString("remote_ufrag"), isolate->NewString(data_->remote_ufrag)).Check();
                data->Set(isolate->context(), isolate->NewString("address"), isolate->NewString(data_->address)).Check();
                data->Set(isolate->context(), isolate->NewString("port"), v8::Number::New(isolate->m_isolate, data_->port)).Check();
                delete data_;

                v8::Local<v8::Value> argv[] = { data };

                v8::Local<v8::Value> result = cb->Call(isolate->context(), v8::Undefined(isolate->m_isolate), 1, argv).FromMaybe(v8::Local<v8::Value>());

                return 0;
            }); }, isolate);

    if (ret == -1)
        return Runtime::setError("rtc.listen() need to be called before RTCPeerConnection object is created");
    else if (ret < 0)
        return Runtime::setError("rtc.listen() failed to bind to the specified port");

    isolate->Ref();

    return 0;
}

result_t rtc_base::listen(int32_t local_port, v8::Local<v8::Function> cb)
{
    return listen("", local_port, cb);
}

result_t rtc_base::stopListen(exlib::string bind_address, int32_t local_port)
{
    Isolate* isolate = Isolate::current();
    if (isolate->m_id != 1)
        return Runtime::setError("rtc.stopListen() can only be called in the main isolate");

    int ret = juice_mux_stop_listen(bind_address.empty() ? nullptr : bind_address.c_str(), local_port);
    if (ret == 0)
        isolate->Unref();

    return 0;
}

result_t rtc_base::stopListen(int32_t local_port)
{
    return stopListen("", local_port);
}

result_t rtc_base::startServer(v8::Local<v8::Object> config)
{
    class ServerOptions : public obj_base {
    public:
        LOAD_OPTIONS(ServerOptions, (credentials)(maxAllocations)(maxPeers)(bindAddress)(port)(relayPortRangeBegin)(relayPortRangeEnd));

    public:
        std::optional<v8::Local<v8::Object>> credentials;
        std::optional<int32_t> maxAllocations = 0;
        std::optional<int32_t> maxPeers = 0;
        std::optional<exlib::string> bindAddress;
        std::optional<int32_t> port = 3478;
        std::optional<int32_t> relayPortRangeBegin = 0;
        std::optional<int32_t> relayPortRangeEnd = 0;
    };

    obj_ptr<ServerOptions> options;
    Isolate* isolate = Isolate::current(config);
    result_t hr = ServerOptions::load(config, options);
    if (hr < 0)
        return hr;

    juice_server_config_t server_config = {};
    std::vector<juice_server_credentials> server_credentials;
    std::vector<std::pair<std::string, std::string>> credentials_;

    if (options->credentials.has_value()) {
        v8::Local<v8::Object> credentials = options->credentials.value();
        v8::Local<v8::Array> keys = credentials->GetPropertyNames(isolate->context()).ToLocalChecked();
        for (uint32_t i = 0; i < keys->Length(); i++) {
            v8::Local<v8::Value> key = keys->Get(isolate->context(), i).ToLocalChecked();
            v8::Local<v8::Value> value = credentials->Get(isolate->context(), key).ToLocalChecked();

            exlib::string username;
            exlib::string password;
            GetArgumentValue(isolate, key, username);
            GetArgumentValue(isolate, value, password);

            credentials_.push_back(std::make_pair(username, password));
        }

        for (auto& [username, password] : credentials_) {
            juice_server_credentials cred = {};
            cred.username = username.c_str();
            cred.password = password.c_str();
            server_credentials.push_back(cred);
        }

        server_config.credentials = server_credentials.data();
        server_config.credentials_count = server_credentials.size();
    }

    server_config.max_allocations = options->maxAllocations.value();
    server_config.max_peers = options->maxPeers.value();
    server_config.bind_address = options->bindAddress.has_value() ? options->bindAddress.value().c_str() : nullptr;
    server_config.port = options->port.value();
    server_config.relay_port_range_begin = options->relayPortRangeBegin.value();
    server_config.relay_port_range_end = options->relayPortRangeEnd.value();

    juice_server_t* server = juice_server_create(&server_config);
    if (!server)
        return Runtime::setError("Failed to create the server");

    return 0;
}

result_t rtc_base::setSctpSettings(v8::Local<v8::Object> settings)
{
    class SctpOptions : public obj_base {
    public:
        LOAD_OPTIONS(SctpOptions, (recvBufferSize)(sendBufferSize)(maxChunksOnQueue)(initialCongestionWindow)(maxBurst)(congestionControlModule)(delayedSackTimeMs)(minRetransmitTimeoutMs)(maxRetransmitTimeoutMs)(initialRetransmitTimeoutMs)(maxRetransmitAttempts)(heartbeatIntervalMs));

    public:
        std::optional<int32_t> recvBufferSize = 0;
        std::optional<int32_t> sendBufferSize = 0;
        std::optional<int32_t> maxChunksOnQueue = 0;
        std::optional<int32_t> initialCongestionWindow = 0;
        std::optional<int32_t> maxBurst = 0;
        std::optional<int32_t> congestionControlModule = 0;
        std::optional<int32_t> delayedSackTimeMs = 0;
        std::optional<int32_t> minRetransmitTimeoutMs = 0;
        std::optional<int32_t> maxRetransmitTimeoutMs = 0;
        std::optional<int32_t> initialRetransmitTimeoutMs = 0;
        std::optional<int32_t> maxRetransmitAttempts = 0;
        std::optional<int32_t> heartbeatIntervalMs = 0;
    };

    obj_ptr<SctpOptions> options;
    Isolate* isolate = Isolate::current(settings);
    result_t hr = SctpOptions::load(settings, options);
    if (hr < 0)
        return hr;

    rtc::SctpSettings s = {};
    s.recvBufferSize = options->recvBufferSize.value();
    s.sendBufferSize = options->sendBufferSize.value();
    s.maxChunksOnQueue = options->maxChunksOnQueue.value();
    s.initialCongestionWindow = options->initialCongestionWindow.value();
    s.maxBurst = options->maxBurst.value();
    s.congestionControlModule = options->congestionControlModule.value();
    s.delayedSackTime = std::chrono::milliseconds(options->delayedSackTimeMs.value());
    s.minRetransmitTimeout = std::chrono::milliseconds(options->minRetransmitTimeoutMs.value());
    s.maxRetransmitTimeout = std::chrono::milliseconds(options->maxRetransmitTimeoutMs.value());
    s.initialRetransmitTimeout = std::chrono::milliseconds(options->initialRetransmitTimeoutMs.value());
    s.maxRetransmitAttempts = options->maxRetransmitAttempts.value();
    s.heartbeatInterval = std::chrono::milliseconds(options->heartbeatIntervalMs.value());

    rtc::SetSctpSettings(s);

    return 0;
}

static rtc::LogLevel s_level = rtc::LogLevel::None;
result_t rtc_base::get_loglevel(exlib::string& retVal)
{
    switch (s_level) {
    case rtc::LogLevel::Verbose:
        retVal = "verbose";
        break;
    case rtc::LogLevel::Debug:
        retVal = "debug";
        break;
    case rtc::LogLevel::Info:
        retVal = "info";
        break;
    case rtc::LogLevel::Warning:
        retVal = "warning";
        break;
    case rtc::LogLevel::Error:
        retVal = "error";
        break;
    case rtc::LogLevel::Fatal:
        retVal = "fatal";
        break;
    case rtc::LogLevel::None:
        retVal = "none";
        break;
    default:
        break;
    }

    return 0;
}

result_t rtc_base::set_loglevel(exlib::string newVal)
{
    rtc::LogLevel level;
    if (newVal == "verbose")
        level = rtc::LogLevel::Verbose;
    else if (newVal == "debug")
        level = rtc::LogLevel::Debug;
    else if (newVal == "info")
        level = rtc::LogLevel::Info;
    else if (newVal == "warning")
        level = rtc::LogLevel::Warning;
    else if (newVal == "error")
        level = rtc::LogLevel::Error;
    else if (newVal == "fatal")
        level = rtc::LogLevel::Fatal;
    else if (newVal == "none")
        level = rtc::LogLevel::None;
    else
        return Runtime::setError("Invalid log level");

    rtc::InitLogger(level);
    s_level = level;

    return 0;
}

}
