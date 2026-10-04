/**
 * @author richardo2016@gmail.com
 * @email richardo2016@gmail.com
 * @create date 2021-05-03 22:47:54
 * @modify date 2021-05-03 22:47:54
 *
 * @desc generate types from fibjs's idl
 */

const dom = require('dts-dom')
const { isUnion, splitUnion } = require('./type-utils');

// dts-dom defaults to CRLF output (config.outputEol === '\r\n'), while the
// repository pins LF through .gitattributes (`* text eol=lf`). Git then stores
// LF blobs but leaves CRLF in the working tree, so every generated file showed
// up as modified in `git status` without any content change. Emit LF so the
// generated files match what is committed.
dom.config.outputEol = '\n';

const fs = require('fs');
const path = require('path');

// function isIDLRestToken(paramName) {
//     return paramName === '...';
// }

// `operator[]` members cannot be emitted by dts-dom (its class printer drops
// index-signature members), so they travel as sentinel properties and are
// rewritten into the real signature here.
const INDEX_SIGNATURE_PLACEHOLDER = '__fibjs_index_signature_';

// The call operator: the IDL member `operator(...)` (idl-def.pegjs). A module
// carrying one is callable as the module object itself (`test(...)`,
// `assert(...)`); a class carrying one is callable as its instances
// (`util.debuglog(section)(msg)` - the runtime instance really is a function
// object). The shapes below follow @types/node's `node:test`: a function named
// after the module merged with a namespace and published with `export =`, and
// a call signature merged into the class through a sibling interface (a class
// declaration cannot carry one, TS1068).
const CALL_OPERATOR_NAME = 'operator';

function isCallOperatorMember(mem) {
    return mem.memType === 'method' && mem.name === CALL_OPERATOR_NAME;
}

// Modules whose runtime name differs from the IDL unit name: SandBox::
// installGlobal() publishes the assert_strict API as `assert/strict` (plus the
// fibjs:/node: variants). The declaration and every reference must use the
// runtime name - otherwise `import strict from 'node:assert/strict'` types
// nothing while `assert_strict` types a module the runtime refuses. The file
// keeps the unit name (the bridge and the triple-slash references point at
// it).
const MODULE_RUNTIME_NAMES = {
    assert_strict: 'assert/strict',
};

function runtimeModuleName(unitName) {
    return MODULE_RUNTIME_NAMES[unitName] || unitName;
}

// dts-dom's `reservedWords` list mixes in Java keywords, so a function named
// `throws` (assert.throws / assert_strict.throws) is emitted commented out and
// silently disappears from the corpus. Unwrap that comment when the name is a
// legal TypeScript function name (an identifier that is not an ECMAScript
// reserved word).
const ECMASCRIPT_RESERVED_WORDS = new Set([
    'break', 'case', 'catch', 'class', 'const', 'continue', 'debugger',
    'default', 'delete', 'do', 'else', 'enum', 'export', 'extends', 'false',
    'finally', 'for', 'function', 'if', 'import', 'in', 'instanceof', 'new',
    'null', 'return', 'super', 'switch', 'this', 'throw', 'true', 'try',
    'typeof', 'var', 'void', 'while', 'with',
    // strict-mode / contextual future reserved words
    'implements', 'interface', 'let', 'package', 'private', 'protected',
    'public', 'static', 'yield', 'await'
]);

function isLegalTsFunctionName(name) {
    return /^[$A-Z_][0-9A-Z_$]*$/i.test(name) && !ECMASCRIPT_RESERVED_WORDS.has(name);
}

function postProcessDtsUnitString(str) {
    return str
        .split('\n')
        // strip trailing blanks before an optional CR, otherwise CRLF lines
        // keep their trailing whitespace (the `$` anchor would sit after the `\r`)
        .map(line => line.replace(/[ \t]+(?=\r?$)/g, ''))
        .join('\n')
        .replace(
            new RegExp('^(\\s*)"?' + INDEX_SIGNATURE_PLACEHOLDER + '(number|string)__"?:\\s*(.*);$', 'gm'),
            '$1[index: $2]: $3;'
        )
        .replace(
            /^([ \t]*)\/\* Illegal function name '([^']+)' can't be used here\n([\s\S]*?)\n\1\*\/$/gm,
            (match, indent, name, body) => isLegalTsFunctionName(name) ? body : match
        )
    // .replace(new RegExp(QUOTE_START_PLACEHOLDER, 'g'), '')
    // .replace(new RegExp(QUOTE_END_PLACEHOLDER, 'g'), '')
}

/**
 * Write a generated file only when its content actually changed: running the
 * generator twice must leave the working tree (mtimes included) untouched.
 */
function writeDtsFile(file, content) {
    if (fs.existsSync(file) && fs.readFileSync(file, 'utf8') === content)
        return false;

    fs.writeFileSync(file, content);
    console.log(`      ✏️  ${path.basename(file)}`);
    return true;
}

function convertIDLCommentToJSDocComment(comment = '') {
    return comment
        .replace('! @brief', '@description')
        .replace('@brief', '@description')
}

function normalizeClazzName(interfaceName) {
    return `Class_${interfaceName}`
}

/* ─────────────────────────── promise variant ─────────────────────────────
 *
 * The runtime builds a *promise variant* of every module and class that has an
 * async member (own or inherited): `ClassInfo::_init()` keeps two function
 * templates (`m_class` / `m_pclass`), the promise one inheriting
 * `base->m_pclass`, and `AsyncCallBack::post()` re-parents the prototype of
 * every object returned through a promise-mode call to the promise template
 * (`GetAsyncPrototype()`), while property getters stay shared with the fiber
 * prototype. See plans/idl-event-types-2026-10-03.md §15 for the source-level
 * evidence.
 *
 * The corpus mirrors it with a second declaration per such class:
 *
 *   - `Class_XPromise` - the instance type produced by promise-mode calls
 *     ("the promise-primary form / `*Async`"), inheriting the base's promise
 *     variant (or the fiber base when the base has no variant of its own);
 *   - async members keep `MSync` (fiber, same implementation) and `MAsync`
 *     (promise), and drop the callback overload the promise prototype does not
 *     have (a callback there is rejected with 20001);
 *   - `Class_X.promises` - the *static* members object `ClassInfo::Attach()`
 *     builds (async statics promise-primary), an object rather than a
 *     constructor.
 *
 * Parameters accept either flavor: both are the same ClassInfo at runtime, so
 * a fiber object and a promise object are interchangeable arguments.
 */

/** IDL class names that have a promise variant (own or inherited async member). */
const PROMISE_VARIANT_CLASSES = new Set();

function promiseVariantName(interfaceName) {
    return `${normalizeClazzName(interfaceName)}Promise`;
}

/**
 * Whether the runtime builds a promise variant for this class: its own
 * members have an async one, or any ancestor does (`base_has_async()` in
 * tools/util/gen_code.js computes the same thing for `ClassData::has_async`).
 */
function classHasPromiseVariant(def, defs) {
    const seen = new Set();

    for (let cur = def; cur; ) {
        if (seen.has(cur.declare.name))
            break;
        seen.add(cur.declare.name);

        if (hasAsyncMembers(cur))
            return true;

        const baseName = cur.declare.extend;
        if (!baseName || baseName === 'object')
            return false;

        cur = defs && defs[baseName];
    }

    return false;
}

/**
 * The name of the instance type a class reference denotes in a given flavor:
 *
 *   - `'fiber'`   - `Class_X`;
 *   - `'promise'` - `Class_XPromise` (promise-mode results);
 *   - `'both'`    - `Class_X | Class_XPromise` (parameter positions, the
 *     runtime accepts either).
 */
function classInstanceTypeRef(interfaceName, flavor) {
    const fiberName = normalizeClazzName(interfaceName);

    if (!PROMISE_VARIANT_CLASSES.has(interfaceName))
        return dom.create.namedTypeReference(fiberName);

    if (flavor === 'promise')
        return dom.create.namedTypeReference(promiseVariantName(interfaceName));

    if (flavor === 'both') {
        return dom.create.union([
            dom.create.namedTypeReference(fiberName),
            dom.create.namedTypeReference(promiseVariantName(interfaceName)),
        ]);
    }

    return dom.create.namedTypeReference(fiberName);
}

function hasDeclaredCallbackParam(host) {
    return (host.params || []).some((param) => {
        if (!param || !param.type)
            return false;

        if (param.type === 'Function')
            return true;

        if (Array.isArray(param.type))
            return param.type.some((t) => t && t.type === 'Function');

        return false;
    });
}

function isSymbolMember(memberInfo) {
    return !!memberInfo.symbol;
}

const EXCLUDE_INTERNALS = ['Iterator'];
function getAddRefToTripleSlashDirectivesHost(
    tripleSlashDirectiveHost,
    {
        allInterfacesNames,
        allModuleNames,
        // the unit being emitted: a reference to its own file is at best
        // noise and TS1006 ("a file cannot have a reference to itself") at
        // worst - a union that carries the class itself (`Object|Array|Headers`,
        // `String|UrlObject`) must not reference the file it lives in
        selfName,
    }
) {
    return (sourceType, {
        refName, refType, refHostName
    }) => {
        if (EXCLUDE_INTERNALS.includes(refType) || refType === selfName) {
            return;
        }

        switch (sourceType) {
            case 'module': {
                if (allModuleNames.has(refType)) {
                    tripleSlashDirectiveHost[refType] = dom.create.tripleSlashReferencePathDirective(`../module/${refType}.d.ts`)
                    break;
                }
            }
            case 'interface': {
                if (allInterfacesNames.has(refType)) {
                    tripleSlashDirectiveHost[refType] = dom.create.tripleSlashReferencePathDirective(`../interface/${refType}.d.ts`)
                    break;
                }
            }
            default: {
                if (allModuleNames.has(refType)) {
                    tripleSlashDirectiveHost[refType] = dom.create.tripleSlashReferencePathDirective(`../module/${refType}.d.ts`)
                    break;
                } else if (allInterfacesNames.has(refType)) {
                    tripleSlashDirectiveHost[refType] = dom.create.tripleSlashReferencePathDirective(`../interface/${refType}.d.ts`)
                    break;
                }

                throw new Error(`[getAddRefToTripleSlashDirectivesHost] unsupported sourceType '${sourceType}' (with name '${refName}') on host '${refHostName}'`)
            }
        }
    }
}

