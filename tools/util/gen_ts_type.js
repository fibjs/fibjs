var fs = require("fs");
var util = require("util");
var path = require('path');
var ejs = require('ejs');

module.exports = function (defs, baseFolder) {
    const defNames = Object.keys(defs)
    const defModules = {}
    const defObjetMirror = {}
    const defObjects = {}
    defNames.forEach((defName) => {
        const def = defs[defName]
        switch (def.declare.type) {
            case 'interface':
                defObjetMirror[defName] = defName
                defObjects[defName] = def
                break
            case 'module':
                defModules[defName] = def
                break
        }
    })
    const defModuleNames = Object.keys(defModules)
    const defObjectNames = Object.keys(defObjects)
    // white list of Object would be declared in global index.d.ts
    const topLevelVariablesInGlobalModule = [
        '__dirname',
        '__filename',
        'require',
        'Master',
        'setHrInterval',
        'clearHrInterval',
        'GC',
        'repl',
    ]

    function gen_ts_type_code(cls, def) {
        var typeMap = {
            "Integer": "number",
            "Long": "number",
            "Number": "number",
            "Boolean": "boolean",
            "String": "string",
            "Date": "Date",
            "Object": "Object",
            "Iterator": "Object",
            "Array": "any[]",
            "TypedArray": "TypedArray",
            "ArrayBuffer": "ArrayBuffer",
            "ArrayBufferView": "ArrayBufferView",
            "Function": "Function",
            "Value": "any",
            "Variant": "any",
            "NObject": "any",
            "NArray": "any[]",
            ...defObjetMirror,
            "...": "any[]"
        };

        function isRestArgs(argType) {
            return argType === '...'
        }

        function transObjectName(className) {
            return className === 'object' ? '_object' : className
        }

        var txts = [];
        var refers = [];
        var hasNew = false;
        var staticCallAsFunc = false;
        var callAsFunc = false;
        var fnIndexed = null;
        var fnNamed = null;

        function get_type(t) {
            return typeMap[t];
        }

        function get_rtype(t) {
            if (Array.isArray(t))
                return `Array<${t.name}>`;
            return typeMap[t] || t;
        }

        function _uglifyClassName(name) {
            return `Class_${name}`
        }

        function uglifyInternalClassName(internalClassName) {
            return _uglifyClassName(internalClassName)
        }

        function uglifyTypeInDefObjects(typeName) {
            if (!defObjects[typeName]) {
                return typeName
            }

            return _uglifyClassName(typeName)
        }

        function params2paramList(params, typeMap = typeMap) {
            return (params || []).map(param => {
                var hasDefault = param.default;
                if (isRestArgs(param.type)) {
                    param.name = `...${param.name}`
                }
                var mappedType = typeMap[param.type] || 'any';
                mappedType = uglifyTypeInDefObjects(mappedType, defObjects)
                return `${param.name}${hasDefault ? '?' : ''}: ${mappedType}${hasDefault ? `/** = ${param.default.value}*/` : ''}`
            })
        }

        function build_refer(def) {
            var types = {};

            function add_type(type) {
                if (type && (type !== cls) && (type !== 'object') &&
                    (type !== def.declare.extend) &&
                    (!typeMap[type]))
                    types[type] = true;
            }

            def.members.forEach(m => {
                if (Array.isArray(m.type))
                    m.type.forEach(rt => add_type(rt.type));
                else
                    add_type(m.type);

                if (!m.overs)
                    return;
                m.overs.forEach(ov => {
                    if (!ov.params)
                        return;
                    ov.params.forEach(p => {
                        add_type(p.type);
                        if (p.default && util.isArray(p.default.const) && p.default.const.length > 1)
                            add_type(p.default.const[0]);
                    });
                });
            });

            refers = Object.keys(types);
            return refers;
        }

        var filename = `${cls}.d.ts`;
        var fname = path.join(baseFolder, filename);

        var typeDTs = ejs.compile(fs.readTextFile(path.join(__dirname, './tmpl/type.d.ts.txt')));
        function getTypeDTsParams(def) {
            return {
                def: def,
                defNames: defNames,
                member_fns: util.flatten(def.members.filter(x => x.memType === 'method')
                    .map(x => util.isArray(x.overs) && x.overs.length ? x.overs : x)
                    , true
                ),
                member_objects: def.members.filter(x => x.memType === 'object'),
                member_constants: def.members.filter(x => x.memType === 'const'),
                member_props: def.members.filter(x => x.memType === 'prop'),
                typeMap,
                defModules,
                defObjects,
                defModuleNames,
                defObjectNames,
                topLevelVariablesInGlobalModule,
                refers: build_refer(def),
                _fns: {
                    get_type,
                    get_rtype,
                    params2paramList,
                    uglifyInternalClassName,
                    uglifyTypeInDefObjects,
                    isRestArgs,
                    transObjectName
                },
                filename: def.filename || ''
            }
        }

        var txt = '';
        switch (cls) {
            case 'index':
                txt = typeDTs(getTypeDTsParams({
                    "declare": {
                        "comments": "! @brief Global objects and functions",
                        "type": "index",
                        "name": "index",
                        "index": true,
                        "doc": {
                            "descript": "Global objects and functions",
                            "detail": [],
                            "params": []
                        }
                    },
                    members: [],
                    filename: 'index'
                }));
                break;
            case '_test_env':
                txt = typeDTs(getTypeDTsParams({
                    "declare": {
                        "comments": "! @brief Global objects and functions",
                        "type": "_test_env",
                        "name": "_test_env",
                        "doc": {
                            "descript": "Global objects and functions",
                            "detail": [],
                            "params": []
                        }
                    },
                    members: defs['test'].members,
                    filename: '_test_env'
                }));
                break;
            default:
                txt = typeDTs(getTypeDTsParams(def));
                break;
        }

        if (!fs.exists(fname) || fs.readTextFile(fname) !== txt) {
            console.log(filename);
            fs.writeTextFile(fname, txt);
        }
    }

    for (var cls in defs) {
        gen_ts_type_code(cls, defs[cls]);
    }
    gen_ts_type_code('index');
    gen_ts_type_code('_test_env');
}

const indexMembers = [{
    "memType": "method",
    "comments": "! @brief Global require",
    "deprecated": null,
    "async": "async",
    "name": "require",
    "type": null,
    "params": [{
        "type": "string",
        "name": "path",
        "default": null
    }]
}]
