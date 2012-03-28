String.prototype.trim = function () {
	return this.replace(/(^\s*)|(\s*$)/g, "");
}

parserIDL("if.idl");

function parserIDL(fname) {
	var st,
	f,
	line,
	ifs,
	sifs,
	svs,
	ffs,
	difs,
	dsifs,
	dsvs,
	refCls,
	ids,
	sids,
	ns,
	basePath = "",
	clsName,
	isRem = false,
	typeMap = {
		"integer" : "int",
		"number" : "double",
		"boolean" : "bool",
		"string" : "std::string",
		"object" : "v8::Local<v8::Object>",
		"array" : "v8::Local<v8::Array>"
	},
	aTypeMap = {
		"integer" : "int",
		"number" : "double",
		"boolean" : "bool",
		"string" : "const char*",
		"object" : "v8::Local<v8::Object>",
		"array" : "v8::Local<v8::Array>"
	};
	
	f = ReadFile(basePath + fname).replace(/\r/g, "").split("\n");
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
				
				if (st.length == 2) {
					if (!_class(st[1]))
						return false;
				} else if (st.length == 3 && st[2] == ";")
					clsName[st[1]] = true;
			} else
				return reportErr();
		}
	}
	
	function _class(name) {
		var st;
		
		ns = name;
		clsName[name] = true;
		ifs = [];
		sifs = [];
		svs = [];
		ffs = [];
		
		difs = [];
		dsifs = [];
		
		refCls = {};
		ids = {};
		sids = {};
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
				} {
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
			 + "#ifndef _" + ns + "_H_\n"
			 + "#define _" + ns + "_H_\n\n"
			 + "/**\n"
			 + " @author Leo Hoo <lion@9465.net>\n"
			 + " */\n\n"
			 + "#include \"if.h\"\n\n"
			 + "namespace fibjs\n{\n");
		
		for (cls in refCls) {
			txt.push("class " + cls + "_base;");
			bRef = true;
		}
		
		if (bRef)
			txt.push("");
		
		txt.push("class " + ns + "_base : public object_base\n{");
		
		if (svs.length) {
			txt.push("public:");
			txt.push(svs.join("\n") + "\n");
		}
		
		if (ifs.length) {
			txt.push("public:\n	// " + ns + "_base");
			txt.push(ifs.join("\n") + "\n");
		}
		
		if (sifs.length) {
			txt.push("public:");
			txt.push(sifs.join("\n") + "\n");
		}
		
		txt.push("public:\n	static ClassInfo& info()\n	{")
		
		if (dsifs.length) {
			txt.push("		static ClassMethod s_smethod[] = \n		{");
			txt.push(dsifs.join(",\n"));
			txt.push("		};\n");
		}
		
		if (difs.length) {
			txt.push("		static ClassMethod s_method[] = \n		{");
			txt.push(difs.join(",\n"));
			txt.push("		};\n");
		}
		
		var strClass = "		static ClassInfo s_ci(\"" + ns + "\"";
		
		if (difs.length)
			strClass += ", " + difs.length + ", s_method";
		else
			strClass += ", 0, NULL";
		
		if (dsifs.length)
			strClass += ", " + dsifs.length + ", s_smethod";
		else
			strClass += ", 0, NULL";
		
		txt.push(strClass + ");\n");
		txt.push("		return s_ci;\n	}\n");
		
		txt.push("private:")
		txt.push(ffs.join("\n"))
		
		txt.push("};\n\n}\n");
		
		for (cls in refCls)
			txt.push("#include \"x" + cls + ".h\"");
		
		if (bRef)
			txt.push("");
		
		txt.push("#endif\n\n");
		
		WriteFile(basePath + ns + ".h", txt.join("\n"));
	}
	
	function _member(st) {
		var attr,
		ftype,
		fname,
		name,
		value,
		args,
		argArray = false;
		var pos = 0,
		s,
		argCount = 0,
		argOpt = 0;
		var ifStr = "";
		var fnStr = "";
		var argVars = "";
		
		args = [];
		
		if (st[0] == "const" || st[0] == "static" || st[0] == "readonly") {
			attr = st[0];
			pos++;
		} else
			attr = "";
		
		ftype = "";
		s = st[pos];
		if (typeMap[s]) {
			ftype = s;
			pos++;
		} else if (clsName[s]) {
			ftype = s;
			pos++;
		}
		
		fname = st[pos++];
		if (!_checkID(fname))
			return reportErr();
		
		if (attr == "const" || attr == "static") {
			if (sids[fname])
				return reportErr();
			sids[fname] = true;
		} else {
			if (ids[fname])
				return reportErr();
			ids[fname] = true;
		}
		
		if (st[pos] == "(") {
			if (attr == "const" || attr == "readonly")
				return reportErr();
			
			if (attr == "static")
				ifStr = "	static result_t " + fname + "(";
			else
				ifStr = "	STDMETHOD(" + fname + ")(";
			
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
				
				if (value == "")
					argVars += "		ARG_" + type + "(" + argCount + ");\n";
				else {
					argVars += "		OPT_ARG_" + type + "(" + argCount + ", " + arg_value(type, value) + ");\n";
					ifStr += " = " + arg_value(type, value);
				}
				
				argCount++;
				if (value == "")
					argOpt = argCount;
			}
			
			if (attr == "")
				fnStr = "	static v8::Handle<v8::Value> m_" + fname + "(const v8::Arguments& args)\n	{\n		METHOD_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n		CHECK_INSTANCE(" + ns + "_base, 0);\n";
			else
				fnStr = "	static v8::Handle<v8::Value> s_" + fname + "(const v8::Arguments& args)\n	{\n		METHOD_ENTER(" + (argArray ? -1 : argCount) + ", " + argOpt + ");\n";
			
			fnStr += "\n";
			
			if (argCount)
				fnStr += argVars + "\n";
			
			if (ftype != "") {
				if (argCount)
					ifStr += ", ";
				
				ifStr += map_type(ftype) + "& retVal";
				fnStr += "		" + map_type(ftype) + " vr;\n";
			}
			
			if (attr == "static") {
				ifStr += ");";
				fnStr += "		hr = " + fname + "(";
			} else {
				ifStr += ") PURE;";
				fnStr += "		hr = pInst->" + fname + "(";
			}
			
			for (var i = 0; i < argCount; i++) {
				if (i > 0)
					fnStr += ", ";
				fnStr += "v" + i;
			}
			
			if (argArray)
				fnStr += ", args";
			
			if (ftype != "") {
				if (argCount)
					fnStr += ", ";
				
				fnStr += "vr";
			}
			
			fnStr += ");\n\n";
			
			if (ftype == "")
				fnStr += "		METHOD_VOID();\n	}\n";
			else
				fnStr += "		METHOD_RETURN();\n	}\n";
			
			ffs.push(fnStr);
			
			if (attr == "static") {
				sifs.push(ifStr);
				dsifs.push("			{\"" + fname + "\", CLASS_METHOD, {s_" + fname + "}}");
			} else {
				ifs.push(ifStr);
				difs.push("			{\"" + fname + "\", CLASS_METHOD, {m_" + fname + "}}");
			}
			
			pos++;
		} else if(ftype != ""){
			if (attr == "const") {
				if (st[pos] != "=")
					return reportErr();
				
				pos++;
				value = st[pos++];
				
				fnStr = "	static v8::Handle<v8::Value> s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n\n";
				
				fnStr += "		" + map_type(ftype) + " vr = " + fname;
				fnStr += ";\n\n		PROPERTY_RETURN();\n	}\n";
				ffs.push(fnStr)
				
				ifStr = "	static const " + arg_type(ftype) + " " + fname + " = " + arg_value(ftype, value) + ";";
				
				svs.push(ifStr);
				
				dsifs.push("			{\"" + fname + "\", CLASS_PROPERTY, {s_get_" + fname + "}}");
			} else if (attr == "static") {
				if (st[pos] != ";")
					return reportErr();
				
				fnStr = "	static v8::Handle<v8::Value> s_get_" + fname + "(v8::Local<v8::String> property, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n\n";
				fnStr += "		" + map_type(ftype) + " vr;\n";
				
				fnStr += "		hr = get_" + fname + "(";
				fnStr += "vr";
				fnStr += ");\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)
				
				ifStr = "	static result_t get_" + fname + "(" + map_type(ftype) + "& retVal);";
				sifs.push(ifStr);
				
				dsifs.push("			{\"" + fname + "\", CLASS_PROPERTY, {s_get_" + fname + "}}");
				
			} else {
				if (st[pos] != ";")
					return reportErr();
				
				fnStr = "	static v8::Handle<v8::Value> m_get_" + fname + "(v8::Local<v8:String> property, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n";
				fnStr += "		CHECK_INSTANCE(" + ns + "_base);\n\n";
				fnStr += "		" + map_type(ftype) + " vr;\n";
				
				fnStr += "		hr = pInst->get_" + fname + "(";
				fnStr += "vr";
				fnStr += ");\n\n		METHOD_RETURN();\n	}\n";
				ffs.push(fnStr)
				
				ifStr = "	STDMETHOD(get_" + fname + ")(" + map_type(ftype) + "& retVal) PURE;";
				ifs.push(ifStr);
				
				if (attr != "readonly") {
					fnStr = "	static void m_set_" + fname + "(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)\n	{\n		PROPERTY_ENTER();\n";
					fnStr += "		CHECK_INSTANCE(" + ns + "_base);\n\n";
					fnStr += "		PROPERTY_VALUE(" + arg_type(ftype) + ");\n"
					fnStr += "		hr = pInst->set_" + fname + "(v0);\n\n		METHOD_VOID();\n	}\n";
					ffs.push(fnStr)
					
					ifStr = "	STDMETHOD(set_" + fname + ")(" + map_type(ftype) + " newVal) PURE;";
					ifs.push(ifStr);
				}
				
				if (attr == "readonly")
					difs.push("			{\"" + fname + "\", CLASS_PROPERTY, {m_get_" + fname + "}}");
				else
					difs.push("			{\"" + fname + "\", CLASS_PROPERTY, {m_get_" + fname + ", m_set_" + fname + "}}");
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
			return n + "_base*";
		}
		
		return aTypeMap[n];
	}
	
	function map_type(n) {
		if (clsName[n]) {
			if (n != ns)
				refCls[n] = true;
			return n + "_base*";
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
			s == "string" || s == "object" || s == "array" || s == "blob" || s == "")
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
					s = s.replace(/,/g, " , ");
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
		print("Line " + line + ": Syntax Error.")
	}
	
}