function generalTypeMap(dataType, {
    allInterfacesNames,
    allModuleNames,
    useRefInstance = false,
    instanceFlavor = 'fiber',
}) {
    const info = {
        type: null,
        refType: null,
        isRestArgs: false,
    };

    // `Buffer|String`: a parameter-position union, rendered as a d.ts union;
    // each alternative is mapped recursively and its triple-slash refs travel
    // through auxRefs (see plans/idl-union-types-2026-10-02.md).
    if (isUnion(dataType)) {
        const types = [];
        const auxRefs = [];

        splitUnion(dataType).forEach((member) => {
            const memberInfo = generalTypeMap(member, { allInterfacesNames, allModuleNames, useRefInstance, instanceFlavor });
            types.push(memberInfo.type || dom.type.any);

            if (memberInfo.refType)
                auxRefs.push({ refType: memberInfo.refType, name: member });
            (memberInfo.auxRefs || []).forEach(aux => auxRefs.push(aux));
        });

        info.type = dom.create.union(types);
        if (auxRefs.length)
            info.auxRefs = auxRefs;
        return info;
    }

    // `Iterator<T>` (IDL: `Iterator<XmlNode>`) is a typing-only refinement: the
    // runtime returns the plain iterator, the d.ts carries the element type.
    const iteratorArg = typeof dataType === 'string' && /^Iterator<([A-Za-z_$][\w$]*)>$/.exec(dataType);
    if (iteratorArg) {
        const argInfo = generalTypeMap(iteratorArg[1], { allInterfacesNames, allModuleNames, useRefInstance: true });
        const typeRef = dom.create.namedTypeReference('Iterator');
        typeRef.typeArguments.push(argInfo.type || dom.type.any);
        info.type = typeRef;
        if (argInfo.refType) {
            info.auxRefs = [{ refType: argInfo.refType, name: iteratorArg[1] }];
        }
        return info;
    }

    switch (dataType) {
        case undefined:
        case null: {
            info.type = dom.type.void;
            break;
        }
        case 'Array':
        case 'NArray': {
            info.type = dom.type.array('any')
            break;
        }
        case 'Promise':
        case 'Uint8Array':
        case 'ArrayBuffer':
        case 'TypedArray':
        case 'ArrayBufferView':
            {
                info.type = dom.create.namedTypeReference(dataType);
                break;
            }
        case 'Buffer': {
            // Buffer in fibjs should map to Class_Buffer
            info.type = useRefInstance ? dom.create.namedTypeReference('Class_Buffer') : dom.create.typeof('Class_Buffer');
            info.isFibjsInterface = true;
            info.refType = 'interface';
            break;
        }
        case 'Value':
        case 'Variant': {
            info.type = dom.type.any;
            break;
        }
        case 'Integer':
        case 'Long':
        case 'Number': {
            info.type = dom.type.number;
            break;
        }
        case 'NMap':
        case 'NObject':
        case 'Object':
        case 'RegExp': {
            info.type = dom.create.namedTypeReference('FIBJS.GeneralObject')
            break;
        }
        case 'String': {
            info.type = dom.type.string;
            break;
        }
        case 'Boolean': {
            info.type = dom.type.boolean;
            break;
        }
        case 'Function': {
            const funcType = dom.create.functionType([], dom.type.any);
            funcType.parameters.push(dom.create.parameter('args', dom.create.array(dom.type.any), dom.ParameterFlags.Rest))
            info.type = funcType;
            break;
        }
        case 'Date': {
            info.type = dom.create.namedTypeReference('Date');
            break;
        }
        // fibjs's cpp stream, not js stream 
        case 'Stream': {
            info.type = dom.type.object;
            break;
        }
        // it's internal module, but process it standalone
        case 'Iterator': {
            const typeRef = dom.create.namedTypeReference('Iterator');
            typeRef.typeArguments.push(dom.create.namedTypeReference('any'))

            info.type = typeRef;
            break;
        }
        case '...': {
            info.type = dom.type.array(dom.type.any)
            info.isRestArgs = true;
            break;
        }
    }

    if (!EXCLUDE_INTERNALS.includes(dataType)) {
        if (allInterfacesNames.has(dataType)) {
            // instance references follow the requested flavor (see
            // classInstanceTypeRef); `typeof Class_X` stays the fiber
            // constructor - the promise variant is a prototype, not a value,
            // and module class constants are shared between both flavors
            info.type = useRefInstance ? classInstanceTypeRef(dataType, instanceFlavor) : dom.create.typeof(normalizeClazzName(dataType));
            info.isFibjsInterface = true;
            info.refType = 'interface';
        } else if (allModuleNames.has(dataType)) {
            const moduleName = runtimeModuleName(dataType);
            info.type = dom.create.typeof(
                dom.create.namedTypeReference(`import ('${moduleName}')`)
            );
            info.isFibjsModule = true;
            info.refType = 'module';
        }
    }

    return info;
}

/**
 * 
 * @param {import('./ir').IIDLDefinition['members'][number]['params'][number]['type']} paramType 
 */
/**
 * Struct types become object literal types with named members: the runtime side
 * (NType::to_value) produces a plain object with named properties, so a
 * labeled tuple (read-only by index) would mismatch. Shared by struct returns
 * and struct properties.
 */
function buildStructDtsType(items, {
    memberInfo,
    memberHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
}) {
    const propertyMembers = items.map(item => {
        const gMap = generalTypeMap(item.type, { allInterfacesNames, allModuleNames, useRefInstance: true });

        if (gMap && gMap.type) {
            if (gMap.refType && dtsUnitName !== item.type) {
                addRefToTripleSlashDirectivesHost(gMap.refType, {
                    refHostName: memberHostName,
                    refName: memberInfo.name,
                    refType: item.type,
                })
            }
            return dom.create.property(item.name, gMap.type);
        }
        return dom.create.property(item.name, dom.type.any);
    });

    return dom.create.objectType(propertyMembers);
}

/**
 * Map the inline callback shape of a `Function(...)` parameter / return to a
 * d.ts function type. The runtime still sees one `Function` value; only the
 * typings split it into parameters and a return type. Inner parameters go
 * through mapParamTypeToDtsType() again, so nested shapes, native refs and the
 * triple-slash bookkeeping work at any depth.
 */
function mapCallbackShapeToDtsType(shape, {
    memberInfo,
    memberHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
    instanceFlavor = 'fiber',
}) {
    const parameters = (shape.params || []).map((paramInfo) => {
        if (paramInfo.type === '...' || !paramInfo.type) {
            const name = paramInfo.name && paramInfo.name !== '...' ? paramInfo.name : 'args';
            return dom.create.parameter(name, dom.create.array(dom.type.any), dom.ParameterFlags.Rest);
        }

        const paramDomInfo = mapParamTypeToDtsType(paramInfo.type, {
            param: paramInfo,
            paramHostName: memberHostName,
            dtsUnitName,
            allInterfacesNames,
            allModuleNames,
            addRefToTripleSlashDirectivesHost,
            // values the runtime hands back to the shape follow the flavor of
            // the call that produced them
            instanceFlavor,
        });

        let paramFlag = dom.ParameterFlags.None;
        if (paramDomInfo.isRestArgs)
            paramFlag |= dom.ParameterFlags.Rest;

        return dom.create.parameter(paramInfo.name, paramDomInfo.type, paramFlag);
    });

    // A callback that consumes no return value is `() => void`; the mapper
    // reuses the member-return path (structs, native refs) with a throw-away
    // memberInfo, so the enclosing member's own `isarray` never leaks in.
    // `void` spells the same thing explicitly and must not reach the IDL type
    // mapper (which knows no `void` type).
    const returnType = shape.ret == null || shape.ret === 'void'
        ? dom.type.void
        : mapMemMethodReturnTypeToDtsType(shape.ret, {
            memberInfo: { name: memberInfo && memberInfo.name },
            memberHostName,
            dtsUnitName,
            allInterfacesNames,
            allModuleNames,
            addRefToTripleSlashDirectivesHost,
            // the value goes back into the runtime, which takes either flavor
            instanceFlavor: 'both',
        });

    return dom.create.functionType(parameters, returnType);
}

function mapMemPropertyTypeToDtsType(propertyType, {
    memberInfo,
    memberHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
    useRefInstance = true,
    instanceFlavor = 'fiber',
}) {
    let propertyDtsType;

    if (Array.isArray(propertyType)) {
        propertyDtsType = buildStructDtsType(propertyType, {
            memberInfo, memberHostName, dtsUnitName, allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
        });
    } else {
        // property getters are shared between the two prototypes at runtime,
        // so properties keep the fiber flavor even on a promise variant class
        const result = generalTypeMap(propertyType, { allInterfacesNames, allModuleNames, useRefInstance, instanceFlavor });

        if (!result.type) {
            throw new Error(`[mapMemPropertyTypeToDtsType] unsupported propertyType '${propertyType}' of member '${memberInfo.name}' on host '${memberHostName}'`)
        }

        if (result.refType && dtsUnitName !== propertyType) {
            addRefToTripleSlashDirectivesHost(result.refType, {
                refHostName: memberHostName,
                refName: memberInfo.name,
                refType: propertyType,
            })
        }
        (result.auxRefs || []).forEach(aux => {
            addRefToTripleSlashDirectivesHost(aux.refType, {
                refHostName: memberHostName,
                refName: memberInfo.name,
                refType: aux.name,
            })
        });
        propertyDtsType = result.type;
    }

    if (memberInfo && memberInfo.isarray)
        propertyDtsType = dom.create.array(propertyDtsType);

    return propertyDtsType;
}

/**
 * 
 * @param {import('./ir').IIDLDefinition['members'][number]['params'][number]['type']} memReturnType 
 */
function mapMemMethodReturnTypeToDtsType(memReturnType, {
    memberInfo,
    memberHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
    instanceFlavor = 'fiber',
}) {
    let returnType;

    // `process.exit` never returns - it terminates the runtime. The IDL
    // declares the two overloads with the void return the C++ side sees; the
    // d.ts says `never` so that the narrowing reads the truth
    // (`function f(): never { process.exit(1); }`), see
    // plans/idl-event-types-2026-10-03.md §22.
    if (memReturnType == null && memberHostName === 'process'
        && memberInfo && memberInfo.name === 'exit')
        return dom.create.namedTypeReference('never');

    // `Function(...)` in the return position (`static Function(...) bind(...)`)
    if (memberInfo && memberInfo.callback) {
        return mapCallbackShapeToDtsType(memberInfo.callback, {
            memberInfo, memberHostName, dtsUnitName, allInterfacesNames, allModuleNames,
            addRefToTripleSlashDirectivesHost, instanceFlavor,
        });
    }

    if (Array.isArray(memReturnType)) {
        returnType = buildStructDtsType(memReturnType, {
            memberInfo, memberHostName, dtsUnitName, allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
        });
    } else {
        const result = generalTypeMap(memReturnType, { allInterfacesNames, allModuleNames, useRefInstance: true, instanceFlavor });

        if (result.type) {
            if (result.refType && dtsUnitName !== memReturnType) {
                addRefToTripleSlashDirectivesHost(result.refType, {
                    refHostName: memberHostName,
                    refName: memberInfo.name,
                    refType: memReturnType,
                })
            }
            (result.auxRefs || []).forEach(aux => {
                addRefToTripleSlashDirectivesHost(aux.refType, {
                    refHostName: memberHostName,
                    refName: memberInfo.name,
                    refType: aux.name,
                })
            });
            returnType = result.type
        } else {
            switch (memReturnType) {
                default: {
                    throw new Error(`[mapMemMethodReturnTypeToDtsType] unsupported memReturnType '${memReturnType}' of member function '${memberInfo.name}' on host '${memberHostName}'`)
                }
            }
        }
    }

    if (memberInfo.isarray)
        returnType = dom.create.array(returnType);

    return returnType;
}

/**
 * 
 * @param {import('./ir').IIDLDefinition['members'][number]['type']} paramType 
 */
