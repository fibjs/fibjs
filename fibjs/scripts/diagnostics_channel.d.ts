/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `diagnostics_channel` module (no IDL unit:
// fibjs/scripts/diagnostics_channel.js).

declare module 'diagnostics_channel' {
    /** A subscriber of a named channel. */
    type ChannelSubscriber = (message: any, name: string) => void;

    /**
     * @description A named publish/subscribe channel
     *
     *      `dc.channel(name)` returns the singleton of that name; a channel
     *      exists whether or not anyone subscribes, so `hasSubscribers` is the
     *      cheap guard before building a message.
     */
    class Channel {
        constructor(name: string);

        /** The channel name. */
        readonly name: string;

        /** Whether anything is subscribed. */
        readonly hasSubscribers: boolean;

        /** Subscribes a listener; returns a function that unsubscribes it. */
        subscribe(listener: ChannelSubscriber): () => void;

        /** Removes a listener. */
        unsubscribe(listener: ChannelSubscriber): boolean;

        /** Publishes a message to the listeners. */
        publish(message: any): void;
    }

    /**
     * @description A group of channels following one operation through its phases
     *
     *      `dc.tracingChannel(name)` exposes the six channels Node defines
     *      (`<name>:start`, `:end`, `:asyncStart`, `:asyncEnd`, `:error`) plus
     *      the traceSync/tracePromise/traceCallback wrappers that publish to
     *      them around a call.
     */
    class TracingChannel {
        constructor(name: string);

        /** Whether anything is subscribed to any of the group's channels. */
        readonly hasSubscribers: boolean;

        /** Subscribes the same handlers to every channel of the group. */
        subscribe(handlers: FIBJS.GeneralObject): void;

        /** Removes handlers from every channel of the group. */
        unsubscribe(handlers: FIBJS.GeneralObject): void;

        /** Runs `fn` inside the trace channels and returns its result. */
        traceSync(fn: (...args: any[]) => any, context?: FIBJS.GeneralObject, thisArg?: any, ...args: any[]): any;

        /** Races the traced call against the Promise it returns. */
        tracePromise(fn: (...args: any[]) => Promise<any>, context?: FIBJS.GeneralObject, thisArg?: any, ...args: any[]): Promise<any>;

        /** Runs a callback-style call inside the trace channels. */
        traceCallback(fn: (...args: any[]) => any, position?: number, context?: FIBJS.GeneralObject, thisArg?: any, ...args: any[]): any;
    }

    /** @description Returns the singleton channel of the given name */
    function channel(name: string): Channel;

    /** @description Whether the named channel has subscribers */
    function hasSubscribers(name: string): boolean;

    /** @description Subscribes to the named channel */
    function subscribe(name: string, listener: ChannelSubscriber): () => void;

    /** @description Removes a listener from the named channel */
    function unsubscribe(name: string, listener: ChannelSubscriber): boolean;

    /** @description Returns the singleton tracing channel group of the given name */
    function tracingChannel(name: string): TracingChannel;

    export {
        Channel,
        ChannelSubscriber,
        TracingChannel,
        channel,
        hasSubscribers,
        subscribe,
        unsubscribe,
        tracingChannel,
    };
}
