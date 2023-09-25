/*
 * fasttext.cpp
 *
 *  Created on: Sep 20, 2023
 *      Author: lion
 */

#include "ifs/fasttext.h"
#include "FTModel.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace fibjs {

DECLARE_MODULE(fasttext);

result_t fasttext_base::loadModel(exlib::string path, obj_ptr<FTModel_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<FTModel> model = new FTModel();

    result_t hr = model->loadModel(path);
    if (hr < 0)
        return hr;

    retVal = model;

    return 0;
}

result_t fasttext_base::train(exlib::string trainFile, v8::Local<v8::Object> args,
    obj_ptr<FTModel_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        obj_ptr<FTModel> model = new FTModel();
        result_t hr = model->loadArgs(args);
        if (hr < 0)
            return hr;
        ac->m_ctx[0] = model;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<FTModel> model = (FTModel*)ac->m_ctx[0].object();
    result_t hr = model->train(trainFile);
    if (hr < 0)
        return hr;

    retVal = model;

    return 0;
}

result_t FTModel::loadArgs(v8::Local<v8::Object> args)
{
    static const char* s_keys[] = {
        "model", "lr", "dim", "ws", "epoch", "minCount", "minCountLabel",
        "minn", "maxn", "neg", "wordNgrams", "loss", "bucket",
        "thread", "lrUpdateRate", "t", "label", "verbose", "pretrainedVectors", "retrain",
        "seed", "autotuneValidationFile", "autotuneMetric",
        "autotunePredictions", "autotuneDuration", "autotuneModelSize", NULL
    };

    result_t hr;
    hr = CheckConfig(args, s_keys);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();
    fasttext::Args args_(m_args);

    exlib::string model;
    switch (args_.model) {
    case fasttext::model_name::cbow:
        model = "cbow";
    case fasttext::model_name::sg:
        model = "sg";
    case fasttext::model_name::sup:
        model = "sup";
    }

    hr = GetConfigValue(isolate, args, "model", model, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (model == "supervised") {
            if (args_.model != fasttext::model_name::sup) {
                args_.lr = 0.1;
                args_.minCount = 1;
                args_.minn = 0;
                args_.maxn = 0;
                args_.loss = fasttext::loss_name::softmax;
            }

            args_.model = fasttext::model_name::sup;
        } else {
            if (args_.model == fasttext::model_name::sup) {
                args_.lr = 0.05;
                args_.minCount = 5;
                args_.minn = 3;
                args_.maxn = 6;
                args_.loss = fasttext::loss_name::ns;
            }

            if (model == "cbow")
                args_.model = fasttext::model_name::cbow;
            else if (model == "skipgram")
                args_.model = fasttext::model_name::sg;
            else
                return CHECK_ERROR(Runtime::setError("fasttext: invalid model"));
        }
    }

    hr = GetConfigValue(isolate, args, "lr", args_.lr, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "dim", args_.dim, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "ws", args_.ws, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "epoch", args_.epoch, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "minCount", args_.minCount, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "minCountLabel", args_.minCountLabel, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "minn", args_.minn, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "maxn", args_.maxn, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "neg", args_.neg, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "wordNgrams", args_.wordNgrams, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string loss(args_.lossToString(args_.loss));
    hr = GetConfigValue(isolate, args, "loss", loss, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (loss == "hs")
            args_.loss = fasttext::loss_name::hs;
        else if (loss == "ns")
            args_.loss = fasttext::loss_name::ns;
        else if (loss == "softmax")
            args_.loss = fasttext::loss_name::softmax;
        else if (loss == "ova")
            args_.loss = fasttext::loss_name::ova;
        else
            return CHECK_ERROR(Runtime::setError("fasttext: invalid loss"));
    }

    hr = GetConfigValue(isolate, args, "bucket", args_.bucket, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "thread", args_.thread, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "lrUpdateRate", args_.lrUpdateRate, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "t", args_.t, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string label(args_.label);
    hr = GetConfigValue(isolate, args, "label", label, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL)
        args_.label = label;

    hr = GetConfigValue(isolate, args, "verbose", args_.verbose, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string pretrainedVectors(args_.pretrainedVectors);
    hr = GetConfigValue(isolate, args, "pretrainedVectors", pretrainedVectors, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL)
        args_.pretrainedVectors = pretrainedVectors;

    hr = GetConfigValue(isolate, args, "retrain", args_.retrain, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "seed", args_.seed, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string autotuneValidationFile(args_.autotuneValidationFile);
    hr = GetConfigValue(isolate, args, "autotuneValidationFile", autotuneValidationFile, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL)
        args_.autotuneValidationFile = autotuneValidationFile;

    exlib::string autotuneMetric(args_.autotuneMetric);
    hr = GetConfigValue(isolate, args, "autotuneMetric", autotuneMetric, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL)
        args_.autotuneMetric = autotuneMetric;

    hr = GetConfigValue(isolate, args, "autotunePredictions", args_.autotunePredictions, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate, args, "autotuneDuration", args_.autotuneDuration, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string autotuneModelSize(args_.autotuneModelSize);
    hr = GetConfigValue(isolate, args, "autotuneModelSize", autotuneModelSize, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    if (hr != CALL_E_PARAMNOTOPTIONAL)
        args_.autotuneModelSize = autotuneModelSize;

    try {
        args_.getAutotuneMetric();
        args_.getAutotuneModelSize();
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    m_args = args_;

    return 0;
}

result_t FTModel::loadModel(exlib::string path)
{
    try {
        fastText_.loadModel(path);
        m_args = fastText_.getArgs();
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    return 0;
}

result_t FTModel::train(exlib::string trainFile)
{
    m_args.input = trainFile;

    try {
        fastText_.train(m_args);
        m_args = fastText_.getArgs();
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    return 0;
}

result_t FTModel::quantize(exlib::string trainFile, v8::Local<v8::Object> args, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = loadArgs(args);
        if (hr < 0)
            return hr;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    m_args.input = trainFile;

    try {
        fastText_.quantize(m_args);
        m_args = fastText_.getArgs();
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    return 0;
}

result_t FTModel::isQuantized(bool& retVal)
{
    retVal = fastText_.isQuant();
    return 0;
}

result_t FTModel::getDimension(int32_t& retVal)
{
    retVal = fastText_.getDimension();
    return 0;
}

inline void _fill_array(Isolate* isolate, v8::Local<v8::Array>& arr, fasttext::Vector& vec)
{
    v8::Local<v8::Context> context = isolate->context();

    arr = v8::Array::New(isolate->m_isolate, vec.size());
    for (int i = 0; i < vec.size(); i++)
        arr->Set(context, i, v8::Number::New(arr->GetIsolate(), vec[i]));
}

result_t FTModel::getWordVector(exlib::string word, v8::Local<v8::Array>& retVal)
{
    fasttext::Vector vec(fastText_.getDimension());

    fastText_.getWordVector(vec, word);
    _fill_array(holder(), retVal, vec);

    return 0;
}

result_t FTModel::getSentenceVector(exlib::string text, v8::Local<v8::Array>& retVal)
{
    if (text.find('\n') != exlib::string::npos)
        return CHECK_ERROR(Runtime::setError("fasttext: invalid text"));

    fasttext::Vector vec(fastText_.getDimension());

    std::vector<std::pair<fasttext::real, std::string>> predictions;
    std::istringstream in;
    in.str(text + '\n');

    fastText_.getSentenceVector(in, vec);
    _fill_array(holder(), retVal, vec);

    return 0;
}

result_t FTModel::getNearestNeighbors(exlib::string word, int32_t k, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    std::vector<std::pair<fasttext::real, std::string>> neighbors;

    neighbors = fastText_.getNN(word, k);
    retVal = v8::Array::New(holder()->m_isolate, neighbors.size());
    for (int i = 0; i < neighbors.size(); i++) {
        v8::Local<v8::Object> obj = v8::Object::New(holder()->m_isolate);

        obj->Set(context, isolate->NewString("label"), isolate->NewString(neighbors[i].second));
        obj->Set(context, isolate->NewString("value"), v8::Number::New(holder()->m_isolate, neighbors[i].first));

        retVal->Set(context, i, obj);
    }

    return 0;
}

result_t FTModel::getAnalogies(exlib::string wordA, exlib::string wordB, exlib::string wordC, int32_t k, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    std::vector<std::pair<fasttext::real, std::string>> analogies;

    analogies = fastText_.getAnalogies(k, wordA, wordB, wordC);
    retVal = v8::Array::New(holder()->m_isolate, analogies.size());
    for (int i = 0; i < analogies.size(); i++) {
        v8::Local<v8::Object> obj = v8::Object::New(holder()->m_isolate);

        obj->Set(context, isolate->NewString("label"), isolate->NewString(analogies[i].second));
        obj->Set(context, isolate->NewString("value"), v8::Number::New(holder()->m_isolate, analogies[i].first));

        retVal->Set(context, i, obj);
    }

    return 0;
}

result_t FTModel::getWordId(exlib::string word, int32_t& retVal)
{
    retVal = fastText_.getWordId(word);
    return 0;
}

result_t FTModel::getSubwordId(exlib::string subword, int32_t& retVal)
{
    retVal = fastText_.getSubwordId(subword);
    return 0;
}

result_t FTModel::getSubwords(exlib::string word, obj_ptr<GetSubwordsType>& retVal)
{
    Isolate* isolate = holder();
    std::string word_(word);
    std::vector<std::string> subwords;
    std::vector<int32_t> ngrams;

    std::shared_ptr<const fasttext::Dictionary> d = fastText_.getDictionary();
    d->getSubwords(word_, ngrams, subwords);

    retVal = new GetSubwordsType();
    retVal->subwords = GetReturnValue(isolate, subwords).As<v8::Array>();
    retVal->ngrams = GetReturnValue(isolate, ngrams).As<v8::Array>();

    return 0;
}

result_t FTModel::getInputVector(int32_t id, v8::Local<v8::Array>& retVal)
{
    fasttext::Vector vec(fastText_.getDimension());

    fastText_.getInputVector(vec, id);
    _fill_array(holder(), retVal, vec);

    return 0;
}

result_t FTModel::predict(exlib::string text, int32_t k, double threshold, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        if (text.find('\n') != exlib::string::npos)
            return CHECK_ERROR(Runtime::setError("fasttext: invalid text"));
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    std::vector<std::pair<fasttext::real, std::string>> predictions;
    std::istringstream in;
    in.str(text + '\n');

    try {
        fastText_.predictLine(in, predictions, k, threshold);
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    retVal = new NArray();
    for (auto it = predictions.cbegin(); it != predictions.cend(); it++) {
        obj_ptr<NObject> obj = new NObject();

        obj->add("label", exlib::string(it->second));
        obj->add("value", it->first);

        retVal->append(obj);
    }

    return 0;
}

result_t FTModel::predict(v8::Local<v8::Array> texts, int32_t k, double threshold, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        obj_ptr<NArray> _texts = new NArray();
        result_t hr = _texts->append_array<exlib::string>(texts);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = _texts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t hr;
    obj_ptr<NArray> _texts = (NArray*)ac->m_ctx[0].object();

    retVal = new NArray();
    for (int i = 0; i < _texts->length(); i++) {
        Variant v;
        obj_ptr<NArray> result;
        _texts->_indexed_getter(i, v);

        hr = predict(v.string(), k, threshold, result, ac);
        if (hr < 0)
            return hr;

        retVal->append(result);
    }

    return 0;
}

result_t FTModel::getWords(obj_ptr<GetWordsType>& retVal)
{
    std::vector<std::string> vocabs;
    std::vector<int64_t> freqs;
    std::shared_ptr<const fasttext::Dictionary> d = fastText_.getDictionary();

    freqs = d->getCounts(fasttext::entry_type::word);
    for (int32_t i = 0; i < freqs.size(); i++)
        vocabs.push_back(d->getWord(i));

    retVal = new GetWordsType();
    retVal->vocabs = GetReturnValue(holder(), vocabs).As<v8::Array>();
    retVal->freqs = GetReturnValue(holder(), freqs).As<v8::Array>();

    return 0;
}

result_t FTModel::getLabels(obj_ptr<GetLabelsType>& retVal)
{
    std::vector<std::string> labels;
    std::vector<int64_t> freqs;
    std::shared_ptr<const fasttext::Dictionary> d = fastText_.getDictionary();

    freqs = d->getCounts(fasttext::entry_type::label);
    for (int32_t i = 0; i < freqs.size(); i++)
        labels.push_back(d->getLabel(i));

    retVal = new GetLabelsType();
    retVal->labels = GetReturnValue(holder(), labels).As<v8::Array>();
    retVal->freqs = GetReturnValue(holder(), freqs).As<v8::Array>();
    return 0;
}

result_t FTModel::saveModel(exlib::string filename)
{
    try {
        fastText_.saveModel(filename);
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    return 0;
}

} // namespace fibjs