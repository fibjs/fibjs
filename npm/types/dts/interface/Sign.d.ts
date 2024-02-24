/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
declare class Class_Sign extends Class_object {
    update(data: Class_Buffer): Class_Sign;

    update(data: string, codec?: string): Class_Sign;

    sign(privateKey: Class_Buffer, encoding?: string): any;

    sign(privateKey: Class_KeyObject, encoding?: string): any;

    sign(key: FIBJS.GeneralObject, encoding?: string): any;

}

