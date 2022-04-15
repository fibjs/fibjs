var test = require("test");
test.setup();

var crypto = require("crypto");
var hash = require("hash");
var fs = require("fs");
var os = require("os");
var encoding = require("encoding");
var hex = require("hex");
var path = require("path");
var ecsdsa_case = require("./crypto_case/ecsdsa.json");

var rsa4096_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
    "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\n" +
    "-----END RSA PRIVATE KEY-----\n";

var pub_rsa4096_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAiAUwIJyWHEnusONGDVCg\n" +
    "uTr3FkkVoSiDJ2mmFYYibt1paXpIiEKQy+OwykFcOG0Ew9UGEEvveRQbKN+bpUV5\n" +
    "uH6q7pgnfDubA2VSLST7lHcXJd2as4DNyFMtgXLOj0dVSk4NduWlVgY2fUu17sJJ\n" +
    "BmysYjCjmB76P92DLfIvlI+4OYEOFSLQCG56KA8ZvP9HTnHiI0Lnerjf14cjr1Ub\n" +
    "gw0ce9KMJ96LLnZp9Q6hBCXalagDo9MofuyScpFX1X0ixirQ2VtomzTvle9EIdfm\n" +
    "at6EoLzxz8YNfl6mllvCVrlFJw1DvXfxLlkDR9I+te1B89L7Eljt90VBKmzjjJiK\n" +
    "muhmW8HhJMpe3VBG7t8c3J+F/cJLx7ZDk6/XNhxWPbzL9SNhTRg6015YDsX01bFy\n" +
    "i1GUHdFocPmzgZ6nw3anFT85+q7l9M4yGhRQbfUWQ4rT0zCFobJZQeO4ZyiBM0fu\n" +
    "AGhln0MogIdFaoh7xmVggL6XxJVOApXXGV7Z8xDjzbLpBI2UHiX8W33ZbrXYiDex\n" +
    "vfBQuR2SFw372h/LCF+fOI8kxZr9BNU1RSf26RCX0Mi0Fdbz0eeXLa3N42fGAxHY\n" +
    "UCN5M4WEtAPcdQjQAZ7gZ+Voy9nNjMoQMWzzH84XHUI1byA8T795CnVmKMiHyCWE\n" +
    "nOGV1ptmZrzoZoa6x6bOHukCAwEAAQ==\n" +
    "-----END PUBLIC KEY-----\n";

var ec_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
    "MIHcAgEBBEIB+QhtQdd9bjWeN2mgq6qoqW51ygslLwP+gwTCSP4ZVpcU0pxwigXm\n" +
    "Ioa7Zzr2Q0OOjzdH/vDIaV9FzOySWSKTVJOgBwYFK4EEACOhgYkDgYYABAE3zTRb\n" +
    "rr5Rsa0xqF4Mn0gBoETTQ5zt6zd5O+LqyiXVUteEWIXZ0sJvdQZvhdZfk9VypQGN\n" +
    "skZ8xvcNVJL6PHNMCwEgR6q67gepHZBFAd75Vt/lCJJOB7SnVHk8Dfu5t49q4Gb2\n" +
    "EdX30QGqQ71qihogY6Jqlmn8aJAZGwyNHPeX+n+F1A==\n" +
    "-----END EC PRIVATE KEY-----\n";

var ec256_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
    "MHQCAQEEIAp9Kd9+KD3LRbZLxxA6IG+brTA/pgHIR3ULfOgJc6a1oAcGBSuBBAAK\n" +
    "oUQDQgAEpNdY6eCd+synm6iEU5MTUZZUZumR+DglOJh3OQ/y8PafXkcwXZq5mNp2\n" +
    "LG2Ho+7ATaE5e03TTWWhhqfHj4scBQ==\n" +
    "-----END EC PRIVATE KEY-----\n";

var pub_ec_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBN800W66+UbGtMaheDJ9IAaBE00Oc\n" +
    "7es3eTvi6sol1VLXhFiF2dLCb3UGb4XWX5PVcqUBjbJGfMb3DVSS+jxzTAsBIEeq\n" +
    "uu4HqR2QRQHe+Vbf5QiSTge0p1R5PA37ubePauBm9hHV99EBqkO9aooaIGOiapZp\n" +
    "/GiQGRsMjRz3l/p/hdQ=\n" +
    "-----END PUBLIC KEY-----\n";

var sm2_pem = "-----BEGIN EC PRIVATE KEY-----\n" +
    "MHcCAQEEIH3EUWpWsnLGl6SkGBnG5lPEIvdyql56aHQMCCt7xDqCoAoGCCqBHM9V\n" +
    "AYItoUQDQgAE1KnIoMvdNODUrcEzQNnHbplwxNNyuHwIUnU0oNQ/0R1z97YIe/k8\n" +
    "HX6wrPMUazfS1PVd/A9R8gadvlURQ3lufg==\n" +
    "-----END EC PRIVATE KEY-----\n";

var pub_sm2_pem = "-----BEGIN PUBLIC KEY-----\n" +
    "MFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAE1KnIoMvdNODUrcEzQNnHbplwxNNy\n" +
    "uHwIUnU0oNQ/0R1z97YIe/k8HX6wrPMUazfS1PVd/A9R8gadvlURQ3lufg==\n" +
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