function mapParamTypeToDtsType(paramType, {
    param: paramInfo,
    paramHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
    // parameter positions accept either flavor (same ClassInfo at runtime);
    // shapes handed *to* the caller keep the enclosing context's flavor
    instanceFlavor = 'both',
}) {
    // `Function(...)` parameter: the shape carries the typings, the runtime
    // still handles the argument as one `Function`
    if (paramInfo && paramInfo.callback) {
        let callbackType = mapCallbackShapeToDtsType(paramInfo.callback, {
            memberInfo: { name: paramInfo.name },
            memberHostName: paramHostName,
            dtsUnitName,
            allInterfacesNames,
            allModuleNames,
            addRefToTripleSlashDirectivesHost,
            instanceFlavor,
        });
        if (paramInfo.isarray)
            callbackType = dom.create.array(callbackType);
        return { type: callbackType };
    }

    const result = generalTypeMap(paramType, { allInterfacesNames, allModuleNames, useRefInstance: true, instanceFlavor });

    if (result.type) {
        if (result.refType && dtsUnitName !== paramType) {
            addRefToTripleSlashDirectivesHost('methodParam', {
                refHostName: paramHostName,
                refName: paramInfo.name,
                refType: paramType,
            })
        }

        (result.auxRefs || []).forEach(aux => {
            addRefToTripleSlashDirectivesHost('methodParam', {
                refHostName: paramHostName,
                refName: paramInfo.name,
                refType: aux.name,
            })
        });

        // Handle array types if isarray is present
        if (paramInfo.isarray) {
            result.type = dom.create.array(result.type);
        }

        return result
    }

    switch (paramType) {
        default: {
            throw new Error(`[mapParamTypeToDtsType] unsupported paramType '${paramType}' of param '${paramInfo.name}' on host '${paramHostName}'`)
        }
    }
}

function isVoidDomType(type) {
    return type === dom.type.undefined || type === dom.type.void
}

/**
 * @description A parameter is optional as soon as the IDL gives it a default:
 * a literal value (`= 1`, `= ""`, `= undefined`) or a constant reference
 * (`= net.AF_INET`, `= DEFAULT_COMPRESSION`); gen_code emits `OPT_ARG` for
 * both, so the d.ts must mark both optional.
 *
 * @param {import('./ir').IIDLParam} p
 */
function hasDefaultValue(p) {
    return !!p && !!p.default && (p.default.value !== undefined || p.default.const !== undefined);
}

/**
 * 
 * @param {import('./ir').IIDLDefinition['members'][number]['overs']} methodHost 
 * @returns 
 */
function generateDtsFunction(functionHost, normalParams, returnType, {
    funcFlags,
    withOptionalParam = false,
    withRestArgs = false,
    hasDeclaredMemberName = () => false,
    mapReturnType = null,
} = {}) {
    let syncFunc;
    let asyncFunc;
    let syncVariant;
    let asyncVariant;

    const params = Array.from(normalParams);
    const declaredWithCallback = hasDeclaredCallbackParam(functionHost);
    const isExplicitSyncName = functionHost.name.endsWith('Sync');
    // `ClassInfo::bind_async_method()` binds `MSync` (the fiber implementation)
    // and `MAsync` (the promise one) *per alias*: each is skipped only when the
    // module/class already declares a method of that name. The primary of an
    // explicit `...Sync` member stays synchronous-only in the typings even
    // though the runtime would strip a trailing callback.
    const wantsSyncAlias = !hasDeclaredMemberName(functionHost.name + 'Sync');
    const wantsAsyncAlias = !hasDeclaredMemberName(functionHost.name + 'Async');
    const wantsCallbackOverload = !declaredWithCallback && !isExplicitSyncName;

    // Every emitted shape gets the return type of the flavor it carries at
    // runtime: the primary/callback/`Sync` forms run the fiber (a returned
    // object keeps the fiber prototype), the `Async` alias resolves through the
    // resolver and the promise prototype re-parents its result. See
    // plans/idl-event-types-2026-10-03.md §15.1.
    const retOf = (flavor) => (mapReturnType ? mapReturnType(flavor) : returnType);
    const promiseTypeOf = (flavor) => {
        const promiseType = dom.create.namedTypeReference('Promise');
        promiseType.typeArguments = [retOf(flavor)];
        return promiseType;
    };

    syncFunc = dom.create.function(
        functionHost.name,
        params,
        retOf('fiber'),
        funcFlags
    )

    // `MSync` runs the fiber, `MAsync` the resolver: same alias pair for every
    // async member, including the rest-args ones (child_process.sh) whose
    // callback overload the typings do not spell.
    const emitAliases = () => {
        if (wantsSyncAlias)
            syncVariant = dom.create.function(
                functionHost.name + 'Sync',
                Array.from(params),
                retOf('fiber'),
                funcFlags
            );

        if (wantsAsyncAlias)
            asyncVariant = dom.create.function(
                functionHost.name + 'Async',
                Array.from(params),
                promiseTypeOf('promise'),
                funcFlags
            );
    };

    // Handle different types of async functions
    if (withRestArgs && functionHost.async) {
        if (functionHost.async === 'promise')
            syncFunc.returnType = promiseTypeOf('promise');

        emitAliases();
    } else if (functionHost.async) {
        if (functionHost.async === 'promise') {
            // Promise-based function: return Promise<T>
            syncFunc.returnType = promiseTypeOf('promise');
            emitAliases();
        } else {
            emitAliases();

            if (!wantsCallbackOverload)
                return {
                    syncFunc,
                    asyncFunc,
                    syncVariant,
                    asyncVariant
                };

            // Callback-based async function: generate callback version
            const errParamType = dom.create.union([
                dom.create.namedTypeReference('Error'),
                dom.type.undefined,
                dom.type.null
            ]);
            const errorParam = dom.create.parameter('err', errParamType);

            const fiberReturnType = retOf('fiber');
            const callbackType = dom.create.functionType([
                errorParam,
                !isVoidDomType(fiberReturnType) && dom.create.parameter('retVal', fiberReturnType)
            ].filter(Boolean), dom.type.any);

            const callbackParams = Array.from(normalParams);
            callbackParams.push(
                dom.create.parameter(
                    'callback', callbackType,
                    withOptionalParam ? dom.ParameterFlags.Optional : dom.ParameterFlags.None
                )
            )

            asyncFunc = dom.create.function(
                functionHost.name,
                callbackParams,
                dom.type.void,
                funcFlags
            )
        }
    }

    return {
        syncFunc,
        asyncFunc,
        syncVariant,
        asyncVariant
    }
}

/**
 * 
 * @param {import('./ir').IIDLDefinition['members'][number]['overs']} methodHost 
 * @returns 
 */
function generateDtsMethod(methodHost, normalParams, returnType, {
    memFlags,
    withOptionalParam = false,
    withRestArgs = false,
    hasDeclaredMemberName = () => false,
    mapReturnType = null,
} = {}) {
    let syncMethod;
    let asyncMethod;
    let syncVariant;
    let asyncVariant;

    const params = Array.from(normalParams);
    const declaredWithCallback = hasDeclaredCallbackParam(methodHost);
    const isExplicitSyncName = methodHost.name.endsWith('Sync');
    // see generateDtsFunction(): aliases are a per-alias decision of
    // ClassInfo::bind_async_method()
    const wantsSyncAlias = !hasDeclaredMemberName(methodHost.name + 'Sync');
    const wantsAsyncAlias = !hasDeclaredMemberName(methodHost.name + 'Async');
    const wantsCallbackOverload = !declaredWithCallback && !isExplicitSyncName;

    // see generateDtsFunction(): each emitted shape carries the flavor it runs
    // in at runtime
    const retOf = (flavor) => (mapReturnType ? mapReturnType(flavor) : returnType);
    const promiseTypeOf = (flavor) => {
        const promiseType = dom.create.namedTypeReference('Promise');
        promiseType.typeArguments = [retOf(flavor)];
        return promiseType;
    };

    syncMethod = dom.create.method(
        methodHost.name,
        params,
        retOf('fiber'),
        memFlags
    )

    // see generateDtsFunction(): one alias pair per async member
    const emitAliases = () => {
        if (wantsSyncAlias)
            syncVariant = dom.create.method(
                methodHost.name + 'Sync',
                Array.from(params),
                retOf('fiber'),
                memFlags
            );

        if (wantsAsyncAlias)
            asyncVariant = dom.create.method(
                methodHost.name + 'Async',
                Array.from(params),
                promiseTypeOf('promise'),
                memFlags
            );
    };

    // Handle different types of async methods
    if (withRestArgs && methodHost.async) {
        if (methodHost.async === 'promise')
            syncMethod.returnType = promiseTypeOf('promise');

        emitAliases();
    } else if (methodHost.async) {
        if (methodHost.async === 'promise') {
            // Promise-based method: return Promise<T>
            syncMethod.returnType = promiseTypeOf('promise');
            emitAliases();
        } else {
            emitAliases();

            if (!wantsCallbackOverload)
                return {
                    syncMethod,
                    asyncMethod,
                    syncVariant,
                    asyncVariant
                };

            // Callback-based async method: generate callback version
            const errorParam = dom.create.parameter('err', dom.create.union([
                dom.create.namedTypeReference('Error'),
                dom.type.undefined,
                dom.type.null
            ]));

            const fiberReturnType = retOf('fiber');
            const callbackType = dom.create.functionType([
                errorParam,
                !isVoidDomType(fiberReturnType) && dom.create.parameter('retVal', fiberReturnType)
            ].filter(Boolean), dom.type.any);

            const callbackParams = Array.from(normalParams);
            callbackParams.push(
                dom.create.parameter(
                    'callback', callbackType,
                    withOptionalParam ? dom.ParameterFlags.Optional : dom.ParameterFlags.None
                )
            )

            asyncMethod = dom.create.method(
                methodHost.name,
                callbackParams,
                dom.type.void,
                memFlags
            )
        }
    }

    return {
        syncMethod,
        asyncMethod,
        syncVariant,
        asyncVariant
    }
}

/**
 * A member declared as two overloads that differ only in one parameter
 * position - a Buffer in one, a String in the other - accepts either form at
 * runtime, but TypeScript cannot pass a `Buffer | string` value to the pair:
 * no single overload accepts the union. Returns the members for which an
 * additional merged union signature should be emitted, keyed by the longer
 * declaration.
 *
 * Only exactly two overloads with exactly one differing position qualify:
 * with more than one differing position a per-position union would be wider
 * than the union of the two overloads.
 */
function mergedBufferStringOverloads(members) {
    const groups = new Map();

    for (const mem of members) {
        if (mem.memType !== 'method' || !mem.params || !mem.params.length)
            continue;
        const key = (mem.static ? 'static:' : '') + mem.name;
        const group = groups.get(key) || [];
        group.push(mem);
        groups.set(key, group);
    }

    const merged = new Map();

    for (const group of groups.values()) {
        if (group.length !== 2)
            continue;

        const [a, b] = group;
        const short = a.params.length <= b.params.length ? a : b;
        const long = short === a ? b : a;

        let pos = -1;
        let ok = true;

        for (let i = 0; ok && i < short.params.length; i++) {
            const p = short.params[i];
            const q = long.params[i];

            if (p.name !== q.name || !!p.default !== !!q.default) {
                ok = false;
                break;
            }

            if (p.type === q.type)
                continue;

            if ([p.type, q.type].sort().join('|') !== 'Buffer|String' || pos !== -1) {
                ok = false;
                break;
            }

            pos = i;
        }

        // the extra parameters of the longer form must be optional
        if (!ok || pos === -1 || !long.params.slice(short.params.length).every(p => hasDefaultValue(p)))
            continue;

        merged.set(long, { short, long, pos });
    }

    return merged;
}

