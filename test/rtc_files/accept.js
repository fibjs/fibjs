var rtc = require("rtc");

const key_pem =
    `-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg3bbuT2SjSlMZH/J1
vHwmF0Blb/DBc/v7f1Za9GPUXHmhRANCAATDpmYxZozjVw6xlERNjJJGgfY3bEmj
xAKFRq3nbxbDHvMEs34u9HntMZWJ0hp3GUC+Ax7JHTv3cYqSaAg2SpR4
-----END PRIVATE KEY-----`

const cert_pem =
    `-----BEGIN CERTIFICATE-----
MIIBgjCCASigAwIBAgIJAPMXEoZXOaDEMAoGCCqGSM49BAMCMEoxDzANBgNVBAMM
BmNhLmNvbTELMAkGA1UEBhMCVVMxCzAJBgNVBAcMAkNBMRAwDgYDVQQKDAdleGFt
cGxlMQswCQYDVQQIDAJDQTAeFw0yNDA1MDUxNjAzMjFaFw0yNDA4MTMxNjAzMjFa
MDExCzAJBgNVBAYTAkNOMRAwDgYDVQQKDAdiYW96LmNuMRAwDgYDVQQDDAdiYW96
Lm1lMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEw6ZmMWaM41cOsZRETYySRoH2
N2xJo8QChUat528Wwx7zBLN+LvR57TGVidIadxlAvgMeyR0793GKkmgINkqUeKMQ
MA4wDAYDVR0TAQH/BAIwADAKBggqhkjOPQQDAgNIADBFAiAPNldqGJHryfjPFyX3
zfHHWlO7xSDTzdyoxzroFdwy+gIhAKmZizEVvDlBiIe+3ptCArU3dbp+bzLynTcr
Ma9ayzQy
-----END CERTIFICATE-----`

rtc.listen(60917, function (binding) {
    rtc.stopListen(60917);

    var { local_ufrag, remote_ufrag, address, port } = binding;
    var family = address.indexOf(':') >= 0 ? 'IP6' : 'IP4';

    peer1 = new rtc.RTCPeerConnection({
        enableIceUdpMux: true,
        disableFingerprintVerification: true,
        iceUfrag: remote_ufrag,
        icePwd: remote_ufrag,
        certPem: cert_pem,
        keyPem: key_pem,
        maxMessageSize: 16384
    });

    const sdp = `v=0\r\no=- 0 0 IN ${family} ${address}\r\ns=-\r\nc=IN ${family} ${address}\r\nt=0 0\r\na=ice-lite\r\nm=application ${port} UDP/DTLS/SCTP webrtc-datachannel\r\na=mid:0\r\na=setup:active\r\na=ice-ufrag:${remote_ufrag}\r\na=ice-pwd:${remote_ufrag}\r\na=fingerprint:SHA-256 00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\r\na=sctp-port:5000\r\na=max-message-size:16384\r\na=candidate:1467250027 1 UDP 1467250027 ${address} ${port} typ host\r\n`;

    peer1.setRemoteDescription({
        "type": "offer",
        "sdp": sdp
    });

    peer1.ondatachannel = function (channel) {
        console.log("ondatachannel");

        setTimeout(function () {
            peer1.close();
            console.log("closed");
        }, 100);
    };
});
