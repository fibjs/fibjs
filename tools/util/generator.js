/**
 * @description generate normalized .idl from defs[_[i18n]].json
 */

const fs = require('fs')
const path = require('path')

const ejs = require('ejs');

var { translate } = require('../translate');

const IDL_LANG = process.env.FIBJS_IDL_LANG || 'us-en';
const LOG_PREFIX = `[generator]`;

const _formatParamDefaultValue = (param, member) => {
    if (param.default.value) {
        switch (param.default.value) {
            case 'v8::Undefined(isolate->m_isolate)':
                return 'undefined'
            case 'v8::Object::New(isolate->m_isolate)':
                return '{}'
            case 'v8::Array::New(isolate->m_isolate)':
                return '[]'
            case 'NULL':
                return 'null'
        }

        return param.default.value;
    } else if (param.default.const) {
        if (Array.isArray(param.default.const))
            return param.default.const.join('.')
        else if (typeof param.default.const === 'string')
            return param.default.const
    }

    throw new Error(`no default value for method '${member.name}' param '${param.name}'!`)
}

const _formatMethodReturnType = (member) => {
    if (Array.isArray(member.type)) {
        return `(${member.type
            .map(rt => `${rt.type} ${rt.name}`)
            .join(', ')
            }) `
    }
    return member.type ? `${member.type}${member.isarray ? "[]" : ""} ` : ''
}

const _formatParamTypeName = (param) => {
    if (param.type === '...')
        return `...${param.name}`
    return `${param.type} ${param.name}${param.isarray?"[]":""}`
}

const _formatConstructorObject = (member) => {
    return ` new ${member.name}()`;
}

const ejs_tpl_module = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/generator_module.idl.ejs'), "utf8"));

const ejs_tpl_module_member_const = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/module_member_const.idl.ejs'), "utf8"));
const ejs_tpl_module_member_prop = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/module_member_prop.idl.ejs'), "utf8"));
const ejs_tpl_module_member_object = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/module_member_object.idl.ejs'), "utf8"));
const ejs_tpl_module_member_method = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/module_member_method.idl.ejs'), "utf8"));

function normalizeIDLTextFromModuleDef(mdef, idlLang = IDL_LANG) {
    const _translate = (input) => {
        if (idlLang === 'zh-CN') return input;

        return translate(input, {
            format: 'text',
            from: 'zh-CN',
            to: idlLang
        });
    };

    return ejs_tpl_module({
        declare: mdef.declare,
        members: mdef.members || [],
        render_member_block: (member_def, mem_idx) => {
            const ctx = {
                module: mdef,
                member: member_def,
                _translate
            };

            const lineEOL = mem_idx < mdef.members.length - 1 ? '\n' : '';
            switch (member_def.memType) {
                case 'const':
                    return ejs_tpl_module_member_const({
                        ...ctx
                    }) + lineEOL
                case 'prop':
                    return ejs_tpl_module_member_prop({
                        ...ctx
                    }) + lineEOL
                case 'object':
                    return ejs_tpl_module_member_object({
                        ...ctx,
                        _formatConstructorObject
                    }) + lineEOL
                case 'method':
                    return ejs_tpl_module_member_method({
                        ...ctx,
                        _formatParamDefaultValue,
                        _formatParamTypeName,
                        _formatMethodReturnType,
                    }) + lineEOL
            }

            return 'Unknown'
        },
        _translate,
    });
}

const ejs_tpl_interface = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/generator_interface.idl.ejs'), "utf8"));

const ejs_tpl_interface_member_const = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/interface_member_const.idl.ejs'), "utf8"));
const ejs_tpl_interface_member_prop = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/interface_member_prop.idl.ejs'), "utf8"));
const ejs_tpl_interface_member_object = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/interface_member_object.idl.ejs'), "utf8"));
const ejs_tpl_interface_member_method = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/interface_member_method.idl.ejs'), "utf8"));
const ejs_tpl_interface_member_operator = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/interface_member_operator.idl.ejs'), "utf8"));
const ejs_tpl_interface_member_event = ejs.compile(fs.readFileSync(path.resolve(__dirname, './tmpl/interface_member_event.idl.ejs'), "utf8"));

function normalizeIDLTextFromInterfaceDef(mdef, idlLang = IDL_LANG) {
    const _translate = (input) => {
        if (idlLang === 'zh-CN') return input;

        return translate(input, {
            format: 'text',
            from: 'zh-CN',
            to: idlLang
        });
    };

    return ejs_tpl_interface({
        declare: mdef.declare,
        members: mdef.members || [],
        render_member_block: (member_def, mem_idx) => {
            const ctx = {
                module: mdef,
                member: member_def,
                _translate
            };

            const lineEOL = mem_idx < mdef.members.length - 1 ? '\n' : '';
            switch (member_def.memType) {
                case 'const':
                    return ejs_tpl_interface_member_const({
                        ...ctx
                    }) + lineEOL
                case 'prop':
                    return ejs_tpl_interface_member_prop({
                        ...ctx
                    }) + lineEOL
                case 'object':
                    return ejs_tpl_interface_member_object({
                        ...ctx
                    }) + lineEOL
                case 'method':
                    return ejs_tpl_interface_member_method({
                        ...ctx,
                        _formatParamDefaultValue,
                        _formatParamTypeName,
                        _formatMethodReturnType,
                    }) + lineEOL
                case 'event':
                    return ejs_tpl_interface_member_event({
                        ...ctx,
                        _formatParamTypeName,
                        _formatMethodReturnType,
                    }) + lineEOL
                case 'operator':
                    return ejs_tpl_interface_member_operator({
                        ...ctx,
                    }) + lineEOL
            }

            return 'Unknown'
        },
        _translate,
    });
}

module.exports = (
    defs,
    idlLang = IDL_LANG,
    langDirname = idlLang
) => {
    const totalDefs = Object.keys(defs).length;
    console.log(`   📊 Generating IDL files for ${totalDefs} definitions...`);
    
    // dump defs as json;
    var snapshotsDir = path.resolve(__dirname, '../../idl/__snapshots__');
    try { fs.mkdirSync(snapshotsDir) } catch (error) { };

    fs.writeFileSync(
        path.resolve(snapshotsDir, `./defs_${idlLang}.json`),
        JSON.stringify(defs, null, '  ')
    )
    console.log(`   💾 Saved definitions snapshot to ${path.basename(snapshotsDir)}`);

    let processedCount = 0;
    for (const [dname, def] of Object.entries(defs)) {
        // TODO: deal with 'interface'

        const targetDir = path.resolve(__dirname, `../../idl/${langDirname}`)
        if (!fs.existsSync(targetDir)) {
            try {
                fs.mkdirSync(targetDir)
            } catch (error) { }
        };

        const name = def.declare.name;

        switch (def.declare.type) {
            case 'module':
                fs.writeFileSync(
                    path.resolve(targetDir, `./${name}.idl`),
                    normalizeIDLTextFromModuleDef(def, idlLang)
                )
                break
            case 'interface':
                fs.writeFileSync(
                    path.resolve(targetDir, `./${name}.idl`),
                    normalizeIDLTextFromInterfaceDef(def, idlLang)
                )
                break
        }
        
        processedCount++;
    }
    
    console.log(`   ✅ Generated ${processedCount} IDL files`);
}