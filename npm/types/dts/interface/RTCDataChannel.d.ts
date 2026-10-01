/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description the RTCDataChannel interface defines a bidirectional data channel
 */
declare class Class_RTCDataChannel extends Class_EventEmitter {
    /**
     * @description sends binary data; this method is used to send data to the remote end
     *
     *      @param data the binary data to send
     *
     */
    send(data: Class_Buffer): void;

    /**
     * @description sends text data; this method is used to send data to the remote end
     *
     *      @param data the text data to send
     *
     */
    send(data: string): void;

    /**
     * @description closes the channel; this method is used to close the channel
     */
    close(): void;

    /**
     * @description returns the ID number that uniquely identifies the RTCDataChannel
     */
    readonly id: number;

    /**
     * @description returns a string containing the name describing the data channel
     */
    readonly label: string;

    /**
     * @description returns a string containing the name of the sub-protocol in use
     */
    readonly protocol: string;

    /**
     * @description returns the number of bytes of data currently queued to be sent over the data channel
     */
    readonly bufferedAmount: number;

    /**
     * @description channel open event, emitted when the channel is opened
     */
    on(event: "open", listener: ()=>void): this;

    /**
     * @description channel message event, emitted when a message is received
     */
    on(event: "message", listener: ()=>void): this;

    /**
     * @description channel close event, emitted when the channel is closed
     */
    on(event: "close", listener: ()=>void): this;

    /**
     * @description channel error event, emitted when an error occurs on the channel
     */
    on(event: "error", listener: ()=>void): this;

    /**
     * @description channel buffered amount low event, emitted when the channel buffered amount is low
     */
    on(event: "bufferedamountlow", listener: ()=>void): this;

}

