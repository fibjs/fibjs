/**
 * @author richardo2016@gmail.com
 * @email richardo2016@gmail.com
 * @create date 2021-05-03 22:47:54
 * @modify date 2021-05-03 22:47:54
 *
 * @desc generate types from fibjs's idl
 */

const dom = require('dts-dom')

const fs = require('fs');
const path = require('path');

// function isIDLRestToken(paramName) {
//     return paramName === '...';
// }

function postProcessDtsUnitString(str) {
    return str
    // .replace(new RegExp(QUOTE_START_PLACEHOLDER, 'g'), '')
    // .replace(new RegExp(QUOTE_END_PLACEHOLDER, 'g'), '')
}

function convertIDLCommentToJSDocComment(comment = '') {
    return comment
        .replace('! @brief', '@description')
        .replace('@brief', '@description')
}

function normalizeClazzName(interfaceName) {
    return `Class_${interfaceName}`
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
    }
) {
    return (sourceType, {
        refName, refType, refHostName
    }) => {
        if (EXCLUDE_INTERNALS.includes(refType)) {
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
}) {
    const info = {
        type: null,
        refType: null,
        isRestArgs: false,
    };

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
            {
                info.type = dom.create.namedTypeReference(dataType);
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
        case 'Object': {
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
            info.type = dom.create.typeof('Date');
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
            info.type = useRefInstance ? dom.create.namedTypeReference(normalizeClazzName(dataType)) : dom.create.typeof(normalizeClazzName(dataType));
            info.isFibjsInterface = true;
            info.refType = 'interface';
        } else if (allModuleNames.has(dataType)) {
            const moduleName = dataType;
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
 * @param {import('../../idl/ir').IIDLDefinition['members'][number]['params'][number]['type']} paramType 
 */
function mapMemPropertyTypeToDtsType(propertyType, {
    memberInfo,
    memberHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
    useRefInstance = true
}) {
    const result = generalTypeMap(propertyType, { allInterfacesNames, allModuleNames, useRefInstance });

    if (result.type) {
        if (result.refType && dtsUnitName !== propertyType) {
            addRefToTripleSlashDirectivesHost(result.refType, {
                refHostName: memberHostName,
                refName: memberInfo.name,
                refType: propertyType,
            })
        }
        return result.type
    }

    switch (propertyType) {
        default: {
            throw new Error(`[mapMemPropertyTypeToDtsType] unsupported propertyType '${propertyType}' of member '${memberInfo.name}' on host '${memberHostName}'`)
        }
    }
}

/**
 * 
 * @param {import('../../idl/ir').IIDLDefinition['members'][number]['params'][number]['type']} memReturnType 
 */
function mapMemMethodReturnTypeToDtsType(memReturnType, {
    memberInfo,
    memberHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
}) {
    if (Array.isArray(memReturnType)) {
        const typeRef = dom.create.namedTypeReference(`[${memReturnType.map(returnParam => {
            const gMap = generalTypeMap(returnParam.type, { allInterfacesNames, allModuleNames });

            return `${returnParam.name}: ${gMap ? gMap.type : returnParam.type}`
        }).join(', ')}]`)

        return typeRef
    }

    const result = generalTypeMap(memReturnType, { allInterfacesNames, allModuleNames, useRefInstance: true });

    if (result.type) {
        if (result.refType && dtsUnitName !== memReturnType) {
            addRefToTripleSlashDirectivesHost(result.refType, {
                refHostName: memberHostName,
                refName: memberInfo.name,
                refType: memReturnType,
            })
        }
        return result.type
    }

    switch (memReturnType) {
        default: {
            throw new Error(`[mapMemPropertyTypeToDtsType] unsupported memReturnType '${memReturnType}' of member function '${memberInfo.name}' on host '${memberHostName}'`)
        }
    }
}

/**
 * 
 * @param {import('../../idl/ir').IIDLDefinition['members'][number]['type']} paramType 
 */
function mapParamTypeToDtsType(paramType, {
    param: paramInfo,
    paramHostName,
    dtsUnitName,
    allInterfacesNames,
    allModuleNames,
    addRefToTripleSlashDirectivesHost,
}) {
    const result = generalTypeMap(paramType, { allInterfacesNames, allModuleNames, useRefInstance: true });

    if (result.type) {
        if (result.refType && dtsUnitName !== paramType) {
            addRefToTripleSlashDirectivesHost('methodParam', {
                refHostName: paramHostName,
                refName: paramInfo.name,
                refType: paramType,
            })
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
 * 
 * @param {import('../../idl/ir').IIDLDefinition['members'][number]['overs']} methodHost 
 * @returns 
 */
function generateDtsFunction(functionHost, normalParams, returnType, {
    funcFlags,
    withOptionalParam = false,
    withRestArgs = false,
} = {}) {
    let syncFunc;
    let asyncFunc;

    const params = Array.from(normalParams);

    syncFunc = dom.create.function(
        functionHost.name,
        params,
        returnType,
        funcFlags
    )

    if (!withRestArgs && functionHost.async) {
        const errorParam = dom.create.parameter('err', dom.create.union([
            dom.create.namedTypeReference('Error'),
            dom.type.undefined,
            dom.type.null
        ]));

        const callbackType = dom.create.functionType([
            errorParam,
            !isVoidDomType(returnType) && dom.create.parameter('retVal', returnType)
        ].filter(Boolean), dom.type.any);

        const params = Array.from(normalParams);
        params.push(
            dom.create.parameter(
                'callback', callbackType,
                withOptionalParam ? dom.ParameterFlags.Optional : dom.ParameterFlags.None
            )
        )

        asyncFunc = dom.create.function(
            functionHost.name,
            params,
            dom.type.void,
            funcFlags
        )
    }

    return {
        syncFunc,
        asyncFunc
    }
}

/**
 * 
 * @param {import('../../idl/ir').IIDLDefinition['members'][number]['overs']} methodHost 
 * @returns 
 */
function generateDtsMethod(methodHost, normalParams, returnType, {
    memFlags,
    withOptionalParam = false,
    withRestArgs = false
} = {}) {
    let syncMethod;
    let asyncMethod;

    const params = Array.from(normalParams);

    syncMethod = dom.create.method(
        methodHost.name,
        params,
        returnType,
        memFlags
    )

    if (!withRestArgs && methodHost.async) {
        const errorParam = dom.create.parameter('err', dom.create.union([
            dom.create.namedTypeReference('Error'),
            dom.type.undefined,
            dom.type.null
        ]));

        const callbackType = dom.create.functionType([
            errorParam,
            !isVoidDomType(returnType) && dom.create.parameter('retVal', returnType)
        ].filter(Boolean), dom.type.any);

        const params = Array.from(normalParams);
        params.push(
            dom.create.parameter(
                'callback', callbackType,
                withOptionalParam ? dom.ParameterFlags.Optional : dom.ParameterFlags.None
            )
        )

        asyncMethod = dom.create.method(
            methodHost.name,
            params,
            dom.type.void,
            memFlags
        )
    }

    return {
        syncMethod,
        asyncMethod
    }
}

/**
 * @param {import('../../idl/ir').IIDLDefinition} def
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
    allInterfacesNames,
    allModuleNames,
}, {
    dtsUnit,
    tripleSlashDirectiveMap,
}) {
    if (def.declare.extend) {
        const refType = def.declare.extend;
        dtsUnit.baseType = dom.create.namedTypeReference(normalizeClazzName(refType))
        tripleSlashDirectiveMap[refType] = dom.create.tripleSlashReferencePathDirective(`../interface/${refType}.d.ts`)
    }

    const addRefToTripleSlashDirectivesHost = getAddRefToTripleSlashDirectivesHost(tripleSlashDirectiveMap, { allInterfacesNames, allModuleNames });

    def.members.forEach(mem => {
        if (!ismodule && isSymbolMember(mem)) {
            switch (mem.type) {
                case 'Iterator': {
                    dtsUnit.members.push(
                        dom.create.method(
                            `[Symbol.${mem.name}]`,
                            [],
                            dom.create.namedTypeReference(`${mem.type}<any>`)
                        )
                    )
                    return;
                }
                default: {
                    throw new Error(`unsupported symbom member '${mem.type}' (with name '${mem.name}') on ${unitCategory} '${unitName}'`)
                }
            }
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

        function getMapMemberTypeOptions() {
            return {
                memberInfo: mem, dtsUnitName: unitName,
                memberHostName: unitName, allInterfacesNames, allModuleNames,
                addRefToTripleSlashDirectivesHost: addRefToTripleSlashDirectivesHost,
            }
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
                let withOptionalParam = false;
                let withRestArgs = false;
                function getMethodParam(paramsHost) {
                    return (paramsHost.params || []).map(memParam => {
                        const paramDomInfo = mapParamTypeToDtsType(memParam.type, getMapParamOptions(memParam));
                        withRestArgs = !!paramDomInfo.isRestArgs;

                        let paramFlag = withRestArgs ? dom.ParameterFlags.Rest : dom.ParameterFlags.None;

                        if (!paramDomInfo.isRestArgs &&
                            (withOptionalParam || (!!memParam.default && !!memParam.default.value))
                        ) {
                            withOptionalParam = true;
                            paramFlag |= dom.ParameterFlags.Optional
                        }

                        return dom.create.parameter(
                            memParam.name,
                            paramDomInfo.type,
                            paramFlag
                        )
                    });
                }

                if (isInterfaceConstructor) {
                    dtsUnit.members.push(dtsUnitMember = dom.create.constructor(getMethodParam(mem), memFlags))
                } else {
                    if (mem.overs && mem.overs.length > 1) {
                        memberIsOver = true;
                        mem.overs.forEach(over => {
                            const { syncMethod, asyncMethod } = generateDtsMethod(
                                over,
                                getMethodParam(over),
                                mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                                { memFlags, withOptionalParam, withRestArgs }
                            )

                            syncMethod.jsDocComment = convertIDLCommentToJSDocComment(over.comments)
                            dtsUnit.members.push(syncMethod);

                            if (asyncMethod) {
                                dtsUnit.members.push(asyncMethod);
                            }
                        });
                    } else {
                        const { syncMethod, asyncMethod } = generateDtsMethod(
                            mem, getMethodParam(mem), mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                            { memFlags, withOptionalParam, withRestArgs }
                        )

                        dtsUnitMember = syncMethod;
                        dtsUnit.members.push(syncMethod);
                        dtsUnitMember.jsDocComment = convertIDLCommentToJSDocComment(mem.comments || '')

                        if (asyncMethod) {
                            dtsUnit.members.push(asyncMethod)
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
                // number indexSignature
                if (mem.name === '[]') {
                    dtsUnit.members.push(dtsUnitMember = dom.create.indexSignature(
                        def.declare.name,
                        mem.type === 'Integer' ? 'number' : 'string',
                        dom.type.any
                    ))
                } else if (mem.name === '[String]') { // string indexSignature
                    dtsUnit.members.push(dtsUnitMember = dom.create.indexSignature(
                        def.declare.name,
                        'string',
                        dom.type.any
                    ))
                }
                break
            };
            case 'const': {
                dtsUnit.members.push(dtsUnitMember = dom.create.const(
                    mem.name,
                    (() => {
                        if (mem.default && mem.default.value) {
                            return dom.type.numberLiteral(mem.default.value);
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

    dtsUnit.jsDocComment = convertIDLCommentToJSDocComment(def.declare.comments || '');

    // console.notice(`:--- try to emit dts for ${unitCategory}: ${unitName} ---->`)
    return dom.emit(dtsUnit, {
        tripleSlashDirectives: Object.values(tripleSlashDirectiveMap),
    });
}

/**
 * @param {import('../../idl/ir').IIDLDefinition} def
 * @param {*} configuration
 * @param {*} retValue
 */
function processDeclareModule(def, {
    unitName,
    unitCategory,
    allInterfacesNames,
    allModuleNames,
}, {
    dtsUnit,
    tripleSlashDirectiveMap,
}) {
    const addRefToTripleSlashDirectivesHost = getAddRefToTripleSlashDirectivesHost(tripleSlashDirectiveMap, { allInterfacesNames, allModuleNames });

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

        function getMapMemberTypeOptions(useRefInstance = true) {
            return {
                memberInfo: mem, dtsUnitName: unitName,
                memberHostName: unitName, allInterfacesNames, allModuleNames,
                addRefToTripleSlashDirectivesHost: addRefToTripleSlashDirectivesHost,
                useRefInstance,
            }
        }

        /**
         * @typedef {import('dts-dom').ModuleDeclaration} IDTSUnit
         * @type {IDTSUnit['members'][number]}
         */
        let dtsUnitMember;
        let memberIsOver = false;

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
                    return (paramsHost.params || []).map(param => {
                        const paramDomInfo = mapParamTypeToDtsType(param.type, getMapParamOptions(param));
                        withRestArgs = !!paramDomInfo.isRestArgs;

                        let paramFlag = withRestArgs ? dom.ParameterFlags.Rest : dom.ParameterFlags.None;

                        if (!paramDomInfo.isRestArgs &&
                            (withOptionalParam || (!!param.default && !!param.default.value))
                        ) {
                            withOptionalParam = true;
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
                        const { syncFunc, asyncFunc } = generateDtsFunction(
                            over,
                            getFunctionParams(over),
                            mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                            { funcFlags: memFlags, withOptionalParam, withRestArgs }
                        )

                        syncFunc.jsDocComment = convertIDLCommentToJSDocComment(over.comments)
                        dtsUnit.members.push(syncFunc);

                        if (asyncFunc) {
                            dtsUnit.members.push(asyncFunc);
                        }
                    });
                } else {
                    const { asyncFunc, syncFunc } = generateDtsFunction(
                        mem,
                        getFunctionParams(mem), mapMemMethodReturnTypeToDtsType(mem.type, getMapMemberTypeOptions()),
                        { withOptionalParam, withRestArgs }
                    );

                    dtsUnit.members.push(dtsUnitMember = syncFunc);

                    if (asyncFunc) {
                        dtsUnit.members.push(asyncFunc);
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
                            return dom.type.numberLiteral(mem.default.value);
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

    dtsUnit.jsDocComment = convertIDLCommentToJSDocComment(def.declare.comments || '');

    // console.notice(`:--- try to emit dts for ${unitCategory}: ${unitName} ---->`)
    return dom.emit(dtsUnit, {
        // rootFlags: dom.ContextFlags.Module,
        tripleSlashDirectives: Object.values(tripleSlashDirectiveMap),
    });
}

/**
 * @description generate *.d.ts for definitions
 * 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} defs 
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

    Object.values(defs)
        .filter(
            // ignore some special idl json from
            (def) => !EXCLUDE_INTERNALS.includes(def.declare.name)
        )
        .forEach((def) => {
            const ismodule = def.declare.module;
            const unitCategory = ismodule ? 'module' : 'interface';

            const basedir = path.resolve(DTS_DIST_DIR, `./${unitCategory}`);
            if (!fs.exists(basedir)) {
                fs.mkdir(basedir);
            }

            const unitName = def.declare.name;
            const dtsUnit = !ismodule ? dom.create.class(normalizeClazzName(unitName))
                : dom.create.module(`${unitName}`)
            const tripleSlashDirectiveMap = {};

            tripleSlashDirectiveMap['_fibjs.d.ts'] = dom.create.tripleSlashReferencePathDirective(`../_import/_fibjs.d.ts`)

            const processOptions = {
                unitName,
                unitCategory,
                ismodule,
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

            fs.writeTextFile(path.join(basedir, `${unitName}.d.ts`), unitDeclare);
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
    if (!fs.exists(basedir)) {
        fs.mkdir(basedir);
    }

    const topDeclarition = dom.create.namespace('FIBJS');

    const typedArrayType = dom.create.union([
        dom.create.namedTypeReference('Promise'),
        dom.create.namedTypeReference('Int8Array'),
        dom.create.namedTypeReference('Uint8Array'),
        dom.create.namedTypeReference('Int16Array'),
        dom.create.namedTypeReference('Uint16Array'),
        dom.create.namedTypeReference('Int32Array'),
        dom.create.namedTypeReference('Uint32Array'),
        dom.create.namedTypeReference('Uint8ClampedArray'),
        dom.create.namedTypeReference('Float32Array'),
        dom.create.namedTypeReference('Float64Array'),
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
    fs.writeTextFile(path.join(basedir, `_fibjs.d.ts`), commonDeclaration);
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
    if (!fs.exists(basedir)) {
        fs.mkdir(basedir);
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
    fs.writeTextFile(path.join(basedir, `bridge.d.ts`), bridgeDeclaration);
}

/**
 * @description generate *.d.ts for fibjs
 * 
 * @param {Record<string, import('../../idl/ir').IIDLDefinition>} defs 
 */
module.exports = function gen_dts(defs, { DTS_DIST_DIR }) {
    const {
        allModuleNames,
    } = gen_dts_for_declare(defs, { DTS_DIST_DIR });

    gen_fibjs_import_dts({ DTS_DIST_DIR });
    gen_bridge_dts({ allModuleNames, DTS_DIST_DIR });
}