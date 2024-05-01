/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
declare class Class_RTCDataChannel extends Class_EventEmitter {
    send(data: Class_Buffer): void;

    send(data: string): void;

    close(): void;

    readonly id: number;

    readonly label: string;

    readonly protocol: string;

    readonly bufferedAmount: number;

    onopen: (...args: any[])=>any;

    onmessage: (...args: any[])=>any;

    onclose: (...args: any[])=>any;

    onerror: (...args: any[])=>any;

    onbufferedamountlow: (...args: any[])=>any;

}

