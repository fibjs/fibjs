#!/bin/js

String.prototype.trim = function() {
	return this.replace(/(^\s*)|(\s*$)/g, "");
}

var fs = require('fs');
var os = require('os');
var process = require('process');
var path = require('path');

var clsName = {};

var dir = fs.readdir('.');

for (var idx = 0; idx < dir.length; idx++)
	if (path.extname(dir[idx].name) === '.idl')
		preparserIDL(dir[idx].name);

for (var idx = 0; idx < dir.length; idx++)
	if (path.extname(dir[idx].name) === '.idl')
		parserIDL(dir[idx].name);

process.system('doxygen');

function preparserIDL(fname) {
	var f, line = 0,
		st, isRem;

	f = fs.readFile(fname).replace(/\r/g, "").split("\n");
	f.reverse();

	while (f.length) {
		st = getStock();

		if (st.length > 0) {
			if (st[0] == "interface" && st.length > 1) {
				if (clsName[st[1]])
					return reportErr();

				if (st.length == 2 || (st.length == 4 && st[2] == ":"))
					clsName[st[1]] = true;
			}
		}
	}

	function getStock() {
		var pos = 0,
			n, n1, st2;
		var bString = false;
		var s = f.pop();
		line++;

		if (isRem) {
			n = s.indexOf("*/", 0);
			if (n >= 0) {
				s = s.substr(n + 2);
				isRem = false;
			}
		}

		var st = s.split("\"");
		var st1 = [];
		while (pos < st.length) {
			s = st[pos++];

			if (isRem) {
				n = s.indexOf("*/", 0);
				if (n >= 0) {
					s = s.substr(n + 2);
					isRem = false;
				}
			}

			if (!isRem) {
				if (!bString) {
					n = s.indexOf("//");
					if (n >= 0)
						s = s.substr(0, n);

					while (1) {
						n = s.indexOf("/*");
						if (n >= 0) {
							n1 = s.indexOf("*/", n + 2);
							if (n1 >= 0)
								s = s.substr(0, n) + s.substr(n1 + 2);
							else {
								isRem = true;
								s = s.substr(0, n);
							}
						} else
							break;
					}

					s = s.replace(/=/g, " = ");
					s = s.replace(/\(/g, " ( ");
					s = s.replace(/\)/g, " ) ");
					s = s.replace(/\[/g, " [ ");
					s = s.replace(/\]/g, " ] ");
					s = s.replace(/,/g, " , ");
					s = s.replace(/:/g, " : ");
					s = s.replace(/;/g, " ; ");
					s = s.replace(/\s+/g, " ").trim();

					if (s != "") {
						st2 = s.split(" ");

						for (var i = 0; i < st2.length; i++) {
							s = st2[i];
							st1.push(s);
						}
					}

					if (!isRem)
						bString = true;
				} else {
					while ((s.charAt(s.length - 1) == "\\") && pos < st.length)
						s = s + "\"" + st[pos++];

					st1.push("\"" + s + "\"")
					bString = false;
				}
			}
		}

		return st1;
	}
}

