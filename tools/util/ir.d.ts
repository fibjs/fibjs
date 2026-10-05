declare namespace FIBJS_IDL {
    /**
     * @description the IDL data types of fibjs; the type of every parameter, member and variable is one of the following
     */
    export type IIDLDataType =
        null
        | 'Value'
        | 'Array'
        | 'ArrayBuffer'
        | 'TypedArray'
        | 'ArrayBufferView'
        | 'Buffer'
        | 'String'
        | 'Boolean'
        | 'Integer'
        | 'Long'
        | 'Number'

    export type IIDLDataTuple = IIDLParam[]

    /**
    * @description the inline callback shape of a `Function(...)` parameter or
    * return type, or of the `Function(...)` alternative of a parameter union;
    * typing-only, the runtime and the C++ side still see one `Function` value
    */
    export interface IIDLCallbackShape {
        /**
         * @description the callback parameters declared between the parentheses
         */
        params: IIDLParam[]
        /**
         * @description the callback return type as written in the IDL (a type
         * name, or the struct item list when it is a `(...)` struct); null when
         * no `=> Type` was declared, which renders as `void`. A nested callback
         * shape in the return position keeps its type name only, it is not kept
         * recursively.
         */
        ret: IIDLDataType | IIDLDataTuple | null
    }

    /**
    * @description parameter information in the IDL, describing constructors and member functions
    */
    export interface IIDLParam {
        /**
         * @description the parameter type; `A|B` is a parameter-position union
         * whose alternatives are the runtime conversion's preference order
         * (the C++ side receives one `std::variant`, see plans/idl-union-types-2026-10-02.md)
         */
        type: "Array" | string,
        /**
         * @description the parameter name
         */
        name: string,
        /**
         * @description the default value; null means there is no default value
         */
        default: {
            value: string
        } | null
        /**
         * @description the inline callback shape when the parameter was declared
         * as `Function(...)`, or when the `Function` alternative of a union
         * carries one; absent for a bare `Function` or any other type
         */
        callback?: IIDLCallbackShape
    }

    export interface ISimpleParsedDoc {
        descript: string
        detail: string[]
    }

    /**
    * @description the documentation information parsed from the comment area
    * 
    * the first line of the brief area is parsed into descript, the rest is parsed line by line into the detail object
    * 
    * params holds the parameter information parsed from the params areas
    */
    export interface IParsedDoc {
        descript: string
        detail?: string[]
        /**
         * @description the documentation information parsed from the parameter
         */
        params: {
            name: IIDLParam['name']
            descript?: IParsedDoc['descript']
            detail?: IParsedDoc['detail']
        }[]
    }

    export type IDeclareType = "interface" | "module"

    /**
    * @description the declaration of an IDL object
    */
    export interface IDeclare<TType extends IDeclareType> {
        /**
         * @description whether it is a module
         */
        module: TType extends 'module' ? true : false | undefined
        /**
         * @description the comment part
         */
        comments: string

        /**
         * @description the type of the declared object
         * 
         * @value interface - a built-in object
         * @value module - a module
         */
        type: TType

        /**
         * @description the name of the module or built-in object
         */
        name: string

        /**
         * @description the parent type of the declared object type
         */
        extend: string

        doc?: IParsedDoc
    }

    /**
    * @description
    */
    export interface IMember {
        /**
         * @description the member name
         *
         * The call operator (`operator(...)` in the IDL, idl-def.pegjs) keeps
         * the literal name `operator`: gen_code turns it into the C++
         * `_function` call stub, gen_dts into the callable-module /
         * call-signature declarations.
         */
        name: string
        /**
         * @description the member type
         * 
         * @value method a member method
         * @value method a member object
         */
        memType: "method" | "object" | "prop" | "operator" | "const"
        /**
         * @description the comment information
         */
        comments: string
        /**
         * @description whether it is deprecated; true means deprecated
         */
        deprecated: true | null
        /**
         * @description truthy means a constant
         */
        const: true | null | 'const'
        /**
         * @description true only; means a static member
         */
        static: null
        /**
         * @description true only; means a method that supports asynchronous calls
         */
        async: null
        /**
         * @description truthy means a symbol member
         */
        symbol: "@"
        /**
         * @description truthy means a read-only property
         */
        readonly: null | 'readonly' | true
        /**
         * @description the member type
         * 
         * when memType is 'method', this is the return type
         */
        type?: IIDLDataType | IIDLDataTuple
        /**
         * @description the inline callback shape when the method returns a
         * `Function(...)`; typing-only, see IIDLParam['callback']
         */
        callback?: IIDLCallbackShape
        /**
         * @description the default value; for a member whose memType is 'const' it is the enum constant.
         */
        default?: {
            value: string
        }
        params: IIDLParam[]
        doc: IParsedDoc

        /**
         * @description member overloads; meaningful for members whose memType is 'method'
         */
        overs?: {
            memType: IMember['memType']
            comments: IMember['comments']
            deprecated: IMember['deprecated']
            static: IMember['static']
            async: IMember['async']
            symbol: IMember['symbol']
            name: IMember['name']
            type: IMember['type']
            params: IMember['params']
            doc: IMember['doc']
        }[]
    }

    /**
    * @description the base of a module or an object in fibjs
    */
    export interface IIDLDefinition<T extends IDeclareType> {
        /**
         * @description the basic description of the module
         */
        declare: IDeclare<T>
        /**
         * @description the members, including properties, static properties, functions, etc.
         */
        members: IMember[]
        /**
         * @description the collection it belongs to
         */
        collect: string
    }

    /**
    * @description a built-in object in fibjs, such as Buffer or MySQL
    */
    export type IClazz = IIDLDefinition<"interface">

    /**
    * @description a static module in fibjs
    */
    export type IModule = IIDLDefinition<"module">
}

export = FIBJS_IDL;