var ca3 = "-----BEGIN CERTIFICATE-----\n" +
    "MIIBiTCCATSgAwIBAgIBATAMBggqgRzPVQGDdgUAMB8xCzAJBgNVBAYTAkNOMRAw\n" +
    "DgYDVQQKEwdiYW96LmNuMB4XDTE5MTIwMzA3MDE1OFoXDTIwMTIwMzA3MDE1OFow\n" +
    "MTELMAkGA1UEBhMCQ04xEDAOBgNVBAoTB2Jhb3ouY24xEDAOBgNVBAMTB2Jhb3ou\n" +
    "bWUwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAATUqcigy9004NStwTNA2cdumXDE\n" +
    "03K4fAhSdTSg1D/RHXP3tgh7+TwdfrCs8xRrN9LU9V38D1HyBp2+VRFDeW5+o00w\n" +
    "SzAJBgNVHRMEAjAAMB0GA1UdDgQWBBSr7lpWpgygM4V6wgXDSpwXBWffXzAfBgNV\n" +
    "HSMEGDAWgBSr7lpWpgygM4V6wgXDSpwXBWffXzAMBggqgRzPVQGDdgUAA0EA2eb6\n" +
    "MBVIaMFVbDpUj4O/Umpskcx0yIG3yDH8HrH2JThVK4kcN5jQu+WYZ9fk1Rs/2oVg\n" +
    "j3U6JanA9EXDi7dezQ==\n" +
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
    "MIIEdjCCAl4CAQAwMTELMAkGA1UEBhMCQ04xEDAOBgNVBAoMB2Jhb3ouY24xEDAO\n" +
    "BgNVBAMMB2Jhb3oubWUwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCI\n" +
    "BTAgnJYcSe6w40YNUKC5OvcWSRWhKIMnaaYVhiJu3WlpekiIQpDL47DKQVw4bQTD\n" +
    "1QYQS+95FBso35ulRXm4fqrumCd8O5sDZVItJPuUdxcl3ZqzgM3IUy2Bcs6PR1VK\n" +
    "Tg125aVWBjZ9S7XuwkkGbKxiMKOYHvo/3YMt8i+Uj7g5gQ4VItAIbnooDxm8/0dO\n" +
    "ceIjQud6uN/XhyOvVRuDDRx70own3osudmn1DqEEJdqVqAOj0yh+7JJykVfVfSLG\n" +
    "KtDZW2ibNO+V70Qh1+Zq3oSgvPHPxg1+XqaWW8JWuUUnDUO9d/EuWQNH0j617UHz\n" +
    "0vsSWO33RUEqbOOMmIqa6GZbweEkyl7dUEbu3xzcn4X9wkvHtkOTr9c2HFY9vMv1\n" +
    "I2FNGDrTXlgOxfTVsXKLUZQd0Whw+bOBnqfDdqcVPzn6ruX0zjIaFFBt9RZDitPT\n" +
    "MIWhsllB47hnKIEzR+4AaGWfQyiAh0VqiHvGZWCAvpfElU4CldcZXtnzEOPNsukE\n" +
    "jZQeJfxbfdlutdiIN7G98FC5HZIXDfvaH8sIX584jyTFmv0E1TVFJ/bpEJfQyLQV\n" +
    "1vPR55ctrc3jZ8YDEdhQI3kzhYS0A9x1CNABnuBn5WjL2c2MyhAxbPMfzhcdQjVv\n" +
    "IDxPv3kKdWYoyIfIJYSc4ZXWm2ZmvOhmhrrHps4e6QIDAQABoAAwDQYJKoZIhvcN\n" +
    "AQELBQADggIBABQJpGu/2a5uBBrCq74gwN+xdn+UvGTyTyrr67ddU3sLAMFiS980\n" +
    "17UwomPhLdhOkEcMmpi/YUuvagPd5KXCEEzCzJwx0EB3qXnlghD5J/CwYnL1YXFN\n" +
    "LXTfKTeKwsV+60uHTU+OH0ZWDpS25Bv83Ok3tRpyv9QGfcm2CmcPbbmfVR7gN5Wg\n" +
    "5OwaWduv2D3odUZOyy1ANGrHZpD0HIFIHlm0PxBtboxvG99IubsDPzORXLEYMs/U\n" +
    "E/ix9F2C//650kdRGPy6HEnaFWg8Clfty9GSZX7HoXqGmWELCm7l2DaQwAxP+zJ4\n" +
    "1unq2vXE/F8faNrUXkanODfLjkWYZFbnFWGARk4XW48Cr7Nzyp3iljTU0LynSdhS\n" +
    "bUccyOPDaVWsAJc/B4Ed3qf0mdDMuejqNwkEmJl6/jZL0SfkUOSeR15mxrIOr2GS\n" +
    "lcO94RQ0W830VQ+PKp7C6ZGtvSLd6xuZaQRTwKEjIuZyO1mqqGzOKgMfso6YZRPM\n" +
    "RqqZMuUYSDWWgBZxeugpTDu4MFFiXwL/LKe/aBHd3OwtQFZqnvxPfhwMsn4g+JxN\n" +
    "IgYhwpmIZ44ZnOcohcm7SBOtsNIixEJeGMInnYxJmkH+ix1zSB22JYSFweBzOQ24\n" +
    "2ojyVJc1FZSUL8gHRbcZn8eR13wdkzZ91uTpWCu94iTtU0+69S9m5qri\n" +
    "-----END CERTIFICATE REQUEST-----\n"