function parserIDL(fname) {
	var st, f, line, cvs, ifs, afs, svs, ffs, iffs, tjfs, difms, difos, difps, dsvs, refCls, ids, ns, baseClass, isRem = false,
		hasNew = false,
		hasIndexed = false,
		hasNamed = false,
		typeMap = {
			"Integer": "int32_t",
			"Long": "int64_t",
			"Number": "double",
			"Boolean": "bool",
			"String": "std::string",
			"Date": "date_t",
			"Object": "v8::Local<v8::Object>",
			"Array": "v8::Local<v8::Array>",
			"Function": "v8::Local<v8::Function>",
			"Value": "v8::Local<v8::Value>",
			"Variant": "Variant"
		},
		aTypeMap = {
			"Integer": "int32_t",
			"Long": "int64_t",
			"Number": "double",
			"Boolean": "bool",
			"String": "const char*",
			"Date": "date_t",
			"Object": "v8::Local<v8::Object>",
			"Array": "v8::Local<v8::Array>",
			"Function": "v8::Local<v8::Function>",
			"Value": "v8::Local<v8::Value>",
			"Variant": "Variant"
		};

	function cxxSafe(fname) {
		var ckws = {
			"union": true,
			"and": true,
			"or": true,
			"xor": true,
			"new": true
		};

		return ckws.hasOwnProperty(fname) ? "_" + fname : fname;
	}

	function defMap(value) {
		var defs = {
			"undefined": "v8::Undefined(isolate)"
		};

		return defs[value] || value;
	}

	f = fs.readFile(fname).replace(/\r/g, "").split("\n");
	f.reverse();

	line = 0;
	while (f.length) {
		st = getStock();

		if (st.length > 0) {
			if (st[0] == "interface" && st.length > 1) {
				if (!_checkID(st[1]))
					return false;

				if (st.length == 2 || (st.length == 4 && st[2] == ":" && clsName[st[3]])) {
					if (!_class(st[1], st[3]))
						return false;
				} else if (st.length == 3 && st[2] == ";")
					clsName[st[1]] = true;
				else
					return reportErr();
			} else
				return reportErr();
		}
	}

	function _class(name, base) {
		var st;

		ns = name;
		baseClass = base;
		clsName[name] = true;
		hasNew = false;
		hasIndexed = false;
		hasNamed = false;

		ifs = [];
		afs = [];
		svs = [];
		ffs = [];
		iffs = [];
		tjfs = [];

		difms = [];
		difos = [];
		difps = [];

		cvs = {};
		refCls = {};

		if (!baseClass)
			baseClass = "object";

		if (baseClass != "object")
			refCls[baseClass] = true;

		ids = {};

		while (f.length) {
			st = getStock();

			if (st.length > 0) {
				if (st[0] != "{" || st.length != 1)
					return reportErr();
				else
					break;
			}
		}

		while (f.length) {
			st = getStock();

			if (st.length > 0) {
				if (st[0] == "}") {
					if (st.length == 2 && st[1] == ";") {
						putClass();

						ns = "";
						return true;
					} else
						return reportErr();
				} else {
					if (!_member(st))
						return false;
				}
			}
		}

		return reportErr();
	}

	function putClass() {
		var txt = [];
		var bRef = false;

		txt.push("/***************************************************************************");
		txt.push(" *                                                                         *");
		txt.push(" *   This file was automatically generated using idlc.js                   *");
		txt.push(" *   PLEASE DO NOT EDIT!!!!                                                *");
		txt.push(" *                                                                         *");
		txt.push(" ***************************************************************************/\n");

		txt
			.push("#ifndef _" + ns + "_base_H_\n" + "#define _" + ns + "_base_H_\n\n" + "/**\n" + " @author Leo Hoo <lion@9465.net>\n" + " */\n\n" + "#include \"../object.h\"");

		if (baseClass !== "object")
			txt.push("#include \"" + baseClass + ".h\"");

		txt.push("\nnamespace fibjs\n{\n");

		for (var cls in refCls) {
			txt.push("class " + cls + "_base;");
			bRef = true;
		}

		if (bRef)
			txt.push("");

		txt.push("class " + ns + "_base : public " + baseClass + "_base\n{");

		txt.push("	DECLARE_CLASS(" + ns + "_base);\n")

		if (svs.length) {
			txt.push("public:\n	enum{");
			txt.push(svs.join(",\n"));
			txt.push("	};\n");
		}

		if (ifs.length) {
			txt.push("public:\n	// " + ns + "_base");
			txt.push(ifs.join("\n") + "\n");
		}

		if (hasNew)
			txt.push("public:\n	template<typename T>\n	static void __new(const T &args);\n");

		txt.push("public:");
		txt.push(iffs.join("\n"));

		if (afs.length) {
			txt.push("\npublic:");
			txt.push(afs.join("\n"));
		}

		txt.push("};\n\n}\n");

		for (cls in refCls)
			if (cls !== baseClass && cls !== "object")
				txt.push("#include \"" + cls + ".h\"");

		if (bRef)
			txt.push("");

		txt.push("namespace fibjs\n{");

		txt.push("	inline ClassInfo& " + ns + "_base::class_info()\n	{")

		if (difms.length) {
			txt.push("		static ClassData::ClassMethod s_method[] = \n		{");
			txt.push(difms.join(",\n"));
			txt.push("		};\n");
		}

		if (difos.length) {
			txt.push("		static ClassData::ClassObject s_object[] = \n		{");
			txt.push(difos.join(",\n"));
			txt.push("		};\n");
		}

		if (difps.length) {
			txt.push("		static ClassData::ClassProperty s_property[] = \n		{");
			txt.push(difps.join(",\n"));
			txt.push("		};\n");
		}

		if (hasIndexed) {
			txt.push("		static ClassData::ClassIndexed s_indexed = \n		{");
			txt.push("			i_IndexedGetter, i_IndexedSetter");
			txt.push("		};\n");
		}

		if (hasNamed) {
			txt.push("		static ClassData::ClassNamed s_named = \n		{");
			txt.push("			i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator");
			txt.push("		};\n");
		}

		var strClass = "		static ClassData s_cd = \n		{ \n			\"" + ns + "\"";

		if (hasNew)
			strClass += ", s__new"
		else
			strClass += ", NULL"

		if (difms.length)
			strClass += ", \n			" + difms.length + ", s_method";
		else
			strClass += ", \n			0, NULL";

		if (difos.length)
			strClass += ", " + difos.length + ", s_object";
		else
			strClass += ", 0, NULL";

		if (difps.length)
			strClass += ", " + difps.length + ", s_property";
		else
			strClass += ", 0, NULL";

		if (hasIndexed)
			strClass += ", &s_indexed";
		else
			strClass += ", NULL";

		if (hasNamed)
			strClass += ", &s_named";
		else
			strClass += ", NULL";

		if (ns != "object")
			strClass += ",\n			&" + baseClass + "_base::class_info()";

		txt.push(strClass + "\n		};\n");
		txt.push("		static ClassInfo s_ci(s_cd);");
		txt.push("		return s_ci;\n	}\n");

		txt.push(ffs.join("\n"));

		for (var fname in ids)
			if (ids[fname].length > 0) {
				txt.push(ids[fname][1]);
				if (ids[fname][0] == "")
					txt.push("		METHOD_VOID();\n	}\n");
				else if (fname !== "_new")
					txt.push("		METHOD_RETURN();\n	}\n");
				else
					txt.push("		CONSTRUCT_RETURN();\n	}\n");
			}

		txt.push("}\n");

		txt.push("#endif\n\n");

		var sTxt = txt.join("\n");
		var sOld = "";

		try {
			sOld = fs.readFile(ns + ".h");
		} catch (e) {}

		if (sOld != sTxt) {
			fs.writeFile(ns + ".h", txt.join("\n"));
			console.log("Building:", ns + ".h");
		}
	}

	function _member(st) {
		var attr, attr1, ftype, fname, name, value, args, argArray = false,
			pos = 0,
			s, argStra = "",
			argCount = 0,
			argOpt = 0,
			ifStr = "",
			fnStr = "",
			argVars = "",
			type, r;

		args = [];

		if (st[0] == "const" || st[0] == "static" || st[0] == "readonly") {
			attr = st[0];
			pos++;
		} else
			attr = "";

		if (attr == "static" && st[pos] == "readonly") {
			attr1 = "readonly";
			pos++;
		}

		ftype = "";
		s = st[pos];
		if (s === ns && st[pos + 1] === "(") {
			attr = "static";
			fname = "_new";
			hasNew = true;
			ftype = ns;
			pos++;
		} else if (st[pos + 1] === "(") {
			fname = st[pos++];
		} else {
			if (typeMap[s]) {
				ftype = s;
				pos++;
			} else if (clsName[s]) {
				ftype = s;
				pos++;
			}

			fname = st[pos++];
		}

		if (fname == 'new') {
			if (attr != 'static')
				return reportErr();

			if (clsName[ftype] && ftype != ns)
				refCls[ftype] = true;
			else
				return reportErr();

			attr = 'new';
			fname = st[pos++];
		}

		if (fname == "operator") {
			if (ids.hasOwnProperty(st[pos] + st[pos + 1]))
				return reportErr();
			ids[st[pos] + st[pos + 1]] = [];
		} else if (st[pos] != "(") {
			if (ids.hasOwnProperty(fname))
				return reportErr();
			ids[fname] = [];
		}

		if (attr == 'new') {
			if (st[pos] != "(" || st[pos + 1] != ")")
				return reportErr();
			pos += 2;

			difos
				.push("			{\"" + fname + "\", " + ftype + "_base::class_info}");
		} else if (st[pos] == "(") {
			if (attr == "const" || attr == "readonly")
				return reportErr();

			if (ids.hasOwnProperty(fname) && (ids[fname].length == 0 || ids[fname][0] != ftype))
				return reportErr();

			if (attr == "static")
				ifStr = "	static result_t " + cxxSafe(fname) + "(";
			else
				ifStr = "	virtual result_t " + cxxSafe(fname) + "(";

			pos++;

			s = st[pos];
			while (s != ")") {
				if (s == "...") {
					pos++;
					s = st[pos];
					if (s != ")")
						return reportErr();

					if (argCount)
						ifStr += ", ";
					ifStr += "const v8::FunctionCallbackInfo<v8::Value>& args";

					argArray = true;
					break;
				}

				if (typeMap[s]) {
					type = s;
					pos++;
				} else if (clsName[s]) {
					type = s;
					pos++;
				} else
					return reportErr();

				name = st[pos++];
				if (!_checkID(name))
					return false;

				if (st[pos] == "=") {
					pos++;

					value = st[pos++];
					if (cvs[value])
						value = "_" + value;
					else if (value === '{') {
						if (st[pos] == '}' && type === "Object") {
							value = 'v8::Object::New(isolate)';
							pos++;
						} else
							reportErr();
					} else if (value === '[') {
						if (st[pos] == ']' && type === "Array") {
							value = 'v8::Array::New(isolate)';
							pos++;
						} else
							reportErr();
					} else if (r = /^(\w[\w\d_]*)\.(\w[\w\d_]*)$/.exec(value)) {
						if (clsName[r[1]]) {
							if (r[1] != ns)
								refCls[r[1]] = true;
						} else
							reportErr();

						value = r[1] + "_base::_" + r[2];
					}
				} else
					value = "";

				s = st[pos];
				if (s == ",") {
					pos++;
					s = st[pos];
					if (s == ")")
						return reportErr();
				}

				if (argCount)
					ifStr += ", ";
				ifStr += arg_type(type) + " " + name;

				argStra += ", " + arg_type(type);

				if (type === "String") {
					if (value == "")
						argVars += "		ARG(arg_string, " + argCount + ");\n";
					else
						argVars += "		OPT_ARG(arg_string, " + argCount + ", " + defMap(value) + ");\n";
				} else {
					if (value == "")
						argVars += "		ARG(" + map_type(type) + ", " + argCount + ");\n";
					else
						argVars += "		OPT_ARG(" + map_type(type) + ", " + argCount + ", " + defMap(value) + ");\n";
				}

				argCount++;
				if (value == "")
					argOpt = argCount;
			}

			pos++;

			if (ids.hasOwnProperty(fname)) {
				fnStr = ids[fname][1] + "\n		METHOD_OVER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n\n";
			} else {
				iffs.push("	static void s_" + fname + "(const v8::FunctionCallbackInfo<v8::Value>& args);");

				fnStr = "	inline void " + ns + "_base::s_" + fname + "(const v8::FunctionCallbackInfo<v8::Value>& args)\n	{\n";

				if (fname === "_new") {
					fnStr += "		CONSTRUCT_INIT();\n		__new(args);\n	}\n\n";
					fnStr += "	template<typename T>void " + ns + "_base::__new(const T& args)\n	{\n";
				}

				if (ftype != "")
					fnStr += "		" + map_type(ftype) + " vr;\n\n";

				if (attr == "")
					fnStr += "		METHOD_INSTANCE(" + ns + "_base);\n		METHOD_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n\n";
				else if (fname !== "_new")
					fnStr += "		METHOD_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n\n";
				else
					fnStr += "		CONSTRUCT_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n\n";
			}

			if (argCount)
				fnStr += argVars + "\n";

			if (ftype != "") {
				if (argCount || argArray)
					ifStr += ", ";

				ifStr += map_type(ftype) + "& retVal";

				argStra += ", " + map_type(ftype);
			}

			if (st[pos] == "async") {
				pos += 1;

				if (ftype != "" || argCount || argArray)
					ifStr += ", ";

				if (attr == "static") {
					ifStr += "exlib::AsyncEvent* ac);";
					fnStr += "		hr = ac_" + fname + "(";
					afs.push('	ASYNC_STATIC' + (ftype != "" ? "VALUE" : "") + (ftype == "" ? argCount : argCount + 1) + '(' + ns + '_base, ' + fname + argStra + ');');
				} else {
					ifStr += "exlib::AsyncEvent* ac) = 0;";
					fnStr += "		hr = pInst->ac_" + fname + "(";
					afs.push('	ASYNC_MEMBER' + (ftype != "" ? "VALUE" : "") + (ftype == "" ? argCount : argCount + 1) + '(' + ns + '_base, ' + fname + argStra + ');');
				}

				// if (argArray || (ftype != "") || (argCount > 0))
				// fnStr += ", ";
			} else {
				if (attr == "static") {
					if (fname === "_new")
						ifStr += ", v8::Local<v8::Object> This = v8::Local<v8::Object>());";
					else
						ifStr += ");";
					fnStr += "		hr = " + cxxSafe(fname) + "(";
				} else {
					ifStr += ") = 0;";
					fnStr += "		hr = pInst->" + cxxSafe(fname) + "(";
				}
			}

			for (var i = 0; i < argCount; i++) {
				if (i > 0)
					fnStr += ", ";
				fnStr += "v" + i;
			}

			if (argArray)
				if (argCount > 0)
					fnStr += ", args";
				else
					fnStr += "args";

			if (ftype != "") {
				if (argCount || argArray)
					fnStr += ", ";

				fnStr += "vr";
			}

			if (fname === "_new")
				fnStr += ", args.This());\n";
			else
				fnStr += ");\n";

			ifs.push(ifStr);

			if (!ids.hasOwnProperty(fname)) {
				if (attr == "static") {
					if (fname !== "_new")
						difms.push("			{\"" + fname + "\", s_" + fname + "}");
				} else
					difms.push("			{\"" + fname + "\", s_" + fname + "}");
			}

			ids[fname] = [ftype, fnStr];
		} else if (ftype != "") {
			if (attr == "const") {
				if (st[pos] != "=" || ftype != "Integer")
					return reportErr();

				cvs[fname] = true;
				pos++;
				value = st[pos++];

				iffs
					.push("	static void s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);");
				fnStr = "	inline void " + ns + "_base::s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n";
				fnStr += "		" + map_type(ftype) + " vr = _" + fname + ";\n";
				fnStr += "		PROPERTY_ENTER();\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)

				ifStr = "		_" + fname + " = " + value;

				svs.push(ifStr);

				difps.push("			{\"" + fname + "\", s_get_" + fname + ", block_set}");
			} else if (attr == "static") {
				if (st[pos] != ";")
					return reportErr();

				iffs
					.push("	static void s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);");
				fnStr = "	inline void " + ns + "_base::s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n";
				fnStr += "		" + map_type(ftype) + " vr;\n\n		PROPERTY_ENTER();\n\n";

				fnStr += "		hr = get_" + fname + "(";
				fnStr += "vr";
				fnStr += ");\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)

				ifStr = "	static result_t get_" + fname + "(" + map_type(ftype) + "& retVal);";
				ifs.push(ifStr);

				if (attr1 != "readonly") {
					iffs
						.push("	static void s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);");
					fnStr = "	inline void " + ns + "_base::s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)\n	{\n		PROPERTY_ENTER();\n";
					if (ftype === "String")
						fnStr += "		PROPERTY_VAL(arg_string);\n";
					else
						fnStr += "		PROPERTY_VAL(" + map_type(ftype) + ");\n\n";
					fnStr += "		hr = set_" + fname + "(v0);\n\n		PROPERTY_SET_LEAVE();\n	}\n";
					ffs.push(fnStr)

					ifStr = "	static result_t set_" + fname + "(" + arg_type(ftype) + " newVal);";
					ifs.push(ifStr);
				}

				if (attr1 == "readonly")
					difps.push("			{\"" + fname + "\", s_get_" + fname + ", block_set}");
				else
					difps.push("			{\"" + fname + "\", s_get_" + fname + ", s_set_" + fname + "}");

			} else if (fname === "operator") {
				if ((st[pos] === "[") && (st[pos + 1] === "]") && (st[pos + 2] === ";")) {
					pos += 2;
					hasIndexed = true;

					iffs
						.push("	static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args);");
					fnStr = "	inline void " + ns + "_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n";
					fnStr += "		" + map_type(ftype) + " vr;\n\n";
					fnStr += "		PROPERTY_ENTER();\n		PROPERTY_INSTANCE(" + ns + "_base);\n\n";

					fnStr += "		hr = pInst->_indexed_getter(index, vr);\n\n		METHOD_RETURN();\n	}\n";
					ffs.push(fnStr)

					ifStr = "	virtual result_t _indexed_getter(uint32_t index, " + map_type(ftype) + "& retVal) = 0;";
					ifs.push(ifStr);

					if (attr != "readonly") {
						iffs
							.push("	static void i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args);");
						fnStr = "	inline void " + ns + "_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n		PROPERTY_ENTER();\n";
						fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
						if (ftype === "String")
							fnStr += "		PROPERTY_VAL(arg_string);\n";
						else
							fnStr += "		PROPERTY_VAL(" + map_type(ftype) + ");\n";
						fnStr += "		hr = pInst->_indexed_setter(index, v0);\n\n		METHOD_VOID();\n	}\n";
						ffs.push(fnStr);

						ifStr = "	virtual result_t _indexed_setter(uint32_t index, " + arg_type(ftype) + " newVal) = 0;";
						ifs.push(ifStr);
					}
				} else if ((st[pos] === "[") && (st[pos + 1] === "String") && (st[pos + 2] === "]") && (st[pos + 3] === ";")) {
					pos += 3;
					hasNamed = true;

					iffs
						.push("	static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);");
					fnStr = "	inline void " + ns + "_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n";
					fnStr += "		" + map_type(ftype) + " vr;\n\n";
					fnStr += "		PROPERTY_ENTER();\n		PROPERTY_INSTANCE(" + ns + "_base);\n\n";

					fnStr += "		v8::String::Utf8Value k(property);\n		if(class_info().has(*k))return;\n\n"
					fnStr += "		hr = pInst->_named_getter(*k, vr);\n		if(hr == CALL_RETURN_NULL)return;\n\n		METHOD_RETURN();\n	}\n";

					ffs.push(fnStr)

					ifStr = "	virtual result_t _named_getter(const char* property, " + map_type(ftype) + "& retVal) = 0;";
					ifs.push(ifStr);

					iffs
						.push("	static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args);");
					fnStr = "	inline void " + ns + "_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args)\n	{\n";
					fnStr += "		v8::Local<v8::Array> vr;\n\n";
					fnStr += "		PROPERTY_ENTER();\n		PROPERTY_INSTANCE(" + ns + "_base);\n\n";

					fnStr += "		hr = pInst->_named_enumerator(vr);\n\n		METHOD_RETURN1();\n	}\n";

					ffs.push(fnStr)

					ifStr = "	virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;";
					ifs.push(ifStr);

					if (attr != "readonly") {
						iffs
							.push("	static void i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args);");
						fnStr = "	inline void " + ns + "_base::i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n		PROPERTY_ENTER();\n";
						fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
						if (ftype === "String")
							fnStr += "		PROPERTY_VAL(arg_string);\n";
						else
							fnStr += "		PROPERTY_VAL(" + map_type(ftype) + ");\n";
						fnStr += "		v8::String::Utf8Value k(property);\n		if(class_info().has(*k))return;\n\n"
						fnStr += "		hr = pInst->_named_setter(*k, v0);\n\n		METHOD_VOID();\n	}\n";
						ffs.push(fnStr);

						ifStr = "	virtual result_t _named_setter(const char* property, " + arg_type(ftype) + " newVal) = 0;";
						ifs.push(ifStr);

						iffs
							.push("	static void i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean> &args);");
						fnStr = "	inline void " + ns + "_base::i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean> &args)\n	{\n";
						fnStr += "		v8::Local<v8::Boolean> vr;\n\n";
						fnStr += "		PROPERTY_ENTER();\n		PROPERTY_INSTANCE(" + ns + "_base);\n\n";

						fnStr += "		v8::String::Utf8Value k(property);\n		if(class_info().has(*k)){args.GetReturnValue().Set(v8::False(isolate));return;}\n\n"
						fnStr += "		hr = pInst->_named_deleter(*k, vr);\n		METHOD_RETURN1();\n	}\n";

						ffs.push(fnStr)

						ifStr = "	virtual result_t _named_deleter(const char* property, v8::Local<v8::Boolean>& retVal) = 0;";
						ifs.push(ifStr);
					}
				} else
					return reportErr();
			} else {
				if (st[pos] != ";")
					return reportErr();

				if (ftype === "String")
					tjfs.push("		CLONE_String(" + fname + ");");
				else if (typeMap[ftype])
					tjfs.push("		CLONE(" + fname + ", " + map_type(ftype) + ");");
				else if (clsName[ftype])
					tjfs.push("		CLONE_CLASS(" + fname + ", " + ftype + "_base);");

				iffs
					.push("	static void s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);");
				fnStr = "	inline void " + ns + "_base::s_get_" + fname + "(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)\n	{\n";
				fnStr += "		" + map_type(ftype) + " vr;\n\n";
				fnStr += "		PROPERTY_ENTER();\n		PROPERTY_INSTANCE(" + ns + "_base);\n\n";

				fnStr += "		hr = pInst->get_" + fname + "(";
				fnStr += "vr";
				fnStr += ");\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)

				ifStr = "	virtual result_t get_" + fname + "(" + map_type(ftype) + "& retVal) = 0;";
				ifs.push(ifStr);

				if (attr != "readonly") {
					iffs
						.push("	static void s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);");
					fnStr = "	inline void " + ns + "_base::s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)\n	{\n		PROPERTY_ENTER();\n";
					fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
					if (ftype === "String")
						fnStr += "		PROPERTY_VAL(arg_string);\n";
					else
						fnStr += "		PROPERTY_VAL(" + map_type(ftype) + ");\n";
					fnStr += "		hr = pInst->set_" + fname + "(v0);\n\n		PROPERTY_SET_LEAVE();\n	}\n";
					ffs.push(fnStr)

					ifStr = "	virtual result_t set_" + fname + "(" + arg_type(ftype) + " newVal) = 0;";
					ifs.push(ifStr);
				}

				if (attr == "readonly")
					difps.push("			{\"" + fname + "\", s_get_" + fname + ", block_set}");
				else
					difps.push("			{\"" + fname + "\", s_get_" + fname + ", s_set_" + fname + "}");
			}
		} else
			return reportErr();

		if (st[pos] == ";")
			pos++;
		else
			return reportErr();

		if (pos < st.length)
			return reportErr();

		return true;
	}

	function arg_type(n) {
		if (aTypeMap[n])
			return aTypeMap[n];

		if (clsName[n]) {
			if (n != ns)
				refCls[n] = true;
			return n + "_base*";
		}

		return reportErr();
	}

	function map_type(n) {
		if (typeMap[n])
			return typeMap[n];

		if (clsName[n]) {
			if (n != ns)
				refCls[n] = true;
			return "obj_ptr<" + n + "_base>";
		}

		return reportErr();
	}

	function _checkID(s) {
		var r = s.match(/[a-zA-Z]\w*/);
		if (!r)
			return reportErr();
		if (r[0] != s)
			return reportErr();

		if (s == "interface" || s == "const" || s == "static" || s == "readonly" || s == "")
			return reportErr();

		return true;
	}

	function getStock() {
		var pos = 0,
			n, n1, st2;
		var bString = false;
		var s = f.pop();
		line++;

		if (isRem) {
			n = s.indexOf("*/", 0);
			if (n >= 0) {
				s = s.substr(n + 2);
				isRem = false;
			}
		}

		var st = s.split("\"");
		var st1 = [];
		while (pos < st.length) {
			s = st[pos++];

			if (isRem) {
				n = s.indexOf("*/", 0);
				if (n >= 0) {
					s = s.substr(n + 2);
					isRem = false;
				}
			}

			if (!isRem) {
				if (!bString) {
					n = s.indexOf("//");
					n1 = s.indexOf("/*");
					if (n >= 0 && (n1 < 0 || n < n1))
						s = s.substr(0, n);

					while (1) {
						n = s.indexOf("/*");
						if (n >= 0) {
							n1 = s.indexOf("*/", n + 2);
							if (n1 >= 0)
								s = s.substr(0, n) + s.substr(n1 + 2);
							else {
								isRem = true;
								s = s.substr(0, n);
							}
						} else
							break;
					}

					s = s.replace(/=/g, " = ");
					s = s.replace(/\(/g, " ( ");
					s = s.replace(/\)/g, " ) ");
					s = s.replace(/\[/g, " [ ");
					s = s.replace(/\]/g, " ] ");
					s = s.replace(/\{/g, " { ");
					s = s.replace(/\}/g, " } ");
					s = s.replace(/,/g, " , ");
					s = s.replace(/:/g, " : ");
					s = s.replace(/;/g, " ; ");
					s = s.replace(/\s+/g, " ").trim();

					if (s != "") {
						st2 = s.split(" ");

						for (var i = 0; i < st2.length; i++) {
							s = st2[i];
							st1.push(s);
						}
					}

					if (!isRem)
						bString = true;
				} else {
					while ((s.charAt(s.length - 1) == "\\") && pos < st.length)
						s = s + "\"" + st[pos++];

					st1.push("\"" + s + "\"")
					bString = false;
				}
			}
		}

		return st1;
	}

	function reportErr() {
		throw new Error("<" + fname + "> Line " + line + ": Syntax Error.");
	}

}