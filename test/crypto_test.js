var test = require("test");
test.setup();

var crypto = require("crypto");
var hash = require("hash");
var fs = require("fs");
var os = require("os");
var encoding = require("encoding");
var hex = require("hex");

var rsa1024_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
    "MIICXQIBAAKBgQDSbmW8qlarL0lLu1XYcg+ocJgcuq5K7EgLcXyy2shAsko7etmZ\n" +
    "NP3opOeGw58E7tjVsjuadPQ8Hf+9wd316RYwShklDDsy4Hwp4z9afnj56UmvkM0u\n" +
    "TlsymlIbFftme6aJcYbnX9fdin78Rsa0MbzQbFdeLHsy5zKjsrbm4TS5bwIDAQAB\n" +
    "AoGAagN2O9NxMHL1MTMi75WfL9Pxvl+KWXKqZSF6mjzAsF9iKI8euyHIXYFepzU8\n" +
    "kual1RsjDhCnzvWqFvZplW8lXqrHf/P+rS/9Y4gBUw6pjnI/DnFIRwWHRvrUHHSC\n" +
    "fWOdTCIKdOTkgLZuGFuhEY3RMIW0WSYejjLtftwy0RVxAzkCQQDprgbWqZ/BaafV\n" +
    "uKKA3shUWWRst/2hV7qDus6YfEj6GfUZHEoNJW4BSuZHUiG4Cdxr0zTLtIP7tNSz\n" +
    "rCM7FbFrAkEA5ofkxFKdPBD0CQHMb9q13AMHUVe0rJ+hSjqqIBrmqApUOneyAcMV\n" +
    "76M0QyIQnI2p3POa4Qu/7XChDwRVl7LlDQJBANplxohsAh5fI/hQVriA/tQus/gU\n" +
    "QdzARFaHijzjs8Tj67mrQd5lhBl7KhuwPEloFfVEcUyNiuj9yeme0VKQZL8CQQCh\n" +
    "qCfm99vk1Cqc6lL3GRKwPrtx8iPzbVlIWU0ViGe47M1V1rvP+oK2cebjMM8fSUQV\n" +
    "egpgx8GF+pYmlq6C22M9AkB2oRCDE2Y5UWsXizvvqFOp4LqgQOKVrwS+lVvYXZm/\n" +
    "AA4uMJDro6IBkrUGJgYepTLQ16o2a2WwtK4ERlBm+pnC\n" +
    "-----END RSA PRIVATE KEY-----\n";

var pub_rsa1024_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDSbmW8qlarL0lLu1XYcg+ocJgc\n" +
    "uq5K7EgLcXyy2shAsko7etmZNP3opOeGw58E7tjVsjuadPQ8Hf+9wd316RYwShkl\n" +
    "DDsy4Hwp4z9afnj56UmvkM0uTlsymlIbFftme6aJcYbnX9fdin78Rsa0MbzQbFde\n" +
    "LHsy5zKjsrbm4TS5bwIDAQAB\n" +
    "-----END PUBLIC KEY-----\n";

var ec_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
    "MIHcAgEBBEIB+QhtQdd9bjWeN2mgq6qoqW51ygslLwP+gwTCSP4ZVpcU0pxwigXm\n" +
    "Ioa7Zzr2Q0OOjzdH/vDIaV9FzOySWSKTVJOgBwYFK4EEACOhgYkDgYYABAE3zTRb\n" +
    "rr5Rsa0xqF4Mn0gBoETTQ5zt6zd5O+LqyiXVUteEWIXZ0sJvdQZvhdZfk9VypQGN\n" +
    "skZ8xvcNVJL6PHNMCwEgR6q67gepHZBFAd75Vt/lCJJOB7SnVHk8Dfu5t49q4Gb2\n" +
    "EdX30QGqQ71qihogY6Jqlmn8aJAZGwyNHPeX+n+F1A==\n" +
    "-----END EC PRIVATE KEY-----\n";

var pub_ec_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBN800W66+UbGtMaheDJ9IAaBE00Oc\n" +
    "7es3eTvi6sol1VLXhFiF2dLCb3UGb4XWX5PVcqUBjbJGfMb3DVSS+jxzTAsBIEeq\n" +
    "uu4HqR2QRQHe+Vbf5QiSTge0p1R5PA37ubePauBm9hHV99EBqkO9aooaIGOiapZp\n" +
    "/GiQGRsMjRz3l/p/hdQ=\n" +
    "-----END PUBLIC KEY-----\n";

