/*
 * FTModel.cpp
 *
 *  Created on: Sep 20, 2023
 *      Author: lion
 */

#pragma once

#include "ifs/FTModel.h"
#include "fasttext/include/fasttext.h"

namespace fibjs {

class FTModel : public FTModel_base {
public:
    // FTModel_base
    virtual result_t isQuantized(bool& retVal);
    virtual result_t getDimension(int32_t& retVal);
    virtual result_t getWordVector(exlib::string word, v8::Local<v8::Array>& retVal);
    virtual result_t getSentenceVector(exlib::string text, v8::Local<v8::Array>& retVal);
    virtual result_t getNearestNeighbors(exlib::string word, int32_t k, v8::Local<v8::Array>& retVal);
    virtual result_t getAnalogies(exlib::string wordA, exlib::string wordB, exlib::string wordC, int32_t k, v8::Local<v8::Array>& retVal);
    virtual result_t getWordId(exlib::string word, int32_t& retVal);
    virtual result_t getSubwordId(exlib::string subword, int32_t& retVal);
    virtual result_t getInputVector(int32_t id, v8::Local<v8::Array>& retVal);
    virtual result_t getSubwords(exlib::string word, obj_ptr<GetSubwordsType>& retVal);
    virtual result_t predict(exlib::string text, int32_t k, double threshold, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t predict(v8::Local<v8::Array> texts, int32_t k, double threshold, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t quantize(exlib::string trainFile, v8::Local<v8::Object> args, AsyncEvent* ac);
    virtual result_t getWords(obj_ptr<GetWordsType>& retVal);
    virtual result_t getLabels(obj_ptr<GetLabelsType>& retVal);
    virtual result_t saveModel(exlib::string filename);

public:
    result_t loadArgs(v8::Local<v8::Object> args);
    result_t loadModel(exlib::string path);
    result_t train(exlib::string trainFile);

private:
    fasttext::Args m_args;
    fasttext::FastText fastText_;
};

} // namespace fibjs
