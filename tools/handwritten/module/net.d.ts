// Hand-written addition to the generated `net` module declarations
// (tools/handwritten/module/README.md explains the mechanism). It is merged
// into the same file, so this is a second `declare module 'net'` block.

declare module 'net' {
    /**
     * @description The address a server or socket is bound to
     *
     *      Node.js returns this shape from `server.address()` / `socket.address()`;
     *      fibjs returns the same three fields, so the interface is the shared type of
     *      both. Node types it `AddressInfo` (`net.AddressInfo`), which is what
     *      `import type { AddressInfo } from 'node:net'` resolves to.
     *
     *      The `family` is the string form (`'IPv4'` / `'IPv6'`); Node's newest
     *      definitions also allow the number `4` / `6` and `'IPv4'`/`'IPv6'`, so both
     *      are accepted here. For a unix domain socket `address` is the path and the
     *      other two fields are placeholders.
     */
    interface AddressInfo {
        address: string;
        family: string;
        port: number;
    }
}