var ca1 = "-----BEGIN CERTIFICATE-----\n" +
    "MIIDcjCCAlqgAwIBAgIBETANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\n" +
    "MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n" +
    "MTIwNTEwMTMyMzQxWhcNMjIwNTExMTMyMzQxWjA6MQswCQYDVQQGEwJOTDERMA8G\n" +
    "A1UEChMIUG9sYXJTU0wxGDAWBgNVBAMTD3d3dy5leGFtcGxlLmNvbTCCASIwDQYJ\n" +
    "KoZIhvcNAQEBBQADggEPADCCAQoCggEBALk8SsXIo46QF6SeUqpxdSZhgOfHtW2M\n" +
    "/6q2QSa3vhGtXHMWDGQRSAT/1uE7BduJu7OXCdUcFN1ohzmwPXHL4nbQGtgYLYAb\n" +
    "VPblRJrxy69hLt9JDZ0Jt+2x/Tz9PPokz12/fORT5yW16kQi6SbT6iCUnuZhZ7ou\n" +
    "B2cLAy+iCe3wM48LzhDvZ6TGCNrB7cI/10rdFT35XhyBYEY+tbM9L6beRxy8kq7r\n" +
    "3ydrFla33OzRVVelbux1JfW3e9+r0jpakZh9lxcLEwqna0qLwUcw+zr4QQTVwd+4\n" +
    "Hb97AaVlouAeNremXMwwWvjNb83xGWIlygHjNX/6IPXc/WmyagB9F/cCAwEAAaOB\n" +
    "gTB/MAkGA1UdEwQCMAAwHQYDVR0OBBYEFH3knGvm+XF9RtISPa1rHf3CqnhMMB8G\n" +
    "A1UdIwQYMBaAFLRa5KWz3tJS9rnVppUP6z68x/3/MDIGA1UdEQQrMCmCC2V4YW1w\n" +
    "bGUuY29tggtleGFtcGxlLm5ldIINKi5leGFtcGxlLm9yZzANBgkqhkiG9w0BAQUF\n" +
    "AAOCAQEATwnLetXu9e9iDdx7ooXWjMqVtGvaEVuSAHUTucoLzur7wx/iP38hdHni\n" +
    "5rzaBuUvb/ZVxnM5z0i8DS8M0noGw0pM2UhdoNBzieTUhR2Wmg5XmcZvHSEnH40F\n" +
    "KehAroI5aMOXB888k0wa3y+mpFVIf3yMGski2iTNkjnGiuywjfVpgmfLBO7eU0GW\n" +
    "wSfcL/4z+tMOuNQyqYQoU6Xw0YnVopjnFpG7nMBBjoxYrP/j3S56q7C5cXatDycz\n" +
    "96kp08B2wL8GQHwO1aR8iuIybhau2mQfsFV8293xpLpEfLOZWNI0bgDql2wUOvIQ\n" +
    "HgqiSRB2AfTyyBj9zGNGEosJG/GU5g==\n" +
    "-----END CERTIFICATE-----\n";