/**
 * Map the declared parameters of one member (or one of its overloads) to d.ts
 * parameters. A parameter becomes optional when it declares a default value or
 * follows an optional one; a rest parameter ends the list. `typeOverride(i,
 * memParam)` may substitute the mapped type of a single position (used by the
 * merged Buffer|String signature).
 *
 * @returns {{ params: Array<*>, withRestArgs: boolean }}
 */
function buildDtsMethodParams(paramsHost, getMapParamOptions, { typeOverride } = {}) {
    let currentWithOptionalParam = false;
    let withRestArgs = false;

    const params = (paramsHost.params || []).map((memParam, i) => {
        const overrideType = typeOverride ? typeOverride(i, memParam) : null;
        const paramDomInfo = overrideType
            ? { type: overrideType }
            : mapParamTypeToDtsType(memParam.type, getMapParamOptions(memParam));

        withRestArgs = !!paramDomInfo.isRestArgs;

        let paramFlag = withRestArgs ? dom.ParameterFlags.Rest : dom.ParameterFlags.None;

        // Check if this specific parameter has a default value
        const hasDefault = hasDefaultValue(memParam);
        if (!paramDomInfo.isRestArgs && (currentWithOptionalParam || hasDefault)) {
            currentWithOptionalParam = true;
            paramFlag |= dom.ParameterFlags.Optional;
        }

        return dom.create.parameter(
            memParam.name,
            paramDomInfo.type,
            paramFlag
        );
    });

    return { params, withRestArgs };
}

/**
 * Map the listener signature of a declared event: the declared params are the
 * listener arguments, the runtime never consumes a listener return value.
 *
 * @returns {*}
 */
function mapEventListenerType(mem, {
    unitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
}) {
    return mapCallbackShapeToDtsType({ params: mem.params || [], ret: null }, {
        memberInfo: mem, memberHostName: unitName, dtsUnitName: unitName,
        allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
    });
}

/**
 * The EventEmitter methods whose leading (event, listener) pair is exactly the
 * listener of a declared event (idl/EventEmitter.idl). Every declared event
 * synthesizes one overload per name the class chain provides, and the
 * inherited declarations of these names are materialized (R1) just like
 * `on`/`once` (plans/idl-event-types-2026-10-03.md).
 */
const EVENT_LISTENER_METHOD_NAMES = [
    'on',
    'once',
    'off',
    'addListener',
    'removeListener',
    'addEventListener',
    'removeEventListener',
    'prependListener',
    'prependOnceListener',
];

/**
 * Find the declaration an event overload mirrors: the nearest ancestor
 * declaring `name` as an instance method whose second parameter is a function
 * (the listener). That declaration supplies the parameters following the
 * listener (`addEventListener(event, listener, options)`). Returns null when
 * the chain does not declare the method, in which case no overload is
 * synthesized — the class only gets the `on<name>` accessor its runtime
 * provides.
 */
function resolveListenerMethodDeclaration(def, defs, name) {
    const seen = new Set();

    for (let base = defs[def.declare.extend]; base; base = defs[base.declare.extend]) {
        if (seen.has(base))
            break;
        seen.add(base);

        const declaration = (base.members || []).find(mem =>
            mem.memType === 'method' && !mem.static && mem.name === name &&
            (mem.params || [])[1] && mem.params[1].type === 'Function');

        if (declaration)
            return declaration;
    }

    return null;
}

/**
 * Map the parameters a listener-pair method declares after the listener, e.g.
 * the `options` of `addEventListener(event, listener, options)`.
 */
function mapListenerMethodTailParams(declaration, listenerHostName, ctx) {
    const tail = (declaration.params || []).slice(2);
    if (!tail.length)
        return [];

    return buildDtsMethodParams({ params: tail }, (memParam) => ({
        param: memParam, dtsUnitName: ctx.unitName, paramHostName: listenerHostName,
        allInterfacesNames: ctx.allInterfacesNames, allModuleNames: ctx.allModuleNames,
        addRefToTripleSlashDirectivesHost: ctx.addRefToTripleSlashDirectivesHost,
    })).params;
}

/**
 * Build the typed overload one declared event contributes to a listener-pair
 * method, e.g. `on(event: "message", listener: (msg: Class_Buffer)=>void): this`
 * or `off(event: "message", listener: (msg: Class_Buffer)=>void): this`.
 */
function createEventRegistrationMember(registrationName, mem, ctx, declaration) {
    return dom.create.method(
        registrationName,
        [
            dom.create.parameter('event', dom.type.stringLiteral(mem.name)),
            dom.create.parameter('listener', mapEventListenerType(mem, ctx)),
            ...mapListenerMethodTailParams(declaration, mem.name, ctx),
        ],
        dom.create.namedTypeReference('this'),
        ctx.memFlags
    );
}

/**
 * Build the `on<name>` accessor property the runtime generates for every
 * declared event (`get_onX` / `set_onX`): reading returns the first bound
 * listener, or null when none is bound; writing takes a listener function.
 */
function createEventAccessorMember(mem, ctx) {
    const accessor = dom.create.property(
        'on' + mem.name,
        dom.create.union([mapEventListenerType(mem, ctx), dom.type.null]),
        ctx.memFlags
    );

    accessor.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');

    return accessor;
}

/**
 * Materialize the inherited overloads of one member name (R1, see
 * plans/idl-event-types-2026-10-03.md).
 *
 * TypeScript resolves a member through the nearest declaration: a class that
 * declares `on` hides every `on` overload of its bases, even though the
 * runtime keeps them callable (each class prototype only carries its own
 * members, the base prototype stays on the prototype chain). Every member the
 * d.ts layer *synthesizes* (events -> `on`/`once`, see the `event` case) must
 * therefore re-emit the inherited call surface of the same name after its
 * own, so the generated declaration keeps offering everything the runtime
 * offers.
 *
 * The surface of one ancestor is its own synthesized event overloads (for the
 * `on`/`once` names) plus its declared methods of that name; walking the chain
 * from the nearest base to the farthest therefore reproduces exactly what a
 * call on an instance of the derived class can reach. The materialized
 * members carry no jsDoc: the owning class is the documented host of the
 * signature.
 */
function materializeInheritedOverloads(def, name, {
    defs,
    dtsUnit,
    memFlags,
    declaredMethodNames,
    addRefToTripleSlashDirectivesHost,
    allInterfacesNames,
    allModuleNames,
    unitName,
    // the flavor of the class being generated: the promise variant walks the
    // promise-flavored copy of the base chain, so the inherited signatures it
    // re-emits are promise-shaped as well
    unitFlavor = 'fiber',
}) {
    const seen = new Set();

    for (let base = defs[def.declare.extend]; base; base = defs[base.declare.extend]) {
        if (seen.has(base))
            break;
        seen.add(base);

        // (a) the overloads the base synthesizes for its own events: a base
        //     that declares events exposes them through every listener-pair
        //     method, and so does every instance of this class
        if (EVENT_LISTENER_METHOD_NAMES.indexOf(name) >= 0) {
            const declaration = resolveListenerMethodDeclaration(base, defs, name);

            if (declaration)
                (base.members || []).forEach(mem => {
                    if (mem.memType !== 'event')
                        return;

                    dtsUnit.members.push(createEventRegistrationMember(name, mem, {
                        memFlags, unitName, allInterfacesNames, allModuleNames,
                        addRefToTripleSlashDirectivesHost,
                    }, declaration));
                });
        }

        // (b) the methods the base declares itself
        (base.members || []).forEach(mem => {
            if (mem.memType !== 'method' || mem.name !== name || mem.static)
                return;

            const getMapParamOptions = (memParam) => ({
                param: memParam, dtsUnitName: unitName, paramHostName: mem.name,
                allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
            });
            const getMapMemberTypeOptions = (flavor = unitFlavor) => ({
                memberInfo: mem, dtsUnitName: unitName, memberHostName: unitName,
                allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
                instanceFlavor: flavor,
            });
            const mapMemberReturnType = (flavor) =>
                mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions(flavor));

            const overloads = (mem.overs && mem.overs.length > 1) ? mem.overs : [mem];

            overloads.forEach(over => {
                const { params, withRestArgs } = buildDtsMethodParams(over, getMapParamOptions);
                const { syncMethod, asyncMethod, syncVariant, asyncVariant } = generateDtsMethod(
                    over,
                    params,
                    mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                    {
                        memFlags,
                        withRestArgs,
                        hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                        mapReturnType: mapMemberReturnType,
                    }
                );

                dtsUnit.members.push(syncMethod);

                if (asyncMethod)
                    dtsUnit.members.push(asyncMethod);

                if (syncVariant)
                    dtsUnit.members.push(syncVariant);

                if (asyncVariant)
                    dtsUnit.members.push(asyncVariant);
            });
        });
    }
}

/**
 * The runtime builds a *promise variant* of every module and class that has an
 * async member: `ClassInfo::Attach()` defines a `promises` property (the same
 * bindings with the async members' primary form returning a Promise instead of
 * running the fiber to completion), and `SandBox::installRootModules()`
 * registers a module's `promises` object as `<name>/promises` - plus the
 * `fibjs:` and `node:` aliases every module gets.
 *
 * The corpus mirrors that automation without any C++ involvement:
 *
 *   - a module with async members gets a `<name>.promises.d.ts` declaring the
 *     variant module - the *same* member set as the module itself, only with
 *     the async members' primary form promise-flavored (the `xxxSync` /
 *     `xxxAsync` aliases are part of that set, see §15.1) - and merges
 *     `const promises: typeof import("<name>/promises")` into the module;
 *   - a class with async members gets `Class_XPromise` (the instance type of
 *     every promise-mode result) and `Class_X.promises`, the static-members
 *     object `ClassInfo::Attach()` builds.
 */
function hasAsyncMembers(def) {
    return (def.members || []).some(mem =>
        mem.async || (mem.overs || []).some(ov => !!ov.async)
    );
}

/**
 * Only the runtime's *root modules* take part in the promise automation:
 * `SandBox::installRootModules()` iterates them and turns a module's `promises`
 * object into `<name>/promises` (plus the `fibjs:`/`node:` aliases). `global`
 * (the global object itself) and `subtle` (installed by crypto's module init)
 * are not root modules. The list is read from the runtime source (registered
 * with IMPORT_MODULE) instead of being duplicated here.
 */
function rootModuleNames() {
    const file = path.resolve(__dirname, '../../fibjs/src/base/modules.cpp');

    if (!fs.existsSync(file))
        return null;

    const names = new Set();
    const text = fs.readFileSync(file, 'utf8');
    text.replace(/IMPORT_MODULE\(([A-Za-z0-9_]+)\)/g, (match, name) => (names.add(name), match));

    return names;
}

const ROOT_MODULE_NAMES = rootModuleNames();

