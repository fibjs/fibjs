/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
declare class Class_Verify extends Class_object {
    update(data: Class_Buffer): Class_Verify;

    update(data: string, codec?: string): Class_Verify;

    verify(privateKey: Class_Buffer, signature: Class_Buffer): boolean;

    verify(privateKey: Class_KeyObject, signature: Class_Buffer): boolean;

    verify(key: FIBJS.GeneralObject, signature: Class_Buffer): boolean;

    verify(privateKey: Class_Buffer, signature: string, encoding?: string): boolean;

    verify(privateKey: Class_KeyObject, signature: string, encoding?: string): boolean;

    verify(key: FIBJS.GeneralObject, signature: string, encoding?: string): boolean;

}

