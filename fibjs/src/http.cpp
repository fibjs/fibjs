/*
 * http.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "HttpHandler.h"
#include "HttpFileHandler.h"
#include "JSHandler.h"
#include "ifs/http.h"

namespace fibjs
{

result_t http_base::handler(obj_ptr<Handler_base>& hdlr,
		obj_ptr<Handler_base>& retVal)
{
	retVal = new HttpHandler(hdlr);
	return 0;
}

result_t http_base::handler(v8::Handle<v8::Function> hdlr,
		obj_ptr<Handler_base>& retVal)
{
	obj_ptr<Handler_base> hdlr1 = new JSHandler(hdlr);
	return handler(hdlr1, retVal);
}

result_t http_base::fileHandler(const char* root, obj_ptr<Handler_base>& retVal)
{
	retVal = new HttpFileHandler(root);
	return 0;
}

}