/**
 * A copy of `def` whose `async` members take the *promise flavor*: the primary
 * form returns a Promise and there is no callback overload, while `MSync` and
 * `MAsync` keep their implementations. That is exactly what the runtime binds
 * on its promise variant (see plans/idl-event-types-2026-10-03.md §15), and
 * exactly what `generateDtsFunction`/`generateDtsMethod` emit for an IDL
 * `promise` member - so the variant reuses the whole declaration pipeline
 * instead of re-implementing it.
 */
function asPromiseFlavorDef(def, { comments } = {}) {
    const clone = Object.assign({}, def, {
        declare: Object.assign({}, def.declare, comments === undefined ? {} : { comments }),
    });

    clone.members = (def.members || []).map((mem) => {
        const overs = mem.overs
            ? mem.overs.map(ov => (ov.async ? Object.assign({}, ov, { async: 'promise' }) : ov))
            : null;

        if (mem.async)
            return Object.assign({}, mem, { async: 'promise' }, overs ? { overs } : {});

        return overs ? Object.assign({}, mem, { overs }) : mem;
    });

    return clone;
}

function buildPromiseModuleDeclare(def, processOptions) {
    const unitName = def.declare.name;
    const promiseDef = asPromiseFlavorDef(def, {
        comments: `The promise variant of the ${unitName} module: async members return a Promise as their primary form.`,
    });

    const dtsUnit = dom.create.module(`${unitName}/promises`);
    const tripleSlashDirectiveMap = {};
    tripleSlashDirectiveMap['_fibjs.d.ts'] = dom.create.tripleSlashReferencePathDirective(`../_import/_fibjs.d.ts`);

    const declared = postProcessDtsUnitString(processDeclareModule(promiseDef, {
        ...processOptions,
        unitName: `${unitName}/promises`,
        unitFlavor: 'promise',
    }, { dtsUnit, tripleSlashDirectiveMap }));

    // the module object carries the variant (ClassInfo::Attach sets `promises`,
    // SandBox registers it as `<name>/promises`); merge the property in
    return declared + `\ndeclare module "${unitName}" {\n    const promises: typeof import("${unitName}/promises");\n}\n`;
}

/**
 * The promise variant of a class: the instance type every promise-mode call
 * produces (`AsyncCallBack::post()` re-parents the result to
 * `GetAsyncPrototype()`), plus `Class_X.promises` - the object
 * `ClassInfo::Attach()` fills with the *static* members' promise primaries.
 */
function buildPromiseClassDeclare(def, processOptions) {
    const unitName = def.declare.name;
    const promiseDef = asPromiseFlavorDef(def, {
        comments: `The promise variant of the ${unitName} class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).`,
    });

    const dtsUnit = dom.create.class(promiseVariantName(unitName));
    const tripleSlashDirectiveMap = {};
    tripleSlashDirectiveMap['_fibjs.d.ts'] = dom.create.tripleSlashReferencePathDirective(`../_import/_fibjs.d.ts`);

    // `m_pclass` inherits the base's promise template (or the fiber one when the
    // base has no async member of its own)
    const baseName = def.declare.extend;
    const baseTypeName = baseName && baseName !== 'object' && PROMISE_VARIANT_CLASSES.has(baseName)
        ? promiseVariantName(baseName)
        : null;

    let declared = postProcessDtsUnitString(processDeclareInterface(promiseDef, {
        ...processOptions,
        // R1's walk must see the promise-flavored base members
        defs: processOptions.promiseDefs || processOptions.defs,
        unitFlavor: 'promise',
        baseTypeName,
    }, { dtsUnit, tripleSlashDirectiveMap }));

    return declared + buildPromisesStaticsDeclare(def, processOptions);
}

/**
 * `Class_X.promises` is the object `ClassInfo::Attach()` builds next to the
 * class object: it carries the *static* members with the async ones
 * promise-primary (an object, not a constructor - `new net.Socket.promises()`
 * throws, and a class without static members gets an empty object).
 */
function buildPromisesStaticsDeclare(def, {
    unitName,
    allInterfacesNames,
    allModuleNames,
}) {
    const statics = (def.members || []).filter(mem => mem.static);
    const className = normalizeClazzName(unitName);

    if (!statics.length)
        return `\ndeclare namespace ${className} {\n    const promises: FIBJS.GeneralObject;\n}\n`;

    const tripleSlashDirectiveMap = {};
    const addRefToTripleSlashDirectivesHost = getAddRefToTripleSlashDirectivesHost(
        tripleSlashDirectiveMap, { allInterfacesNames, allModuleNames, selfName: unitName }
    );
    const properties = [];

    statics.forEach(mem => {
        const mapParamOptions = (memParam) => ({
            param: memParam, dtsUnitName: unitName, paramHostName: mem.name,
            allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
        });
        const mapMemberTypeOptions = (flavor) => ({
            memberInfo: mem, dtsUnitName: unitName, memberHostName: unitName,
            allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
            instanceFlavor: flavor,
        });

        if (mem.memType === 'method') {
            // the class object binds the statics of the *promise* prototype's
            // flavor, i.e. the async ones promise-primary
            const flavor = mem.async ? 'promise' : 'fiber';
            const overloads = (mem.overs && mem.overs.length > 1) ? mem.overs : [mem];

            overloads.forEach(over => {
                const { params } = buildDtsMethodParams(over, mapParamOptions);
                let retType = mapMemMethodReturnTypeToDtsType(mem.type, mapMemberTypeOptions(flavor));

                if (over.async) {
                    const promiseType = dom.create.namedTypeReference('Promise');
                    promiseType.typeArguments = [retType];
                    retType = promiseType;
                }

                properties.push(dom.create.property(over.name, dom.create.functionType(params, retType), dom.DeclarationFlags.ReadOnly));
            });
            return;
        }

        if (mem.memType === 'prop' || mem.memType === 'const') {
            const type = mem.memType === 'const'
                ? mapMemConstType(mem, 'interface', unitName)
                : mapMemPropertyTypeToDtsType(mem.type, mapMemberTypeOptions('fiber'));
            properties.push(dom.create.property(mem.name, type, dom.DeclarationFlags.ReadOnly));
        }
    });

    const ns = dom.create.namespace(className);
    ns.members.push(dom.create.const('promises', dom.create.objectType(properties), dom.DeclarationFlags.ReadOnly));

    return '\n' + postProcessDtsUnitString(dom.emit(ns, {
        tripleSlashDirectives: Object.values(tripleSlashDirectiveMap),
    }));
}

/**
 * The literal type of an IDL const member (`const OPEN = 1;` -> `1`).
 */
function mapMemConstType(mem, unitCategory, unitName) {
    if (mem.default && mem.default.value) {
        const value = mem.default.value;

        if (value === 'true' || value === 'false')
            return dom.create.namedTypeReference(value);

        if (value.startsWith('"') && value.endsWith('"'))
            return dom.type.stringLiteral(value.replace(/^"|"$/g, ''));

        return dom.type.numberLiteral(value);
    }

    throw new Error(`unsupported const-memType member '${mem.name}' on ${unitCategory} '${unitName}'`);
}

/**
 * @param {import('./ir').IIDLDefinition} def
 * @param {*} configuration
 * @param {{
 *  dtsUnit: dom.ClassDeclaration
 *  tripleSlashDirectiveMap: Record<string, string>
 * }} retValue
 */