var ca2 = "-----BEGIN CERTIFICATE-----\n" +
    "MIIDQjCCAiqgAwIBAgIBCDANBgkqhkiG9w0BAQ4FADA7MQswCQYDVQQGEwJOTDER\n" +
    "MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n" +
    "MTEwMjEyMTQ0NDA3WhcNMjEwMjEyMTQ0NDA3WjA/MQswCQYDVQQGEwJOTDERMA8G\n" +
    "A1UEChMIUG9sYXJTU0wxHTAbBgNVBAMTFFBvbGFyU1NMIENlcnQgU0hBMjI0MIIB\n" +
    "IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuTxKxcijjpAXpJ5SqnF1JmGA\n" +
    "58e1bYz/qrZBJre+Ea1ccxYMZBFIBP/W4TsF24m7s5cJ1RwU3WiHObA9ccvidtAa\n" +
    "2BgtgBtU9uVEmvHLr2Eu30kNnQm37bH9PP08+iTPXb985FPnJbXqRCLpJtPqIJSe\n" +
    "5mFnui4HZwsDL6IJ7fAzjwvOEO9npMYI2sHtwj/XSt0VPfleHIFgRj61sz0vpt5H\n" +
    "HLySruvfJ2sWVrfc7NFVV6Vu7HUl9bd736vSOlqRmH2XFwsTCqdrSovBRzD7OvhB\n" +
    "BNXB37gdv3sBpWWi4B42t6ZczDBa+M1vzfEZYiXKAeM1f/og9dz9abJqAH0X9wID\n" +
    "AQABo00wSzAJBgNVHRMEAjAAMB0GA1UdDgQWBBR95Jxr5vlxfUbSEj2tax39wqp4\n" +
    "TDAfBgNVHSMEGDAWgBS0WuSls97SUva51aaVD+s+vMf9/zANBgkqhkiG9w0BAQ4F\n" +
    "AAOCAQEAuJsK0bTRpM4FOUJ6O3te/ZdXijZgQjnQ5gycfi8rvu/nRTR3SHoQSv12\n" +
    "ykI5JTz6GfhjbOc2J5rsBs7k9ywuxjbBJb2rCari2k7errX1up6QJFI0lpZhTCa1\n" +
    "V2WxEO0TK1SQztMhy4zTTGzl4XgiFj/hvvHuXTlIoeaARvRG8nkDPvH8UUfZBeiF\n" +
    "gRsLT/qFnc7ndlpv2pifQ/HzLy9XKKpwFIJ/1WkUjPmCti+m37VrDkPJlpFkPYuo\n" +
    "FxWaiEKk0JDAo6Lh3faVbTudcaYeniwe2/Zfk0Ms7VNwVVBW382WbNWRD7Gn9LcX\n" +
    "nR8L9gv4/ud83sEgt/xpE7riYZulYg==\n" +
    "-----END CERTIFICATE-----\n";

var pk = "-----BEGIN PUBLIC KEY-----\n" +
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuTxKxcijjpAXpJ5SqnF1\n" +
    "JmGA58e1bYz/qrZBJre+Ea1ccxYMZBFIBP/W4TsF24m7s5cJ1RwU3WiHObA9ccvi\n" +
    "dtAa2BgtgBtU9uVEmvHLr2Eu30kNnQm37bH9PP08+iTPXb985FPnJbXqRCLpJtPq\n" +
    "IJSe5mFnui4HZwsDL6IJ7fAzjwvOEO9npMYI2sHtwj/XSt0VPfleHIFgRj61sz0v\n" +
    "pt5HHLySruvfJ2sWVrfc7NFVV6Vu7HUl9bd736vSOlqRmH2XFwsTCqdrSovBRzD7\n" +
    "OvhBBNXB37gdv3sBpWWi4B42t6ZczDBa+M1vzfEZYiXKAeM1f/og9dz9abJqAH0X\n" +
    "9wIDAQAB\n" +
    "-----END PUBLIC KEY-----\n";

var req1 = "-----BEGIN CERTIFICATE REQUEST-----\n" +
    "MIIBcDCB2gIBADAxMQswCQYDVQQGEwJDTjEQMA4GA1UEChMHYmFvei5jbjEQMA4G\n" +
    "A1UEAxMHYmFvei5tZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA0m5lvKpW\n" +
    "qy9JS7tV2HIPqHCYHLquSuxIC3F8strIQLJKO3rZmTT96KTnhsOfBO7Y1bI7mnT0\n" +
    "PB3/vcHd9ekWMEoZJQw7MuB8KeM/Wn54+elJr5DNLk5bMppSGxX7ZnumiXGG51/X\n" +
    "3Yp+/EbGtDG80GxXXix7Mucyo7K25uE0uW8CAwEAAaAAMA0GCSqGSIb3DQEBBQUA\n" +
    "A4GBACx6pUvyyU3YmwR42MjRT9q2cN5KVt5oWCdSgOC+WTNi4FeMRCxPC8ExNkzN\n" +
    "mn71AolBNc1ve5cgyr3CGN3mYK86heH2y0x4xApN/IJeUCdAtzMFvLbvkEBSLtkz\n" +
    "G3f1zTP01fknEN1WH762dXrisQONk6qmbga6ytMo7KbJ0Xgl\n" +
    "-----END CERTIFICATE REQUEST-----\n";

