// Minimal DNS message builder and UDP server for the dns tests.
//
// The tests never touch a real DNS server: a local server answers the exact
// records each case needs, so the suite is deterministic and network free.

const dgram = require('dgram');
const net = require('net');

const T = {
    A: 1,
    NS: 2,
    CNAME: 5,
    SOA: 6,
    PTR: 12,
    MX: 15,
    TXT: 16,
    AAAA: 28,
    SRV: 33,
    NAPTR: 35,
    TLSA: 52,
    ANY: 255,
    CAA: 257
};

function encodeName(name) {
    if (!name || name === '.')
        return Buffer.from([0]);

    const parts = name.replace(/\.$/, '').split('.');
    const chunks = [];

    for (const part of parts) {
        const b = Buffer.from(part, 'utf8');
        chunks.push(Buffer.from([b.length]), b);
    }

    chunks.push(Buffer.from([0]));
    return Buffer.concat(chunks);
}

function decodeName(buf, offset) {
    const labels = [];
    let jumped = false;
    let end = offset;
    let pos = offset;

    while (true) {
        const len = buf[pos];

        if (len === 0) {
            pos++;
            if (!jumped)
                end = pos;
            break;
        }

        if ((len & 0xc0) === 0xc0) {
            const ptr = ((len & 0x3f) << 8) | buf[pos + 1];
            pos += 2;
            if (!jumped)
                end = pos;
            pos = ptr;
            jumped = true;
            continue;
        }

        labels.push(buf.slice(pos + 1, pos + 1 + len).toString());
        pos += 1 + len;
    }

    return { name: labels.join('.'), end };
}

function parseQuery(buf) {
    const id = buf.readUInt16BE(0);
    let off = 12;
    const q = decodeName(buf, off);
    off = q.end;

    return {
        id,
        name: q.name,
        type: buf.readUInt16BE(off),
        class: buf.readUInt16BE(off + 2)
    };
}

function u16(v) {
    const b = Buffer.alloc(2);
    b.writeUInt16BE(v & 0xffff);
    return b;
}

function u32(v) {
    const b = Buffer.alloc(4);
    b.writeUInt32BE(v >>> 0);
    return b;
}

function record(name, type, ttl, rdata) {
    return Buffer.concat([
        encodeName(name),
        u16(type),
        u16(1),
        u32(ttl),
        u16(rdata.length),
        rdata
    ]);
}

function aRecord(name, address, ttl = 60) {
    const parts = address.split('.').map((n) => parseInt(n, 10));
    return record(name, T.A, ttl, Buffer.from(parts));
}

function aaaaRecord(name, address, ttl = 60) {
    const b = Buffer.alloc(16);
    const full = expandIPv6(address);

    for (let i = 0; i < 8; i++)
        b.writeUInt16BE(parseInt(full[i], 16), i * 2);

    return record(name, T.AAAA, ttl, b);
}

function expandIPv6(address) {
    if (address === '::')
        return ['0', '0', '0', '0', '0', '0', '0', '0'];

    const parts = address.split('::');
    const left = parts[0] ? parts[0].split(':') : [];
    const right = parts.length > 1 && parts[1] ? parts[1].split(':') : [];
    const fill = 8 - left.length - right.length;

    return left.concat(Array(fill).fill('0'), right);
}

function cnameRecord(name, target, ttl = 60) {
    return record(name, T.CNAME, ttl, encodeName(target));
}

function nsRecord(name, target, ttl = 60) {
    return record(name, T.NS, ttl, encodeName(target));
}

function ptrRecord(name, target, ttl = 60) {
    return record(name, T.PTR, ttl, encodeName(target));
}

function mxRecord(name, exchange, priority, ttl = 60) {
    return record(name, T.MX, ttl,
        Buffer.concat([u16(priority), encodeName(exchange)]));
}

function txtRecord(name, chunks, ttl = 60) {
    const parts = [];

    for (const chunk of chunks) {
        const b = Buffer.from(chunk, 'utf8');
        parts.push(Buffer.from([b.length]), b);
    }

    return record(name, T.TXT, ttl, Buffer.concat(parts));
}

function srvRecord(name, target, port, priority, weight, ttl = 60) {
    return record(name, T.SRV, ttl,
        Buffer.concat([u16(priority), u16(weight), u16(port),
            encodeName(target)]));
}

function naptrRecord(name, order, preference, flags, service, regexp,
    replacement, ttl = 60) {
    const str = (s) => {
        const b = Buffer.from(s, 'utf8');
        return Buffer.concat([Buffer.from([b.length]), b]);
    };

    return record(name, T.NAPTR, ttl,
        Buffer.concat([u16(order), u16(preference), str(flags), str(service),
            str(regexp), encodeName(replacement)]));
}

function caaRecord(name, critical, tag, value, ttl = 60) {
    const t = Buffer.from(tag, 'utf8');
    const v = Buffer.from(value, 'utf8');

    return record(name, T.CAA, ttl,
        Buffer.concat([Buffer.from([critical, t.length]), t, v]));
}

function tlsaRecord(name, certUsage, selector, match, data, ttl = 60) {
    return record(name, T.TLSA, ttl,
        Buffer.concat([Buffer.from([certUsage, selector, match]), data]));
}

function soaRecord(name, mname, rname, serial, refresh, retry, expire, minttl,
    ttl = 60) {
    return record(name, T.SOA, ttl,
        Buffer.concat([encodeName(mname), encodeName(rname), u32(serial),
            u32(refresh), u32(retry), u32(expire), u32(minttl)]));
}

function buildResponse(query, options) {
    const rcode = options.rcode || 0;
    const flags = 0x8180 | rcode;
    const answers = options.answers || [];
    const ancount = options.ancount === undefined
        ? answers.length
        : options.ancount;
    const buf = Buffer.concat([
        u16(query.id),
        u16(flags),
        u16(1),
        u16(ancount),
        u16(0),
        u16(0),
        encodeName(query.name),
        u16(query.type),
        u16(1),
        Buffer.concat(answers)
    ]);

    return buf;
}

function startDnsServer(handler) {
    const server = dgram.createSocket('udp4');
    const state = { queries: [] };

    server.on('message', (msg, rinfo) => {
        const query = parseQuery(msg);
        state.queries.push(query);

        const result = handler(query);

        if (!result || result.drop)
            return;

        const response = buildResponse(query, result);
        server.send(response, rinfo.port, rinfo.address);
    });

    server.bind(0, '127.0.0.1');

    return {
        server,
        state,
        get port() {
            return server.address().port;
        },
        close() {
            try {
                server.close();
            } catch (e) {
            }
        }
    };
}

module.exports = {
    A: T.A,
    NS: T.NS,
    CNAME: T.CNAME,
    SOA: T.SOA,
    PTR: T.PTR,
    MX: T.MX,
    TXT: T.TXT,
    AAAA: T.AAAA,
    SRV: T.SRV,
    NAPTR: T.NAPTR,
    TLSA: T.TLSA,
    ANY: T.ANY,
    CAA: T.CAA,
    T,
    encodeName,
    parseQuery,
    aRecord,
    aaaaRecord,
    cnameRecord,
    nsRecord,
    ptrRecord,
    mxRecord,
    txtRecord,
    srvRecord,
    naptrRecord,
    caaRecord,
    tlsaRecord,
    soaRecord,
    startDnsServer
};
