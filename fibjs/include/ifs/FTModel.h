/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class FTModel_base : public object_base {
    DECLARE_CLASS(FTModel_base);

public:
    class GetSubwordsType : public NType {
    public:
        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("subwords"), GetReturnValue(isolate, subwords)).Check();
            retVal->Set(context, isolate->NewString("ngrams"), GetReturnValue(isolate, ngrams)).Check();
        }

        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, subwords));
            args.push_back(GetReturnValue(isolate, ngrams));
        }

    public:
        v8::Local<v8::Array> subwords;
        v8::Local<v8::Array> ngrams;
    };
    class GetWordsType : public NType {
    public:
        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("vocabs"), GetReturnValue(isolate, vocabs)).Check();
            retVal->Set(context, isolate->NewString("freqs"), GetReturnValue(isolate, freqs)).Check();
        }

        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, vocabs));
            args.push_back(GetReturnValue(isolate, freqs));
        }

    public:
        v8::Local<v8::Array> vocabs;
        v8::Local<v8::Array> freqs;
    };
    class GetLabelsType : public NType {
    public:
        virtual void fillMembers(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("labels"), GetReturnValue(isolate, labels)).Check();
            retVal->Set(context, isolate->NewString("freqs"), GetReturnValue(isolate, freqs)).Check();
        }

        virtual void fillArguments(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, labels));
            args.push_back(GetReturnValue(isolate, freqs));
        }

    public:
        v8::Local<v8::Array> labels;
        v8::Local<v8::Array> freqs;
    };

public:
    // FTModel_base
    virtual result_t isQuantized(bool& retVal) = 0;
    virtual result_t getDimension(int32_t& retVal) = 0;
    virtual result_t getWordVector(exlib::string word, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t getSentenceVector(exlib::string text, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t getNearestNeighbors(exlib::string word, int32_t k, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t getAnalogies(exlib::string wordA, exlib::string wordB, exlib::string wordC, int32_t k, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t getWordId(exlib::string word, int32_t& retVal) = 0;
    virtual result_t getSubwordId(exlib::string subword, int32_t& retVal) = 0;
    virtual result_t getInputVector(int32_t id, v8::Local<v8::Array>& retVal) = 0;
    virtual result_t getSubwords(exlib::string word, obj_ptr<GetSubwordsType>& retVal) = 0;
    virtual result_t predict(exlib::string text, int32_t k, double threshold, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t predict(v8::Local<v8::Array> texts, int32_t k, double threshold, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t quantize(exlib::string trainFile, v8::Local<v8::Object> args, AsyncEvent* ac) = 0;
    virtual result_t getWords(obj_ptr<GetWordsType>& retVal) = 0;
    virtual result_t getLabels(obj_ptr<GetLabelsType>& retVal) = 0;
    virtual result_t saveModel(exlib::string filename) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_isQuantized(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getDimension(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getWordVector(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSentenceVector(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getNearestNeighbors(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getAnalogies(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getWordId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSubwordId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getInputVector(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSubwords(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_predict(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_quantize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getWords(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getLabels(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_saveModel(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE4(FTModel_base, predict, exlib::string, int32_t, double, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE4(FTModel_base, predict, v8::Local<v8::Array>, int32_t, double, obj_ptr<NArray>);
    ASYNC_MEMBER2(FTModel_base, quantize, exlib::string, v8::Local<v8::Object>);
};
}

namespace fibjs {
inline ClassInfo& FTModel_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isQuantized", s_isQuantized, false, false },
        { "getDimension", s_getDimension, false, false },
        { "getWordVector", s_getWordVector, false, false },
        { "getSentenceVector", s_getSentenceVector, false, false },
        { "getNearestNeighbors", s_getNearestNeighbors, false, false },
        { "getAnalogies", s_getAnalogies, false, false },
        { "getWordId", s_getWordId, false, false },
        { "getSubwordId", s_getSubwordId, false, false },
        { "getInputVector", s_getInputVector, false, false },
        { "getSubwords", s_getSubwords, false, false },
        { "predict", s_predict, false, true },
        { "predictSync", s_predict, false, false },
        { "quantize", s_quantize, false, true },
        { "quantizeSync", s_quantize, false, false },
        { "getWords", s_getWords, false, false },
        { "getLabels", s_getLabels, false, false },
        { "saveModel", s_saveModel, false, false }
    };

    static ClassData s_cd = {
        "FTModel", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FTModel_base::s_isQuantized(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isQuantized(vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getDimension(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getDimension(vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getWordVector(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getWordVector(v0, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getSentenceVector(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getSentenceVector(v0, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getNearestNeighbors(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 10);

    hr = pInst->getNearestNeighbors(v0, v1, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getAnalogies(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);
    OPT_ARG(int32_t, 3, 10);

    hr = pInst->getAnalogies(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getWordId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getWordId(v0, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getSubwordId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getSubwordId(v0, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getInputVector(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->getInputVector(v0, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getSubwords(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GetSubwordsType> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getSubwords(v0, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_predict(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 3);
    OPT_ARG(double, 2, 0.0);

    if (!cb.IsEmpty())
        hr = pInst->acb_predict(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_predict(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 1);

    ARG(v8::Local<v8::Array>, 0);
    OPT_ARG(int32_t, 1, 3);
    OPT_ARG(double, 2, 0.0);

    if (!cb.IsEmpty())
        hr = pInst->acb_predict(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_predict(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_quantize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_quantize(v0, v1, cb, args);
    else
        hr = pInst->ac_quantize(v0, v1);

    METHOD_VOID();
}

inline void FTModel_base::s_getWords(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GetWordsType> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getWords(vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_getLabels(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GetLabelsType> vr;

    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getLabels(vr);

    METHOD_RETURN();
}

inline void FTModel_base::s_saveModel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->saveModel(v0);

    METHOD_VOID();
}
}