function processDeclareInterface(def, {
    unitName,
    unitCategory,
    ismodule,
    defs,
    allInterfacesNames,
    allModuleNames,
    // the flavor this unit's own members return (`'promise'` for the promise
    // variant declaration, see plans/idl-event-types-2026-10-03.md §15)
    unitFlavor = 'fiber',
    // the promise variant is derived from the fiber declaration; its inherited
    // overloads are re-materialized from the promise-flavored base chain (the
    // `defs` the caller passes for a promise variant is that copy)
    // override of the fiber base type name (the promise variant extends the
    // base's promise variant); the triple-slash reference stays the base file,
    // which declares both flavors
    baseTypeName = null,
}, {
    dtsUnit,
    tripleSlashDirectiveMap,
}) {
    // member names synthesized by this generator (not declared by the IDL);
    // their inherited overloads are materialized after the member loop (R1)
    const synthesizedMemberNames = [];
    // the call signatures of the `operator(...)` member (see
    // CALL_OPERATOR_NAME): emitted through a merged interface next to the
    // class, since a class declaration cannot carry them
    const callSignatures = [];
    const declaredMethodNames = new Set(
        def.members
            .filter(mem => mem.memType === 'method')
            .map(mem => mem.name)
    );

    if (def.declare.extend) {
        const refType = def.declare.extend;
        dtsUnit.baseType = dom.create.namedTypeReference(baseTypeName || normalizeClazzName(refType))
        tripleSlashDirectiveMap[refType] = dom.create.tripleSlashReferencePathDirective(`../interface/${refType}.d.ts`)
    }

    const addRefToTripleSlashDirectivesHost = getAddRefToTripleSlashDirectivesHost(tripleSlashDirectiveMap, { allInterfacesNames, allModuleNames, selfName: unitName });

    const mergedOverloads = mergedBufferStringOverloads(def.members);

    def.members.forEach(mem => {
        if (!ismodule && isSymbolMember(mem)) {
            switch (String(mem.type).replace(/<.*>$/, '')) {
                case 'Iterator': {
                    const symbolInfo = generalTypeMap(mem.type, { allInterfacesNames, allModuleNames, useRefInstance: true });
                    (symbolInfo.auxRefs || []).forEach(aux => {
                        addRefToTripleSlashDirectivesHost(aux.refType, {
                            refHostName: unitName,
                            refName: mem.name,
                            refType: aux.name,
                        })
                    });
                    dtsUnit.members.push(
                        dom.create.method(
                            `[Symbol.${mem.name}]`,
                            [],
                            symbolInfo.type
                        )
                    )
                    return;
                }
                default: {
                    throw new Error(`unsupported symbom member '${mem.type}' (with name '${mem.name}') on ${unitCategory} '${unitName}'`)
                }
            }
        }

        if (isCallOperatorMember(mem)) {
            // the instances are callable; the signature travels through the
            // merged interface emitted next to the class (see the tail of this
            // function)
            if (mem.static)
                throw new Error(`the static call operator of '${unitName}' is only supported on modules (see processDeclareModule)`);

            const overloads = (mem.overs && mem.overs.length > 1) ? mem.overs : [mem];

            overloads.forEach(over => {
                const { params } = buildDtsMethodParams(over, getMapParamOptions);
                const signature = dom.create.callSignature(
                    params,
                    mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions())
                );

                signature.jsDocComment = convertIDLCommentToJSDocComment(over.comments || '');
                callSignatures.push(signature);
            });

            return;
        }

        const isInterfaceConstructor = mem.memType === 'method' && mem.name === def.declare.name && !mem.static;
        let memFlags = 0;
        if (mem.static) {
            memFlags |= dom.DeclarationFlags.Static
        }

        if (mem.readonly) {
            memFlags |= dom.DeclarationFlags.ReadOnly
        }

        function getMapParamOptions(memParam) {
            return {
                param: memParam, dtsUnitName: unitName,
                paramHostName: mem.name, allInterfacesNames, allModuleNames,
                addRefToTripleSlashDirectivesHost: addRefToTripleSlashDirectivesHost,
            }
        }

        function getMapMemberTypeOptions(flavor = unitFlavor) {
            return {
                memberInfo: mem, dtsUnitName: unitName,
                memberHostName: unitName, allInterfacesNames, allModuleNames,
                addRefToTripleSlashDirectivesHost: addRefToTripleSlashDirectivesHost,
                instanceFlavor: flavor,
            }
        }

        // per-variant return type of this member: the primary/callback/`Sync`
        // forms return fiber instances, the `Async` alias promise instances
        function mapMemberReturnType(flavor) {
            return mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions(flavor));
        }

        /**
         * @typedef {import('dts-dom').ClassDeclaration} IDTSUnit
         * @type {IDTSUnit['members'][number]}
         */
        let dtsUnitMember;
        let memberIsOver = false;

        switch (mem.memType) {
            case 'prop': {
                dtsUnit.members.push(dtsUnitMember = dom.create.property(
                    mem.name,
                    mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                    memFlags
                ))
                break
            }
            case 'method': {
                const withOptionalParam = false;
                let withRestArgs = false;
                function getMethodParam(paramsHost) {
                    const built = buildDtsMethodParams(paramsHost, getMapParamOptions);
                    withRestArgs = built.withRestArgs;
                    return built.params;
                }

                function getMergedMethodParams({ short, long, pos }) {
                    const built = buildDtsMethodParams(long, getMapParamOptions, {
                        typeOverride: (i, memParam) => {
                            if (i !== pos)
                                return null;

                            const shortInfo = mapParamTypeToDtsType(short.params[i].type, getMapParamOptions(short.params[i]));
                            const longInfo = mapParamTypeToDtsType(memParam.type, getMapParamOptions(memParam));
                            return dom.create.union([shortInfo.type, longInfo.type]);
                        },
                    });
                    withRestArgs = built.withRestArgs;
                    return built.params;
                }

                if (isInterfaceConstructor) {
                    dtsUnit.members.push(dtsUnitMember = dom.create.constructor(getMethodParam(mem), memFlags))
                } else {
                    if (mem.overs && mem.overs.length > 1) {
                        memberIsOver = true;
                        mem.overs.forEach(over => {
                            const { syncMethod, asyncMethod, syncVariant, asyncVariant } = generateDtsMethod(
                                over,
                                getMethodParam(over),
                                mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                                {
                                    memFlags,
                                    withOptionalParam,
                                    withRestArgs,
                                    hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                                    mapReturnType: mapMemberReturnType,
                                }
                            )

                            syncMethod.jsDocComment = convertIDLCommentToJSDocComment(over.comments)
                            dtsUnit.members.push(syncMethod);

                            if (asyncMethod) {
                                dtsUnit.members.push(asyncMethod);
                            }
                            
                            if (syncVariant) {
                                syncVariant.jsDocComment = convertIDLCommentToJSDocComment(over.comments);
                                dtsUnit.members.push(syncVariant);
                            }
                            
                            if (asyncVariant) {
                                asyncVariant.jsDocComment = convertIDLCommentToJSDocComment(over.comments);
                                dtsUnit.members.push(asyncVariant);
                            }
                        });
                    } else {
                        const { syncMethod, asyncMethod, syncVariant, asyncVariant } = generateDtsMethod(
                            mem, getMethodParam(mem), mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                            {
                                memFlags,
                                withOptionalParam,
                                withRestArgs,
                                hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                                mapReturnType: mapMemberReturnType,
                            }
                        )

                        dtsUnitMember = syncMethod;
                        dtsUnit.members.push(syncMethod);
                        dtsUnitMember.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '')

                        if (asyncMethod) {
                            dtsUnit.members.push(asyncMethod)
                        }
                        
                        if (syncVariant) {
                            syncVariant.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');
                            dtsUnit.members.push(syncVariant);
                        }
                        
                        if (asyncVariant) {
                            asyncVariant.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');
                            dtsUnit.members.push(asyncVariant);
                        }

                        const merged = mergedOverloads.get(mem);
                        if (merged) {
                            // the Buffer/String pair accepts either form at runtime; add the union signature
                            const {
                                syncMethod: mergedMethod,
                                asyncMethod: mergedAsyncMethod,
                                syncVariant: mergedSyncVariant,
                                asyncVariant: mergedAsyncVariant
                            } = generateDtsMethod(
                                merged.long, getMergedMethodParams(merged), mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                                {
                                    memFlags,
                                    withOptionalParam,
                                    withRestArgs,
                                    hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                                    mapReturnType: mapMemberReturnType,
                                }
                            );

                            dtsUnit.members.push(mergedMethod);

                            if (mergedAsyncMethod) {
                                dtsUnit.members.push(mergedAsyncMethod);
                            }

                            if (mergedSyncVariant) {
                                dtsUnit.members.push(mergedSyncVariant);
                            }

                            if (mergedAsyncVariant) {
                                dtsUnit.members.push(mergedAsyncVariant);
                            }
                        }
                    }
                }
                break
            }
            case 'object': {
                dtsUnit.members.push(dtsUnitMember = dom.create.property(
                    mem.name,
                    mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                    dom.DeclarationFlags.Static | memFlags
                ))
                break
            }
            case 'operator': {
                // `Type operator[]` / `Type operator[String]` is the indexed
                // accessor (`list[0]`, `map['name']`). dts-dom's class printer
                // drops index-signature members, so emit a sentinel property
                // and let postProcessDtsUnitString() rewrite it.
                dtsUnit.members.push(dtsUnitMember = dom.create.property(
                    INDEX_SIGNATURE_PLACEHOLDER + (mem.index ? 'string' : 'number') + '__',
                    mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions())
                ));
                break
            };
            case 'event': {
                // `event message(Buffer data)` maps to the event listener
                // signature: every listener-pair method the class chain
                // provides (on/once/off/addListener/...) gets an overload
                // narrowed by the event name, the declared params being the
                // listener arguments. The runtime accessor `on<name>` is
                // emitted as well. The inherited forms of these names are
                // materialized after the member loop (R1).
                let eventDocHost;

                EVENT_LISTENER_METHOD_NAMES.forEach((name) => {
                    const declaration = resolveListenerMethodDeclaration(def, defs, name);
                    if (!declaration)
                        return;

                    const member = createEventRegistrationMember(name, mem, {
                        memFlags, unitName, allInterfacesNames, allModuleNames,
                        addRefToTripleSlashDirectivesHost,
                    }, declaration);

                    dtsUnit.members.push(member);
                    if (!eventDocHost)
                        eventDocHost = member;

                    if (!synthesizedMemberNames.includes(name))
                        synthesizedMemberNames.push(name);
                });

                const accessor = createEventAccessorMember(mem, {
                    memFlags, unitName, allInterfacesNames, allModuleNames,
                    addRefToTripleSlashDirectivesHost,
                });
                dtsUnit.members.push(accessor);

                // the event comment documents the accessor when the class chain
                // provides no listener-pair method to hang it on
                dtsUnitMember = eventDocHost || accessor;
                break
            };
            case 'const': {
                // a class constant is a static readonly member of the class
                // (`WebSocket.OPEN`, `Message.TEXT`, ...): dts-dom's const
                // declaration is a top-level binding, not a class member.
                dtsUnit.members.push(dtsUnitMember = dom.create.property(
                    mem.name,
                    mapMemConstType(mem, unitCategory, unitName),
                    memFlags | dom.DeclarationFlags.Static | dom.DeclarationFlags.ReadOnly
                ))
                break
            }
        }

        if (dtsUnitMember) {
            dtsUnitMember.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');
        } else if (!memberIsOver) {
            throw new Error(`generating dtsUnitMember failed! unsupported member '${mem.name}' (with memType '${mem.memType}') on ${unitCategory} '${unitName}'`)
        }

    });

    // R1: the synthesized members above hide the inherited overloads of the
    // same name in TypeScript; re-emit them so the call surface of the runtime
    // (base prototypes stay on the prototype chain) is preserved. The promise
    // variant does the same walk over the promise-flavored base chain, so the
    // signatures it re-emits are promise-shaped.
    synthesizedMemberNames.forEach(name => materializeInheritedOverloads(def, name, {
        defs,
        dtsUnit,
        memFlags: 0,
        declaredMethodNames,
        addRefToTripleSlashDirectivesHost,
        allInterfacesNames,
        allModuleNames,
        unitName,
        unitFlavor,
    }));

    dtsUnit.jsDocComment = convertIDLCommentToJSDocComment(def.declare.comments || '');

    // console.notice(`:--- try to emit dts for ${unitCategory}: ${unitName} ---->`)
    let declared = dom.emit(dtsUnit, {
        tripleSlashDirectives: Object.values(tripleSlashDirectiveMap),
    });

    if (callSignatures.length) {
        // the call signature merges into the instance type through a sibling
        // interface - a class declaration cannot hold one (TS1068). The name
        // tracks the emitted class, so the promise variant gets its own.
        const merged = dom.create.interface(dtsUnit.name);
        callSignatures.forEach(signature => merged.members.push(signature));
        declared += '\n' + dom.emit(merged);
    }

    return declared;
}

/**
 * @param {import('./ir').IIDLDefinition} def
 * @param {*} configuration
 * @param {*} retValue
 */
