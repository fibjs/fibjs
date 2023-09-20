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
    virtual result_t predict(exlib::string text, int32_t k, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t predict(v8::Local<v8::Array> texts, int32_t k, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    result_t loadModel(exlib::string path);

private:
  fasttext::FastText fastText_;
};

} // namespace fibjs
