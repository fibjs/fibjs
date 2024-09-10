var test = require("test");
test.setup();

var test_util = require('./test_util');
const child_process = require('child_process');
const coroutine = require('coroutine');
const path = require('path');
const io = require('io');
const rtc = require("rtc");

const cmd = process.execPath;

function munge(desc, ufrag) {
    if (desc.sdp === undefined) {
        throw invalidArgument("Can't munge a missing SDP");
    }
    desc.sdp = desc.sdp
        .replace(/\na=ice-ufrag:[^\n]*\n/, '\na=ice-ufrag:' + ufrag + '\n')
        .replace(/\na=ice-pwd:[^\n]*\n/, '\na=ice-pwd:' + ufrag + '\n');
    return desc;
}

function answerSdp(ip, port, ufrag) {
    const family = ip.indexOf(':') >= 0 ? 'IPv6' : 'IPv4';
    const certhash = '07E56F2A1A0C2C320EC1C39C345A784EA58B3205D157D6F4E7024112E601C68F';
    const fingerprint = certhash.match(/.{1,2}/g).join(':').toUpperCase();
    return {
        "type": "answer",
        "sdp": `v=0\r\no=- 0 0 IN ${family} ${ip}\r\ns=-\r\nc=IN ${family} ${ip}\r\nt=0 0\r\na=ice-lite\r\nm=application ${port} UDP/DTLS/SCTP webrtc-datachannel\r\na=mid:0\r\na=setup:passive\r\na=ice-ufrag:${ufrag}\r\na=ice-pwd:${ufrag}\r\na=fingerprint:SHA-256 ${fingerprint}\r\na=sctp-port:5000\r\na=max-message-size:16384\r\na=candidate:1467250027 1 UDP 1467250027 ${ip} ${port} typ host\r\n`
    };
}

const charset = Array.from('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/');
const genUfrag = (len) => [...Array(len)].map(() => charset.at(Math.floor(Math.random() * charset.length))).join('');

function connect(ip, port, ufrag) {
    const peerConnection = new rtc.RTCPeerConnection({
        iceUfrag: ufrag,
        icePwd: ufrag,
        maxMessageSize: 16384
    });

    const handshakeDataChannel = peerConnection.createDataChannel('temp');

    handshakeDataChannel.onopen = (_) => {
        // peerConnection.close();
    };

    peerConnection.createOffer().then(offerSdp => {
        const mungedOfferSdp = munge(offerSdp, ufrag);
        peerConnection.setLocalDescription(mungedOfferSdp);

        peerConnection.setRemoteDescription(answerSdp(ip, port, ufrag));
    }).catch(e => {
        console.error(e);
    });

    return peerConnection;
}

describe('rtc', function () {
    it('listen will hold the process', function () {
        var p = child_process.spawn(cmd, [path.join(__dirname, 'rtc_files', 'listen.js')]);
        var stdout = new io.BufferedStream(p.stdout);
        assert.deepEqual(stdout.readLines(), ["0", "1", "2", "3"]);
    });

    it('binding information', function () {
        var p = child_process.spawn(cmd, [path.join(__dirname, 'rtc_files', 'accept_info.js')]);
        const ufrag = 'libp2p+webrtc+v1/' + genUfrag(16);
        const peerConnection = connect('127.0.0.1', 3223, ufrag);

        var stdout = new io.BufferedStream(p.stdout);
        const binding = JSON.parse(stdout.readLine());

        assert.equal(binding.local_ufrag, ufrag);
        assert.equal(binding.remote_ufrag, ufrag);

        peerConnection.close();
    });

    it('accept', function () {
        var p = child_process.spawn(cmd, [path.join(__dirname, 'rtc_files', 'accept.js')]);
        const ufrag = 'libp2p+webrtc+v1/' + genUfrag(16);
        const peerConnection = connect('127.0.0.1', 3224, ufrag);

        var stdout = new io.BufferedStream(p.stdout);
        assert.deepEqual(stdout.readLines(), [
            "ondatachannel",
            "closed"
        ]);

        peerConnection.close();
    });

    it('datachannel', function () {
        var p = child_process.spawn(cmd, [path.join(__dirname, 'rtc_files', 'datachannel.js')], {
            stdio: "inherit"
        });

        const ufrag = 'libp2p+webrtc+v1/' + genUfrag(16);
        const peerConnection = connect('127.0.0.1', 3225, ufrag);

        for (var i = 0; i < 10; i++) {
            var msg = '';

            console.log(`channel_${i}`);
            var test_channel = peerConnection.createDataChannel(`channel_${i}`);

            test_channel.onmessage = (ev) => {
                console.log('client accept message:', ev.data);
                msg = ev.data;
            };

            test_channel.onopen = (_) => {
                console.log('client onopen');
                test_channel.send('hello');
            };

            for (var j = 0; j < 100 && msg !== 'hello'; j++)
                coroutine.sleep(100);

            assert.equal(msg, 'hello');
        }

        p.kill(0);
        peerConnection.close();
    });

    it('connect timeout', function () {
        const ufrag = 'libp2p+webrtc+v1/' + genUfrag(16);
        const peerConnection = connect('127.0.0.1', 60919, ufrag);

        while (peerConnection.connectionState !== 'closed') {
            console.log(peerConnection.connectionState);
            coroutine.sleep(3000);
        }
    });
});

require.main === module && test.run(console.DEBUG);