var rsa_pub_key = "-----BEGIN RSA PUBLIC KEY-----\n" +
    "MIIBCgKCAQEAniqcAxl7LclB0kE6q9AcAd8EE+0W6AsriR9Fs9T+6QVXl8uiCiAb\n" +
    "h/KCyy8X8C2bHsFpNBvwGTqMwHbqZqWBVUvYRtfCFcy3Xmertb09DnOBeWqKS418\n" +
    "1kss97JDO6G07QNbuLSWwkkO82CHD1kUmeF5/dof0Ra6bsRXqppdo86NzlgFud+E\n" +
    "2s5BM3XwewZVSpA69bwEiXaRDhrsg5mqeOm68VyxE8LQu+895kKsBnTvTueZTrXT\n" +
    "+HNaIveoYe8+Lb7b/mZYtlhrDK0i/8EDox85vxnzKZ7wNswqqcDg6vfC2911phST\n" +
    "Ph13jv2FIOkjO/WHhHEzRnS2VQqivqIbsQIDAQAB\n" +
    "-----END RSA PUBLIC KEY-----";

var data = new Buffer([0x9b, 0x4c, 0x7b, 0xce, 0x7a, 0xbd, 0x0a, 0x13,
    0x61, 0xfb, 0x17, 0xc2, 0x06, 0x12, 0x0c, 0xed
]);

var art1 = "+-----[lion]------+\n" +
    "|    .+.          |\n" +
    "|      o.         |\n" +
    "|     .. +        |\n" +
    "|      Eo =       |\n" +
    "|        S + .    |\n" +
    "|       o B . .   |\n" +
    "|        B o..    |\n" +
    "|         *...    |\n" +
    "|        .o+...   |\n" +
    "+-----------------+\n";

var art2 = "+-----------------+\n" +
    "|    .+.          |\n" +
    "|      o.         |\n" +
    "|     .. +        |\n" +
    "|      Eo =       |\n" +
    "|        S + .    |\n" +
    "|       o B . .   |\n" +
    "|        B o..    |\n" +
    "|         *...    |\n" +
    "|        .o+...   |\n" +
    "+-----------------+\n";

var art3 = "+01234567890123456+\n" +
    "|    .+.          |\n" +
    "|      o.         |\n" +
    "|     .. +        |\n" +
    "|      Eo =       |\n" +
    "|        S + .    |\n" +
    "|       o B . .   |\n" +
    "|        B o..    |\n" +
    "|         *...    |\n" +
    "|        .o+...   |\n" +
    "+-----------------+\n";