var sm2_req = "-----BEGIN CERTIFICATE REQUEST-----\n" +
    "MIHnMIGTAgEAMDExCzAJBgNVBAYTAkNOMRAwDgYDVQQKEwdiYW96LmNuMRAwDgYD\n" +
    "VQQDEwdiYW96Lm1lMFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAE1KnIoMvdNODU\n" +
    "rcEzQNnHbplwxNNyuHwIUnU0oNQ/0R1z97YIe/k8HX6wrPMUazfS1PVd/A9R8gad\n" +
    "vlURQ3lufqAAMAwGCCqBHM9VAYN2BQADQQDzqeEAzhohe1R/chlIdKMFgWLmcZ52\n" +
    "9UImdf5/efmrsL6X6hZyKhGy6vypOr8FpwaRC98o9fOL41Gl8/MBr6vJ\n" +
    "-----END CERTIFICATE REQUEST-----\n"

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

        assert.throws(() => {
            crypto.randomBytes(-125);
        });
    });

    it("simpleRandomBytes", () => {
        assert.notEqual(crypto.simpleRandomBytes(8).hex(), crypto.simpleRandomBytes(8).hex());

        assert.throws(() => {
            crypto.simpleRandomBytes(-125);
        });
    });

    it("pseudoRandomBytes", () => {
        assert.notEqual(crypto.pseudoRandomBytes(8).hex(), crypto.pseudoRandomBytes(8).hex());

        assert.throws(() => {
            crypto.pseudoRandomBytes(-125);
        });
    });

    it("randomFill", () => {
        var buf = Buffer.alloc(10);
        var before = buf.toString('hex');
        var buf1 = crypto.randomFill(buf, 5, 5);
        var after = buf.toString('hex');
        assert.equal(buf, buf1);
        assert.notStrictEqual(before, after);

        crypto.randomFill(buf, 0);
        assert.throws(() => {
            crypto.randomFill(buf, -1);
        });

        crypto.randomFill(buf, 10);
        assert.throws(() => {
            crypto.randomFill(buf, 11);
        });

        crypto.randomFill(buf, 9, 1);
        assert.throws(() => {
            crypto.randomFill(buf, 9, 2);
        });
    });

    it("randomArt", () => {
        assert.notEqual(crypto.randomArt(data, "lion"), art1);
        assert.notEqual(crypto.randomArt(data, ""), art2);
        assert.notEqual(crypto.randomArt(data, "01234567890123456789"), art3);

        assert.throws(() => {
            crypto.randomArt(data, "title", 0);
        });

        assert.throws(() => {
            crypto.randomArt(undefined, '', 1064203628);
        });
    });

    describe('Cipher', () => {
        function test_cipher(provider, file) {
            it(file, () => {
                var cases = encoding.json.decode(fs.readTextFile(path.join(__dirname, "crypto_case", file + ".json")));

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
                    assert.equal(c[item[1]](hex.decode(item[4])).hex(), item[5]);
                });

            });
        }

        describe('AES', () => {
            test_cipher(crypto.AES, "aes.cbc");
            test_cipher(crypto.AES, "aes.cfb");
            test_cipher(crypto.AES, "aes.ecb");
            // test_cipher(crypto.AES, "aes.xts");
        });

        test_cipher(crypto.DES, "des");
        test_cipher(crypto.DES_EDE3, "des_ede3");
        test_cipher(crypto.CAMELLIA, "camellia");
        test_cipher(crypto.SM4, "sm4");

        // test_cipher(crypto.ARIA, "aria");

        it('DES bug fix', () => {
            let passwd = "vio7t449";
            let key = "5KLPqC7pAUUfL";
            let cipher = new crypto.Cipher(crypto.DES, crypto.CBC, new Buffer(passwd));
            let encrypted = cipher.encrypt(new Buffer(key)).hex();

            assert.equal(key, cipher.decrypt(new Buffer(encrypted, "hex")).toString("utf8"));
        });
    });

    describe("PKey", () => {
        describe("RSA", () => {
            it("PEM import/export", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);
                assert.equal(pk.pem(), rsa4096_pem);

                var pk = new crypto.PKey(rsa4096_pem);
                assert.equal(pk.pem(), rsa4096_pem);
            });

            it("toString", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);
                assert.equal(pk, rsa4096_pem);
            });

            it("Der import/export", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);
                var der = pk.der();
                pk.import(der);
                assert.equal(pk.pem(), rsa4096_pem);

                var pk = new crypto.PKey(der);
                assert.equal(pk.pem(), rsa4096_pem);
            });

            it("Json import/export", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);
                var json = pk.json();

                assert.deepEqual(json, {
                    "kty": "RSA",
                    "n": "iAUwIJyWHEnusONGDVCguTr3FkkVoSiDJ2mmFYYibt1paXpIiEKQy-OwykFcOG0Ew9UGEEvveRQbKN-bpUV5uH6q7pgnfDubA2VSLST7lHcXJd2as4DNyFMtgXLOj0dVSk4NduWlVgY2fUu17sJJBmysYjCjmB76P92DLfIvlI-4OYEOFSLQCG56KA8ZvP9HTnHiI0Lnerjf14cjr1Ubgw0ce9KMJ96LLnZp9Q6hBCXalagDo9MofuyScpFX1X0ixirQ2VtomzTvle9EIdfmat6EoLzxz8YNfl6mllvCVrlFJw1DvXfxLlkDR9I-te1B89L7Eljt90VBKmzjjJiKmuhmW8HhJMpe3VBG7t8c3J-F_cJLx7ZDk6_XNhxWPbzL9SNhTRg6015YDsX01bFyi1GUHdFocPmzgZ6nw3anFT85-q7l9M4yGhRQbfUWQ4rT0zCFobJZQeO4ZyiBM0fuAGhln0MogIdFaoh7xmVggL6XxJVOApXXGV7Z8xDjzbLpBI2UHiX8W33ZbrXYiDexvfBQuR2SFw372h_LCF-fOI8kxZr9BNU1RSf26RCX0Mi0Fdbz0eeXLa3N42fGAxHYUCN5M4WEtAPcdQjQAZ7gZ-Voy9nNjMoQMWzzH84XHUI1byA8T795CnVmKMiHyCWEnOGV1ptmZrzoZoa6x6bOHuk",
                    "e": "AQAB",
                    "d": "Le6qkOiv3D-vJfapqkdm7wYVeGYXssNOftmzZyNTr2J_fUn5AmKQKPnrsnEd5kQSTts53_j0lkuO3F-eXNI4R3Am4slo1BAAycXGWU5a0ojgvT23RxSg6RwFiugtDr9E8wGmlFb6XTFd5zPQ7R6YsF20XLgWFoGnejegW7ykCqP5zxlgmwyVebNs9ugahfMirj_Q4J1wM8YIeOy-xS-VScz8qGeyQqhpEbiVEVbuUstBpDYHfKQK0cmrrpqcMZC_VHIS8sNmcLsQiO4ycMdKnR4lZW5IriQwXFM0KCQ2XTjVPFnDXVEKld8jN3atDJxhIChH9qsnONAfMGdy9ABzFVyEn7kJ4s8Las4-U-pGvejZTlk-8uDArMA-vQK4ojTJHSSYS5are5EZClxsR4XQ3CnR0errCtSdybot1godeMutRHtQGVUfVI4HySRvi9ch76iA3iJwFz3SZR4qcRyhjkF1_yIvtEu4uqFdOyvwkJE1Qr8i2GPiKqYv_Rr7VRkgWu_eXcQ-kFLZjNSOkSeSXbv847Zo75X4DQqnd9SThoZjXpMCHioxeId2U8U2_8FGJtub7PkkWgzO_Y4-zMCNY_5a17RP1yLftBy2BdLZnC7C5RPW-wmV8QhjF5gM1kAFGvR9D3tCJr3p-PgYCPQPJ4Z45XPdkmURnQhlHZCTKb0",
                    "p": "vpZmPuu9eS25yMcYKQwtwouShk8C-VCexKymleuslN7riCemy3JZhkRkd_-InaQ-bDS93CfCU4p__unoFjoA6rKjBqPyIGnUiZOPFNukVBz4dfKoiScYtTEU0cy8e_a7DEcePOXUedzK6HuDkQqvsSU0Xoy55_KdipClQ37_H-IR3Xon6I17lAQ8x4sl8CILKrmeZeHCo9ve6R2L3bUz2lacuxlZIff6QeVPmLWTKCOviFh5nmwI-IdzX5nI0WOjPH8s4EOg5SyY1NWAq0u75gjbH4K5ljByNolNJ1ErdheQ--KNcL_60q3wOlRxyXOd18i2AFhnIPyH1XT2JiRLJw",
                    "q": "trRYO3zWY5Hv7kB1aPvBXUVK_UbPf9Hv8g4IQtrOod3cn69ni_bBNW5TbWcdO6-lk3ag5qG_deexPe6Zi_LYymaO7noypYnhX_r8aqwYg5seNVtcawAxazdAkIY5wRin8BB6MpDW7ZkKe_44DuGdiJjRSbgGGZdGshjYJle5ImyLYz5YiIxsSqdzCAwCAHMxSASbCzLSlQtkHGbrSexjd9WWIAXlmndRezut5tpx0QhsHeAhRSOoMaAI_2ORAPu7JQ-t-p2txNUkb-tvyWx_Y5g02ghk9Tx_2ogJj4d3DnDFrnRjQPkw8wBSiQeihNQ2w1-hY6Vp8QjqJ99XrWvPbw",
                    "dp": "c6zTOANbB86SAWOdrWQ_7eFKuIwCMRIWmyZzIXFeWSkH6aavXLH11kFyfUayQ4Ppg0FrrtDO6Geakjyj6cGgxtJPkns1PSken7XY1P8OuSKwRt5WHpCFNYwpp8DM8axqEOSlXK7eDGzPFtXIn-LNigYbx_s_5qDhr7VlKRdOf6p1z2gSCtq8k7USssYo65uuDWuF0S56NXwAypptRs5xP2bsT77t02kwEoi8i-JBA87M_kM85cOKmlWT2AV91mDUNaa2tRM294He7Ob85nrb-52KaZCtVZPn7mPdWSyx5UYecFMsJ6_N68s4kB6qxWuP9OVUWddOGL5NL2iC8dl7Zw",
                    "dq": "Fz6moxrk49IjZnqI9YxiM42nVsX1jFuXXYJvhqHz_CAP-7kDQYlpV3Tr5FVkIL3eIP7Mxcr6YvPRVwHOQ0HoVwfpU2n0m58JL37tMl0m2UeY9kzjWzGxF-T1r-2D149dfmt6QIiw75WzAcd8Nlh6BHD835YsIIFgmzQR-_7ZLQoGSeAav3SuKmlXd0Sf1JLTSEhduXUD2baTTBHECamrlgl0iip15csccMNNArv0ExYxOzPSx8sbkQS9YFC4mBlu2-48wk_hVdOQSTT5CfVl17P7tWmv_ott8M2x-bLQcOPkIqfoINITajc2SPzRrniZZw35qIXZDrOFpwEBVK0MvQ",
                    "qi": "FxeLSz-9JNnPO_rlHl1ZKfZZ4Nt_Ys7LhCoZ5Gz4TjlUN3LDpYe8EoeW-0NDo-vsWPM8jDVGBp_OawaXxti3ruqyW8dSBGP4x0BgoV5I8nA3LWyRszw02nWRm99TFGqeM_wzGyoVLfJ-KnK0J_HXSgIeJMw9yV-l23_N9OyoBjo3d1zuPMAK6BDqPxcIXR-Sp-q84HU3WN1UHk1uRbSasUGGsFNDIkmCaMpHFaTuv5tgqUV0ljGlh1zG616zKkNaYRiRK2h4JGAu6O-xdQFUjsUVaCPi6BtFtiscxqvXU4dcYmosNMhtTwjHaWhhC8877i2gZ97Vum2RzMz3xi---g"
                });

                pk.import(json);
                assert.equal(pk.pem(), rsa4096_pem);

                var pk = new crypto.PKey();
                pk.import(pub_rsa4096_pem);
                var json = pk.json();

                assert.deepEqual(json, {
                    "kty": "RSA",
                    "n": "iAUwIJyWHEnusONGDVCguTr3FkkVoSiDJ2mmFYYibt1paXpIiEKQy-OwykFcOG0Ew9UGEEvveRQbKN-bpUV5uH6q7pgnfDubA2VSLST7lHcXJd2as4DNyFMtgXLOj0dVSk4NduWlVgY2fUu17sJJBmysYjCjmB76P92DLfIvlI-4OYEOFSLQCG56KA8ZvP9HTnHiI0Lnerjf14cjr1Ubgw0ce9KMJ96LLnZp9Q6hBCXalagDo9MofuyScpFX1X0ixirQ2VtomzTvle9EIdfmat6EoLzxz8YNfl6mllvCVrlFJw1DvXfxLlkDR9I-te1B89L7Eljt90VBKmzjjJiKmuhmW8HhJMpe3VBG7t8c3J-F_cJLx7ZDk6_XNhxWPbzL9SNhTRg6015YDsX01bFyi1GUHdFocPmzgZ6nw3anFT85-q7l9M4yGhRQbfUWQ4rT0zCFobJZQeO4ZyiBM0fuAGhln0MogIdFaoh7xmVggL6XxJVOApXXGV7Z8xDjzbLpBI2UHiX8W33ZbrXYiDexvfBQuR2SFw372h_LCF-fOI8kxZr9BNU1RSf26RCX0Mi0Fdbz0eeXLa3N42fGAxHYUCN5M4WEtAPcdQjQAZ7gZ-Voy9nNjMoQMWzzH84XHUI1byA8T795CnVmKMiHyCWEnOGV1ptmZrzoZoa6x6bOHuk",
                    "e": "AQAB"
                });

                pk.import(json);
                assert.equal(pk.pem(), pub_rsa4096_pem);

                pk = new crypto.PKey(json);
                assert.equal(pk.pem(), pub_rsa4096_pem);
            });

            it("import publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(pub_rsa4096_pem);
                assert.isFalse(pk.isPrivate());

                assert.equal(pk, pub_rsa4096_pem);

                var pk1 = new crypto.PKey();
                pk1.import(pk.der());
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1.pem(), pub_rsa4096_pem);
            });

            it("import rsa format publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(rsa_pub_key);
                assert.isFalse(pk.isPrivate());
            });

            it("publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);
                assert.isTrue(pk.isPrivate());

                var pk1 = pk.publicKey;
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1, pub_rsa4096_pem);
            });

            it("clone", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);

                var pk1 = pk.clone();

                assert.equal(pk1.pem(), pk.pem());
            });

            it("gen_key", () => {
                var pk = crypto.generateKey(512);
                var pk1 = crypto.generateKey(512);

                assert.notEqual(pk.pem(), pk1.pem());
            });

            it("equal", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);

                var pk1 = pk.clone();

                assert.ok(pk1.equal(pk));

                var pk = crypto.generateKey(512);
                var pk1 = crypto.generateKey(512);

                assert.notOk(pk1.equal(pk));
            });

            it("encrypt/decrypt", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);

                var pk1 = pk.publicKey;

                var d = pk1.encrypt("abcdefg");
                assert.equal(pk.decrypt(d).toString(), "abcdefg");

                assert.throws(() => {
                    pk1.decrypt(d);
                });
            });

            it("sign/verify", () => {
                var pk = new crypto.PKey();
                pk.import(rsa4096_pem);

                var pk1 = pk.publicKey;

                var md = hash.md5("abcdefg").digest();
                var md1 = hash.md5("abcdefg1").digest();
                var d = pk.sign(md);
                assert.isTrue(pk1.verify(md, d));
                assert.isFalse(pk1.verify(md1, d));

                assert.throws(() => {
                    pk1.sign(md);
                });
            });
        });

        describe("EC", () => {
            it("PEM import/export", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);
                assert.equal(pk.pem(), ec_pem);

                pk = new crypto.PKey(ec_pem);
                assert.equal(pk.pem(), ec_pem);
            });

            it("toString", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);
                assert.equal(pk, ec_pem);
            });

            it("Der import/export", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);
                var der = pk.der();
                pk.import(der);
                assert.equal(pk.pem(), ec_pem);

                pk = new crypto.PKey(der);
                assert.equal(pk.pem(), ec_pem);
            });

            it("Json import/export", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);
                var json = pk.json();

                assert.deepEqual(json, {
                    "kty": "EC",
                    "crv": "P-521",
                    "x": "ATfNNFuuvlGxrTGoXgyfSAGgRNNDnO3rN3k74urKJdVS14RYhdnSwm91Bm-F1l-T1XKlAY2yRnzG9w1Ukvo8c0wL",
                    "y": "ASBHqrruB6kdkEUB3vlW3-UIkk4HtKdUeTwN-7m3j2rgZvYR1ffRAapDvWqKGiBjomqWafxokBkbDI0c95f6f4XU",
                    "d": "AfkIbUHXfW41njdpoKuqqKludcoLJS8D_oMEwkj-GVaXFNKccIoF5iKGu2c69kNDjo83R_7wyGlfRczsklkik1ST"
                });

                pk.import(json);
                assert.equal(pk.pem(), ec_pem);

                pk.import({
                    "kty": "EC",
                    "crv": "P-521",
                    "d": "AfkIbUHXfW41njdpoKuqqKludcoLJS8D_oMEwkj-GVaXFNKccIoF5iKGu2c69kNDjo83R_7wyGlfRczsklkik1ST"
                });
                assert.equal(pk.pem(), ec_pem);

                var pk = new crypto.PKey();
                pk.import(pub_ec_pem);
                var json = pk.json();

                assert.deepEqual(json, {
                    "kty": "EC",
                    "crv": "P-521",
                    "x": "ATfNNFuuvlGxrTGoXgyfSAGgRNNDnO3rN3k74urKJdVS14RYhdnSwm91Bm-F1l-T1XKlAY2yRnzG9w1Ukvo8c0wL",
                    "y": "ASBHqrruB6kdkEUB3vlW3-UIkk4HtKdUeTwN-7m3j2rgZvYR1ffRAapDvWqKGiBjomqWafxokBkbDI0c95f6f4XU"
                });

                pk.import(json);
                assert.equal(pk.pem(), pub_ec_pem);

                var pk = new crypto.PKey(json);
                assert.equal(pk.pem(), pub_ec_pem);
            });

            it("import publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(pub_ec_pem);
                assert.isFalse(pk.isPrivate());

                assert.equal(pk, pub_ec_pem);

                var pk1 = new crypto.PKey();
                pk1.import(pk.der());
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1.pem(), pub_ec_pem);
            });

            it("publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);
                assert.isTrue(pk.isPrivate());

                var pk1 = pk.publicKey;
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1, pub_ec_pem);
            });

            it("clone", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);

                var pk1 = pk.clone();

                assert.equal(pk1.pem(), pk.pem());
            });

            it("gen_key", () => {
                var pk = crypto.generateKey();
                var pk1 = crypto.generateKey();

                assert.notEqual(pk.pem(), pk1.pem());
            });

            it("equal", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);

                var pk1 = pk.clone();

                assert.ok(pk1.equal(pk));

                var pk = crypto.generateKey();
                var pk1 = crypto.generateKey();

                assert.notOk(pk1.equal(pk));
            });

            it("sign/verify", () => {
                var pk = new crypto.PKey();
                pk.import(ec_pem);

                var pk1 = pk.publicKey;

                var md = hash.md5("abcdefg").digest();
                var md1 = hash.md5("abcdefg1").digest();

                console.time('ecc sign');
                var d = pk.sign(md);
                console.timeEnd('ecc sign');

                console.time('ecc verify');
                assert.isTrue(pk1.verify(md, d));
                console.timeEnd('ecc verify');

                assert.isFalse(pk1.verify(md1, d));

                assert.throws(() => {
                    pk1.sign(md);
                });
            });

            it("secp256k1 sign/verify", () => {
                var pk = new crypto.PKey();
                pk.import(ec256_pem);

                var pk1 = pk.publicKey;

                var md = hash.md5("abcdefg").digest();
                var md1 = hash.md5("abcdefg1").digest();

                console.time('secp256k1 sign');
                var d = pk.sign(md);
                console.timeEnd('secp256k1 sign');

                console.time('secp256k1 verify');
                assert.isTrue(pk1.verify(md, d));
                console.timeEnd('secp256k1 verify');

                assert.isFalse(pk1.verify(md1, d));

                assert.throws(() => {
                    pk1.sign(md);
                });
            });

            it('FIX: secp256k1 verify error.', () => {
                var pk = new crypto.PKey();
                console.time('secp256k1 import');
                pk.import({
                    "kty": "EC",
                    "crv": "secp256k1",
                    "d": "rSnfXs7h-q2yNflXjMTJHZz_Md3KKKr2Lk1ot0-BN2k"
                });
                console.timeEnd('secp256k1 import');

                var test_data = [
                    ['ebd7f85f3c944eea17a01a95b749f306b958d8e2',
                        '304502202876bd6b6091e71294c39c76ff5c3f0b5b29bd8a554b3b6ba1a23d71cbfc43d7022100c2445201eae4fd0da1f3844aaa0888dca7754509a2e18330daceb5f333725bec'],
                    ['ebd7f85f3c944eea17a01a95b749f306b958d8e2',
                        '3045022100d02bb0c6fd3e389dd966be55b333d307438a67e07cf6534b45c642df4a83a7dd022056a6dd25df0d1de94d232e68d33fec0171dffd5f7bef264cf379e3f8a962d355'],
                    ['4b06387d492c935e2b1d63d15baaf5aa3ccff13e5275c1311914969f67833756',
                        '3046022100f799d5961ce2ad769d605e7834dd64e14766b23b51f0310f150bf2064547cbc8022100caae88bbc104e6589727b6a70cae511a610f456e487f6482eb819138d1d7c978'],
                    ['4b06387d492c935e2b1d63d15baaf5aa3ccff13e5275c1311914969f67833756',
                        '30440220651892d5a9c0560447d0d5e526d245f1007cce63bb19a1568b30e2d85a68652f02203476fd6e2798acfb7f39986d7c9afdb3b5461dfb9e366b044ad1bf6693e8101e'],
                ]

                test_data.forEach(d => assert.ok(pk.verify(new Buffer(d[0], 'hex'), new Buffer(d[1], 'hex'))));
            });
        });

        describe("SM2", () => {
            it("PEM import/export", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);
                assert.equal(pk.pem(), sm2_pem);

                pk = new crypto.PKey(sm2_pem);
                assert.equal(pk.pem(), sm2_pem);
            });

            it("toString", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);
                assert.equal(pk, sm2_pem);
            });

            it("Der import/export", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);
                var der = pk.der();
                pk.import(der);
                assert.equal(pk.pem(), sm2_pem);

                pk = new crypto.PKey(der);
                assert.equal(pk.pem(), sm2_pem);
            });

            it("Json import/export", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);
                var json = pk.json();

                assert.deepEqual(json, {
                    "kty": "EC",
                    "crv": "SM2",
                    "x": "1KnIoMvdNODUrcEzQNnHbplwxNNyuHwIUnU0oNQ_0R0",
                    "y": "c_e2CHv5PB1-sKzzFGs30tT1XfwPUfIGnb5VEUN5bn4",
                    "d": "fcRRalaycsaXpKQYGcbmU8Qi93KqXnpodAwIK3vEOoI"
                });

                pk.import(json);
                assert.equal(pk.pem(), sm2_pem);

                pk.import({
                    "kty": "EC",
                    "crv": "SM2",
                    "d": "fcRRalaycsaXpKQYGcbmU8Qi93KqXnpodAwIK3vEOoI"
                });
                assert.equal(pk.pem(), sm2_pem);

                var pk = new crypto.PKey();
                pk.import(pub_sm2_pem);
                var json = pk.json();

                assert.deepEqual(json, {
                    "kty": "EC",
                    "crv": "SM2",
                    "x": "1KnIoMvdNODUrcEzQNnHbplwxNNyuHwIUnU0oNQ_0R0",
                    "y": "c_e2CHv5PB1-sKzzFGs30tT1XfwPUfIGnb5VEUN5bn4",
                });

                pk.import(json);
                assert.equal(pk.pem(), pub_sm2_pem);

                var pk = new crypto.PKey(json);
                assert.equal(pk.pem(), pub_sm2_pem);
            });

            it("import publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(pub_sm2_pem);
                assert.isFalse(pk.isPrivate());

                assert.equal(pk, pub_sm2_pem);

                var pk1 = new crypto.PKey();
                pk1.import(pk.der());
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1.pem(), pub_sm2_pem);
            });

            it("publicKey", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);
                assert.isTrue(pk.isPrivate());

                var pk1 = pk.publicKey;
                assert.isFalse(pk1.isPrivate());

                assert.equal(pk1, pub_sm2_pem);
            });

            it("clone", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);

                var pk1 = pk.clone();

                assert.equal(pk1.pem(), pk.pem());
            });

            it("gen_key", () => {
                var pk = crypto.generateKey("SM2");
                var pk1 = crypto.generateKey("SM2");

                assert.notEqual(pk.pem(), pk1.pem());
            });


            it("equal", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);

                var pk1 = pk.clone();

                assert.ok(pk1.equal(pk));

                var pk = crypto.generateKey("SM2");
                var pk1 = crypto.generateKey("SM2");

                assert.notOk(pk1.equal(pk));
            });

            it("sign/verify", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);

                var pk1 = pk.publicKey;

                var md = hash.md5("abcdefg").digest();
                var md1 = hash.md5("abcdefg1").digest();
                var d = pk.sign(md);
                assert.isTrue(pk1.verify(md, d));
                assert.isFalse(pk1.verify(md1, d));

                assert.throws(() => {
                    pk1.sign(md);
                });
            });

            it("encrypt/decrypt", () => {
                var pk = new crypto.PKey();
                pk.import(sm2_pem);

                var pk1 = pk.publicKey;

                var d = pk1.encrypt("abcdefg");
                assert.equal(pk.decrypt(d).toString(), "abcdefg");

                assert.throws(() => {
                    pk1.decrypt(d);
                });
            });

            it("sign/verify with same key", () => {
                var pk = crypto.generateKey("SM2");
                var pk1 = new crypto.PKey();
                var json = pk.json();
                pk1.import(json);

                var md = hash.md5("abcdefg").digest();
                var d = pk.sign(md);
                assert.isTrue(pk1.verify(md, d));
            });

            it("sign/verify to", () => {
                var pk = crypto.generateKey("SM2");
                var pk1 = crypto.generateKey("SM2");

                var sig = pk.sign('abc', pk1.publicKey);
                assert.isTrue(pk.publicKey.verify('abc', sig, pk1));
                assert.isFalse(pk.publicKey.verify('abcd', sig, pk1));
                assert.isFalse(pk.publicKey.verify('abc', sig, pk));
            });

            it("ecsdsa sign/verify to", () => {
                var pk = crypto.generateKey("SM2");
                var pk1 = crypto.generateKey("SM2");
                pk.sigType = 'ecsdsa';

                var sig = pk.sign('abc', pk1.publicKey);
                assert.isTrue(pk.publicKey.verify('abc', sig, pk1));
                assert.isFalse(pk.publicKey.verify('abcd', sig, pk1));
                assert.isFalse(pk.publicKey.verify('abc', sig, pk));
            });
        });

        it("ECDH", () => {
            var alice = crypto.generateKey('secp256r1');
            var bob = crypto.generateKey('secp256r1');
            var aliceSecret = alice.computeSecret(bob);
            var bobSecret = bob.computeSecret(alice);

            assert.deepEqual(aliceSecret, bobSecret);

            var alice = crypto.generateKey('secp256k1');
            var bob = crypto.generateKey('secp256k1');
            var aliceSecret = alice.computeSecret(bob);
            var bobSecret = bob.computeSecret(alice);

            assert.deepEqual(aliceSecret, bobSecret);

            var alice = crypto.generateKey("SM2");
            var bob = crypto.generateKey("SM2");
            var aliceSecret = alice.computeSecret(bob);
            var bobSecret = bob.computeSecret(alice);

            assert.deepEqual(aliceSecret, bobSecret);

            var alice = new crypto.PKey({
                "kty": "EC",
                "crv": "secp256k1",
                "d": "D8VXPuvb2ROceGlswZ-TAao40WZvnbXw0sDOqQKoye8"
            });
            var bob = new crypto.PKey({
                "kty": "EC",
                "crv": "secp256k1",
                "d": "3S1mHUDELnyOtglI_3-cEJYoKWcVIAvixHEU5-SGCGU"
            });

            assert.equal("b2885b8ed48ad77ae3a531c64b85a2fcb08196e93cc13a4f783cc90674ed764a",
                bob.computeSecret(alice).hex());
        });

        it("name", () => {
            var pk = new crypto.PKey();

            assert.equal(pk.name, "invalid PK");

            pk.import(rsa4096_pem);
            assert.equal(pk.name, "RSA");

            pk.import(ec_pem);
            assert.equal(pk.name, "EC");

            pk.import(sm2_pem);
            assert.equal(pk.name, "SM2");
        });

        it("keySize", () => {
            var pk = new crypto.PKey();

            assert.equal(pk.keySize, 0);

            pk.import(rsa4096_pem);
            assert.equal(pk.keySize, 4096);

            pk.import(ec_pem);
            assert.equal(pk.keySize, 521);

            pk.import(sm2_pem);
            assert.equal(pk.keySize, 256);
        });

        it("curve", () => {
            var pk = new crypto.PKey();

            assert.equal(pk.curve, '');

            pk.import(rsa4096_pem);
            assert.equal(pk.curve, '');

            pk.import(ec_pem);
            assert.equal(pk.curve, 'P-521');

            pk.import(sm2_pem);
            assert.equal(pk.curve, 'SM2');
        });
    });

    describe("ecsdsa", () => {
        ecsdsa_case.forEach(c => {
            describe(c.key.crv, () => {
                it("verify", () => {
                    var pk = new crypto.PKey(c.key);
                    pk.sigType = 'ecsdsa';

                    var sig = Buffer.from(c.sig, 'base64');
                    assert.ok(pk.publicKey.verify('abc', sig));
                });

                it("sign/verify", () => {
                    var pk = new crypto.PKey(c.key);
                    pk.sigType = 'ecsdsa';

                    var sig = pk.sign('abc');
                    assert.ok(pk.publicKey.verify('abc', sig));
                });

                it("sign/verify to", () => {
                    var pk = new crypto.PKey(c.key);
                    pk.sigType = 'ecsdsa';

                    var pk1 = crypto.generateKey(c.key.crv);

                    var sig = pk.sign('abc', pk1.publicKey);
                    assert.isTrue(pk.publicKey.verify('abc', sig, pk1));
                    assert.isFalse(pk.publicKey.verify('abcb', sig, pk1));
                    assert.isFalse(pk.publicKey.verify('abc', sig, pk));
                });
            });
        });

        describe("sm2", () => {
            it("verify", () => {
                var pk = new crypto.PKey({
                    "kty": "EC",
                    "crv": "SM2",
                    "d": "fcRRalaycsaXpKQYGcbmU8Qi93KqXnpodAwIK3vEOoI"
                });
                pk.sigType = 'ecsdsa';

                var sig = Buffer.from("MEUCIQDhT4yilg4Y7iGmUFM8CfJ3VQCdqibyNNjjiQh2m9vM9AIgB7UETe0t9gw8Ga1sPMjFxgKYDVdeuPy7EZXanKM2VUo", 'base64');
                assert.ok(pk.publicKey.verify('abc', sig));
            });

            it("sign/verify", () => {
                var pk = new crypto.PKey({
                    "kty": "EC",
                    "crv": "SM2",
                    "d": "fcRRalaycsaXpKQYGcbmU8Qi93KqXnpodAwIK3vEOoI"
                });
                pk.sigType = 'ecsdsa';

                var sig = pk.sign('abc');
                assert.ok(pk.publicKey.verify('abc', sig));
            });

            it("sign/verify to", () => {
                var pk = new crypto.PKey({
                    "kty": "EC",
                    "crv": "SM2",
                    "d": "fcRRalaycsaXpKQYGcbmU8Qi93KqXnpodAwIK3vEOoI"
                });
                pk.sigType = 'ecsdsa';

                var pk1 = crypto.generateKey("SM2");

                var sig = pk.sign('abc', pk1.publicKey);
                assert.isTrue(pk.publicKey.verify('abc', sig, pk1));
                assert.isFalse(pk.publicKey.verify('abcb', sig, pk1));
                assert.isFalse(pk.publicKey.verify('abc', sig, pk));
            });
        });
    });

    describe("X509 Cert", () => {
        var cert = new crypto.X509Cert();

        it("load", () => {
            var fl = fs.readdir(path.join(__dirname, 'cert_files'));
            fl.forEach((s) => {
                if (s.match(/\.crt/))
                    cert.import(fs.readTextFile(path.join(__dirname, 'cert_files', s)));
            });
        });

        it("clear/export", () => {
            var s = cert.pem();
            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.import(s);

            assert.deepEqual(cert.pem(), s);

            var cert1 = new crypto.X509Cert(cert.pem(false));
            assert.deepEqual(cert1.pem(), cert.pem(false));

            var cert2 = new crypto.X509Cert(cert1.pem(false));
            assert.deepEqual(cert2.pem(), cert.pem(false));
        });

        it("load file", () => {
            var s = cert.pem();
            cert.clear();
            assert.deepEqual(cert.pem(), "");

            var fl = fs.readdir(path.join(__dirname, 'cert_files'));
            fl.forEach((s) => {
                if (s.match(/\.crt/))
                    cert.import(fs.readFile(path.join(__dirname, 'cert_files', s), 'utf-8'));
            });

            assert.deepEqual(cert.pem(), s);
        });

        it("certdata.txt", () => {
            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.import(fs.readTextFile(path.join(__dirname, 'cert_files', 'certdata.txt')));
            var s = cert.pem();
            assert.notDeepEqual(s, "");

            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.import(fs.readTextFile(path.join(__dirname, 'cert_files', 'ca-bundle.crt')));
            var s1 = cert.pem();

            assert.deepEqual(s.slice(s.length - s1.length), s1);
        });

        it("root ca", () => {
            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.import(fs.readTextFile(path.join(__dirname, 'cert_files', 'ca-bundle.crt')));
            var s = cert.pem();

            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.loadRootCerts();
            var s1 = cert.pem();

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
            assert.deepEqual(cert.pem(), "");

            cert.loadRootCerts();
            var cnt1 = count(cert);

            cert.loadRootCerts();
            assert.equal(cnt1, count(cert));

            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.loadRootCerts();
            assert.equal(cnt1, count(cert));
        });

        it("unknown format", () => {
            assert.throws(() => {
                cert.import(path.join(__dirname, 'cert_files', 'certdata.txt'));
            });
        });

        it("info", () => {
            cert.clear();
            assert.deepEqual(cert.pem(), "");

            cert.import(ca1);
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

            cert.import(ca2);

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

            cert.import(ca3);

            var cert2 = cert1.next;
            assert.equal(cert2.issuer, "C=CN, O=baoz.cn");
            assert.equal(cert2.subject, "C=CN, O=baoz.cn, CN=baoz.me");
            assert.deepEqual(cert2.notBefore, new Date("Tue Dec 03 2019 15:01:58 GMT+0800 (CST)"));
            assert.deepEqual(cert2.notAfter, new Date("Thu Dec 03 2020 15:01:58 GMT+0800 (CST)"));
            assert.equal(cert2.ca, false);
            assert.equal(cert2.pathlen, 0);
            assert.equal(cert2.usage, "");
            assert.equal(cert2.type, "");
            assert.equal(cert2.publicKey, pub_sm2_pem);
        });
    });

    describe("X509 Crl", () => {
        var crl = new crypto.X509Crl();

        it("load", () => {
            var fl = fs.readdir(path.join(__dirname, 'crl_files'));
            fl.forEach((s) => {
                if (s.match(/\.pem/))
                    crl.import(fs.readTextFile(path.join(__dirname, 'crl_files', s)));
            });
        });

        it("clear/export", () => {
            var s = crl.pem();

            crl.clear();
            assert.deepEqual(crl.pem(), "");

            crl.import(s);

            assert.deepEqual(crl.pem(), s);

            var crl1 = new crypto.X509Crl(s);
            assert.deepEqual(crl1.pem(), s);

            var crl2 = new crypto.X509Crl(crl1.pem(false));
            assert.deepEqual(crl2.pem(), crl1.pem(false));
        });

        it("load x509 crl file", () => {
            var s = crl.pem();
            crl.clear();

            var fl = fs.readdir(path.join(__dirname, 'crl_files'));
            fl.forEach((s) => {
                if (s.match(/\.pem/))
                    crl.import(fs.readFile(path.join(__dirname, 'crl_files', s), 'utf-8'));
            });

            assert.deepEqual(crl.pem(), s);
        });

    });

    describe("X509 Req", () => {
        var req = new crypto.X509Req();
        var fl = fs.readdir(path.join(__dirname, 'req_files'));
        fl.forEach((s) => {
            it("load", () => {
                if (s.match(/\.req/))
                    req.import(fs.readTextFile(path.join(__dirname, 'req_files', s)));
            });

            it("toString", () => {
                assert.equal(req.pem(), req);
            });

            it("import/export pem", () => {
                var s = req.pem();

                var req1 = new crypto.X509Req();
                req1.import(s);

                assert.equal(req1.pem(), s);
                assert.equal(new crypto.X509Req(s).pem(), s);
            });

            it("import/export der", () => {
                var s = req.der();

                var req1 = new crypto.X509Req();
                req1.import(s);

                assert.deepEqual(req1.der(), s);
                assert.deepEqual(new crypto.X509Req(s).der(), s);
            });
        });

        it("create", () => {
            var pk = new crypto.PKey();
            pk.import(rsa4096_pem);

            req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk);
            assert.deepEqual(req.pem(), req1);
        })

        it("info", () => {
            req.import(req1);
            assert.equal(req.subject, "C=CN, O=baoz.cn, CN=baoz.me");
            assert.equal(req.publicKey, pub_rsa4096_pem);
        });

        it("sm2 info", () => {
            req.import(sm2_req);
            assert.equal(req.subject, "C=CN, O=baoz.cn, CN=baoz.me");
            assert.equal(req.publicKey, pub_sm2_pem);
        });

    });

    describe("CA sign/verify", () => {
        var req;
        var ca;
        var pk;

        function ca_test(private_pem, public_pem, md_alg) {
            it("create", () => {
                pk = new crypto.PKey();
                pk.import(private_pem);
                req = new crypto.X509Req("C=CN, O=baoz.cn, CN=baoz.me", pk, md_alg);
            });

            it("sign", () => {
                var cert = req.sign("C=CN, O=baoz.cn", pk);
                assert.equal(cert.issuer, "C=CN, O=baoz.cn");
                assert.equal(cert.subject, "C=CN, O=baoz.cn, CN=baoz.me");
                assert.equal(cert.publicKey, public_pem);
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
        }
        ca_test(rsa4096_pem, pub_rsa4096_pem, hash.SHA256);
        // ca_test(sm2_pem, pub_sm2_pem, hash.SM3);
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
            ],
            [hash.SM3, 'password', 'salt', 1, 20,
            hex.decode('e974b457cc9790eead63a178dc5e3dd861d2a36d'),
                'sm3'
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

    it("getHashes", () => {
        var hashes = crypto.getHashes();
        assert.isArray(hashes);

        assert.greaterThan(hashes.length, 1);

        assert.isTrue(hashes.includes('md5'))
        assert.isTrue(hashes.includes('sha384'))
    });

    it("FIX: Illegal iterations and size parameters will cause crypto.pbkdf1 to crash", () => {
        assert.throws(() => {
            crypto.pbkdf1(null, null, 0, -1, 1);
        })
    });

    it("FIX: Illegal iterations and size parameters will cause crypto.pbkdf2 to crash", () => {
        assert.throws(() => {
            crypto.pbkdf2(null, null, 0, -1, 1);
        })
    });
});

require.main === module && test.run(console.DEBUG);