String.prototype.trim = function () {
	return this.replace(/(^\s*)|(\s*$)/g, "");
}

var fs = require('fs');

parserIDL("if.idl");

function parserIDL(fname) {
	var st,
	f,
	line,
	ifs,
	afs,
	svs,
	ffs,
	iffs,
	difms,
	difps,
	dsvs,
	refCls,
	ids,
	ns,
	baseClass,
	basePath = "",
	clsName,
	isRem = false,
	hasNew = false,
	hasIndexed = false,
	typeMap = {
		"Integer" : "int32_t",
		"Long" : "int64_t",
		"Number" : "double",
		"Boolean" : "bool",
		"String" : "std::string",
		"Object" : "v8::Handle<v8::Object>",
		"Array" : "v8::Handle<v8::Array>",
		"Function" : "v8::Handle<v8::Function>",
		"Value" : "v8::Handle<v8::Value>"
	},
	aTypeMap = {
		"Integer" : "int32_t",
		"Long" : "int64_t",
		"Number" : "double",
		"Boolean" : "bool",
		"String" : "const char*",
		"Object" : "v8::Handle<v8::Object>",
		"Function" : "v8::Handle<v8::Function>",
		"Value" : "v8::Handle<v8::Value>"
	};
	
	f = fs.readFile(basePath + fname).replace(/\r/g, "").split("\n");
	f.reverse();
	
	line = 0;
	clsName = {};
	while (f.length) {
		st = getStock();
		
		if (st.length > 0) {
			if (st[0] == "class" && st.length > 1) {
				if (!_checkID(st[1]))
					return false;
				
				if (clsName[st[1]])
					return reportErr();
				
				if (st.length == 2 || (st.length == 4 && st[2] == ":" && clsName[st[3]])) {
					if (!_class(st[1], st[3]))
						return false;
				} else if (st.length == 3 && st[2] == ";")
					clsName[st[1]] = true;
				else return reportErr(); 
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
		ifs = [];
		afs = [];
		svs = [];
		ffs = [];
		iffs = [];
		
		difms = [];
		difps = [];
		
		refCls = {};
		
		if(!baseClass)
			baseClass = "object";
		
		if(baseClass != "object")
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
				}else
				{
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
		
		txt.push("/***************************************************************************\n"
			 + " *   Copyright (C) 2012 by Leo Hoo                                         *\n"
			 + " *   lion@9465.net                                                         *\n"
			 + " *                                                                         *\n"
			 + " ***************************************************************************/\n\n"
			 + "#ifndef _" + ns + "_base_H_\n"
			 + "#define _" + ns + "_base_H_\n\n"
			 + "/**\n"
			 + " @author Leo Hoo <lion@9465.net>\n"
			 + " */\n\n"
			 + "#include \"../object.h\"");

		if(baseClass !== "object")
			txt.push("#include \"" + baseClass + ".h\"");

		txt.push("\nnamespace fibjs\n{\n");
		
		for (cls in refCls) {
			txt.push("class " + cls + "_base;");
			bRef = true;
		}
		
		if (bRef)
			txt.push("");
		
		txt.push("class " + ns + "_base : public " + baseClass + "_base\n{");
		
		if (svs.length) {
			txt.push("public:");
			txt.push(svs.join("\n") + "\n");
		}
		
		if (ifs.length) {
			txt.push("public:\n	// " + ns + "_base");
			txt.push(ifs.join("\n") + "\n");
		}
		
		txt.push("public:\n	static ClassInfo& class_info()\n	{")
		
		if (difms.length) {
			txt.push("		static ClassMethod s_method[] = \n		{");
			txt.push(difms.join(",\n"));
			txt.push("		};\n");
		}

		if (difps.length) {
			txt.push("		static ClassProperty s_property[] = \n		{");
			txt.push(difps.join(",\n"));
			txt.push("		};\n");
		}
		
		if(hasIndexed)
		{
			txt.push("		static ClassIndexed s_indexed = \n		{");
			txt.push("			i_IndexedGetter, i_IndexedSetter");
			txt.push("		};\n");
		}
		
		var strClass = "		static ClassData s_cd = \n		{ \n			\"" + ns + "\"";

		if(hasNew)
			strClass += ", s__new"
		else strClass += ", NULL"

		if (difms.length)
			strClass += ", \n			" + difms.length + ", s_method";
		else
			strClass += ", \n			0, NULL";

		if (difps.length)
			strClass += ", " + difps.length + ", s_property";
		else
			strClass += ", 0, NULL";
			
		if(hasIndexed)
			strClass += ", &s_indexed";
		else
			strClass += ", NULL";

		if(ns != "object")
			strClass += ",\n			&" + baseClass + "_base::class_info()";
		
		txt.push(strClass + "\n		};\n");
		txt.push("		static ClassInfo s_ci(s_cd);");
		txt.push("		return s_ci;\n	}\n");

		txt.push("	virtual ClassInfo& Classinfo()\n	{\n		return class_info();\n	}\n");
		
		txt.push("private:");
		txt.push(iffs.join("\n"));
		
		if(afs.length)
		{
			txt.push("\nprivate:");
			txt.push(afs.join("\n"));
		}
		
		txt.push("};\n\n}\n");

		
		for (cls in refCls)
			if(cls !== baseClass)
				txt.push("#include \"" + cls + ".h\"");
		
		if (bRef)
			txt.push("");
		
		txt.push("namespace fibjs\n{");
		txt.push(ffs.join("\n"));
		txt.push("}\n");
		
		txt.push("#endif\n\n");
		
		fs.writeFile(basePath + ns + ".h", txt.join("\n"));
	}
	
	function _member(st) {
		var attr,
		ftype,
		fname,
		name,
		value,
		args,
		argArray = false,
		pos = 0,
		s,
		argCount = 0,
		argOpt = 0,
		ifStr = "",
		fnStr = "",
		argVars = "";

		args = [];
		
		if (st[0] == "const" || st[0] == "static" || st[0] == "readonly") {
			attr = st[0];
			pos++;
		} else
			attr = "";
		
		ftype = "";
		s = st[pos];
		if(s === ns && st[pos + 1] === "(")
		{
			attr = "static";
			fname = "_new";
			hasNew = true;
			ftype = ns;		
			pos ++;
		}else if(st[pos + 1] === "(")
		{
			fname = st[pos++];
		}else
		{
			if (typeMap[s]) {
				ftype = s;
				pos++;
			} else if (clsName[s]) {
				ftype = s;
				pos++;
			}
		
			fname = st[pos++];
		}
		
		if (ids.hasOwnProperty(fname))
			return reportErr();
		ids[fname] = true;
		
		if (st[pos] == "(") {
			if (attr == "const" || attr == "readonly")
				return reportErr();
			
			if (attr == "static")
				ifStr = "	static result_t " + fname + "(";
			else
				ifStr = "	virtual result_t " + fname + "(";
			
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
					ifStr += "const v8::Arguments& args";
					
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
				
				if(clsName[type])
				{
					if (value == "")
						argVars += "		ARG_CLASS(" + type + "_base, " + argCount + ");\n";
					else
						return reportErr();
				}else
				{
					if(type === "String")
					{
						if (value == "")
							argVars += "		ARG_String(" + argCount + ");\n";
						else
							argVars += "		OPT_ARG_String(" + argCount + ", " + arg_value(type, value) + ");\n";
					}else
					{
						if (value == "")
							argVars += "		ARG(" + arg_type(type) + ", " + argCount + ");\n";
						else
							argVars += "		OPT_ARG(" + arg_type(type) + ", " + argCount + ", " + arg_value(type, value) + ");\n";
					}
				}
				
				argCount++;
				if (value == "")
					argOpt = argCount;
			}
			
			pos++;
			
			iffs.push("	static v8::Handle<v8::Value> s_" + fname + "(const v8::Arguments& args);");
			fnStr = "	inline v8::Handle<v8::Value> " + ns + "_base::s_" + fname + "(const v8::Arguments& args)\n	{\n";
			if (attr == "")
				fnStr += "		METHOD_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n		METHOD_INSTANCE(" + ns + "_base);\n";
			else if(fname !== "_new")
				fnStr += "		METHOD_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n";
			else
				fnStr += "		CONSTRUCT_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n";

			
			fnStr += "\n";
			
			if (argCount)
				fnStr += argVars + "\n";
			
			if (ftype != "") {
				if (argCount || argArray)
					ifStr += ", ";
				
				ifStr += map_type(ftype) + "& retVal";
				fnStr += "		" + map_type(ftype) + " vr;\n";
			}

			if (st[pos] == ":" && st[pos+1] == "async")
			{
				pos += 2;

				if (attr == "static") {
					ifStr += ");";
					fnStr += "		hr = ac_" + fname + "(s_acPool, ";
					afs.push('	ASYNC_STATIC' + (ftype == "" ? argCount : argCount + 1) + '('+ns+'_base, '+fname+');');
				} else {
					ifStr += ") = 0;";
					fnStr += "		hr = pInst->ac_" + fname + "(s_acPool, ";
					afs.push('	ASYNC_MEMBER' + (ftype == "" ? argCount : argCount + 1) + '('+ns+'_base, '+fname+');');
				}
			}else
			{
				if (attr == "static") {
					ifStr += ");";
					fnStr += "		hr = " + fname + "(";
				} else {
					ifStr += ") = 0;";
					fnStr += "		hr = pInst->" + fname + "(";
				}
			}
			
			for (var i = 0; i < argCount; i++) {
				if (i > 0)
					fnStr += ", ";
				fnStr += "v" + i;
			}
			
			if (argArray)
				if(argCount > 0)
					fnStr += ", args";
				else fnStr += "args";
			
			if (ftype != "") {
				if (argCount || argArray)
					fnStr += ", ";
				
				fnStr += "vr";
			}
			
			fnStr += ");\n\n";
			
			if (ftype == "")
				fnStr += "		METHOD_VOID();\n	}\n";
			else if(fname !== "_new")
				fnStr += "		METHOD_RETURN();\n	}\n";
			else fnStr += "		CONSTRUCT_RETURN();\n	}\n";
			
			ffs.push(fnStr);
			ifs.push(ifStr);

			if (attr == "static")
			{
				if(fname !== "_new")
					difms.push("			{\"" + fname + "\", s_" + fname + "}");
			}else
				difms.push("			{\"" + fname + "\", s_" + fname + "}");
		} else if(ftype != ""){
			if (attr == "const") {
				if (st[pos] != "=")
					return reportErr();
				
				pos++;
				value = st[pos++];
				
				iffs.push("	static v8::Handle<v8::Value> s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info);");
				fnStr = "	inline v8::Handle<v8::Value> " + ns + "_base::s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n\n";
				
				fnStr += "		" + map_type(ftype) + " vr = " + fname;
				fnStr += ";\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)
				
				ifStr = "	static const " + arg_type(ftype) + " " + fname + " = " + arg_value(ftype, value) + ";";
				
				svs.push(ifStr);
				
				difps.push("			{\"" + fname + "\", s_get_" + fname + "}");
			} else if (attr == "static") {
				if (st[pos] != ";")
					return reportErr();
				
				iffs.push("	static v8::Handle<v8::Value> s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info);");
				fnStr = "	inline v8::Handle<v8::Value> " + ns + "_base::s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n\n";
				fnStr += "		" + map_type(ftype) + " vr;\n";
				
				fnStr += "		hr = get_" + fname + "(";
				fnStr += "vr";
				fnStr += ");\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)
				
				ifStr = "	static result_t get_" + fname + "(" + map_type(ftype) + "& retVal);";
				ifs.push(ifStr);
				
				difps.push("			{\"" + fname + "\", s_get_" + fname + "}");
				
			} else if(fname === "operator")
			{
				if ((st[pos] !== "[") || (st[pos + 1] !== "]") || (st[pos + 2] !== ";"))
					return reportErr();
			
				pos += 2;
				hasIndexed = true;
			
				iffs.push("	static v8::Handle<v8::Value> i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info);");
				fnStr = "	inline v8::Handle<v8::Value> " + ns + "_base::i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info)\n	{\n		PROPERTY_ENTER();\n";
				fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
				fnStr += "		" + map_type(ftype) + " vr;\n";
				
				fnStr += "		hr = pInst->_indexed_getter(index, vr);\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)

				ifStr = "	virtual result_t _indexed_getter(uint32_t index, " + map_type(ftype) + "& retVal) = 0;";
				ifs.push(ifStr);

				if (attr != "readonly") {
					iffs.push("	static v8::Handle<v8::Value> i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);");
					fnStr = "	inline v8::Handle<v8::Value> " + ns + "_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)\n	{\n		PROPERTY_ENTER();\n";
					fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
					if(ftype === "String")
						fnStr += "		PROPERTY_VAL_String();\n";
					else fnStr += "		PROPERTY_VAL(" + arg_type(ftype) + ");\n";
					fnStr += "		hr = pInst->_indexed_setter(index, v0);\n\n		METHOD_VOID();\n	}\n";
					ffs.push(fnStr);
					
					ifStr = "	virtual result_t _indexed_setter(uint32_t index, " + arg_type(ftype) + " newVal) = 0;";
					ifs.push(ifStr);
				}else
				{
					fnStr = "	static v8::Handle<v8::Value> i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)\n	{\n";
					fnStr += "		return ThrowException(v8::String::NewSymbol(\"Indexed Property is read-only.\"));\n	}\n";
					ffs.push(fnStr);
				}
		
			}else
			{
				if (st[pos] != ";")
					return reportErr();
				
				iffs.push("	static v8::Handle<v8::Value> s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info);");
				fnStr = "	inline v8::Handle<v8::Value> " + ns + "_base::s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n";
				fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
				fnStr += "		" + map_type(ftype) + " vr;\n";
				
				fnStr += "		hr = pInst->get_" + fname + "(";
				fnStr += "vr";
				fnStr += ");\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)
				
				ifStr = "	virtual result_t get_" + fname + "(" + map_type(ftype) + "& retVal) = 0;";
				ifs.push(ifStr);
				
				if (attr != "readonly") {
					iffs.push("	static void s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);");
					fnStr = "	inline void " + ns + "_base::s_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n";
					fnStr += "		PROPERTY_INSTANCE(" + ns + "_base);\n\n";
					if(ftype === "String")
						fnStr += "		PROPERTY_VAL_String();\n";
					else fnStr += "		PROPERTY_VAL(" + arg_type(ftype) + ");\n";
					fnStr += "		hr = pInst->set_" + fname + "(v0);\n\n		PROPERTY_SET_LEAVE();\n	}\n";
					ffs.push(fnStr)
					
					ifStr = "	virtual result_t set_" + fname + "(" + arg_type(ftype) + " newVal) = 0;";
					ifs.push(ifStr);
				}
				
				if (attr == "readonly")
					difps.push("			{\"" + fname + "\", s_get_" + fname + "}");
				else
					difps.push("			{\"" + fname + "\", s_get_" + fname + ", s_set_" + fname + "}");
			}
		}else
			return reportErr();
		
		if (st[pos] == ";")
			pos++;
		else
			return reportErr();
		
		if (pos < st.length)
			return reportErr();
		
		return true;
	}
	
	function arg_value(t, v) {
		return v;
	}
	
	function arg_type(n) {
		if (clsName[n]) {
			if (n != ns)
				refCls[n] = true;
			return "obj_ptr<" + n + "_base>";
		}
		
		return aTypeMap[n];
	}
	
	function map_type(n) {
		if (clsName[n]) {
			if (n != ns)
				refCls[n] = true;
			return "obj_ptr<" + n + "_base>";
		}
		
		return typeMap[n];
	}
	
	function _checkID(s) {
		r = s.match(/[a-zA-Z]\w*/);
		if (!r)
			return reportErr();
		if (r[0] != s)
			return reportErr();
		
		if (s == "namespace" || s == "class" || s == "const" || s == "static" || s == "readonly" || s == "unsigned" ||
			s == "int" || s == "long" || s == "float" || s == "boolean" || s == "date" ||
			s == "string" || s == "array" || s == "")
			return reportErr();
		
		return true;
	}
	
	function getStock() {
		var pos = 0,
		n,
		n1;
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
						
						for (i = 0; i < st2.length; i++) {
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
		throw new Error("Line " + line + ": Syntax Error.");
	}
	
}