describe('crypto', () => {
    it("random", () => {
        assert.notEqual(crypto.randomBytes(8).hex(), crypto.randomBytes(8).hex());
    });

    it("pseudoRandomBytes", () => {
        assert.notEqual(crypto.pseudoRandomBytes(8).hex(), crypto.pseudoRandomBytes(8).hex());
    });

    it("randomArt", () => {
        assert.notEqual(crypto.randomArt(data, "lion"), art1);
        assert.notEqual(crypto.randomArt(data, ""), art2);
        assert.notEqual(crypto.randomArt(data, "01234567890123456789"), art3);
    });

    describe('Cipher', () => {
        function test_cipher(provider, file) {
            it(file, () => {
                var cases = encoding.json.decode(fs.readTextFile(__dirname + "/crypto_case/" + file + ".json"));

                cases.forEach((item) => {
                    var c;
                    var mode = crypto[item[2]];

                    if (item.length == 7)
                        c = new crypto.Cipher(provider, mode,
                            hex.decode(item[3]), hex.decode(item[6]));
                    else if (item.length == 6)
                        c = new crypto.Cipher(provider, mode,
                            hex.decode(item[3]));

                    if (mode == crypto.CBC)
                        c.paddingMode(crypto.NOPADDING);
                    assert.equal(c[item[1]](hex.decode(item[4])).hex(), item[5], item[0]);
                });

            });
        }

        test_cipher(crypto.CAMELLIA, "camellia");
        test_cipher(crypto.DES, "des");
        test_cipher(crypto.DES_EDE3, "des_ede3");
        test_cipher(crypto.BLOWFISH, "blowfish");
        test_cipher(crypto.ARC4, "arc4");

        describe('AES', () => {
            test_cipher(crypto.AES, "aes.cbc");
            test_cipher(crypto.AES, "aes.cfb");
            test_cipher(crypto.AES, "aes.ecb");
        });
    });

    describe("PKey", () => {
        describe("RSA", () => {
            it("PEM import/export", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);
                assert.equal(pk.exportPem(), rsa1024_pem);
            });

            it("toString", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);
                assert.equal(pk, rsa1024_pem);
            });

            it("Der import/export", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);
                var der = pk.exportDer();
                pk.importKey(der);
                assert.equal(pk.exportPem(), rsa1024_pem);
            });

            it("import publicKey", () => {
                var pk = new crypto.PKey();
                pk.importKey(pub_rsa1024_pem);
                assert.isFalse(pk.isPrivate());

                assert.equal(pk, pub_rsa1024_pem);

                var pk1 = new crypto.PKey();
                pk1.importKey(pk.exportDer());
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1.exportPem(), pub_rsa1024_pem);
            });

            it("import rsa format publicKey", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa_pub_key);
                assert.isFalse(pk.isPrivate());
            });

            it("publicKey", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);
                assert.isTrue(pk.isPrivate());

                var pk1 = pk.publicKey;
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1, pub_rsa1024_pem);
            });

            it("clone", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);

                var pk1 = pk.clone();

                assert.equal(pk1.exportPem(), pk.exportPem());
            });

            it("gen_key", () => {
                var pk = new crypto.PKey();
                var pk1 = new crypto.PKey();
                pk.genRsaKey(512);
                pk1.genRsaKey(512);

                assert.notEqual(pk.exportPem(), pk1.exportPem());
            });

            it("encrypt/decrypt", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);

                var pk1 = pk.publicKey;

                var d = pk1.encrypt("abcdefg");
                assert.equal(pk.decrypt(d).toString(), "abcdefg");

                assert.throws(() => {
                    pk1.decrypt(d);
                });
            });

            it("sign/verify", () => {
                var pk = new crypto.PKey();
                pk.importKey(rsa1024_pem);

                var pk1 = pk.publicKey;

                var md = hash.md5("abcdefg").digest();
                var md1 = hash.md5("abcdefg1").digest();
                var d = pk.sign(md);
                assert.isTrue(pk1.verify(d, md));
                assert.isFalse(pk1.verify(d, md1));

                assert.throws(() => {
                    pk1.sign(md);
                });
            });
        });

        describe("EC", () => {
            it("PEM import/export", () => {
                var pk = new crypto.PKey();
                pk.importKey(ec_pem);
                assert.equal(pk.exportPem(), ec_pem);
            });

            it("toString", () => {
                var pk = new crypto.PKey();
                pk.importKey(ec_pem);
                assert.equal(pk, ec_pem);
            });

            it("Der import/export", () => {
                var pk = new crypto.PKey();
                pk.importKey(ec_pem);
                var der = pk.exportDer();
                pk.importKey(der);
                assert.equal(pk.exportPem(), ec_pem);
            });

            it("import publicKey", () => {
                var pk = new crypto.PKey();
                pk.importKey(pub_ec_pem);
                assert.isFalse(pk.isPrivate());

                assert.equal(pk, pub_ec_pem);

                var pk1 = new crypto.PKey();
                pk1.importKey(pk.exportDer());
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1.exportPem(), pub_ec_pem);
            });

            it("publicKey", () => {
                var pk = new crypto.PKey();
                pk.importKey(ec_pem);
                assert.isTrue(pk.isPrivate());

                var pk1 = pk.publicKey;
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1, pub_ec_pem);
            });

            it("clone", () => {
                var pk = new crypto.PKey();
                pk.importKey(ec_pem);

                var pk1 = pk.clone();

                assert.equal(pk1.exportPem(), pk.exportPem());
            });

            it("gen_key", () => {
                var pk = new crypto.PKey();
                var pk1 = new crypto.PKey();
                pk.genEcKey();
                pk1.genEcKey();

                assert.notEqual(pk.exportPem(), pk1.exportPem());
            });

            it("sign/verify", () => {
                var pk = new crypto.PKey();
                pk.importKey(ec_pem);

                var pk1 = pk.publicKey;

                var md = hash.md5("abcdefg").digest();
                var md1 = hash.md5("abcdefg1").digest();
                var d = pk.sign(md);
                assert.isTrue(pk1.verify(d, md));
                assert.isFalse(pk1.verify(d, md1));

                assert.throws(() => {
                    pk1.sign(md);
                });
            });
        });

        it("name", () => {
            var pk = new crypto.PKey();

            assert.equal(pk.name, "invalid PK");

            pk.importKey(rsa1024_pem);
            assert.equal(pk.name, "RSA");

            pk.importKey(ec_pem);
            assert.equal(pk.name, "EC");
        });

        it("keySize", () => {
            var pk = new crypto.PKey();

            assert.equal(pk.keySize, 0);

            pk.importKey(rsa1024_pem);
            assert.equal(pk.keySize, 1024);

            pk.importKey(ec_pem);
            assert.equal(pk.keySize, 521);
        });

    });

    describe("X509 Cert", () => {
        var cert = new crypto.X509Cert();

        it("load", () => {
            var fl = fs.readdir(__dirname + '/cert_files/');
            fl.forEach((s) => {
                if (s.match(/\.crt/))
                    cert.load(fs.readTextFile(__dirname + '/cert_files/' + s));
            });
        });

        it("clear/export", () => {
            var s = cert.dump();
            cert.clear();
            assert.deepEqual(cert.dump(), []);

            s.forEach((c) => {
                cert.load(c);
            });

            assert.deepEqual(cert.dump(), s);

            cert.clear();
            cert.load(s.join('\n'));

            assert.deepEqual(cert.dump(), s);
        });

        it("load file", () => {
            var s = cert.dump();
            cert.clear();
            assert.deepEqual(cert.dump(), []);

            var fl = fs.readdir(__dirname + '/cert_files/');
            fl.forEach((s) => {
                if (s.match(/\.crt/))
                    cert.loadFile(__dirname + '/cert_files/' + s);
            });

            assert.deepEqual(cert.dump(), s);
        });

        it("certdata.txt", () => {
            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.load(fs.readTextFile(__dirname + '/cert_files/certdata.txt'));
            var s = cert.dump();
            assert.notDeepEqual(s, []);

            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.load(fs.readTextFile(__dirname + '/cert_files/ca-bundle.crt'));
            var s1 = cert.dump();

            assert.deepEqual(s.slice(s.length - s1.length), s1);
        });

        it("root ca", () => {
            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.load(fs.readTextFile(__dirname + '/cert_files/ca-bundle.crt'));
            var s = cert.dump();

            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.loadRootCerts();
            var s1 = cert.dump();

            assert.deepEqual(s, s1.slice(s1.length - s.length));
        });

        it("load root ca times", () => {
            function count(ca) {
                var cnt = 1;
                var ca1;

                while (ca1 = ca.next) {
                    cnt++;
                    ca = ca1;
                }

                return cnt;
            }

            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.loadRootCerts();
            var cnt1 = count(cert);

            cert.loadRootCerts();
            assert.equal(cnt1, count(cert));

            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.loadRootCerts();
            assert.equal(cnt1, count(cert));
        });

        it("unknown format", () => {
            assert.throws(() => {
                cert.load(__dirname + '/cert_files/certdata.txt');
            });
        });

        it("info", () => {
            cert.clear();
            assert.deepEqual(cert.dump(), []);

            cert.load(ca1);
            assert.equal(cert.version, 3);
            assert.equal(cert.serial, "17");
            assert.equal(cert.issuer, "C=NL, O=PolarSSL, CN=PolarSSL Test CA");
            assert.equal(cert.subject, "C=NL, O=PolarSSL, CN=www.example.com");
            assert.deepEqual(cert.notBefore, new Date("May 10 13:23:41 2012 GMT"));
            assert.deepEqual(cert.notAfter, new Date("May 11 13:23:41 2022 GMT"));
            assert.equal(cert.ca, false);
            assert.equal(cert.pathlen, 0);
            assert.equal(cert.usage, "");
            assert.equal(cert.type, "");
            assert.equal(cert.publicKey, pk);

            cert.load(ca2);

            var cert1 = cert.next;
            assert.equal(cert1.issuer, "C=NL, O=PolarSSL, CN=PolarSSL Test CA");
            assert.equal(cert1.subject, "C=NL, O=PolarSSL, CN=PolarSSL Cert SHA224");
            assert.deepEqual(cert1.notBefore, new Date("Feb 12 14:44:07 2011 GMT"));
            assert.deepEqual(cert1.notAfter, new Date("Feb 12 14:44:07 2021 GMT"));
            assert.equal(cert1.ca, false);
            assert.equal(cert1.pathlen, 0);
            assert.equal(cert1.usage, "");
            assert.equal(cert1.type, "");
            assert.equal(cert1.publicKey, pk);
        });
    });

    describe("X509 Crl", () => {
        var crl = new crypto.X509Crl();

        it("load", () => {
            var fl = fs.readdir(__dirname + '/crl_files/');
            fl.forEach((s) => {
                if (s.match(/\.pem/))
                    crl.load(fs.readTextFile(__dirname + '/crl_files/' + s));
            });
        });

        it("clear/export", () => {
            var s = crl.dump();

            crl.clear();
            assert.deepEqual(crl.dump(), []);

            s.forEach((c) => {
                crl.load(c);
            });

            assert.deepEqual(crl.dump(), s);

            crl.clear();
            crl.load(s.join('\n'));

            assert.deepEqual(crl.dump(), s);
        });

        it("load x509 crl file", () => {
            var s = crl.dump();
            crl.clear();

            var fl = fs.readdir(__dirname + '/crl_files/');
            fl.forEach((s) => {
                if (s.match(/\.pem/))
                    crl.loadFile(__dirname + '/crl_files/' + s);
            });

            assert.deepEqual(crl.dump(), s);
        });

    });

    describe("X509 Req", () => {
        var req = new crypto.X509Req();

        it("load", () => {
            var fl = fs.readdir(__dirname + '/req_files/');
            fl.forEach((s) => {
                if (s.match(/\.req/))
                    req.load(fs.readTextFile(__dirname + '/req_files/' + s));
            });
        });

        it("toString", () => {
            assert.equal(req.exportPem(), req);
        });

        it("import/export pem", () => {
            var s = req.exportPem();

            var req1 = new crypto.X509Req();
            req1.load(s);

            assert.equal(req1.exportPem(), s);
        });

        it("import/export der", () => {
            var s = req.exportDer();

            var req1 = new crypto.X509Req();
            req1.load(s);

            assert.deepEqual(req1.exportDer().toJSON(), s.toJSON());
        });

        it("create", () => {
            var pk = new crypto.PKey();
            pk.importKey(rsa1024_pem);

            req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);

            assert.deepEqual(req.exportPem(), req1);
        })

        it("info", () => {
            req.load(req1);
            assert.equal(req.subject, "C=CN, O=baoz.cn, CN=baoz.me");
            assert.equal(req.publicKey, pub_rsa1024_pem);
        });
    });

    describe("CA sign/verify", () => {
        var req;
        var ca;
        var pk;

        before(() => {
            pk = new crypto.PKey();
            pk.importKey(rsa1024_pem);
            req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);
        });

        it("sign", () => {
            var cert = req.sign("C=CN, O=baoz.cn", pk);
            assert.equal(cert.issuer, "C=CN, O=baoz.cn");
            assert.equal(cert.subject, "C=CN, O=baoz.cn, CN=baoz.me");
            assert.equal(cert.publicKey, pub_rsa1024_pem);
            assert.equal(cert.serial, "1");
        });

        it("ca/pathlen", () => {
            var cert = req.sign("C=CN, O=baoz.cn", pk);
            assert.isFalse(cert.ca);
            assert.equal(cert.pathlen, 0);

            cert = req.sign("C=CN, O=baoz.cn", pk, {
                ca: true,
                pathlen: 10
            });
            assert.isTrue(cert.ca);
            assert.equal(cert.pathlen, 11);
        });

        it("before/after", () => {
            var cert = req.sign("C=CN, O=baoz.cn", pk);
            assert.deepEqual(os.dateAdd(cert.notBefore, 1, "year"), cert.notAfter);

            cert = req.sign("C=CN, O=baoz.cn", pk, {
                notBefore: new Date("2014-12-20 20:20:20")
            });
            assert.deepEqual(cert.notBefore, new Date("2014-12-20 20:20:20"));
            assert.deepEqual(cert.notAfter, new Date("2015-12-20 20:20:20"));

            cert = req.sign("C=CN, O=baoz.cn", pk, {
                notBefore: new Date("2014-12-20 20:20:20"),
                notAfter: new Date("2018-12-20 20:20:20")
            });
            assert.deepEqual(cert.notBefore, new Date("2014-12-20 20:20:20"));
            assert.deepEqual(cert.notAfter, new Date("2018-12-20 20:20:20"));
        });

        it("usage/type", () => {
            var cert = req.sign("C=CN, O=baoz.cn", pk);
            assert.equal(cert.usage, "");
            assert.equal(cert.type, "");

            cert = req.sign("C=CN, O=baoz.cn", pk, {
                usage: "",
                type: ""
            });
            assert.equal(cert.usage, "");
            assert.equal(cert.type, "");

            cert = req.sign("C=CN, O=baoz.cn", pk, {
                usage: "digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment, keyAgreement, keyCertSign, cRLSign",
                type: "client, server, email, objsign, reserved, sslCA, emailCA, objCA"
            });
            assert.equal(cert.usage, "digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment, keyAgreement, keyCertSign, cRLSign");
            assert.equal(cert.type, "client, server, email, objsign, reserved, sslCA, emailCA, objCA");
        });

        it("self-sign", () => {
            req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);
            ca = req.sign("C=CN, O=baoz.cn, CN=baoz.me", pk, {
                ca: true,
                notBefore: new Date(new Date().getTime() - 1000)
            });
        });

        it("verify", () => {
            assert.isTrue(ca.verify(ca));
            assert.isFalse(ca.verify(req.sign("C=CN, O=baoz.cn", pk)));
        });
    });

    it('pbkdf1', () => {
        var tests = [
            [hash.SHA1, 'password', 'salt', 1, 20,
                hex.decode('c88e9c67041a74e0357befdff93f87dde0904214'),
                'sha1'
            ],
            [hash.SHA256, 'password', 'salt', 2, 20,
                hex.decode('a6b9d96cc74d52749372886896349c07e2137fe8'),
                'sha256'
            ],
            [hash.MD5, 'password', '', 1, 32,
                hex.decode('5F4DCC3B5AA765D61D8327DEB882CF992B95990A9151374ABD8FF8C5A7A0FE08'),
                'md5'
            ],
            [hash.MD5, '', '', 1, 32,
                hex.decode('D41D8CD98F00B204E9800998ECF8427E59ADB24EF3CDBE0297F05B395827453F'),
                'md5'
            ],
            [hash.MD5, '', hex.decode('E3936A9A8ACFE9BE'), 1, 32,
                hex.decode('E9FAB75961E5DE62D6982C3F569114A5652D875150F652F75154666E1FD0E8AC'),
                'md5'
            ],
            [hash.MD5, '012345678910111231415161717',
                hex.decode('F7560045C70A96DB'), 1, 32,
                hex.decode('2E14B2EC7E2F8CDC18F15BB773CCD6F25C8AADA268F9B86F960DF0464AE5E981'),
                'md5'
            ]
        ];

        tests.forEach((t) => {
            assert.deepEqual(crypto.pbkdf1(t[1], t[2], t[3], t[4], t[0]), t[5]);
        });

        tests.forEach((t) => {
            assert.deepEqual(crypto.pbkdf1(t[1], t[2], t[3], t[4], t[6]), t[5]);
        });
    });

    it('pbkdf2', () => {
        var tests = [
            ["password", "salt", 1, 20,
                new Buffer([
                    0x0c, 0x60, 0xc8, 0x0f, 0x96, 0x1f, 0x0e, 0x71,
                    0xf3, 0xa9, 0xb5, 0x24, 0xaf, 0x60, 0x12, 0x06,
                    0x2f, 0xe0, 0x37, 0xa6
                ])
            ],
            ["password", "salt", 2, 20,
                new Buffer([
                    0xea, 0x6c, 0x01, 0x4d, 0xc7, 0x2d, 0x6f, 0x8c,
                    0xcd, 0x1e, 0xd9, 0x2a, 0xce, 0x1d, 0x41, 0xf0,
                    0xd8, 0xde, 0x89, 0x57
                ])
            ],
            ["password", "salt", 4096, 20,
                new Buffer([
                    0x4b, 0x00, 0x79, 0x01, 0xb7, 0x65, 0x48, 0x9a,
                    0xbe, 0xad, 0x49, 0xd9, 0x26, 0xf7, 0x21, 0xd0,
                    0x65, 0xa4, 0x29, 0xc1
                ])
            ],
            ["passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 25,
                new Buffer([
                    0x3d, 0x2e, 0xec, 0x4f, 0xe4, 0x1c, 0x84, 0x9b,
                    0x80, 0xc8, 0xd8, 0x36, 0x62, 0xc0, 0xe4, 0x4a,
                    0x8b, 0x29, 0x1a, 0x96, 0x4c, 0xf2, 0xf0, 0x70,
                    0x38
                ])
            ]
        ];

        tests.forEach((t) => {
            assert.deepEqual(crypto.pbkdf2(t[0], t[1], t[2], t[3], hash.SHA1), t[4]);
        });

        tests.forEach((t) => {
            assert.deepEqual(crypto.pbkdf2(t[0], t[1], t[2], t[3], 'sha1'), t[4]);
        });

        tests.forEach((t) => {
            assert.deepEqual(crypto.pbkdf2Sync(t[0], t[1], t[2], t[3], 'sha1'), t[4]);
        });
    });

});

repl && test.run(console.DEBUG);