function processDeclareModule(def, {
    unitName,
    unitCategory,
    defs,
    allInterfacesNames,
    allModuleNames,
    // the flavor this unit's own members return (`'promise'` for the
    // `<name>/promises` variant module, see plans/idl-event-types-2026-10-03.md §15)
    unitFlavor = 'fiber',
}, {
    dtsUnit,
    tripleSlashDirectiveMap,
}) {
    const declaredMethodNames = new Set(
        def.members
            .filter(mem => mem.memType === 'method')
            .map(mem => mem.name)
    );

    const addRefToTripleSlashDirectivesHost = getAddRefToTripleSlashDirectivesHost(tripleSlashDirectiveMap, { allInterfacesNames, allModuleNames, selfName: unitName });

    const mergedOverloads = mergedBufferStringOverloads(def.members);

    // `operator(...)`: the module object itself is callable. The overloads are
    // emitted as functions named after the module, everything else gathers
    // into a namespace and `export = <module>` publishes the merged entity
    // (the shape @types/node gives `node:test`). This is also what makes the
    // module aliases (`const it: typeof import('test')`) callable.
    const callFunctions = [];

    if (def.members.some(isCallOperatorMember) && !/^[A-Za-z_$][\w$]*$/.test(unitName))
        throw new Error(`the call operator of module '${unitName}' needs a valid identifier to merge the function with (rename the module or drop the operator(...) declaration)`);

    def.members.forEach(mem => {
        let memFlags = 0;
        if (mem.static) {
            memFlags |= dom.DeclarationFlags.Static
        }

        if (mem.readonly) {
            memFlags |= dom.DeclarationFlags.ReadOnly
        }

        function getMapParamOptions(memParam) {
            return {
                param: memParam, dtsUnitName: unitName,
                paramHostName: mem.name, allInterfacesNames, allModuleNames,
                addRefToTripleSlashDirectivesHost: addRefToTripleSlashDirectivesHost,
            }
        }

        function getMapMemberTypeOptions(useRefInstance = true, flavor = unitFlavor) {
            return {
                memberInfo: mem, dtsUnitName: unitName,
                memberHostName: unitName, allInterfacesNames, allModuleNames,
                addRefToTripleSlashDirectivesHost: addRefToTripleSlashDirectivesHost,
                useRefInstance,
                instanceFlavor: flavor,
            }
        }

        // per-variant return type of this member (see processDeclareInterface)
        function mapMemberReturnType(flavor) {
            return mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions(true, flavor));
        }

        /**
         * @typedef {import('dts-dom').ModuleDeclaration} IDTSUnit
         * @type {IDTSUnit['members'][number]}
         */
        let dtsUnitMember;
        let memberIsOver = false;

        if (isCallOperatorMember(mem)) {
            // the call overloads become functions named after the module (see
            // the callFunctions comment above)
            if (!mem.static)
                throw new Error(`the call operator of module '${unitName}' must be static`);

            const overloads = (mem.overs && mem.overs.length > 1) ? mem.overs : [mem];

            overloads.forEach(over => {
                const { params, withRestArgs } = buildDtsMethodParams(over, getMapParamOptions);
                const { syncFunc, asyncFunc, syncVariant, asyncVariant } = generateDtsFunction(
                    Object.assign({}, over, { name: unitName }),
                    params,
                    mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                    {
                        withRestArgs,
                        hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                        mapReturnType: mapMemberReturnType,
                    }
                );

                syncFunc.jsDocComment = convertIDLCommentToJSDocComment(over.comments || '');
                callFunctions.push(syncFunc);

                [asyncFunc, syncVariant, asyncVariant]
                    .filter(Boolean)
                    .forEach(member => callFunctions.push(member));
            });

            return;
        }

        switch (mem.memType) {
            case 'prop': {
                dtsUnitMember = mem.readonly ? dom.create.const(
                    mem.name,
                    mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                    dom.DeclarationFlags.Static | memFlags
                ) : dom.create.variable(
                    mem.name,
                    mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                )

                dtsUnit.members.push(dtsUnitMember)
                break
            }
            case 'method': {
                let withOptionalParam = false;
                let withRestArgs = false;
                function getFunctionParams(paramsHost) {
                    let currentWithOptionalParam = false;
                    return (paramsHost.params || []).map(param => {
                        const paramDomInfo = mapParamTypeToDtsType(param.type, getMapParamOptions(param));
                        withRestArgs = !!paramDomInfo.isRestArgs;

                        let paramFlag = withRestArgs ? dom.ParameterFlags.Rest : dom.ParameterFlags.None;

                        // Check if this specific parameter has a default value
                        const hasDefault = hasDefaultValue(param);
                        if (!paramDomInfo.isRestArgs && (currentWithOptionalParam || hasDefault)) {
                            currentWithOptionalParam = true;
                            paramFlag |= dom.ParameterFlags.Optional;
                        }

                        return dom.create.parameter(
                            param.name,
                            paramDomInfo.type,
                            paramFlag
                        )
                    });
                }

                function getMergedFunctionParams({ short, long, pos }) {
                    let currentWithOptionalParam = false;
                    return long.params.map((param, i) => {
                        let paramDomInfo;
                        if (i === pos) {
                            const shortInfo = mapParamTypeToDtsType(short.params[i].type, getMapParamOptions(short.params[i]));
                            const longInfo = mapParamTypeToDtsType(param.type, getMapParamOptions(param));
                            paramDomInfo = { type: dom.create.union([shortInfo.type, longInfo.type]) };
                        } else {
                            paramDomInfo = mapParamTypeToDtsType(param.type, getMapParamOptions(param));
                        }

                        withRestArgs = !!paramDomInfo.isRestArgs;

                        let paramFlag = withRestArgs ? dom.ParameterFlags.Rest : dom.ParameterFlags.None;

                        const hasDefault = hasDefaultValue(param);
                        if (!paramDomInfo.isRestArgs && (currentWithOptionalParam || hasDefault)) {
                            currentWithOptionalParam = true;
                            paramFlag |= dom.ParameterFlags.Optional;
                        }

                        return dom.create.parameter(
                            param.name,
                            paramDomInfo.type,
                            paramFlag
                        )
                    });
                }

                if (mem.overs && mem.overs.length > 1) {
                    memberIsOver = true;
                    mem.overs.forEach(over => {
                        const { syncFunc, asyncFunc, syncVariant, asyncVariant } = generateDtsFunction(
                            over,
                            getFunctionParams(over),
                            mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                            {
                                funcFlags: memFlags,
                                withOptionalParam,
                                withRestArgs,
                                hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                                mapReturnType: mapMemberReturnType,
                            }
                        )

                        syncFunc.jsDocComment = convertIDLCommentToJSDocComment(over.comments)
                        dtsUnit.members.push(syncFunc);

                        if (asyncFunc) {
                            dtsUnit.members.push(asyncFunc);
                        }
                        
                        if (syncVariant) {
                            syncVariant.jsDocComment = convertIDLCommentToJSDocComment(over.comments);
                            dtsUnit.members.push(syncVariant);
                        }
                        
                        if (asyncVariant) {
                            asyncVariant.jsDocComment = convertIDLCommentToJSDocComment(over.comments);
                            dtsUnit.members.push(asyncVariant);
                        }
                    });
                } else {
                    const { asyncFunc, syncFunc, syncVariant, asyncVariant } = generateDtsFunction(
                        mem,
                        getFunctionParams(mem), mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                        {
                            withOptionalParam,
                            withRestArgs,
                            hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                            mapReturnType: mapMemberReturnType,
                        }
                    );

                    dtsUnit.members.push(dtsUnitMember = syncFunc);

                    if (asyncFunc) {
                        dtsUnit.members.push(asyncFunc);
                    }
                    
                    if (syncVariant) {
                        syncVariant.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');
                        dtsUnit.members.push(syncVariant);
                    }
                    
                    if (asyncVariant) {
                        asyncVariant.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');
                        dtsUnit.members.push(asyncVariant);
                    }

                    const merged = mergedOverloads.get(mem);
                    if (merged) {
                        // the Buffer/String pair accepts either form at runtime; add the union signature
                        const {
                            syncFunc: mergedFunc,
                            asyncFunc: mergedAsyncFunc,
                            syncVariant: mergedSyncVariant,
                            asyncVariant: mergedAsyncVariant
                        } = generateDtsFunction(
                            merged.long, getMergedFunctionParams(merged), mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                            {
                                funcFlags: memFlags,
                                withOptionalParam,
                                withRestArgs,
                                hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                                mapReturnType: mapMemberReturnType,
                            }
                        );

                        dtsUnit.members.push(mergedFunc);

                        if (mergedAsyncFunc) {
                            dtsUnit.members.push(mergedAsyncFunc);
                        }

                        if (mergedSyncVariant) {
                            dtsUnit.members.push(mergedSyncVariant);
                        }

                        if (mergedAsyncVariant) {
                            dtsUnit.members.push(mergedAsyncVariant);
                        }
                    }
                }

                break
            }
            case 'object': {
                dtsUnit.members.push(dtsUnitMember = dom.create.const(
                    mem.name,
                    mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions(false)),
                    dom.DeclarationFlags.Static | memFlags
                ))
                break
            }
            case 'const': {
                dtsUnit.members.push(dtsUnitMember = dom.create.const(
                    mem.name,
                    (() => {
                        if (mem.default && mem.default.value) {
                            const value = mem.default.value;
                            // Infer type from value
                            if (value === 'true' || value === 'false') {
                                return dom.create.namedTypeReference(value);
                            } else if (value.startsWith('"') && value.endsWith('"')) {
                                const strValue = value.replace(/^"|"$/g, '');
                                return dom.type.stringLiteral(strValue);
                            } else {
                                return dom.type.numberLiteral(value);
                            }
                        }

                        throw new Error(`unsupported const-memType member '${mem.name}' on ${unitCategory} '${unitName}'`)
                    })(),
                    memFlags | dom.DeclarationFlags.Export
                ))
                break
            }
        }

        if (dtsUnitMember) {
            dtsUnitMember.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '');
        } else if (!memberIsOver) {
            throw new Error(`generating dtsUnitMember failed! unsupported member '${mem.name}' (with memType '${mem.memType}') on ${unitCategory} '${unitName}'`)
        }
    });

    materializeModuleBase(def, {
        defs, dtsUnit, unitName, unitFlavor, declaredMethodNames,
        allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
    });

    if (callFunctions.length) {
        // function + namespace + `export =`: what the call overloads merge
        // with. The namespace takes the rest of the module surface (the
        // materialized base members included).
        const ns = dom.create.namespace(unitName);
        ns.members = dtsUnit.members;
        dtsUnit.members = callFunctions.concat([ns, dom.create.exportEquals(unitName)]);
    }

    dtsUnit.jsDocComment = convertIDLCommentToJSDocComment(def.declare.comments || '');

    // console.notice(`:--- try to emit dts for ${unitCategory}: ${unitName} ---->`)
    return dom.emit(dtsUnit, {
        // rootFlags: dom.ContextFlags.Module,
        tripleSlashDirectives: Object.values(tripleSlashDirectiveMap),
    });
}

/**
 * A module can extend a class at runtime - `module process : EventEmitter` in
 * idl/process.idl, which is how the runtime object carries `on`/`once`/`off`/
 * `emit` and the rest of the event surface. A d.ts namespace cannot inherit, so
 * the inherited members are copied in as module-level declarations instead
 * (the checker has no way to spell `declare module "process" extends
 * Class_EventEmitter`).
 *
 * The walk goes from the nearest base outward, like the class path's R1
 * materialization, and the members a module declares itself always win (they
 * were emitted first and `hasDeclaredMemberName` keeps the aliases out).
 */
function materializeModuleBase(def, {
    defs,
    dtsUnit,
    unitName,
    unitFlavor,
    declaredMethodNames,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
}) {
    // `object` is the parser's "no base" sentinel (gen_code reads it the same
    // way: `base_has_async()` stops on it), so only a real class extends here
    if (!def.declare.extend || def.declare.extend === 'object')
        return;

    const seen = new Set();

    for (let base = defs[def.declare.extend]; base; base = defs[base.declare.extend]) {
        if (seen.has(base.declare.name))
            break;
        seen.add(base.declare.name);

        (base.members || []).forEach(mem => {
            if (mem.memType !== 'method' && mem.memType !== 'prop' && mem.memType !== 'const')
                return;

            const getMapParamOptions = (memParam) => ({
                param: memParam, dtsUnitName: unitName, paramHostName: mem.name,
                allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
            });
            const getMapMemberTypeOptions = (useRefInstance = true, flavor = unitFlavor) => ({
                memberInfo: mem, dtsUnitName: unitName, memberHostName: unitName,
                allInterfacesNames, allModuleNames, addRefToTripleSlashDirectivesHost,
                useRefInstance, instanceFlavor: flavor,
            });
            const mapMemberReturnType = (flavor) =>
                mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions(true, flavor));

            if (mem.memType === 'method') {
                const overloads = (mem.overs && mem.overs.length > 1) ? mem.overs : [mem];

                overloads.forEach(over => {
                    const { params, withRestArgs } = buildDtsMethodParams(over, getMapParamOptions);
                    const built = generateDtsFunction(
                        over,
                        params,
                        mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                        {
                            withRestArgs,
                            hasDeclaredMemberName: (name) => declaredMethodNames.has(name),
                            mapReturnType: mapMemberReturnType,
                        }
                    );

                    [built.syncFunc, built.asyncFunc, built.syncVariant, built.asyncVariant]
                        .filter(Boolean)
                        .forEach(member => dtsUnit.members.push(member));
                });

                return;
            }

            if (mem.memType === 'const') {
                dtsUnit.members.push(dom.create.const(
                    mem.name,
                    mapMemConstType(mem, 'module', unitName),
                    dom.DeclarationFlags.ReadOnly
                ));
                return;
            }

            dtsUnit.members.push(dom.create.const(
                mem.name,
                mapMemPropertyTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                dom.DeclarationFlags.ReadOnly
            ));
        });
    }
}

