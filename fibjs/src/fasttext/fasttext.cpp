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

result_t FTModel::loadModel(exlib::string path)
{
    try {
        fastText_.loadModel(path);
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError("fasttext: " + exlib::string(e.what())));
    }

    return 0;
}

result_t FTModel::predict(exlib::string text, int32_t k, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    std::vector<std::pair<fasttext::real, std::string>> predictions;
    std::istringstream in;
    in.str(text);

    try {
        fastText_.predictLine(in, predictions, k, 0);
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

result_t FTModel::predict(v8::Local<v8::Array> texts, int32_t k, obj_ptr<NArray>& retVal, AsyncEvent* ac)
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

        hr = predict(v.string(), k, result, ac);
        if (hr < 0)
            return hr;

        retVal->append(result);
    }

    return 0;
}

} // namespace fibjs