// A shaped callback overload only types the user's arrow when TypeScript picks
// it as the first match. A function argument is also assignable to the
// value-ish parameter types (`Value` -> any, `Object` -> FIBJS.GeneralObject,
// `NMap`/`NObject`/`RegExp` -> the same, and a rest parameter), so an overload
// that takes one of them at the callback position swallows the call and the
// arrow silently loses its contextual type (`resp` becomes implicit `any`).
// Move every shaped overload before the earliest member of its own overload
// group that would swallow it; the rest keep the declared order.
const FUNCTION_SWALLOWING_TYPES = ['Value', 'Variant', 'Object', 'NMap', 'NObject', 'RegExp', '...'];

function swallowsFunction(p) {
    return !!p && !p.isarray && FUNCTION_SWALLOWING_TYPES.includes(p.type);
}

function shapedCallbackSlots(mem) {
    const slots = [];

    if (mem.memType === 'method') {
        (mem.params || []).forEach((p, i) => {
            if (p && p.callback)
                slots.push(i);
        });
    }

    return slots;
}

function reorderSwallowedCallbackMembers(members) {
    const out = [];

    members.forEach((mem) => {
        const slots = shapedCallbackSlots(mem);

        if (!slots.length) {
            out.push(mem);
            return;
        }

        let at = -1;
        for (let i = 0; i < out.length && at < 0; i++) {
            const other = out[i];

            if (other.memType !== 'method' || !!other.static !== !!mem.static || other.name !== mem.name)
                continue;

            if (slots.some(s => swallowsFunction((other.params || [])[s])))
                at = i;
        }

        if (at >= 0)
            out.splice(at, 0, mem);
        else
            out.push(mem);
    });

    return out;
}

/**
 * @description generate *.d.ts for definitions
 * 
 * @param {Record<string, import('./ir').IIDLDefinition>} defs 
 * 
 * @returns {{
 *  allInterfacesNames: Set<string>
 *  allModuleNames: Set<string>
 * }}
 */
function gen_dts_for_declare(defs, { DTS_DIST_DIR }) {
    const allInterfacesNames = new Set();
    const allModuleNames = new Set();

    Object.values(defs)
        .forEach(def => {
            if (!def.declare.module) {
                allInterfacesNames.add(def.declare.name)
            } else {
                allModuleNames.add(def.declare.name)
            }
        });

    // classes the runtime gives a promise variant (own or inherited async
    // member, see classHasPromiseVariant): every class reference then resolves
    // through classInstanceTypeRef() to the flavor of the position it sits in
    PROMISE_VARIANT_CLASSES.clear();
    Object.values(defs)
        .filter(def => !def.declare.module && !EXCLUDE_INTERNALS.includes(def.declare.name))
        .forEach(def => {
            if (classHasPromiseVariant(def, defs))
                PROMISE_VARIANT_CLASSES.add(def.declare.name);
        });

    // the promise-flavored copy of every async-capable class: R1's inherited
    // overload materialization walks it when it generates a promise variant
    const promiseDefs = {};
    Object.values(defs).forEach(def => {
        if (!def.declare.module && PROMISE_VARIANT_CLASSES.has(def.declare.name))
            promiseDefs[def.declare.name] = asPromiseFlavorDef(def);
    });

    Object.values(defs)
        .filter(
            // ignore some special idl json from
            (def) => !EXCLUDE_INTERNALS.includes(def.declare.name)
        )
        .forEach((def) => {
            // shaped callbacks must be emitted before same-group overloads that
            // would swallow a function argument (Value / Object / rest), see
            // reorderSwallowedCallbackMembers()
            def.members = reorderSwallowedCallbackMembers(def.members);

            const ismodule = def.declare.module;
            const unitCategory = ismodule ? 'module' : 'interface';

            const basedir = path.resolve(DTS_DIST_DIR, `./${unitCategory}`);
            if (!fs.existsSync(basedir)) {
                fs.mkdir(basedir);
            }

            const unitName = def.declare.name;
            const dtsUnit = !ismodule ? dom.create.class(normalizeClazzName(unitName))
                : dom.create.module(runtimeModuleName(unitName))
            const tripleSlashDirectiveMap = {};

            tripleSlashDirectiveMap['_fibjs.d.ts'] = dom.create.tripleSlashReferencePathDirective(`../_import/_fibjs.d.ts`)

            const processOptions = {
                unitName,
                unitCategory,
                ismodule,
                defs,
                promiseDefs,
                allInterfacesNames,
                allModuleNames,
            };
            const processRetValue = {
                dtsUnit,
                tripleSlashDirectiveMap,
            };
            let unitDeclare;
            if (!ismodule) {
                unitDeclare = processDeclareInterface(def, processOptions, processRetValue);
            } else {
                unitDeclare = processDeclareModule(def, processOptions, processRetValue);
            }

            unitDeclare = postProcessDtsUnitString(unitDeclare);

            // mirror the runtime's `promises` automation: async-capable modules
            // get a variant module file, async-capable classes get a promise
            // variant declaration plus `Class_X.promises`. A class counts as
            // async-capable when its own members have an async one *or* an
            // ancestor does (`ClassData::has_async`), which is exactly the set
            // PROMISE_VARIANT_CLASSES holds.
            const hasPromiseVariant = ismodule
                ? hasAsyncMembers(def) && (!ROOT_MODULE_NAMES || ROOT_MODULE_NAMES.has(unitName))
                : PROMISE_VARIANT_CLASSES.has(unitName);

            if (hasPromiseVariant && ismodule && def.members.some(isCallOperatorMember))
                throw new Error(`the callable module '${unitName}' cannot also have a promise variant yet: the promises member would have to live inside the merged namespace (see CALL_OPERATOR_NAME)`);

            if (hasPromiseVariant) {
                if (ismodule) {
                    writeDtsFile(path.join(basedir, `${unitName}.promises.d.ts`),
                        buildPromiseModuleDeclare(def, processOptions));
                } else {
                    unitDeclare += '\n' + buildPromiseClassDeclare(def, processOptions);
                }
            }

            // Buffer is hand-written (tools/handwritten/Buffer.d.ts): its surface is
            // `declare class Class_Buffer extends Uint8Array`, which the IDL cannot
            // express. Keep the corpus slot in sync with the hand-written file; the
            // member list is checked by test/buffer_types_test.js.
            if (unitName === 'Buffer' && !ismodule) {
                const handWritten = path.resolve(__dirname, '../../tools/handwritten/Buffer.d.ts');

                writeDtsFile(path.join(basedir, `${unitName}.d.ts`),
                    fs.existsSync(handWritten) ? fs.readFileSync(handWritten, 'utf8') : unitDeclare);
                return;
            }

            writeDtsFile(path.join(basedir, `${unitName}.d.ts`), unitDeclare);
            // console.notice(`---- generated dts for ${unitCategory}: ${unitName} ---<:`)
        });

    return {
        allInterfacesNames,
        allModuleNames
    }
}

/**
 * 
 * @param {{
 *  allModuleNames: Set<string>
 * }} param0 
 */
function gen_fibjs_import_dts({
    DTS_DIST_DIR
}) {
    const basedir = path.resolve(DTS_DIST_DIR, './_import');
    if (!fs.existsSync(basedir)) {
        fs.mkdirSync(basedir);
    }

    const topDeclarition = dom.create.namespace('FIBJS');

    const typedArrayType = dom.create.union([
        dom.create.namedTypeReference('Int8Array'),
        dom.create.namedTypeReference('Uint8Array'),
        dom.create.namedTypeReference('Int16Array'),
        dom.create.namedTypeReference('Uint16Array'),
        dom.create.namedTypeReference('Int32Array'),
        dom.create.namedTypeReference('Uint32Array'),
        dom.create.namedTypeReference('Uint8ClampedArray'),
        dom.create.namedTypeReference('Float32Array'),
        dom.create.namedTypeReference('Float64Array'),
        dom.create.namedTypeReference('BigInt64Array'),
        dom.create.namedTypeReference('BigUint64Array'),
    ]);
    const typeAlias = dom.create.alias('TypedArray', typedArrayType);
    topDeclarition.members.push(typeAlias);

    const typeGeneralObject = dom.create.interface('GeneralObject');
    typeGeneralObject.members.push(
        dom.create.indexSignature('k', 'string', dom.type.any),
    );

    topDeclarition.members.push(typeGeneralObject)

    const commonDeclaration = dom.emit(topDeclarition, {
        rootFlags: dom.DeclarationFlags.None,
    });
    writeDtsFile(path.join(basedir, `_fibjs.d.ts`), commonDeclaration);
}

/**
 * 
 * @param {{
 *  allModuleNames: Set<string>
 * }} param0 
 */
function gen_bridge_dts({
    allModuleNames,
    DTS_DIST_DIR
}) {
    const basedir = path.resolve(DTS_DIST_DIR, './_import');
    if (!fs.existsSync(basedir)) {
        fs.mkdirSync(basedir);
    }

    const tripleSlashDirectives = [];
    const topDeclarition = dom.create.module('@fibjs/types/bridge');

    Array.from(allModuleNames).forEach(moduleName => {
        tripleSlashDirectives.push(
            dom.create.tripleSlashReferencePathDirective(`../module/${moduleName}.d.ts`)
        )
    });

    const bridgeDeclaration = dom.emit(topDeclarition, {
        rootFlags: dom.DeclarationFlags.None,
        tripleSlashDirectives
    });
    writeDtsFile(path.join(basedir, `bridge.d.ts`), bridgeDeclaration);
}

/**
 * @description generate *.d.ts for fibjs
 * 
 * @param {Record<string, import('./ir').IIDLDefinition>} defs 
 */
module.exports = function gen_dts(defs, { DTS_DIST_DIR }) {
    const totalDefs = Object.keys(defs).length;
    console.log(`   🔷 Generating TypeScript definitions for ${totalDefs} declarations...`);

    const {
        allModuleNames,
    } = gen_dts_for_declare(defs, { DTS_DIST_DIR });

    console.log(`   📦 Generating fibjs import definitions...`);
    gen_fibjs_import_dts({ DTS_DIST_DIR });

    console.log(`   🌉 Generating bridge definitions for ${allModuleNames.size} modules...`);
    gen_bridge_dts({ allModuleNames, DTS_DIST_DIR });

    console.log(`   ✅ TypeScript definitions saved to ${path.basename(DTS_DIST_DIR)}/`);
}