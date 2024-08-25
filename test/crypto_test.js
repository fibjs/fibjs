var test = require("test");
test.setup();

var crypto = require("crypto");
var fs = require("fs");
var os = require("os");
var encoding = require("encoding");
var hex = require("hex");
var base64 = require("base64");
var path = require("path");

var rsa4096_pem = "-----BEGIN RSA PRIVATE KEY-----\n" +
    "MIIJJwIBAAKCAgEAiAUwIJyWHEnusONGDVCguTr3FkkVoSiDJ2mmFYYibt1paXpI\n" +
    "iEKQy+OwykFcOG0Ew9UGEEvveRQbKN+bpUV5uH6q7pgnfDubA2VSLST7lHcXJd2a\n" +
    "s4DNyFMtgXLOj0dVSk4NduWlVgY2fUu17sJJBmysYjCjmB76P92DLfIvlI+4OYEO\n" +
    "FSLQCG56KA8ZvP9HTnHiI0Lnerjf14cjr1Ubgw0ce9KMJ96LLnZp9Q6hBCXalagD\n" +
    "o9MofuyScpFX1X0ixirQ2VtomzTvle9EIdfmat6EoLzxz8YNfl6mllvCVrlFJw1D\n" +
    "vXfxLlkDR9I+te1B89L7Eljt90VBKmzjjJiKmuhmW8HhJMpe3VBG7t8c3J+F/cJL\n" +
    "x7ZDk6/XNhxWPbzL9SNhTRg6015YDsX01bFyi1GUHdFocPmzgZ6nw3anFT85+q7l\n" +
    "9M4yGhRQbfUWQ4rT0zCFobJZQeO4ZyiBM0fuAGhln0MogIdFaoh7xmVggL6XxJVO\n" +
    "ApXXGV7Z8xDjzbLpBI2UHiX8W33ZbrXYiDexvfBQuR2SFw372h/LCF+fOI8kxZr9\n" +
    "BNU1RSf26RCX0Mi0Fdbz0eeXLa3N42fGAxHYUCN5M4WEtAPcdQjQAZ7gZ+Voy9nN\n" +
    "jMoQMWzzH84XHUI1byA8T795CnVmKMiHyCWEnOGV1ptmZrzoZoa6x6bOHukCAwEA\n" +
    "AQKCAgAt7qqQ6K/cP68l9qmqR2bvBhV4Zheyw05+2bNnI1OvYn99SfkCYpAo+euy\n" +
    "cR3mRBJO2znf+PSWS47cX55c0jhHcCbiyWjUEADJxcZZTlrSiOC9PbdHFKDpHAWK\n" +
    "6C0Ov0TzAaaUVvpdMV3nM9DtHpiwXbRcuBYWgad6N6BbvKQKo/nPGWCbDJV5s2z2\n" +
    "6BqF8yKuP9DgnXAzxgh47L7FL5VJzPyoZ7JCqGkRuJURVu5Sy0GkNgd8pArRyauu\n" +
    "mpwxkL9UchLyw2ZwuxCI7jJwx0qdHiVlbkiuJDBcUzQoJDZdONU8WcNdUQqV3yM3\n" +
    "dq0MnGEgKEf2qyc40B8wZ3L0AHMVXISfuQnizwtqzj5T6ka96NlOWT7y4MCswD69\n" +
    "AriiNMkdJJhLlqt7kRkKXGxHhdDcKdHR6usK1J3Jui3WCh14y61Ee1AZVR9UjgfJ\n" +
    "JG+L1yHvqIDeInAXPdJlHipxHKGOQXX/Ii+0S7i6oV07K/CQkTVCvyLYY+Iqpi/9\n" +
    "GvtVGSBa795dxD6QUtmM1I6RJ5Jdu/zjtmjvlfgNCqd31JOGhmNekwIeKjF4h3ZT\n" +
    "xTb/wUYm25vs+SRaDM79jj7MwI1j/lrXtE/XIt+0HLYF0tmcLsLlE9b7CZXxCGMX\n" +
    "mAzWQAUa9H0Pe0Imven4+BgI9A8nhnjlc92SZRGdCGUdkJMpvQKCAQEAvpZmPuu9\n" +
    "eS25yMcYKQwtwouShk8C+VCexKymleuslN7riCemy3JZhkRkd/+InaQ+bDS93CfC\n" +
    "U4p//unoFjoA6rKjBqPyIGnUiZOPFNukVBz4dfKoiScYtTEU0cy8e/a7DEcePOXU\n" +
    "edzK6HuDkQqvsSU0Xoy55/KdipClQ37/H+IR3Xon6I17lAQ8x4sl8CILKrmeZeHC\n" +
    "o9ve6R2L3bUz2lacuxlZIff6QeVPmLWTKCOviFh5nmwI+IdzX5nI0WOjPH8s4EOg\n" +
    "5SyY1NWAq0u75gjbH4K5ljByNolNJ1ErdheQ++KNcL/60q3wOlRxyXOd18i2AFhn\n" +
    "IPyH1XT2JiRLJwKCAQEAtrRYO3zWY5Hv7kB1aPvBXUVK/UbPf9Hv8g4IQtrOod3c\n" +
    "n69ni/bBNW5TbWcdO6+lk3ag5qG/deexPe6Zi/LYymaO7noypYnhX/r8aqwYg5se\n" +
    "NVtcawAxazdAkIY5wRin8BB6MpDW7ZkKe/44DuGdiJjRSbgGGZdGshjYJle5ImyL\n" +
    "Yz5YiIxsSqdzCAwCAHMxSASbCzLSlQtkHGbrSexjd9WWIAXlmndRezut5tpx0Qhs\n" +
    "HeAhRSOoMaAI/2ORAPu7JQ+t+p2txNUkb+tvyWx/Y5g02ghk9Tx/2ogJj4d3DnDF\n" +
    "rnRjQPkw8wBSiQeihNQ2w1+hY6Vp8QjqJ99XrWvPbwKCAQBzrNM4A1sHzpIBY52t\n" +
    "ZD/t4Uq4jAIxEhabJnMhcV5ZKQfppq9csfXWQXJ9RrJDg+mDQWuu0M7oZ5qSPKPp\n" +
    "waDG0k+SezU9KR6ftdjU/w65IrBG3lYekIU1jCmnwMzxrGoQ5KVcrt4MbM8W1cif\n" +
    "4s2KBhvH+z/moOGvtWUpF05/qnXPaBIK2ryTtRKyxijrm64Na4XRLno1fADKmm1G\n" +
    "znE/ZuxPvu3TaTASiLyL4kEDzsz+Qzzlw4qaVZPYBX3WYNQ1pra1Ezb3gd7s5vzm\n" +
    "etv7nYppkK1Vk+fuY91ZLLHlRh5wUywnr83ryziQHqrFa4/05VRZ104Yvk0vaILx\n" +
    "2XtnAoIBABc+pqMa5OPSI2Z6iPWMYjONp1bF9Yxbl12Cb4ah8/wgD/u5A0GJaVd0\n" +
    "6+RVZCC93iD+zMXK+mLz0VcBzkNB6FcH6VNp9JufCS9+7TJdJtlHmPZM41sxsRfk\n" +
    "9a/tg9ePXX5rekCIsO+VswHHfDZYegRw/N+WLCCBYJs0Efv+2S0KBkngGr90ripp\n" +
    "V3dEn9SS00hIXbl1A9m2k0wRxAmpq5YJdIoqdeXLHHDDTQK79BMWMTsz0sfLG5EE\n" +
    "vWBQuJgZbtvuPMJP4VXTkEk0+Qn1Zdez+7Vpr/6LbfDNsfmy0HDj5CKn6CDSE2o3\n" +
    "Nkj80a54mWcN+aiF2Q6zhacBAVStDL0CggEAFxeLSz+9JNnPO/rlHl1ZKfZZ4Nt/\n" +
    "Ys7LhCoZ5Gz4TjlUN3LDpYe8EoeW+0NDo+vsWPM8jDVGBp/OawaXxti3ruqyW8dS\n" +
    "BGP4x0BgoV5I8nA3LWyRszw02nWRm99TFGqeM/wzGyoVLfJ+KnK0J/HXSgIeJMw9\n" +
    "yV+l23/N9OyoBjo3d1zuPMAK6BDqPxcIXR+Sp+q84HU3WN1UHk1uRbSasUGGsFND\n" +
    "IkmCaMpHFaTuv5tgqUV0ljGlh1zG616zKkNaYRiRK2h4JGAu6O+xdQFUjsUVaCPi\n" +
    "6BtFtiscxqvXU4dcYmosNMhtTwjHaWhhC8877i2gZ97Vum2RzMz3xi+++g==\n" +
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

var req2 = "-----BEGIN CERTIFICATE REQUEST-----\n" +
    "MIIEdjCCAl4CAQEwMTELMAkGA1UEBhMCQ04xEDAOBgNVBAoMB2Jhb3ouY24xEDAO\n" +
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
    "AQELBQADggIBAGiHM/lXJGlP9ZEpxz6AZZRsJDg76JNG0msdZ61eQzwJMuTM2Dqj\n" +
    "EiLPcKbYXRE13hhQ2MOLmsGBo8K3u1xfygYtTwR5ux9TliVCEftuMfCDN5EW57K5\n" +
    "MSdLNZEpLsNqVo7J7r37uR71Qj9o9H6hUMSEQke6HJsamK3n9YFj8/DtOzc9OkHS\n" +
    "Xqobx6sipX5vEncpPh5VYGsi+XnRTn7TCn8lSC3Q14CC9kSf3sJ3bOgDhBFMNPc6\n" +
    "Msar7o+TXD7dCqHlDh542++mkBSS8G0+BJsnsiPLNfgve4+whHnR3O9JRNhoShtY\n" +
    "pVUlBzlmiRVL62jU1q/FUOMZGzDC2fZCYDbd/1dxexFGe3lK+bTz73rHBYx7uUdQ\n" +
    "aTb4vZnBqGvDv3FWpX0+bDtKlP+KfVM5goqY38/kBA4aHNEnBjj0usZqyd27Sa2D\n" +
    "kcdOGTMn8fYpZr2MZyigP68gRfdHlWs0ifYJz/8qkP36DXfLUZ7HzTKky7Bk9znU\n" +
    "/O7bioOcMGyoCPnIs3RaKEkyH2fVEnx+WgtzLoQNbl7BXiRYIlVWc67Vq0VSpyIm\n" +
    "VG0w1246rTKctJlSSscexrhd4ZDydRjtAci8CxvktJbcSCXZ1xbY06D94SjNdHwr\n" +
    "70bSxKA8pkQFV+EYhYetOTFL3tqnru+YNoNY4crDjTTxaiBphn6KeUNw\n" +
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

function readKey(name, enc) {
    return fs.readFileSync(path.join(__dirname, "crypto_case", 'keys', name), enc);
}

function readSync(name, enc) {
    return fs.readFileSync(path.join(__dirname, "..", "test", "crypto_case", name), enc);
}

const publicPem = readKey('rsa_public.pem', 'ascii');
const privatePem = readKey('rsa_private.pem', 'ascii');

const publicDsa = readKey('dsa_public_1025.pem', 'ascii');
const privateDsa = readKey('dsa_private_encrypted_1025.pem', 'ascii');

const jwk = {
    e: 'AQAB',
    n: 't9xYiIonscC3vz_A2ceR7KhZZlDu_5bye53nCVTcKnWd2seY6UAdKersX6njr83Dd5OVe' +
        '1BW_wJvp5EjWTAGYbFswlNmeD44edEGM939B6Lq-_8iBkrTi8mGN4YCytivE24YI0D4XZ' +
        'MPfkLSpab2y_Hy4DjQKBq1ThZ0UBnK-9IhX37Ju_ZoGYSlTIGIhzyaiYBh7wrZBoPczIE' +
        'u6et_kN2VnnbRUtkYTF97ggcv5h-hDpUQjQW0ZgOMcTc8n-RkGpIt0_iM_bTjI3Tz_gsF' +
        'di6hHcpZgbopPL630296iByyigQCPJVzdusFrQN5DeC-zT_nGypQkZanLb4ZspSx9Q',
    d: 'ktnq2LvIMqBj4txP82IEOorIRQGVsw1khbm8A-cEpuEkgM71Yi_0WzupKktucUeevQ5i0' +
        'Yh8w9e1SJiTLDRAlJz66kdky9uejiWWl6zR4dyNZVMFYRM43ijLC-P8rPne9Fz16IqHFW' +
        '5VbJqA1xCBhKmuPMsD71RNxZ4Hrsa7Kt_xglQTYsLbdGIwDmcZihId9VGXRzvmCPsDRf2' +
        'fCkAj7HDeRxpUdEiEDpajADc-PWikra3r3b40tVHKWm8wxJLivOIN7GiYXKQIW6RhZgH-' +
        'Rk45JIRNKxNagxdeXUqqyhnwhbTo1Hite0iBDexN9tgoZk0XmdYWBn6ElXHRZ7VCDQ',
    p: '8UovlB4nrBm7xH-u7XXBMbqxADQm5vaEZxw9eluc-tP7cIAI4sglMIvL_FMpbd2pEeP_B' +
        'kR76NTDzzDuPAZvUGRavgEjy0O9j2NAs_WPK4tZF-vFdunhnSh4EHAF4Ij9kbsUi90NOp' +
        'bGfVqPdOaHqzgHKoR23Cuusk9wFQ2XTV8',
    q: 'wxHdEYT9xrpfrHPqSBQPpO0dWGKJEkrWOb-76rSfuL8wGR4OBNmQdhLuU9zTIh22pog-X' +
        'PnLPAecC-4yu_wtJ2SPCKiKDbJBre0CKPyRfGqzvA3njXwMxXazU4kGs-2Fg-xu_iKbaI' +
        'jxXrclBLhkxhBtySrwAFhxxOk6fFcPLSs',
    dp: 'qS_Mdr5CMRGGMH0bKhPUWEtAixUGZhJaunX5wY71Xoc_Gh4cnO-b7BNJ_-5L8WZog0vr' +
        '6PgiLhrqBaCYm2wjpyoG2o2wDHm-NAlzN_wp3G2EFhrSxdOux-S1c0kpRcyoiAO2n29rN' +
        'Da-jOzwBBcU8ACEPdLOCQl0IEFFJO33tl8',
    dq: 'WAziKpxLKL7LnL4dzDcx8JIPIuwnTxh0plCDdCffyLaT8WJ9lXbXHFTjOvt8WfPrlDP_' +
        'Ylxmfkw5BbGZOP1VLGjZn2DkH9aMiwNmbDXFPdG0G3hzQovx_9fajiRV4DWghLHeT9wzJ' +
        'fZabRRiI0VQR472300AVEeX4vgbrDBn600',
    qi: 'k7czBCT9rHn_PNwCa17hlTy88C4vXkwbz83Oa-aX5L4e5gw5lhcR2ZuZHLb2r6oMt9rl' +
        'D7EIDItSs-u21LOXWPTAlazdnpYUyw_CzogM_PN-qNwMRXn5uXFFhmlP2mVg2EdELTahX' +
        'ch8kWqHaCSX53yvqCtRKu_j76V31TfQZGM',
    kty: 'RSA',
};
const publicJwk = { kty: jwk.kty, e: jwk.e, n: jwk.n };

describe('crypto', () => {

    it("randomBytes", () => {
        assert.notEqual(crypto.randomBytes(8).hex(), crypto.randomBytes(8).hex());

        assert.throws(() => {
            crypto.randomBytes(-125);
        });
    });

    it("randomFill", () => {
        var buf = Buffer.alloc(10);
        var before = buf.toString('hex');
        var buf1 = crypto.randomFill(buf, 5, 5);
        var after = buf.toString('hex');
        // assert.equal(buf, buf1);
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

    describe('KeyObject', () => {
        describe('createSecretKey', () => {
            it('normal', () => {
                const keybuf = crypto.randomBytes(32);
                const key = crypto.createSecretKey(keybuf);

                assert.strictEqual(key.type, 'secret');
                assert.strictEqual(key.toString(), '[object KeyObject]');
                assert.strictEqual(key.symmetricKeySize, 32);
                assert.strictEqual(key.asymmetricKeyType, undefined);
                assert.strictEqual(key.asymmetricKeyDetails, undefined);

                const exportedKey = key.export();
                assert.ok(keybuf.equals(exportedKey));
            });

            it('encoding', () => {
                const buffer = Buffer.from('Hello World');
                const key1 = crypto.createSecretKey(buffer);
                const key2 = crypto.createSecretKey('Hello World');

                assert.ok(key1.export().equals(key2.export()));
            });

            it('export', () => {
                const buffer = Buffer.from('Hello World');
                const keyObject = crypto.createSecretKey(buffer);
                assert.deepEqual(keyObject.export(), buffer);
                assert.deepEqual(keyObject.export({}), buffer);
                assert.deepEqual(keyObject.export({ format: 'buffer' }), buffer);
                assert.deepEqual(keyObject.export({ format: undefined }), buffer);
            });

            it('export jwk', () => {
                const buffer = Buffer.from('Hello World');
                const keyObject = crypto.createSecretKey(buffer);
                assert.deepEqual(
                    keyObject.export({ format: 'jwk' }),
                    { kty: 'oct', k: 'SGVsbG8gV29ybGQ' }
                );
            });

            it('equals', () => {
                const first = Buffer.from('Hello');
                const second = Buffer.from('World');

                const keyObject = crypto.createSecretKey(first);
                assert.ok(crypto.createSecretKey(first).equals(crypto.createSecretKey(first)));
                assert.ok(!crypto.createSecretKey(first).equals(crypto.createSecretKey(second)));

                assert.throws(() => keyObject.equals(0));

                assert.ok(keyObject.equals(keyObject));
            });

            it('equals with empty key', () => {
                const first = crypto.createSecretKey(Buffer.alloc(0));
                const second = crypto.createSecretKey(new ArrayBuffer(0));
                const third = crypto.createSecretKey(Buffer.alloc(1));
                assert.ok(first.equals(first));
                assert.ok(first.equals(second));
                assert.ok(!first.equals(third));
                assert.ok(!third.equals(first));
            });
        });

        describe('createPublicKey/createPrivateKey', () => {
            it('instanceof', () => {
                const publicKey = crypto.createPublicKey(publicPem);
                const privateKey = crypto.createPrivateKey(privatePem);

                assert.ok(publicKey instanceof crypto.KeyObject);
                assert.ok(privateKey instanceof crypto.KeyObject);
            });

            it('check arguments', () => {
                const publicKey = crypto.createPublicKey(publicPem);
                const publicKey1 = crypto.createPublicKey(publicKey);

                assert.throws(() => crypto.createPrivateKey(crypto.createPublicKey(privatePem)));

                const privateKey = crypto.createPrivateKey(privatePem);
                assert.throws(() => crypto.createPrivateKey(privateKey));

                for (const key of ['', 'foo', null, undefined, true, Boolean]) {
                    assert.throws(() => createPublicKey({ key, format: 'jwk' }));
                    assert.throws(() => createPrivateKey({ key, format: 'jwk' }));
                }

                assert.throws(() => {
                    createPrivateKey({ key: '' });
                });

                assert.throws(() => {
                    createPrivateKey({ key: Buffer.alloc(0), format: 'der', type: 'spki' });
                });

                assert.throws(() => {
                    const key = createPublicKey(publicPem).export({
                        format: 'der',
                        type: 'pkcs1'
                    });
                    createPrivateKey({ key, format: 'der', type: 'pkcs1' });
                });
            });

            it('create with pem', () => {
                const publicKey = crypto.createPublicKey(publicPem);
                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.toString(), '[object KeyObject]');
                assert.equal(publicKey.asymmetricKeyType, 'rsa');
                assert.equal(publicKey.symmetricKeySize, undefined);

                const privateKey = crypto.createPrivateKey(privatePem);
                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.toString(), '[object KeyObject]');
                assert.equal(privateKey.asymmetricKeyType, 'rsa');
                assert.equal(privateKey.symmetricKeySize, undefined);
            });

            it('create public key from private key', () => {
                const privateKey = crypto.createPrivateKey(privatePem);
                const derivedPublicKey = crypto.createPublicKey(privateKey);
                assert.equal(derivedPublicKey.type, 'public');
                assert.equal(derivedPublicKey.toString(), '[object KeyObject]');
                assert.equal(derivedPublicKey.asymmetricKeyType, 'rsa');
                assert.equal(derivedPublicKey.symmetricKeySize, undefined);
            });

            it('create with jwk', () => {
                const publicKeyFromJwk = crypto.createPublicKey({ key: publicJwk, format: 'jwk' });
                assert.equal(publicKeyFromJwk.type, 'public');
                assert.equal(publicKeyFromJwk.toString(), '[object KeyObject]');
                assert.equal(publicKeyFromJwk.asymmetricKeyType, 'rsa');
                assert.equal(publicKeyFromJwk.symmetricKeySize, undefined);

                const privateKeyFromJwk = crypto.createPrivateKey({ key: jwk, format: 'jwk' });
                assert.equal(privateKeyFromJwk.type, 'private');
                assert.equal(privateKeyFromJwk.toString(), '[object KeyObject]');
                assert.equal(privateKeyFromJwk.asymmetricKeyType, 'rsa');
                assert.equal(privateKeyFromJwk.symmetricKeySize, undefined);
            });

            it('export', () => {
                const publicKey = crypto.createPublicKey(publicPem);
                const privateKey = crypto.createPrivateKey(privatePem);
                const publicKeyFromJwk = crypto.createPublicKey({ key: publicJwk, format: 'jwk' });
                const privateKeyFromJwk = crypto.createPrivateKey({ key: jwk, format: 'jwk' });

                for (const keyObject of [publicKey, publicKeyFromJwk]) {
                    assert.deepEqual(
                        keyObject.export({ format: 'jwk' }),
                        { kty: 'RSA', n: jwk.n, e: jwk.e }
                    );
                }

                for (const keyObject of [privateKey, privateKeyFromJwk]) {
                    assert.deepEqual(
                        keyObject.export({ format: 'jwk' }),
                        jwk
                    );
                }

                assert.throws(() => {
                    privateKey.export({ format: 'jwk', passphrase: 'secret' });
                });

                const publicDER = publicKey.export({
                    format: 'der',
                    type: 'pkcs1'
                });

                const privateDER = privateKey.export({
                    format: 'der',
                    type: 'pkcs1'
                });

                assert(Buffer.isBuffer(publicDER));
                assert(Buffer.isBuffer(privateDER));

                assert.equal(publicKey.export(), publicKey.export({
                    format: 'pem',
                    type: 'spki'
                }));

                assert.equal(privateKey.export(), privateKey.export({
                    format: 'pem',
                    type: 'pkcs8'
                }));

                assert.deepEqual(privateKey.toJSON(), {
                    "kty": "RSA",
                    "e": "AQAB",
                    "n": "t9xYiIonscC3vz_A2ceR7KhZZlDu_5bye53nCVTcKnWd2seY6UAdKersX6njr83Dd5OVe1BW_wJvp5EjWTAGYbFswlNmeD44edEGM939B6Lq-_8iBkrTi8mGN4YCytivE24YI0D4XZMPfkLSpab2y_Hy4DjQKBq1ThZ0UBnK-9IhX37Ju_ZoGYSlTIGIhzyaiYBh7wrZBoPczIEu6et_kN2VnnbRUtkYTF97ggcv5h-hDpUQjQW0ZgOMcTc8n-RkGpIt0_iM_bTjI3Tz_gsFdi6hHcpZgbopPL630296iByyigQCPJVzdusFrQN5DeC-zT_nGypQkZanLb4ZspSx9Q",
                    "d": "ktnq2LvIMqBj4txP82IEOorIRQGVsw1khbm8A-cEpuEkgM71Yi_0WzupKktucUeevQ5i0Yh8w9e1SJiTLDRAlJz66kdky9uejiWWl6zR4dyNZVMFYRM43ijLC-P8rPne9Fz16IqHFW5VbJqA1xCBhKmuPMsD71RNxZ4Hrsa7Kt_xglQTYsLbdGIwDmcZihId9VGXRzvmCPsDRf2fCkAj7HDeRxpUdEiEDpajADc-PWikra3r3b40tVHKWm8wxJLivOIN7GiYXKQIW6RhZgH-Rk45JIRNKxNagxdeXUqqyhnwhbTo1Hite0iBDexN9tgoZk0XmdYWBn6ElXHRZ7VCDQ",
                    "p": "8UovlB4nrBm7xH-u7XXBMbqxADQm5vaEZxw9eluc-tP7cIAI4sglMIvL_FMpbd2pEeP_BkR76NTDzzDuPAZvUGRavgEjy0O9j2NAs_WPK4tZF-vFdunhnSh4EHAF4Ij9kbsUi90NOpbGfVqPdOaHqzgHKoR23Cuusk9wFQ2XTV8",
                    "q": "wxHdEYT9xrpfrHPqSBQPpO0dWGKJEkrWOb-76rSfuL8wGR4OBNmQdhLuU9zTIh22pog-XPnLPAecC-4yu_wtJ2SPCKiKDbJBre0CKPyRfGqzvA3njXwMxXazU4kGs-2Fg-xu_iKbaIjxXrclBLhkxhBtySrwAFhxxOk6fFcPLSs",
                    "dp": "qS_Mdr5CMRGGMH0bKhPUWEtAixUGZhJaunX5wY71Xoc_Gh4cnO-b7BNJ_-5L8WZog0vr6PgiLhrqBaCYm2wjpyoG2o2wDHm-NAlzN_wp3G2EFhrSxdOux-S1c0kpRcyoiAO2n29rNDa-jOzwBBcU8ACEPdLOCQl0IEFFJO33tl8",
                    "dq": "WAziKpxLKL7LnL4dzDcx8JIPIuwnTxh0plCDdCffyLaT8WJ9lXbXHFTjOvt8WfPrlDP_Ylxmfkw5BbGZOP1VLGjZn2DkH9aMiwNmbDXFPdG0G3hzQovx_9fajiRV4DWghLHeT9wzJfZabRRiI0VQR472300AVEeX4vgbrDBn600",
                    "qi": "k7czBCT9rHn_PNwCa17hlTy88C4vXkwbz83Oa-aX5L4e5gw5lhcR2ZuZHLb2r6oMt9rlD7EIDItSs-u21LOXWPTAlazdnpYUyw_CzogM_PN-qNwMRXn5uXFFhmlP2mVg2EdELTahXch8kWqHaCSX53yvqCtRKu_j76V31TfQZGM"
                });
            });

            describe('suite', () => {
                it('ed', () => {
                    [
                        {
                            private: readKey('ed25519_private.pem', 'ascii'),
                            public: readKey('ed25519_public.pem', 'ascii'),
                            keyType: 'ed25519',
                            jwk: {
                                crv: 'Ed25519',
                                x: 'K1wIouqnuiA04b3WrMa-xKIKIpfHetNZRv3h9fBf768',
                                d: 'wVK6M3SMhQh3NK-7GRrSV-BVWQx1FO5pW8hhQeu_NdA',
                                kty: 'OKP'
                            }
                        },
                        {
                            private: readKey('ed448_private.pem', 'ascii'),
                            public: readKey('ed448_public.pem', 'ascii'),
                            keyType: 'ed448',
                            jwk: {
                                crv: 'Ed448',
                                x: 'oX_ee5-jlcU53-BbGRsGIzly0V-SZtJ_oGXY0udf84q2hTW2RdstLktvwpkVJOoNb7o' +
                                    'Dgc2V5ZUA',
                                d: '060Ke71sN0GpIc01nnGgMDkp0sFNQ09woVo4AM1ffax1-mjnakK0-p-S7-Xf859QewX' +
                                    'jcR9mxppY',
                                kty: 'OKP'
                            }
                        },
                        {
                            private: readKey('x25519_private.pem', 'ascii'),
                            public: readKey('x25519_public.pem', 'ascii'),
                            keyType: 'x25519',
                            jwk: {
                                crv: 'X25519',
                                x: 'aSb8Q-RndwfNnPeOYGYPDUN3uhAPnMLzXyfi-mqfhig',
                                d: 'mL_IWm55RrALUGRfJYzw40gEYWMvtRkesP9mj8o8Omc',
                                kty: 'OKP'
                            }
                        },
                        {
                            private: readKey('x448_private.pem', 'ascii'),
                            public: readKey('x448_public.pem', 'ascii'),
                            keyType: 'x448',
                            jwk: {
                                crv: 'X448',
                                x: 'ioHSHVpTs6hMvghosEJDIR7ceFiE3-Xccxati64oOVJ7NWjfozE7ae31PXIUFq6cVYg' +
                                    'vSKsDFPA',
                                d: 'tMNtrO_q8dlY6Y4NDeSTxNQ5CACkHiPvmukidPnNIuX_EkcryLEXt_7i6j6YZMKsrWy' +
                                    'S0jlSYJk',
                                kty: 'OKP'
                            }
                        },
                    ].forEach((info) => {
                        const keyType = info.keyType;

                        {
                            const key = crypto.createPrivateKey(info.private);
                            assert.equal(key.type, 'private');
                            assert.equal(key.asymmetricKeyType, keyType);
                            assert.equal(key.symmetricKeySize, undefined);
                            assert.equal(
                                key.export({ type: 'pkcs8', format: 'pem' }), info.private);
                            assert.deepEqual(
                                key.export({ format: 'jwk' }), info.jwk);

                            assert.deepEqual(key.toJSON(), info.jwk);
                        }

                        {
                            const key = crypto.createPrivateKey({ key: info.jwk, format: 'jwk' });
                            assert.equal(key.type, 'private');
                            assert.equal(key.asymmetricKeyType, keyType);
                            assert.equal(key.symmetricKeySize, undefined);
                            assert.equal(
                                key.export({ type: 'pkcs8', format: 'pem' }), info.private);
                            assert.deepEqual(
                                key.export({ format: 'jwk' }), info.jwk);

                            assert.equal(key.export({ format: 'raw' }).toString("base64url"), info.jwk.d);
                        }

                        {
                            for (const input of [
                                info.private, info.public, { key: info.jwk, format: 'jwk' }]) {
                                const key = crypto.createPublicKey(input);
                                assert.equal(key.type, 'public');
                                assert.equal(key.asymmetricKeyType, keyType);
                                assert.equal(key.symmetricKeySize, undefined);
                                assert.equal(
                                    key.export({ type: 'spki', format: 'pem' }), info.public);
                                const jwk = { ...info.jwk };
                                delete jwk.d;
                                assert.deepEqual(
                                    key.export({ format: 'jwk' }), jwk);

                                assert.equal(key.export({ format: 'raw' }).toString("base64url"), info.jwk.x);
                            }
                        }
                    });
                });

                it('toX25519', () => {
                    var ed_jwk = {
                        "kty": "OKP",
                        "crv": "Ed25519",
                        "x": "11qYAYKxCrfVS_7TyWQHOg7hcvPapiMlrwIaaPcHURo",
                        "d": "nWGxne_9WmC6hEr0kuwsxERJxWl7MmkZcDusAxyuf2A"
                    };

                    var x_jwk = {
                        "kty": "OKP",
                        "crv": "X25519",
                        "x": "2F4H7CKwrYgVN8L0TWYtGhQ8-DDFespDBdhcepD2ti4",
                        "d": "MHyDhk8oM8tCei7xwAoBPP3_J2jZgMCjpSDwBpBN6U8"
                    };

                    var x_jwk_pub = {
                        "kty": "OKP",
                        "crv": "X25519",
                        "x": "2F4H7CKwrYgVN8L0TWYtGhQ8-DDFespDBdhcepD2ti4"
                    };

                    var ed_key = crypto.createPrivateKey({ key: ed_jwk, format: 'jwk' });

                    var x_key = crypto.createPrivateKey({ key: ed_jwk, format: 'jwk', toX25519: true });
                    assert.deepEqual(x_key.export({ format: 'jwk' }), x_jwk);
                    assert.deepEqual(x_key.toJSON(), x_jwk);

                    var x_key = crypto.createPrivateKey({ key: ed_key, toX25519: true });
                    assert.deepEqual(x_key.export({ format: 'jwk' }), x_jwk);

                    var ed_key = crypto.createPublicKey({ key: ed_jwk, format: 'jwk' });

                    var x_key = crypto.createPublicKey({ key: ed_jwk, format: 'jwk', toX25519: true });
                    assert.deepEqual(x_key.export({ format: 'jwk' }), x_jwk_pub);

                    var x_key = crypto.createPublicKey({ key: ed_key, toX25519: true });
                    assert.deepEqual(x_key.export({ format: 'jwk' }), x_jwk_pub);

                });

                it('ec', () => {
                    [
                        {
                            private: readKey('ec_p256_private.pem', 'ascii'),
                            public: readKey('ec_p256_public.pem', 'ascii'),
                            keyType: 'ec',
                            namedCurve: 'prime256v1',
                            jwk: {
                                crv: 'P-256',
                                d: 'DxBsPQPIgMuMyQbxzbb9toew6Ev6e9O6ZhpxLNgmAEo',
                                kty: 'EC',
                                x: 'X0mMYR_uleZSIPjNztIkAS3_ud5LhNpbiIFp6fNf2Gs',
                                y: 'UbJuPy2Xi0lW7UYTBxPK3yGgDu9EAKYIecjkHX5s2lI'
                            },
                            uncompressed: "045f498c611fee95e65220f8cdced224012dffb9de4b84da5b888169e9f35fd86b51b26e3f2d978b4956ed46130713cadf21a00eef4400a60879c8e41d7e6cda52",
                            compressed: "025f498c611fee95e65220f8cdced224012dffb9de4b84da5b888169e9f35fd86b",
                            hybrid: "065f498c611fee95e65220f8cdced224012dffb9de4b84da5b888169e9f35fd86b51b26e3f2d978b4956ed46130713cadf21a00eef4400a60879c8e41d7e6cda52"
                        },
                        {
                            private: readKey('ec_secp256k1_private.pem', 'ascii'),
                            public: readKey('ec_secp256k1_public.pem', 'ascii'),
                            keyType: 'ec',
                            namedCurve: 'secp256k1',
                            jwk: {
                                crv: 'secp256k1',
                                d: 'c34ocwTwpFa9NZZh3l88qXyrkoYSxvC0FEsU5v1v4IM',
                                kty: 'EC',
                                x: 'cOzhFSpWxhalCbWNdP2H_yUkdC81C9T2deDpfxK7owA',
                                y: '-A3DAZTk9IPppN-f03JydgHaFvL1fAHaoXf4SX4NXyo'
                            },
                            uncompressed: "0470ece1152a56c616a509b58d74fd87ff2524742f350bd4f675e0e97f12bba300f80dc30194e4f483e9a4df9fd372727601da16f2f57c01daa177f8497e0d5f2a",
                            compressed: "0270ece1152a56c616a509b58d74fd87ff2524742f350bd4f675e0e97f12bba300",
                            hybrid: "0670ece1152a56c616a509b58d74fd87ff2524742f350bd4f675e0e97f12bba300f80dc30194e4f483e9a4df9fd372727601da16f2f57c01daa177f8497e0d5f2a"
                        },
                        {
                            private: readKey('ec_p384_private.pem', 'ascii'),
                            public: readKey('ec_p384_public.pem', 'ascii'),
                            keyType: 'ec',
                            namedCurve: 'secp384r1',
                            jwk: {
                                crv: 'P-384',
                                d: 'dwfuHuAtTlMRn7ZBCBm_0grpc1D_4hPeNAgevgelljuC0--k_LDFosDgBlLLmZsi',
                                kty: 'EC',
                                x: 'hON3nzGJgv-08fdHpQxgRJFZzlK-GZDGa5f3KnvM31cvvjJmsj4UeOgIdy3rDAjV',
                                y: 'fidHhtecNCGCfLqmrLjDena1NSzWzWH1u_oUdMKGo5XSabxzD7-8JZxjpc8sR9cl'
                            },
                            uncompressed: "0484e3779f318982ffb4f1f747a50c60449159ce52be1990c66b97f72a7bccdf572fbe3266b23e1478e808772deb0c08d57e274786d79c3421827cbaa6acb8c37a76b5352cd6cd61f5bbfa1474c286a395d269bc730fbfbc259c63a5cf2c47d725",
                            compressed: "0384e3779f318982ffb4f1f747a50c60449159ce52be1990c66b97f72a7bccdf572fbe3266b23e1478e808772deb0c08d5",
                            hybrid: "0784e3779f318982ffb4f1f747a50c60449159ce52be1990c66b97f72a7bccdf572fbe3266b23e1478e808772deb0c08d57e274786d79c3421827cbaa6acb8c37a76b5352cd6cd61f5bbfa1474c286a395d269bc730fbfbc259c63a5cf2c47d725"
                        },
                        {
                            private: readKey('ec_p521_private.pem', 'ascii'),
                            public: readKey('ec_p521_public.pem', 'ascii'),
                            keyType: 'ec',
                            namedCurve: 'secp521r1',
                            jwk: {
                                crv: 'P-521',
                                d: 'ABIIbmn3Gm_Y11uIDkC3g2ijpRxIrJEBY4i_JJYo5OougzTl3BX2ifRluPJMaaHcNer' +
                                    'bQH_WdVkLLX86ShlHrRyJ',
                                kty: 'EC',
                                x: 'AaLFgjwZtznM3N7qsfb86awVXe6c6djUYOob1FN-kllekv0KEXV0bwcDjPGQz5f6MxL' +
                                    'CbhMeHRavUS6P10rsTtBn',
                                y: 'Ad3flexBeAfXceNzRBH128kFbOWD6W41NjwKRqqIF26vmgW_8COldGKZjFkOSEASxPB' +
                                    'cvA2iFJRUyQ3whC00j0Np'
                            },
                            uncompressed: "0401a2c5823c19b739ccdcdeeab1f6fce9ac155dee9ce9d8d460ea1bd4537e92595e92fd0a1175746f07038cf190cf97fa3312c26e131e1d16af512e8fd74aec4ed06701dddf95ec417807d771e3734411f5dbc9056ce583e96e35363c0a46aa88176eaf9a05bff023a57462998c590e484012c4f05cbc0da2149454c90df0842d348f4369",
                            compressed: "0301a2c5823c19b739ccdcdeeab1f6fce9ac155dee9ce9d8d460ea1bd4537e92595e92fd0a1175746f07038cf190cf97fa3312c26e131e1d16af512e8fd74aec4ed067",
                            hybrid: "0701a2c5823c19b739ccdcdeeab1f6fce9ac155dee9ce9d8d460ea1bd4537e92595e92fd0a1175746f07038cf190cf97fa3312c26e131e1d16af512e8fd74aec4ed06701dddf95ec417807d771e3734411f5dbc9056ce583e96e35363c0a46aa88176eaf9a05bff023a57462998c590e484012c4f05cbc0da2149454c90df0842d348f4369"
                        },
                    ].forEach((info) => {
                        const { keyType, namedCurve } = info;

                        {
                            const key = crypto.createPrivateKey(info.private);
                            assert.equal(key.type, 'private');
                            assert.equal(key.asymmetricKeyType, keyType);
                            assert.deepEqual(key.asymmetricKeyDetails, { namedCurve });
                            assert.equal(key.symmetricKeySize, undefined);
                            assert.equal(
                                key.export({ type: 'pkcs8', format: 'pem' }), info.private);
                            assert.deepEqual(key.export({ format: 'jwk' }), info.jwk);
                            assert.deepEqual(key.toJSON(), info.jwk);
                        }

                        {
                            const key = crypto.createPrivateKey({ key: info.jwk, format: 'jwk' });
                            assert.equal(key.type, 'private');
                            assert.equal(key.asymmetricKeyType, keyType);
                            assert.deepEqual(key.asymmetricKeyDetails, { namedCurve });
                            assert.equal(key.symmetricKeySize, undefined);
                            assert.equal(
                                key.export({ type: 'pkcs8', format: 'pem' }), info.private);
                            assert.deepEqual(
                                key.export({ format: 'jwk' }), info.jwk);

                            assert.equal(key.export({ format: 'raw' }).toString("base64url"), info.jwk.d);
                        }

                        {
                            for (const input of [
                                info.private, info.public, { key: info.jwk, format: 'jwk' }]) {
                                const key = crypto.createPublicKey(input);
                                assert.equal(key.type, 'public');
                                assert.equal(key.asymmetricKeyType, keyType);
                                assert.deepEqual(key.asymmetricKeyDetails, { namedCurve });
                                assert.equal(key.symmetricKeySize, undefined);
                                assert.equal(
                                    key.export({ type: 'spki', format: 'pem' }), info.public);
                                const jwk = { ...info.jwk };
                                delete jwk.d;
                                assert.deepEqual(
                                    key.export({ format: 'jwk' }), jwk);

                                assert.equal(key.export({ format: 'raw', type: "uncompressed" }).toString("hex"), info.uncompressed);
                                assert.equal(key.export({ format: 'raw', type: "compressed" }).toString("hex"), info.compressed);
                                assert.equal(key.export({ format: 'raw', type: "hybrid" }).toString("hex"), info.hybrid);
                            }
                        }
                    });
                });

                describe("raw", () => {
                    [
                        {
                            type: "ec",
                            gen_curve: "P-224",
                            import_curve: "P-224",
                        },
                        {
                            type: "ec",
                            gen_curve: "P-256",
                            import_curve: "P-256",
                        },
                        {
                            type: "ec",
                            gen_curve: "P-384",
                            import_curve: "P-384",
                        },
                        {
                            type: "ec",
                            gen_curve: "P-521",
                            import_curve: "P-521",
                        },
                        {
                            type: "ec",
                            gen_curve: "secp256k1",
                            import_curve: "secp256k1",
                        },
                        {
                            type: "sm2",
                            import_curve: "SM2",
                        },
                        {
                            type: "ed25519",
                            import_curve: "ed25519"
                        },
                        {
                            type: "x25519",
                            import_curve: "x25519"
                        },
                        {
                            type: "x448",
                            import_curve: "x448"
                        },
                        {
                            type: "ed448",
                            import_curve: "ed448"
                        }
                    ].forEach((t) => {
                        it(t.import_curve, () => {
                            const key = crypto.generateKeyPairSync(t.type, {
                                namedCurve: t.gen_curve
                            });

                            if (t.type === "ec" || t.type === "sm2") {
                                ['compressed', 'uncompressed', 'hybrid'].forEach((type) => {
                                    var key1 = crypto.createPublicKey({
                                        key: key.publicKey.export({
                                            format: 'raw',
                                            type
                                        }),
                                        format: 'raw',
                                        namedCurve: t.import_curve
                                    });

                                    assert.deepEqual(key.publicKey.export({
                                        format: "jwk"
                                    }), key1.export({
                                        format: "jwk"
                                    }));
                                });
                            } else {
                                var key1 = crypto.createPublicKey({
                                    key: key.publicKey.export({
                                        format: 'raw'
                                    }),
                                    format: 'raw',
                                    namedCurve: t.import_curve
                                });

                                assert.deepEqual(key.publicKey.export({
                                    format: "jwk"
                                }), key1.export({
                                    format: "jwk"
                                }));
                            }

                            var key1 = crypto.createPrivateKey({
                                key: key.privateKey.export({
                                    format: 'raw'
                                }),
                                format: 'raw',
                                namedCurve: t.import_curve
                            });

                            assert.deepEqual(key.privateKey.export({
                                format: "jwk"
                            }), key1.export({
                                format: "jwk"
                            }));
                        });
                    });
                });

                describe('rsa', () => {
                    it('rsa_pss_public_2048', () => {
                        const publicPem = readKey('rsa_pss_public_2048.pem');
                        const privatePem = readKey('rsa_pss_private_2048.pem');

                        const publicKey = crypto.createPublicKey(publicPem);
                        const privateKey = crypto.createPrivateKey(privatePem);

                        const expectedKeyDetails = {
                            modulusLength: 2048,
                            publicExponent: 65537n
                        };

                        assert.equal(publicKey.type, 'public');
                        assert.equal(publicKey.asymmetricKeyType, 'rsa-pss');
                        assert.deepEqual(publicKey.asymmetricKeyDetails, expectedKeyDetails);

                        assert.equal(privateKey.type, 'private');
                        assert.equal(privateKey.asymmetricKeyType, 'rsa-pss');
                        assert.deepEqual(privateKey.asymmetricKeyDetails, expectedKeyDetails);

                        assert.throws(() => publicKey.export({ format: 'jwk' }));
                        assert.throws(() => privateKey.export({ format: 'jwk' }));

                        assert.throws(() => publicKey.export({ format: 'pem', type: 'pkcs1' }));
                    });

                    it('rsa_pss_public_2048_sha1_sha1_20', () => {
                        const publicPem = readKey('rsa_pss_public_2048_sha1_sha1_20.pem');
                        const privatePem = readKey('rsa_pss_private_2048_sha1_sha1_20.pem');

                        const publicKey = crypto.createPublicKey(publicPem);
                        const privateKey = crypto.createPrivateKey(privatePem);

                        const expectedKeyDetails = {
                            modulusLength: 2048,
                            publicExponent: 65537n,
                            hashAlgorithm: 'sha1',
                            mgf1HashAlgorithm: 'sha1',
                            saltLength: 20
                        };

                        assert.equal(publicKey.type, 'public');
                        assert.equal(publicKey.asymmetricKeyType, 'rsa-pss');
                        assert.deepEqual(publicKey.asymmetricKeyDetails, expectedKeyDetails);

                        assert.equal(privateKey.type, 'private');
                        assert.equal(privateKey.asymmetricKeyType, 'rsa-pss');
                        assert.deepEqual(privateKey.asymmetricKeyDetails, expectedKeyDetails);
                    });

                    it('rsa_pss_public_2048_sha256_sha256_16', () => {
                        // This key pair enforces sha256 as the message digest and the MGF1
                        // message digest and a salt length of at least 16 bytes.
                        const publicPem = readKey('rsa_pss_public_2048_sha256_sha256_16.pem');
                        const privatePem = readKey('rsa_pss_private_2048_sha256_sha256_16.pem');

                        const publicKey = crypto.createPublicKey(publicPem);
                        const privateKey = crypto.createPrivateKey(privatePem);

                        assert.equal(publicKey.type, 'public');
                        assert.equal(publicKey.asymmetricKeyType, 'rsa-pss');

                        assert.equal(privateKey.type, 'private');
                        assert.equal(privateKey.asymmetricKeyType, 'rsa-pss');
                    });

                    it('rsa_pss_public_2048_sha512_sha256_20', () => {
                        const publicPem = readKey('rsa_pss_public_2048_sha512_sha256_20.pem');
                        const privatePem = readKey('rsa_pss_private_2048_sha512_sha256_20.pem');

                        const publicKey = crypto.createPublicKey(publicPem);
                        const privateKey = crypto.createPrivateKey(privatePem);

                        const expectedKeyDetails = {
                            modulusLength: 2048,
                            publicExponent: 65537n,
                            hashAlgorithm: 'sha512',
                            mgf1HashAlgorithm: 'sha256',
                            saltLength: 20
                        };

                        assert.equal(publicKey.type, 'public');
                        assert.equal(publicKey.asymmetricKeyType, 'rsa-pss');
                        assert.deepEqual(publicKey.asymmetricKeyDetails, expectedKeyDetails);

                        assert.equal(privateKey.type, 'private');
                        assert.equal(privateKey.asymmetricKeyType, 'rsa-pss');
                        assert.deepEqual(privateKey.asymmetricKeyDetails, expectedKeyDetails);
                    });
                });
            });

            it('encrypted key', () => {
                assert.throws(() => crypto.createPrivateKey(privateDsa));

                assert.throws(() => crypto.createPrivateKey({
                    key: privateDsa,
                    format: 'pem',
                    passphrase: Buffer.alloc(1025, 'a')
                }));

                assert.throws(() => crypto.createPrivateKey({
                    key: privateDsa,
                    format: 'pem',
                    passphrase: Buffer.alloc(1024, 'a')
                }));

                const publicKey = crypto.createPublicKey(publicDsa);
                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.asymmetricKeyType, 'dsa');
                assert.equal(publicKey.symmetricKeySize, undefined);
                assert.throws(() => publicKey.export({ format: 'jwk' }));
                assert.deepEqual(publicKey.asymmetricKeyDetails, {
                    divisorLength: 160,
                    modulusLength: 1088
                });

                const privateKey = crypto.createPrivateKey({
                    key: privateDsa,
                    format: 'pem',
                    passphrase: 'secret'
                });
                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.asymmetricKeyType, 'dsa');
                assert.equal(privateKey.symmetricKeySize, undefined);
                assert.throws(() => privateKey.export({ format: 'jwk' }));
                assert.deepEqual(privateKey.asymmetricKeyDetails, {
                    divisorLength: 160,
                    modulusLength: 1088
                });
            });

            it('equals', () => {
                assert.ok(crypto.createPublicKey(publicPem).equals(crypto.createPublicKey(publicPem)));
                assert.ok(!crypto.createPublicKey(publicPem).equals(crypto.createPublicKey(publicDsa)));
            });

            it("createPrivateKey({sm2}) issue", () => {
                const key = crypto.generateKeyPair("sm2");
                const key2 = crypto.createPrivateKey({
                    key: key.privateKey
                });

                assert.equal(key2.asymmetricKeyType, "sm2");
                assert.ok(key.privateKey.equals(key2));
            });

            it("createPublicKey(pem) issue", () => {
                const key = crypto.generateKeyPair("sm2");
                const key2 = crypto.createPublicKey(key.publicKey.export());

                assert.equal(key2.asymmetricKeyType, "sm2");
                assert.ok(key.publicKey.equals(key2));
            });

            it("BUGFIX: createPublicKey(sk) issue", () => {
                var k = crypto.createPrivateKey({
                    key: {
                        "kty": "EC",
                        "crv": "secp256k1",
                        "x": "rnh1Pdyngihl7H9lLo0LXrKaFt80xMP-47jH3v6MQSw",
                        "y": "NIYva2P56-m_BMyIR5syIxwNYrWej-B56r6UUNwYzaQ",
                        "d": "wHgJZW2jg20Y7_p3XD0Wt6r8dNSUIS4ur7f9A0G-tWI"
                    },
                    format: 'jwk'
                });

                var k1 = crypto.createPublicKey(k);
            });
        });

        describe('generateKeyPair', () => {
            it('rsa', () => {
                const { publicKey, privateKey } = crypto.generateKeyPairSync('rsa', {
                    publicExponent: 0x10001,
                    modulusLength: 512
                });

                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.asymmetricKeyType, 'rsa');
                assert.equal(publicKey.symmetricKeySize, undefined);
                assert.deepEqual(publicKey.asymmetricKeyDetails, {
                    modulusLength: 512,
                    publicExponent: 65537n
                });

                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.asymmetricKeyType, 'rsa');
                assert.equal(privateKey.symmetricKeySize, undefined);
                assert.deepEqual(privateKey.asymmetricKeyDetails, {
                    modulusLength: 512,
                    publicExponent: 65537n
                });
            });

            it('dsa', () => {
                const { publicKey, privateKey } = crypto.generateKeyPairSync('dsa', {
                    modulusLength: 1024,
                    divisorLength: 160
                });

                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.asymmetricKeyType, 'dsa');
                assert.equal(publicKey.symmetricKeySize, undefined);
                assert.throws(() => publicKey.export({ format: 'jwk' }));
                assert.deepEqual(publicKey.asymmetricKeyDetails, {
                    divisorLength: 160,
                    modulusLength: 1024
                });

                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.asymmetricKeyType, 'dsa');
                assert.equal(privateKey.symmetricKeySize, undefined);
                assert.throws(() => privateKey.export({ format: 'jwk' }));
                assert.deepEqual(privateKey.asymmetricKeyDetails, {
                    divisorLength: 160,
                    modulusLength: 1024
                });
            });

            it('ec', () => {
                const { publicKey, privateKey } = crypto.generateKeyPairSync('ec', {
                    namedCurve: 'secp256k1'
                });

                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.asymmetricKeyType, 'ec');
                assert.equal(publicKey.symmetricKeySize, undefined);
                assert.deepEqual(publicKey.asymmetricKeyDetails, { namedCurve: 'secp256k1' });

                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.asymmetricKeyType, 'ec');
                assert.equal(privateKey.symmetricKeySize, undefined);
                assert.deepEqual(privateKey.asymmetricKeyDetails, { namedCurve: 'secp256k1' });
            });

            it('ed25519', () => {
                const { publicKey, privateKey } = crypto.generateKeyPairSync('ed25519');

                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.asymmetricKeyType, 'ed25519');
                assert.equal(publicKey.symmetricKeySize, undefined);
                assert.equal(publicKey.asymmetricKeyDetails, undefined);

                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.asymmetricKeyType, 'ed25519');
                assert.equal(privateKey.symmetricKeySize, undefined);
                assert.equal(privateKey.asymmetricKeyDetails, undefined);
            });

            it('sm2', () => {
                const { publicKey, privateKey } = crypto.generateKeyPairSync('sm2');

                assert.equal(publicKey.type, 'public');
                assert.equal(publicKey.asymmetricKeyType, 'sm2');

                assert.equal(privateKey.type, 'private');
                assert.equal(privateKey.asymmetricKeyType, 'sm2');
            });

            it('callback', (done) => {
                crypto.generateKeyPair('rsa', {
                    publicExponent: 0x10001,
                    modulusLength: 512,
                    publicKeyEncoding: { type: 'spki', format: 'pem' },
                    privateKeyEncoding: { type: 'pkcs8', format: 'pem' }
                }, (err, publicKey, privateKey) => {
                    done(() => {
                        assert.isString(publicKey);
                        assert.isString(privateKey);
                    });
                });
            });
        });

        describe('encrypt', () => {
            const certPem = readKey('rsa_cert.crt');
            const keyPem = readKey('rsa_private.pem');
            const rsaKeySize = 2048;
            const rsaPubPem = readKey('rsa_public.pem', 'ascii');
            const rsaKeyPem = readKey('rsa_private.pem', 'ascii');
            const rsaKeyPemEncrypted = readKey('rsa_private_encrypted.pem', 'ascii');
            const dsaPubPem = readKey('dsa_public.pem', 'ascii');
            const dsaKeyPem = readKey('dsa_private.pem', 'ascii');
            const dsaKeyPemEncrypted = readKey('dsa_private_encrypted.pem', 'ascii');
            const rsaPkcs8KeyPem = readKey('rsa_private_pkcs8.pem');
            const dsaPkcs8KeyPem = readKey('dsa_private_pkcs8.pem');

            const ec = new TextEncoder();

            function getBufferCopy(buf) {
                return buf.buffer.slice(buf.byteOffset, buf.byteOffset + buf.byteLength);
            }

            describe('RSA encryption/decryption', () => {
                const input = 'I AM THE WALRUS';
                const bufferToEncrypt = Buffer.from(input);
                const bufferPassword = Buffer.from('password');
                var encryptedBuffer;
                var decryptedBuffer;
                var otherEncrypted;

                it('publicEncrypt/privateDecrypt', () => {
                    encryptedBuffer = crypto.publicEncrypt(rsaPubPem, bufferToEncrypt);

                    const ab = getBufferCopy(ec.encode(rsaPubPem));
                    const ab2enc = getBufferCopy(bufferToEncrypt);

                    crypto.publicEncrypt(ab, ab2enc);
                    crypto.publicEncrypt(new Uint8Array(ab), new Uint8Array(ab2enc));
                    crypto.publicEncrypt(new DataView(ab), new DataView(ab2enc));
                    otherEncrypted = crypto.publicEncrypt({
                        key: Buffer.from(ab).toString('hex'),
                        encoding: 'hex'
                    }, Buffer.from(ab2enc).toString('hex'));

                    decryptedBuffer = crypto.privateDecrypt(rsaKeyPem, encryptedBuffer);
                    const otherDecrypted = crypto.privateDecrypt(rsaKeyPem, otherEncrypted);
                    assert.equal(decryptedBuffer.toString(), input);
                    assert.equal(otherDecrypted.toString(), input);

                    decryptedBuffer = crypto.privateDecrypt(rsaPkcs8KeyPem, encryptedBuffer);
                    assert.equal(decryptedBuffer.toString(), input);

                    let decryptedBufferWithPassword = crypto.privateDecrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: 'password'
                    }, encryptedBuffer);

                    const otherDecryptedBufferWithPassword = crypto.privateDecrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: ec.encode('password')
                    }, encryptedBuffer);

                    assert.equal(
                        otherDecryptedBufferWithPassword.toString(),
                        decryptedBufferWithPassword.toString());

                    decryptedBufferWithPassword = crypto.privateDecrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: 'password'
                    }, encryptedBuffer);

                    assert.equal(decryptedBufferWithPassword.toString(), input);

                    encryptedBuffer = crypto.publicEncrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: 'password'
                    }, bufferToEncrypt);

                    decryptedBufferWithPassword = crypto.privateDecrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: 'password'
                    }, encryptedBuffer);
                    assert.equal(decryptedBufferWithPassword.toString(), input);
                });

                it('privateEncrypt/publicDecrypt', () => {
                    encryptedBuffer = crypto.privateEncrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: bufferPassword
                    }, bufferToEncrypt);

                    decryptedBufferWithPassword = crypto.publicDecrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: bufferPassword
                    }, encryptedBuffer);
                    assert.equal(decryptedBufferWithPassword.toString(), input);

                    // Now with explicit RSA_PKCS1_PADDING.
                    encryptedBuffer = crypto.privateEncrypt({
                        padding: crypto.constants.RSA_PKCS1_PADDING,
                        key: rsaKeyPemEncrypted,
                        passphrase: bufferPassword
                    }, bufferToEncrypt);

                    decryptedBufferWithPassword = crypto.publicDecrypt({
                        padding: crypto.constants.RSA_PKCS1_PADDING,
                        key: rsaKeyPemEncrypted,
                        passphrase: bufferPassword
                    }, encryptedBuffer);
                    assert.equal(decryptedBufferWithPassword.toString(), input);

                    // Omitting padding should be okay because RSA_PKCS1_PADDING is the default.
                    decryptedBufferWithPassword = crypto.publicDecrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: bufferPassword
                    }, encryptedBuffer);
                    assert.equal(decryptedBufferWithPassword.toString(), input);

                    {
                        const plaintext = 'x'.repeat(rsaKeySize / 8);
                        encryptedBuffer = crypto.privateEncrypt({
                            padding: crypto.constants.RSA_NO_PADDING,
                            key: rsaKeyPemEncrypted,
                            passphrase: bufferPassword
                        }, Buffer.from(plaintext));

                        decryptedBufferWithPassword = crypto.publicDecrypt({
                            padding: crypto.constants.RSA_NO_PADDING,
                            key: rsaKeyPemEncrypted,
                            passphrase: bufferPassword
                        }, encryptedBuffer);
                        assert.equal(decryptedBufferWithPassword.toString(), plaintext);
                    }

                    encryptedBuffer = crypto.publicEncrypt(certPem, bufferToEncrypt);

                    decryptedBuffer = crypto.privateDecrypt(keyPem, encryptedBuffer);
                    assert.equal(decryptedBuffer.toString(), input);

                    encryptedBuffer = crypto.publicEncrypt(keyPem, bufferToEncrypt);

                    decryptedBuffer = crypto.privateDecrypt(keyPem, encryptedBuffer);
                    assert.equal(decryptedBuffer.toString(), input);

                    encryptedBuffer = crypto.privateEncrypt(keyPem, bufferToEncrypt);

                    decryptedBuffer = crypto.publicDecrypt(keyPem, encryptedBuffer);
                    assert.equal(decryptedBuffer.toString(), input);
                });

                it('publicEncrypt/privateDecrypt with sm2', () => {
                    function encrypt_test(key, data) {
                        let encrypt_res = crypto.publicEncrypt(key.publicKey, Buffer.from(data)).toString("hex");
                        return encrypt_res;
                    }

                    function decrypt_test(key, encrypt_data) {
                        let decrypt_res = crypto.privateDecrypt(key.privateKey, Buffer.from(encrypt_data, "hex")).toString();
                        return decrypt_res;
                    }

                    let data = Buffer.from("aaa");

                    let sm2_key = crypto.generateKeyPairSync('sm2');
                    var sm2_encrypt = encrypt_test(sm2_key, data);
                    var sm2_decrypt = decrypt_test(sm2_key, sm2_encrypt);

                    assert.deepEqual("aaa", sm2_decrypt);
                });

                it('passphrase error', () => {
                    assert.throws(() => {
                        crypto.privateDecrypt({
                            key: rsaKeyPemEncrypted,
                            passphrase: 'wrong'
                        }, bufferToEncrypt);
                    });

                    assert.throws(() => {
                        crypto.publicEncrypt({
                            key: rsaKeyPemEncrypted,
                            passphrase: 'wrong'
                        }, encryptedBuffer);
                    });

                    encryptedBuffer = crypto.privateEncrypt({
                        key: rsaKeyPemEncrypted,
                        passphrase: Buffer.from('password')
                    }, bufferToEncrypt);

                    assert.throws(() => {
                        crypto.publicDecrypt({
                            key: rsaKeyPemEncrypted,
                            passphrase: Buffer.from('wrong')
                        }, encryptedBuffer);
                    });
                });
            });

            describe('rsa', () => {
                const constants = crypto.constants;

                function test_rsa(padding, encryptOaepHash, decryptOaepHash) {
                    it(`${padding}, ${encryptOaepHash}, ${decryptOaepHash}`, () => {
                        const size = (padding === 'RSA_NO_PADDING') ? rsaKeySize / 8 : 32;
                        const input = Buffer.allocUnsafe(size);
                        for (let i = 0; i < input.length; i++)
                            input[i] = (i * 7 + 11) & 0xff;
                        const bufferToEncrypt = Buffer.from(input);

                        padding = constants[padding];

                        const encryptedBuffer = crypto.publicEncrypt({
                            key: rsaPubPem,
                            padding: padding,
                            oaepHash: encryptOaepHash
                        }, bufferToEncrypt);

                        let decryptedBuffer = crypto.privateDecrypt({
                            key: rsaKeyPem,
                            padding: padding,
                            oaepHash: decryptOaepHash
                        }, encryptedBuffer);
                        assert.deepEqual(decryptedBuffer, input);

                        decryptedBuffer = crypto.privateDecrypt({
                            key: rsaPkcs8KeyPem,
                            padding: padding,
                            oaepHash: decryptOaepHash
                        }, encryptedBuffer);
                        assert.deepEqual(decryptedBuffer, input);
                    });
                }

                test_rsa('RSA_NO_PADDING');
                test_rsa('RSA_PKCS1_PADDING');
                test_rsa('RSA_PKCS1_OAEP_PADDING');

                // Test OAEP with different hash functions.
                test_rsa('RSA_PKCS1_OAEP_PADDING', undefined, 'sha1');
                test_rsa('RSA_PKCS1_OAEP_PADDING', 'sha1', undefined);
                test_rsa('RSA_PKCS1_OAEP_PADDING', 'sha256', 'sha256');
                test_rsa('RSA_PKCS1_OAEP_PADDING', 'sha512', 'sha512');

                it('invalid padding', () => {
                    assert.throws(() => {
                        test_rsa('RSA_PKCS1_OAEP_PADDING', 'sha256', 'sha512');
                    });
                });
            });

            it('non-SHA1 hash', () => {
                const { decryptionTests } =
                    JSON.parse(readSync('rsa-oaep-test-vectors.js', 'utf8'));

                for (const { ct, oaepHash, oaepLabel } of decryptionTests) {
                    const label = oaepLabel ? Buffer.from(oaepLabel, 'hex') : undefined;
                    const copiedLabel = oaepLabel ? getBufferCopy(label) : undefined;

                    const decrypted = crypto.privateDecrypt({
                        key: rsaPkcs8KeyPem,
                        oaepHash,
                        oaepLabel: oaepLabel ? label : undefined
                    }, Buffer.from(ct, 'hex'));

                    assert.equal(decrypted.toString('utf8'), 'Hello Node.js');

                    const otherDecrypted = crypto.privateDecrypt({
                        key: rsaPkcs8KeyPem,
                        oaepHash,
                        oaepLabel: copiedLabel
                    }, Buffer.from(ct, 'hex'));

                    assert.equal(otherDecrypted.toString('utf8'), 'Hello Node.js');
                }
            });

            it('invalid oaepHash and oaepLabel options', () => {
                for (const fn of [crypto.publicEncrypt, crypto.privateDecrypt]) {
                    assert.throws(() => {
                        fn({
                            key: rsaPubPem,
                            oaepHash: 'Hello world'
                        }, Buffer.alloc(10));
                    });

                    for (const oaepHash of [0, false, null, Symbol(), () => { }]) {
                        assert.throws(() => {
                            fn({
                                key: rsaPubPem,
                                oaepHash
                            }, Buffer.alloc(10));
                        });
                    }

                    for (const oaepLabel of [0, false, null, Symbol(), () => { }, {}]) {
                        assert.throws(() => {
                            fn({
                                key: rsaPubPem,
                                oaepLabel
                            }, Buffer.alloc(10));
                        });
                    }
                }
            });
        });

        describe('sign/verify', () => {
            const certPem = readKey('rsa_cert.crt');
            const keyPem = readKey('rsa_private.pem');
            const keySize = 2048;
            const rsaPubPem = readKey('rsa_public.pem', 'ascii');
            const rsaKeyPem = readKey('rsa_private.pem', 'ascii');
            const rsaPkcs8KeyPem = readKey('rsa_private_pkcs8.pem');
            const rsaKeyPemEncrypted = readKey('rsa_private_encrypted.pem', 'ascii');
            const dsaPubPem = readKey('dsa_public.pem', 'ascii');
            const dsaKeyPem = readKey('dsa_private.pem', 'ascii');
            const dsaKeyPemEncrypted = readKey('dsa_private_encrypted.pem', 'ascii');
            const dsaPkcs8KeyPem = readKey('dsa_private_pkcs8.pem');

            it('RSA key signing/verification', () => {
                let rsaSign = crypto.createSign('SHA1');
                let rsaVerify = crypto.createVerify('SHA1');
                assert.ok(rsaSign);
                assert.ok(rsaVerify);

                const expectedSignature = readKey(
                    'rsa_public_sha1_signature_signedby_rsa_private_pkcs8.sha1',
                    'hex'
                );

                rsaSign.update(rsaPubPem);
                let rsaSignature = rsaSign.sign(rsaKeyPem, 'hex');
                assert.equal(rsaSignature, expectedSignature);

                rsaVerify.update(rsaPubPem);
                assert.equal(rsaVerify.verify(rsaPubPem, rsaSignature, 'hex'), true);

                // Test RSA PKCS#8 key signing/verification
                rsaSign = crypto.createSign('SHA1');
                rsaSign.update(rsaPubPem);
                rsaSignature = rsaSign.sign(rsaPkcs8KeyPem, 'hex');
                assert.equal(rsaSignature, expectedSignature);

                rsaVerify = crypto.createVerify('SHA1');
                rsaVerify.update(rsaPubPem);
                assert.equal(rsaVerify.verify(rsaPubPem, rsaSignature, 'hex'), true);

                // Test RSA key signing/verification with encrypted key
                rsaSign = crypto.createSign('SHA1');
                rsaSign.update(rsaPubPem);
                const signOptions = { key: rsaKeyPemEncrypted, passphrase: 'password' };
                rsaSignature = rsaSign.sign(signOptions, 'hex');
                assert.equal(rsaSignature, expectedSignature);

                rsaVerify = crypto.createVerify('SHA1');
                rsaVerify.update(rsaPubPem);
                assert.equal(rsaVerify.verify(rsaPubPem, rsaSignature, 'hex'), true);

                rsaSign = crypto.createSign('SHA1');
                rsaSign.update(rsaPubPem);
                assert.throws(() => {
                    const signOptions = { key: rsaKeyPemEncrypted, passphrase: 'wrong' };
                    rsaSign.sign(signOptions, 'hex');
                });
            });

            it('RSA signing and verification', () => {
                const privateKey = readKey('rsa_private_b.pem');
                const publicKey = readKey('rsa_public_b.pem');

                const input = 'I AM THE WALRUS';

                const signature = readKey(
                    'I_AM_THE_WALRUS_sha256_signature_signedby_rsa_private_b.sha256',
                    'hex'
                );

                const sign = crypto.createSign('SHA256');
                sign.update(input);

                const output = sign.sign(privateKey, 'hex');
                assert.equal(output, signature);

                const verify = crypto.createVerify('SHA256');
                verify.update(input);

                assert.equal(verify.verify(publicKey, signature, 'hex'), true);

                const sign2 = crypto.createSign('RSA-SHA256');
                sign2.update(input);

                const output2 = sign2.sign(privateKey, 'hex');
                assert.equal(output2, signature);

                const verify2 = crypto.createVerify('SHA256');
                verify2.update(input);

                assert.equal(verify2.verify(publicKey, signature, 'hex'), true);
            });

            it('DSA signing and verification', () => {
                const input = 'I AM THE WALRUS';

                const sign = crypto.createSign('SHA1');
                sign.update(input);
                const signature = sign.sign(dsaKeyPem, 'hex');

                const verify = crypto.createVerify('SHA1');
                verify.update(input);

                assert.equal(verify.verify(dsaPubPem, signature, 'hex'), true);

                const sign2 = crypto.createSign('DSS1');
                sign2.update(input);
                const signature2 = sign2.sign(dsaKeyPem, 'hex');

                const verify2 = crypto.createVerify('DSS1');
                verify2.update(input);

                assert.equal(verify2.verify(dsaPubPem, signature2, 'hex'), true);
            });

            it('DSA signing and verification with PKCS#8 private key', () => {
                const input = 'I AM THE WALRUS';

                const sign = crypto.createSign('SHA1');
                sign.update(input);
                const signature = sign.sign(dsaPkcs8KeyPem, 'hex');

                const verify = crypto.createVerify('SHA1');
                verify.update(input);

                assert.equal(verify.verify(dsaPubPem, signature, 'hex'), true);
            });

            it('DSA signing and verification with encrypted key', () => {
                const input = 'I AM THE WALRUS';

                const sign = crypto.createSign('SHA1');
                sign.update(input);
                assert.throws(() => {
                    sign.sign({ key: dsaKeyPemEncrypted, passphrase: 'wrong' }, 'hex');
                });
            });

            it('signatures vary across runs so there is no static string to verify against', () => {
                const input = 'I AM THE WALRUS';

                const sign = crypto.createSign('SHA1');
                sign.update(input);
                const signOptions = { key: dsaKeyPemEncrypted, passphrase: 'password' };
                const signature = sign.sign(signOptions, 'hex');

                const verify = crypto.createVerify('SHA1');
                verify.update(input);

                assert.equal(verify.verify(dsaPubPem, signature, 'hex'), true);
            });

            it('check arguments', () => {
                assert.throws(
                    () => crypto.createVerify('SHA256').verify({
                        key: certPem,
                        padding: null,
                    }));

                assert.throws(
                    () => crypto.createVerify('SHA256').verify({
                        key: certPem,
                        saltLength: null,
                    }));

                const sign = crypto.createSign('SHA1');
                const verify = crypto.createVerify('SHA1');

                [1, [], {}, undefined, null, true, Infinity].forEach((input) => {
                    assert.throws(() => crypto.createSign(input));
                    assert.throws(() => crypto.createVerify(input));
                    // assert.throws(() => sign.update(input));
                    // assert.throws(() => verify.update(input));
                    assert.throws(() => sign._write(input, 'utf8', () => { }));
                    assert.throws(() => verify._write(input, 'utf8', () => { }));
                });

                [
                    Uint8Array, Uint16Array, Uint32Array, Float32Array, Float64Array,
                ].forEach((clazz) => {
                    // These should all just work
                    sign.update(new clazz());
                    verify.update(new clazz());
                });

                [1, {}, [], Infinity].forEach((input) => {
                    assert.throws(() => sign.sign(input));
                    assert.throws(() => verify.verify(input));
                    assert.throws(() => verify.verify('test', input));
                });

                assert.throws(() => crypto.createSign('sha8'));
                assert.throws(() => crypto.sign('sha8', Buffer.alloc(1), keyPem));

                assert.throws(() => crypto.createSign('SHA1').update('Test123').sign(null, 'base64'));

                [1, {}, [], true, Infinity].forEach((input) => {
                    const data = Buffer.alloc(1);
                    const sig = Buffer.alloc(1);

                    assert.throws(() => crypto.sign(null, input, 'asdf'));
                    assert.throws(() => crypto.verify(null, input, 'asdf', sig));

                    assert.throws(() => crypto.sign(null, data, input));
                    assert.throws(() => crypto.verify(null, data, input, sig));

                    assert.throws(() => crypto.verify(null, data, 'test', input));
                });

                for (const key of ['', 'foo', null, undefined, true, Boolean]) {
                    assert.throws(() => {
                        crypto.verify('sha256', 'foo', { key, format: 'jwk' }, Buffer.alloc(0));
                    });
                    assert.throws(() => {
                        crypto.createVerify('sha256').verify({ key, format: 'jwk' }, Buffer.alloc(0));
                    });
                    assert.throws(() => {
                        crypto.sign('sha256', 'foo', { key, format: 'jwk' });
                    });
                    assert.throws(() => {
                        crypto.createSign('sha256').sign({ key, format: 'jwk' });
                    });
                }
            });

            it('Special tests for RSA_PKCS1_PSS_PADDING', () => {
                function testPSS(algo, hLen) {
                    // Maximum permissible salt length
                    const max = keySize / 8 - hLen - 2;

                    function getEffectiveSaltLength(saltLength) {
                        switch (saltLength) {
                            case crypto.constants.RSA_PSS_SALTLEN_DIGEST:
                                return hLen;
                            case crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN:
                                return max;
                            default:
                                return saltLength;
                        }
                    }

                    const signSaltLengths = [
                        crypto.constants.RSA_PSS_SALTLEN_DIGEST,
                        getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_DIGEST),
                        crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN,
                        getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN),
                        0, 16, 32, 64, 128,
                    ];

                    const verifySaltLengths = [
                        crypto.constants.RSA_PSS_SALTLEN_DIGEST,
                        getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_DIGEST),
                        getEffectiveSaltLength(crypto.constants.RSA_PSS_SALTLEN_MAX_SIGN),
                        0, 16, 32, 64, 128,
                    ];
                    const errMessage = /^Error:.*data too large for key size$/;

                    const data = Buffer.from('Test123');

                    signSaltLengths.forEach((signSaltLength) => {
                        if (signSaltLength > max) {
                            // If the salt length is too big, an Error should be thrown
                            assert.throws(() => {
                                crypto.createSign(algo)
                                    .update(data)
                                    .sign({
                                        key: keyPem,
                                        padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                        saltLength: signSaltLength
                                    });
                            }, errMessage);
                            assert.throws(() => {
                                crypto.sign(algo, data, {
                                    key: keyPem,
                                    padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                    saltLength: signSaltLength
                                });
                            }, errMessage);
                        } else {
                            // Otherwise, a valid signature should be generated
                            const s4 = crypto.createSign(algo)
                                .update(data)
                                .sign({
                                    key: keyPem,
                                    padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                    saltLength: signSaltLength
                                });
                            const s4_2 = crypto.sign(algo, data, {
                                key: keyPem,
                                padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                saltLength: signSaltLength
                            });

                            [s4, s4_2].forEach((sig) => {
                                let verified;
                                verifySaltLengths.forEach((verifySaltLength) => {
                                    // Verification should succeed if and only if the salt length is
                                    // correct
                                    verified = crypto.createVerify(algo)
                                        .update(data)
                                        .verify({
                                            key: certPem,
                                            padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                            saltLength: verifySaltLength
                                        }, sig);
                                    assert.strictEqual(verified, crypto.verify(algo, data, {
                                        key: certPem,
                                        padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                        saltLength: verifySaltLength
                                    }, sig));
                                    const saltLengthCorrect = getEffectiveSaltLength(signSaltLength) ===
                                        getEffectiveSaltLength(verifySaltLength);
                                    assert.strictEqual(verified, saltLengthCorrect);
                                });

                                // Verification using RSA_PSS_SALTLEN_AUTO should always work
                                verified = crypto.createVerify(algo)
                                    .update(data)
                                    .verify({
                                        key: certPem,
                                        padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                        saltLength: crypto.constants.RSA_PSS_SALTLEN_AUTO
                                    }, sig);
                                assert.strictEqual(verified, true);
                                assert.strictEqual(verified, crypto.verify(algo, data, {
                                    key: certPem,
                                    padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                    saltLength: crypto.constants.RSA_PSS_SALTLEN_AUTO
                                }, sig));

                                // Verifying an incorrect message should never work
                                const wrongData = Buffer.from('Test1234');
                                verified = crypto.createVerify(algo)
                                    .update(wrongData)
                                    .verify({
                                        key: certPem,
                                        padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                        saltLength: crypto.constants.RSA_PSS_SALTLEN_AUTO
                                    }, sig);
                                assert.strictEqual(verified, false);
                                assert.strictEqual(verified, crypto.verify(algo, wrongData, {
                                    key: certPem,
                                    padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                    saltLength: crypto.constants.RSA_PSS_SALTLEN_AUTO
                                }, sig));
                            });
                        }
                    });
                }

                testPSS('SHA1', 20);
                testPSS('SHA256', 32);
            });

            it('Test vectors for RSA_PKCS1_PSS_PADDING provided by the RSA Laboratories', () => {
                function testVerify(cert, vector) {
                    const verified = crypto.createVerify('SHA1')
                        .update(Buffer.from(vector.message, 'hex'))
                        .verify({
                            key: cert,
                            padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                            saltLength: vector.salt.length / 2
                        }, vector.signature, 'hex');
                    assert.strictEqual(verified, true);
                }

                const examples = JSON.parse(readSync('pss-vectors.json', 'utf8'));

                for (const key in examples) {
                    const example = examples[key];
                    const publicKey = example.publicKey.join('\n');
                    example.tests.forEach((test) => testVerify(publicKey, test));
                }
            });

            it('exceptions for invalid `padding` and `saltLength` values', () => {
                [null, NaN, 'boom', {}, [], true, false]
                    .forEach((invalidValue) => {
                        assert.throws(() => {
                            crypto.createSign('SHA256')
                                .update('Test123')
                                .sign({
                                    key: keyPem,
                                    padding: invalidValue
                                });
                        }, {
                            code: 'ERR_INVALID_ARG_VALUE',
                            name: 'TypeError'
                        });

                        assert.throws(() => {
                            crypto.createSign('SHA256')
                                .update('Test123')
                                .sign({
                                    key: keyPem,
                                    padding: crypto.constants.RSA_PKCS1_PSS_PADDING,
                                    saltLength: invalidValue
                                });
                        }, {
                            code: 'ERR_INVALID_ARG_VALUE',
                            name: 'TypeError'
                        });
                    });

                assert.throws(() => {
                    crypto.createSign('SHA1')
                        .update('Test123')
                        .sign({
                            key: keyPem,
                            padding: crypto.constants.RSA_PKCS1_OAEP_PADDING
                        });
                });
            });

            it('ed signing and verification', () => {
                [
                    {
                        private: readKey('ed25519_private.pem', 'ascii'),
                        public: readKey('ed25519_public.pem', 'ascii'),
                        algo: null,
                        sigLen: 64
                    },
                    {
                        private: readKey('ed448_private.pem', 'ascii'),
                        public: readKey('ed448_public.pem', 'ascii'),
                        algo: null,
                        sigLen: 114
                    },
                    {
                        private: readKey('rsa_private_2048.pem', 'ascii'),
                        public: readKey('rsa_public_2048.pem', 'ascii'),
                        algo: 'sha1',
                        sigLen: 256
                    },
                ].forEach((pair) => {
                    const algo = pair.algo;

                    {
                        const data = Buffer.from('Hello world');
                        const sig = crypto.sign(algo, data, pair.private);
                        assert.strictEqual(sig.length, pair.sigLen);

                        assert.strictEqual(crypto.verify(algo, data, pair.private, sig),
                            true);
                        assert.strictEqual(crypto.verify(algo, data, pair.public, sig),
                            true);
                    }

                    {
                        const data = Buffer.from('Hello world');
                        const privKeyObj = crypto.createPrivateKey(pair.private);
                        const pubKeyObj = crypto.createPublicKey(pair.public);

                        const sig = crypto.sign(algo, data, privKeyObj);
                        assert.strictEqual(sig.length, pair.sigLen);

                        assert.strictEqual(crypto.verify(algo, data, privKeyObj, sig), true);
                        assert.strictEqual(crypto.verify(algo, data, pubKeyObj, sig), true);
                    }

                    {
                        const data = Buffer.from('Hello world');
                        const otherData = Buffer.from('Goodbye world');
                        const otherSig = crypto.sign(algo, otherData, pair.private);
                        assert.strictEqual(crypto.verify(algo, data, pair.private, otherSig),
                            false);
                    }
                });
            });

            it('ec signing and verification', () => {
                const data = Buffer.from('Hello world');
                const keys = [['ec-key.pem', 64], ['dsa_private_1025.pem', 40]];

                for (const [file, length] of keys) {
                    const privKey = readKey(file);
                    [
                        crypto.createSign('sha1').update(data).sign(privKey),
                        crypto.sign('sha1', data, privKey),
                        crypto.sign('sha1', data, { key: privKey, dsaEncoding: 'der' }),
                    ].forEach((sig) => {
                        // Signature length variability due to DER encoding
                        assert(sig.length >= length + 4 && sig.length <= length + 8);

                        assert.strictEqual(
                            crypto.createVerify('sha1').update(data).verify(privKey, sig),
                            true
                        );
                        assert.strictEqual(crypto.verify('sha1', data, privKey, sig), true);
                    });

                    // Test (EC)DSA signature conversion.
                    const opts = { key: privKey, dsaEncoding: 'ieee-p1363' };
                    let sig = crypto.sign('sha1', data, opts);
                    // Unlike DER signatures, IEEE P1363 signatures have a predictable length.
                    assert.strictEqual(sig.length, length);
                    assert.strictEqual(crypto.verify('sha1', data, opts, sig), true);
                    assert.strictEqual(crypto.createVerify('sha1')
                        .update(data)
                        .verify(opts, sig), true);

                    // Test invalid signature lengths.
                    for (const i of [-2, -1, 1, 2, 4, 8]) {
                        sig = crypto.randomBytes(length + i);
                        let result;
                        try {
                            result = crypto.verify('sha1', data, opts, sig);
                        } catch (err) {
                            assert.match(err.message, /asn1 encoding/);
                            assert.strictEqual(err.library, 'asn1 encoding routines');
                            continue;
                        }
                        assert.strictEqual(result, false);
                    }
                }

                // Test verifying externally signed messages.
                const extSig = Buffer.from('494c18ab5c8a62a72aea5041966902bcfa229821af2bf65' +
                    '0b5b4870d1fe6aebeaed9460c62210693b5b0a300033823' +
                    '33d9529c8abd8c5948940af944828be16c', 'hex');
                for (const ok of [true, false]) {
                    assert.strictEqual(
                        crypto.verify('sha256', data, {
                            key: readKey('ec-key.pem'),
                            dsaEncoding: 'ieee-p1363'
                        }, extSig),
                        ok
                    );

                    assert.strictEqual(
                        crypto.createVerify('sha256').update(data).verify({
                            key: readKey('ec-key.pem'),
                            dsaEncoding: 'ieee-p1363'
                        }, extSig),
                        ok
                    );

                    extSig[Math.floor(Math.random() * extSig.length)] ^= 1;
                }

                // Non-(EC)DSA keys should ignore the option.
                const sig = crypto.sign('sha1', data, {
                    key: keyPem,
                    dsaEncoding: 'ieee-p1363'
                });
                assert.strictEqual(crypto.verify('sha1', data, certPem, sig), true);
                assert.strictEqual(
                    crypto.verify('sha1', data, {
                        key: certPem,
                        dsaEncoding: 'ieee-p1363'
                    }, sig),
                    true
                );
                assert.strictEqual(
                    crypto.verify('sha1', data, {
                        key: certPem,
                        dsaEncoding: 'der'
                    }, sig),
                    true
                );

                for (const dsaEncoding of ['foo', null, {}, 5, true, NaN]) {
                    assert.throws(() => {
                        crypto.sign('sha1', data, {
                            key: certPem,
                            dsaEncoding
                        });
                    });
                }
            });

            describe('RSA-PSS', () => {
                it('This key pair does not restrict the message digest algorithm or salt length', () => {
                    const publicPem = readKey('rsa_pss_public_2048.pem');
                    const privatePem = readKey('rsa_pss_private_2048.pem');

                    const publicKey = crypto.createPublicKey(publicPem);
                    const privateKey = crypto.createPrivateKey(privatePem);

                    for (const key of [privatePem, privateKey]) {
                        // Any algorithm should work.
                        for (const algo of ['sha1', 'sha256']) {
                            // Any salt length should work.
                            for (const saltLength of [undefined, 8, 10, 12, 16, 18, 20]) {
                                const signature = crypto.sign(algo, 'foo', { key, saltLength });

                                for (const pkey of [key, publicKey, publicPem]) {
                                    const okay = crypto.verify(
                                        algo,
                                        'foo',
                                        { key: pkey, saltLength },
                                        signature
                                    );

                                    assert.ok(okay);
                                }
                            }
                        }
                    }
                });

                it('This key pair enforces sha256 as the message digest and the MGF1 message digest and a salt length of at least 16 bytes', () => {
                    const publicPem = readKey('rsa_pss_public_2048_sha256_sha256_16.pem');
                    const privatePem = readKey('rsa_pss_private_2048_sha256_sha256_16.pem');

                    const publicKey = crypto.createPublicKey(publicPem);
                    const privateKey = crypto.createPrivateKey(privatePem);

                    for (const key of [privatePem, privateKey]) {
                        // Signing with anything other than sha256 should fail.
                        assert.throws(() => {
                            crypto.sign('sha1', 'foo', key);
                        }, /digest not allowed/);

                        // Signing with salt lengths less than 16 bytes should fail.
                        for (const saltLength of [8, 10, 12]) {
                            assert.throws(() => {
                                crypto.sign('sha256', 'foo', { key, saltLength });
                            }, /pss saltlen too small/);
                        }

                        // Signing with sha256 and appropriate salt lengths should work.
                        for (const saltLength of [undefined, 16, 18, 20]) {
                            const signature = crypto.sign('sha256', 'foo', { key, saltLength });

                            for (const pkey of [key, publicKey, publicPem]) {
                                const okay = crypto.verify(
                                    'sha256',
                                    'foo',
                                    { key: pkey, saltLength },
                                    signature
                                );

                                assert.ok(okay);
                            }
                        }
                    }
                });

                it('This key enforces sha512 as the message digest and sha256 as the MGF1 message digest', () => {
                    const publicPem = readKey('rsa_pss_public_2048_sha512_sha256_20.pem');
                    const privatePem = readKey('rsa_pss_private_2048_sha512_sha256_20.pem');

                    const publicKey = crypto.createPublicKey(publicPem);
                    const privateKey = crypto.createPrivateKey(privatePem);

                    for (const key of [privatePem, privateKey]) {
                        // sha256 matches the MGF1 hash function and should be used internally,
                        // but it should not be permitted as the main message digest algorithm.
                        for (const algo of ['sha1', 'sha256']) {
                            assert.throws(() => {
                                crypto.sign(algo, 'foo', key);
                            }, /digest not allowed/);
                        }

                        // sha512 should produce a valid signature.
                        const signature = crypto.sign('sha512', 'foo', key);

                        for (const pkey of [key, publicKey, publicPem]) {
                            const okay = crypto.verify('sha512', 'foo', pkey, signature);

                            assert.ok(okay);
                        }
                    }
                });

            });
        });

        describe('bls', () => {
            var g1_key = {
                "kty": "OKP",
                "crv": "Bls12381G1",
                "x": "tCgCNuUYQotPEsrljWi-lIRIPpzhqsnJV1NPnE7je6glUb-FJm9IYkuv2hbHw22i",
                "d": "TXNvJBBG3h23H5hFJcnRZmYd_j1TqpwtJOllYGU3yyw"
            };
            var g2_key = {
                "kty": "OKP",
                "crv": "Bls12381G2",
                "x": "h_rkcTKXXzRbOPr9UxSfegCbid2U_cVNXQUaKeGF7UhwrMJFP70uMH0VQ9-3-_2zDPAAjflsdeLkOXW3-ShktLxuPy8UlXSNgKNmkfb-rrj-FRwbs13pv_WsIf-eV66-",
                "d": "PofPmtCTsMilP9gluxrSDTC7DPbKwSMEzxVCZxq_L2I"
            };

            var data = [
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G1",
                        "x": "uElgtW9XsEMOa9tPoVcOe2JeShWPP10ChSFnh3V7Q-7R5qwX8BDtzMfJumlzsORn",
                        "d": "CSSRytF2pUMEA6Bogcr0iL3nhj14Lk9vQeA1pzfi6rY"
                    },
                    "msg": "jElpsgs1VmagSI1PBFsZjy2pqgIDdY7IhjhYNZ6keZESuBlqfeUgm7tLUFCiQkS0fkFDvppKA4nVdjUmlTR6oxkogtVD0TBO4ZupYrupy4S01Vzq2a2pdJ5GeG2JVHNEPdk2pysreghhkbR1ivdBoW3FuuL61hNP3KTa4l7BiEg=",
                    "sig": "kv83d2R0yGvrm6W111HTwo9Dj2NkIWXP/Uxc/93Nxe5aMkSQoL/Aa4D2tex0G+p/E3mzNJ6GZfTQrtMardoVXFq84EAahifIwgABo53dmPzFV56n5hfojOn9uYKJeS8f"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G1",
                        "x": "iL__ckqXV81buV8-hlvwbwauTw14ZWTR43q6ldpKvCz-WrAMyWTlRgGhF2wqwPRm",
                        "d": "aA3_w3JT3al7_tnTjcqxLW4qR9b9Yg1z_0nW2SCdPX0"
                    },
                    "msg": "HIlNfFLhWwjgM/qmSH5Z2ssSbHr8PSL+eTuVUclvAd5SfNAFDWaN0yK9lGrPnXXjhNg6mBdFKFWq5kqAN69p2JLEylN5XmVHvnQ3mpToxL1HgQ6wHdCecoDFKw2Xz0OLyxBDazLqxlIIx2HYIY996zh2QoxR+weuSK2tzs2P6cw=",
                    "sig": "suveDMw9QwNvwHNX0kVeicxdrcflsxWgG/d24zlHDuVfn1b61bXlg5I6LhGoiUkyFvo1htWwIoYM8gfMBagYoTm7plfRRKlVNT/w7Kc6Le/YWsF/vioVQesPBgprFh23"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G1",
                        "x": "tUDPV1FO8O095QrxsrAdn8mqv6J4pZV-Y8befAUM3Q2MMSvnVLdzw-CcDFOOcFim",
                        "d": "EZgncKJp5TuYoNVnsrkiZskoB5Mtni3ak27gTqCHUUY"
                    },
                    "msg": "ioTlVvHd45u7qKcFe31QBXlpLWM2LOTGXPQPbf/qMLk67I0nxT3G7rZ7KP54xRJFV+qscadyepgGBnqNk9EHjbUmt5+4bB9F2/JSr+0g5v99krUVR0R1+bQfjO83eleejMzzDUxe60uXUPkKxOg7vxAhOaeR6JG6gGpm1gXAp2U=",
                    "sig": "jreHdvZoTJAsaovcGz2icn0GK2qQAyU35kB1hhLGJDl8ErHFhiAJDjhMPHnhimTaEf5eaKiHqCqoOTDpLzXEzbY3jK+tNqN3/Qh+KxX8aQXFWU62JuqjiQxiuIrCTkhm"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G1",
                        "x": "iCNl2W1XJ6T5I7P5jYng32ZooWxdf2fwgGTG0O8pRSpkk_ghYnl-iy51glBOvBkG",
                        "d": "YI3BbYXyb7-NrgAyz_0mzGyp0mivOswUT-H_I4pTKRs"
                    },
                    "msg": "uzZ52nehqBO1CaBgACRsOO79gMHQzAF85vChfnMA31qHNKKaWPaVYXiXLxRxydLF+I8YwzsLq9PjWFyseKXD/o+xQ9OJNasfpofGhJ+2aHIrUl4CcLNvDwfJBdUPl4f9NoEpJPYnoXs4NJ7snlOQ7Zw0WdXcLTqE5iXgudUDPuE=",
                    "sig": "qiRqZDulMGbho2cTuiGMPcqfZmVEmknAnq6dzQ9yksBgOIxCI/C1HVRicT773CODEftxAjgnY2/E9ERac2mBkj1rkiaKcFbWLUi5+JfMzNQaxdo5YAwX2ylRY+aVTjIW"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G1",
                        "x": "oPMpdV39d4eJnqrZTEu2ynLNc4Y5mDF0FN66XZqpi4TJBn5pvtO-DTcbSH6KV9sA",
                        "d": "beISCSgyCIyNBFUQWtk54W_zt8BakojKzxbFKzz7FvA"
                    },
                    "msg": "iM5SZ7qFMGMzX7gPRvWVnMuWAwaqep0KB9FHHKTuPqSOdnR15BLBuJvfmvVRtVKgeDf73739WQyd5N8h5JDPa2cuOomhjmK5KpwJ7SUxmnuQINK6Dlp8GlYXj8asVv6w0JLsRebsui/YQosNCaLXuZKxfhgXq1K9uP+nLU7KesY=",
                    "sig": "uXsyK7BbREOomV9GcIProtwe6Rl6rsVcB4hgRGIjYygS6hSGi//A/L0fl+EVrSGEFndF4UyqK10K8pPl2fDZC182eGqZeDc4X/Wh5EUjFvR1TcEqjTNMj/3/oPt70bAz"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G2",
                        "x": "tgtAZDN060uN5g_PuHMkUV6h1nSAlM9qX6y1WxEe5AQNX3BXbnxMozbfvcCI6ga8CnA-uqMjhd1faq_y3V_lRcuY6bw2AdlRHoIWQRF3EmEdZNfQbLF1oLJrQg7gIBhE",
                        "d": "LMiMiwVdG2qiBw-GnIxeGDoGjBeHEKXP5FX5OqKS-wU"
                    },
                    "msg": "1GwhHJGZaB84s/S1lUD6fDS0ud+kfD755wpBvYwh1aJEXsVMxveoZ92mTi14PG2ppnLcC0FcnPJHj3QCKBlnbokx5pTei1mC1tPMxHRxG0JgrfX4XR8Fk7tFErVIQVu7FnCyyuZtcSc+dijTxVvssGOKggVe0yihR6MnFbdK/ok=",
                    "sig": "iQHZfwQWbFItoEZaF1DwNuFkoU7j9NWvMbdlc/5gaL2uRXvxCrfkTK7ZefaxdvlS"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G2",
                        "x": "gROJOPH9n1ylOfSXOZlw0_wN-MR44IZIESsOG51OaRaSjtoVW-BT2e5OCl9Jdy-oChtPDfTqcTdzsyCNNBKMIETTxGHhcAbsV3nvM5ZZAyDvzvFF2-q3BnElzORpg9AF",
                        "d": "PTVSoVgZWlQZGjs_xEK6EsNot44DSlgIgN9jEFrjsU8"
                    },
                    "msg": "TkDnNc0eSPLqkqqI+OgZsP2ZUS6q5nNTk0/PHuP9vYfckBXE/ZZjHW0hyNu/bySDle/htSSNph8TIb0mhdjmAEdYH6CnvwzA/LWzc1tDO8RPBPuonKWVogfn3IrW7X7vIYX27QTsNGw/Vhd6V5jM6sg0IN7l/TFU114SdLVb/kw=",
                    "sig": "svIOXfy5H0F8bSf36O5sOD0Zbn7BYQ80p3GBD6atwLZjOskYp34KPLcF4PA4d4Mr"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G2",
                        "x": "keE8femPCf2ySK2T-_RapxMdhhRYDCqqwu3aCUXbASj2nzhnYroO-WUj0NNxcehaADRdHMraw-fLLwyENm1sw0P_lOOV3zWf6UHE30B8uDJD7l5fzpoXaRLjAIjfED8y",
                        "d": "PYg77h91cMKYX9g66IpiGl7lkzrhM97jjd0MPIl_EG8"
                    },
                    "msg": "ATWwyqbyWIJnZf+8ogqnfoe78BtiQafBVePVx9BMAPO+AAMnY3Fe9bh5D3wbqUPOksv84prFdn+8vXgxnu71elfdzjd9XIZKKLgB2/VmQKW8d8QgahcX+1KG/j9FniOmWshTUmnv3Tk3ssJn/iSRrV0r9vWan+NUu/2ski+rBBc=",
                    "sig": "kK9bKX7DdkBB6e2l9ima2iV9APdXFupfGaCY1YeL09I/o7FpmAYoO69zEhzoTGuN"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G2",
                        "x": "te4Ca0MzlN-oRx0Yn6-RSW-k4Erg8IXvWjgIrgePOjnkQoZEZlxPHCP3awIp1X76Aj4w3HADDTXGpvyiJiT7lfUWcN3FM9YCthI7eDI5P6IW4_XBGBlRkzHB3_Fl8239",
                        "d": "aez939T2kwIwinRaAQkvhCy6COKCd2wdilmWQNAYHRs"
                    },
                    "msg": "BwCzYxHjRVMAO2ORRh3VxSqEadRJn9YAleoMOF1JoMqxYir0brCCe66WVvFHHYEZ8mqPGj+4pBpK6+JdzQM/clIL98YDGl2w5L0BpAK4GcaV87+K37uCoq9Vugj5Fx/1jKqYeeGasSLJ8jcGUDDmrSfIpkJtrJ+lPLUtzgmUwhM=",
                    "sig": "hIW/7naMNFKHq3/tn0GvczGaUP/4XGIE45HeG3f5FCyAaIen/SSxTtrwxk0ykPMM"
                },
                {
                    "key": {
                        "kty": "OKP",
                        "crv": "Bls12381G2",
                        "x": "sTkg0tr5zKlD6RBUZPd6lss51M68bgKOuygneAgEoS5BItuBjnbrZ-Wf4z7Uw1ZSGaTZILiz8BMsSxlUJZQlBFfRpY6SSDKwY9s82tHCrU1P2l0M6zO_wxI6ZI8zqk23",
                        "d": "ZTazAk2Zzj0y0wZx0q2lazNiTDlBsAgeUaXywUgPcNA"
                    },
                    "msg": "PhYCksp+xgq8xMIdcMXur4KQSAPJQEc3YMuW74FUj3duOnBZharx1ToEFaUS5U/vq8mWr4zZp8NCN5rqXi1WVX/XPDXmSQRgSXq29dA3Ry8AY5tiBH3i4ka65TACzdMLxT3u7zkiKlKrsosmt+uPZha+AGdg+99w/+XAcNUWZFU=",
                    "sig": "slZnay/PUX6yXQxnhCzla7NMlx7FRD4OqlXDj+mXzqFLIgffY4r3nmN4aR7WIV7o"
                }
            ];

            it("export/import json", () => {
                var sk = crypto.createPrivateKey({
                    key: g1_key
                });
                assert.deepEqual(sk.toJSON(), g1_key);

                var sk = crypto.createPrivateKey({
                    key: g2_key
                });
                assert.deepEqual(sk.toJSON(), g2_key);
            });

            it("private only key", () => {
                var sk = crypto.createPrivateKey({
                    key: {
                        "kty": "OKP",
                        "crv": "Bls12381G1",
                        "d": "TXNvJBBG3h23H5hFJcnRZmYd_j1TqpwtJOllYGU3yyw"
                    }
                });
                assert.deepEqual(sk.toJSON(), g1_key);

                var sk = crypto.createPrivateKey({
                    key: {
                        "kty": "OKP",
                        "crv": "Bls12381G2",
                        "d": "PofPmtCTsMilP9gluxrSDTC7DPbKwSMEzxVCZxq_L2I"
                    }
                });
                assert.deepEqual(sk.toJSON(), g2_key);
            });

            it("get public key", () => {
                var sk = crypto.createPrivateKey({
                    key: g1_key
                });
                assert.deepEqual(crypto.createPublicKey(sk).toJSON(), {
                    "kty": "OKP", "crv": "Bls12381G1",
                    "x": "tCgCNuUYQotPEsrljWi-lIRIPpzhqsnJV1NPnE7je6glUb-FJm9IYkuv2hbHw22i"
                });

                var sk = crypto.createPrivateKey({
                    key: g2_key
                });
                assert.deepEqual(crypto.createPublicKey(sk).toJSON(), {
                    "kty": "OKP", "crv": "Bls12381G2",
                    "x": "h_rkcTKXXzRbOPr9UxSfegCbid2U_cVNXQUaKeGF7UhwrMJFP70uMH0VQ9-3-_2zDPAAjflsdeLkOXW3-ShktLxuPy8UlXSNgKNmkfb-rrj-FRwbs13pv_WsIf-eV66-"
                });
            });

            it("sign/verify", () => {
                var sk = crypto.createPrivateKey({
                    key: g1_key
                });
                var pk = crypto.createPublicKey(sk);

                var sig = crypto.sign('SHA256', 'abcd', sk);
                assert.isTrue(crypto.verify('SHA256', 'abcd', pk, sig));
                assert.isFalse(crypto.verify('SHA256', 'abcd1', pk, sig));
            });

            it("generateKey", () => {
                var keys = crypto.generateKeyPair('Bls12381G1');

                var sig = crypto.sign('SHA256', 'abcd', keys.privateKey);
                assert.isTrue(crypto.verify('SHA256', 'abcd', keys.publicKey, sig));
                assert.isFalse(crypto.verify('SHA256', 'abcd1', keys.publicKey, sig));

                var keys = crypto.generateKeyPair('Bls12381G2');

                var sig = crypto.sign('SHA256', 'abcd', keys.privateKey);
                assert.isTrue(crypto.verify('SHA256', 'abcd', keys.publicKey, sig));
                assert.isFalse(crypto.verify('SHA256', 'abcd1', keys.publicKey, sig));
            });

            it("asymmetricKeyType", () => {
                var sk = crypto.createPrivateKey({
                    key: g1_key
                });
                assert.deepEqual(sk.asymmetricKeyType, "Bls12381G1");

                var sk = crypto.createPrivateKey({
                    key: g2_key
                });
                assert.deepEqual(sk.asymmetricKeyType, "Bls12381G2");
            });

            it("test suite", () => {
                data.forEach(c => {
                    var sk = crypto.createPrivateKey({
                        key: c.key
                    });

                    var pk = crypto.createPublicKey(sk);

                    var data = base64.decode(c.msg);

                    var sig = crypto.sign('SHA256', data, sk);
                    assert.equal(sig.base64(), c.sig);
                    assert.isTrue(crypto.verify('SHA256', data, pk, sig));
                    data[0] = (data[0] + 1) & 0xff;
                    assert.isFalse(crypto.verify('SHA256', data, pk, sig));
                });
            });
        });
    });

    describe('Cipher', () => {
        describe("cipher/decipher", () => {
            it("normal", () => {
                function testCipher1(key) {
                    const plaintext = 'Keep this a secret? No! Tell everyone about node.js!';
                    const cipher = crypto.createCipher('aes192', key);

                    let ciph = cipher.update(plaintext, 'utf8', 'hex');
                    ciph += cipher.final('hex');

                    const decipher = crypto.createDecipher('aes192', key);
                    let txt = decipher.update(ciph, 'hex', 'utf8');
                    txt += decipher.final('utf8');

                    assert.equal(txt, plaintext);
                }

                testCipher1('MySecretKey123');
                testCipher1(Buffer.from('MySecretKey123'));
            });

            it("encoding", () => {
                function testCipher2(key) {
                    const plaintext =
                        '32|RmVZZkFUVmpRRkp0TmJaUm56ZU9qcnJkaXNNWVNpTTU*|iXmckfRWZBGWWELw' +
                        'eCBsThSsfUHLeRe0KCsK8ooHgxie0zOINpXxfZi/oNG7uq9JWFVCk70gfzQH8ZUJ' +
                        'jAfaFg**';
                    const cipher = crypto.createCipher('aes256', key);

                    let ciph = cipher.update(plaintext, 'utf8', 'base64');
                    ciph += cipher.final('base64');

                    const decipher = crypto.createDecipher('aes256', key);
                    let txt = decipher.update(ciph, 'base64', 'utf8');
                    txt += decipher.final('utf8');

                    assert.equal(txt, plaintext);
                }

                testCipher2('0123456789abcdef');
                testCipher2(Buffer.from('0123456789abcdef'));
            });

            it("check arguments", () => {
                assert.throws(() => crypto.createCipher(null));
                assert.throws(() => crypto.createCipher('aes-256-cbc', null));
                assert.throws(() => crypto.createCipher('aes-256-cbc', 'secret').setAAD(null));

                assert.throws(() => crypto.createDecipher(null));
                assert.throws(() => crypto.createDecipher('aes-256-cbc', 'secret').setAuthTag(null));
                assert.throws(() => crypto.createDecipher('aes-256-cbc', null));
            });

            it("base64 padding regression", () => {
                const c = crypto.createCipher('aes-256-cbc', 'secret');
                const s = c.update('test', 'utf8', 'base64') + c.final('base64');
                assert.equal(s, '375oxUQCIocvxmC5At+rvA==');
            });

            it("calling final() twice", () => {
                const c = crypto.createCipher('aes-256-cbc', 'secret');
                assert.throws(() => c.final('xxx'));
                assert.throws(() => c.final('xxx'));
                assert.throws(() => c.final('xxx'));

                const d = crypto.createDecipher('aes-256-cbc', 'secret');
                assert.throws(() => d.final('xxx'));
                assert.throws(() => d.final('xxx'));
                assert.throws(() => d.final('xxx'));
            });

            it("utf8 encoding", () => {
                let c = crypto.createCipher('aes192', '0123456789abcdef');
                c.update('update', '');  // Defaults to "utf8".
                c.final('utf-8');  // Should not throw.

                c = crypto.createCipher('aes192', '0123456789abcdef');
                c.update('update', 'utf8');
                c.final('utf-8');  // Should not throw.

                c = crypto.createCipher('aes192', '0123456789abcdef');
                c.update('update', 'utf-8');
                c.final('utf8');  // Should not throw.              
            });

            it("ucs2 encoding", () => {
                const key = '0123456789abcdef';
                const plaintext = 'Top secret!!!';
                const c = crypto.createCipher('aes192', key);
                let ciph = c.update(plaintext, 'utf16le', 'base64');
                ciph += c.final('base64');

                let decipher = crypto.createDecipher('aes192', key);

                let txt;
                txt = decipher.update(ciph, 'base64', 'ucs2');
                txt += decipher.final('ucs2');
                assert.equal(txt, plaintext);

                decipher = crypto.createDecipher('aes192', key);
                txt = decipher.update(ciph, 'base64', 'ucs-2');
                txt += decipher.final('ucs-2');
                assert.equal(txt, plaintext);

                decipher = crypto.createDecipher('aes192', key);
                txt = decipher.update(ciph, 'base64', 'utf-16le');
                txt += decipher.final('utf-16le');
                assert.equal(txt, plaintext);
            });

            it("result for setAutoPadding/setAuthTag/setAAD", () => {
                const key = '0123456789';
                const tagbuf = Buffer.from('auth_tag');
                const aadbuf = Buffer.from('aadbuf');
                const decipher = crypto.createDecipher('aes-256-gcm', key);
                assert.equal(decipher.setAutoPadding(), decipher);
                assert.equal(decipher.setAuthTag(tagbuf), decipher);
                assert.equal(decipher.setAAD(aadbuf), decipher);
            });

            it("Error throwing in setAAD/setAuthTag/getAuthTag/setAutoPadding", () => {
                const key = '0123456789';
                const aadbuf = Buffer.from('aadbuf');
                const data = Buffer.from('test-crypto-cipher-decipher');

                const cipher = crypto.createCipher('aes-256-gcm', key);
                cipher.setAAD(aadbuf);
                cipher.setAutoPadding();

                assert.throws(() => cipher.getAuthTag());

                const encrypted = Buffer.concat([cipher.update(data), cipher.final()]);

                const decipher = crypto.createDecipher('aes-256-gcm', key);
                decipher.setAAD(aadbuf);
                decipher.setAuthTag(cipher.getAuthTag());
                decipher.setAutoPadding();
                decipher.update(encrypted);
                decipher.final();

                assert.throws(() => decipher.setAAD(aadbuf));
                assert.throws(() => decipher.setAuthTag(cipher.getAuthTag()));
                assert.throws(() => decipher.setAutoPadding());
            });
        });

        describe("cipheriv/decipheriv", () => {
            it("key and iv", () => {
                function testCipher1(key, iv) {
                    const plaintext =
                        '32|RmVZZkFUVmpRRkp0TmJaUm56ZU9qcnJkaXNNWVNpTTU*|iXmckfRWZBGWWELw' +
                        'eCBsThSsfUHLeRe0KCsK8ooHgxie0zOINpXxfZi/oNG7uq9JWFVCk70gfzQH8ZUJ' +
                        'jAfaFg**';
                    const cipher = crypto.createCipheriv('des-ede3-cbc', key, iv);
                    let ciph = cipher.update(plaintext, 'utf8', 'hex');
                    ciph += cipher.final('hex');

                    const decipher = crypto.createDecipheriv('des-ede3-cbc', key, iv);
                    let txt = decipher.update(ciph, 'hex', 'utf8');
                    txt += decipher.final('utf8');

                    assert.equal(txt, plaintext);
                }

                function testCipher2(key, iv) {
                    // Test encryption and decryption with explicit key and iv
                    const plaintext =
                        '32|RmVZZkFUVmpRRkp0TmJaUm56ZU9qcnJkaXNNWVNpTTU*|iXmckfRWZBGWWELw' +
                        'eCBsThSsfUHLeRe0KCsK8ooHgxie0zOINpXxfZi/oNG7uq9JWFVCk70gfzQH8ZUJ' +
                        'jAfaFg**';
                    const cipher = crypto.createCipheriv('des-ede3-cbc', key, iv);
                    let ciph = cipher.update(plaintext, 'utf8', 'buffer');
                    ciph = Buffer.concat([ciph, cipher.final('buffer')]);

                    const decipher = crypto.createDecipheriv('des-ede3-cbc', key, iv);
                    let txt = decipher.update(ciph, 'buffer', 'utf8');
                    txt += decipher.final('utf8');

                    assert.equal(txt, plaintext);
                }

                testCipher1('0123456789abcd0123456789', '12345678');
                testCipher1('0123456789abcd0123456789', Buffer.from('12345678'));
                testCipher1(Buffer.from('0123456789abcd0123456789'), '12345678');
                testCipher1(Buffer.from('0123456789abcd0123456789'), Buffer.from('12345678'));
                testCipher2(Buffer.from('0123456789abcd0123456789'), Buffer.from('12345678'));
                testCipher2(crypto.createSecretKey(Buffer.from('0123456789abcd0123456789')), Buffer.from('12345678'));
            });

            it("check arguments", () => {
                assert.throws(() => crypto.createCipheriv(null));
                assert.throws(() => crypto.createCipheriv('des-ede3-cbc', null));
                assert.throws(() => crypto.createCipheriv('des-ede3-cbc', key, 10));

                assert.throws(() => crypto.createDecipheriv(null));
                assert.throws(() => crypto.createDecipheriv('des-ede3-cbc', null));
                assert.throws(() => crypto.createDecipheriv('des-ede3-cbc', key, 10));
            });

            it("iv size", () => {
                crypto.createCipheriv('aes-128-ecb', Buffer.alloc(16), Buffer.alloc(0));
                for (let n = 1; n < 256; n += 1)
                    assert.throws(() => crypto.createCipheriv('aes-128-ecb', Buffer.alloc(16),
                        Buffer.alloc(n)));
            });

            it("iv in cbc", () => {
                crypto.createCipheriv('aes-128-cbc', Buffer.alloc(16), Buffer.alloc(16));
                for (let n = 0; n < 256; n += 1) {
                    if (n === 16) continue;
                    assert.throws(() => crypto.createCipheriv('aes-128-cbc', Buffer.alloc(16),
                        Buffer.alloc(n)));
                }
            });

            it("iv in gcm mode", () => {
                assert.throws(() => crypto.createCipheriv('aes-128-gcm', Buffer.alloc(16),
                    Buffer.alloc(0)));

                const minIvLength = 8;
                const maxIvLength = 64;
                for (let n = minIvLength; n < maxIvLength; n += 1)
                    crypto.createCipheriv('aes-128-gcm', Buffer.alloc(16), Buffer.alloc(n));
            });

            it("invalid cipher name", () => {
                assert.throws(() => crypto.createCipheriv('aes-127', Buffer.alloc(16), null));
                assert.throws(() => crypto.createCipheriv('aes-128-ecb', Buffer.alloc(17), null));
            });
        });

        describe("authenticated", () => {
            const TEST_CASES = require('../test/crypto_case/aead-vectors.js');
            const ciphers = crypto.getCiphers();

            it("authenticated test", () => {
                for (const test of TEST_CASES) {
                    if (!ciphers.includes(test.algo)) {
                        common.printSkipMessage(`unsupported ${test.algo} test`);
                        continue;
                    }

                    const isCCM = /^aes-(128|192|256)-ccm$/.test(test.algo);
                    const isOCB = /^aes-(128|192|256)-ocb$/.test(test.algo);

                    let options;
                    if (isCCM || isOCB)
                        options = { authTagLength: test.tag.length / 2 };

                    const inputEncoding = test.plainIsHex ? 'hex' : 'ascii';

                    let aadOptions;
                    if (isCCM) {
                        aadOptions = {
                            plaintextLength: Buffer.from(test.plain, inputEncoding).length
                        };
                    }

                    {
                        const encrypt = crypto.createCipheriv(test.algo,
                            Buffer.from(test.key, 'hex'),
                            Buffer.from(test.iv, 'hex'),
                            options);

                        if (test.aad)
                            encrypt.setAAD(Buffer.from(test.aad, 'hex'), aadOptions);

                        let hex = encrypt.update(test.plain, inputEncoding, 'hex');
                        hex += encrypt.final('hex');

                        const auth_tag = encrypt.getAuthTag();
                        // Only test basic encryption run if output is marked as tampered.
                        if (!test.tampered) {
                            assert.equal(hex, test.ct);
                            assert.equal(auth_tag.toString('hex'), test.tag);
                        }
                    }

                    {
                        const decrypt = crypto.createDecipheriv(test.algo,
                            Buffer.from(test.key, 'hex'),
                            Buffer.from(test.iv, 'hex'),
                            options);
                        decrypt.setAuthTag(Buffer.from(test.tag, 'hex'));
                        if (test.aad)
                            decrypt.setAAD(Buffer.from(test.aad, 'hex'), aadOptions);

                        const outputEncoding = test.plainIsHex ? 'hex' : 'ascii';

                        let msg = decrypt.update(test.ct, 'hex', outputEncoding);
                        if (!test.tampered) {
                            msg += decrypt.final(outputEncoding);
                            assert.equal(msg, test.plain);
                        } else {
                            // Assert that final throws if input data could not be verified!
                            assert.throws(function () { decrypt.final('hex'); });
                        }
                    }

                    if (test.password) {
                        const encrypt = crypto.createCipher(test.algo, test.password, options);
                        if (test.aad)
                            encrypt.setAAD(Buffer.from(test.aad, 'hex'), aadOptions);
                        let hex = encrypt.update(test.plain, 'ascii', 'hex');
                        hex += encrypt.final('hex');
                        const auth_tag = encrypt.getAuthTag();
                        // Only test basic encryption run if output is marked as tampered.
                        if (!test.tampered) {
                            assert.equal(hex, test.ct);
                            assert.equal(auth_tag.toString('hex'), test.tag);
                        }
                    }

                    if (test.password) {
                        const decrypt = crypto.createDecipher(test.algo, test.password, options);
                        decrypt.setAuthTag(Buffer.from(test.tag, 'hex'));
                        if (test.aad)
                            decrypt.setAAD(Buffer.from(test.aad, 'hex'), aadOptions);
                        let msg = decrypt.update(test.ct, 'hex', 'ascii');
                        if (!test.tampered) {
                            msg += decrypt.final('ascii');
                            assert.equal(msg, test.plain);
                        } else {
                            // Assert that final throws if input data could not be verified!
                            assert.throws(function () { decrypt.final('ascii'); });
                        }
                    }

                    {
                        // Trying to get tag before inputting all data:
                        const encrypt = crypto.createCipheriv(test.algo,
                            Buffer.from(test.key, 'hex'),
                            Buffer.from(test.iv, 'hex'),
                            options);
                        encrypt.update('blah', 'ascii');
                        assert.throws(function () { encrypt.getAuthTag(); });
                    }

                    {
                        // Trying to create cipher with incorrect IV length
                        assert.throws(function () {
                            crypto.createCipheriv(
                                test.algo,
                                Buffer.from(test.key, 'hex'),
                                Buffer.alloc(0)
                            );
                        });
                    }
                }
            });

            it("non-authenticating", () => {
                const encrypt = crypto.createCipheriv('aes-128-cbc',
                    'ipxp9a6i1Mb4USb4', '6fKjEjR3Vl30EUYC');
                encrypt.update('blah', 'ascii');
                encrypt.final();
                assert.throws(() => encrypt.getAuthTag());
                assert.throws(() => encrypt.setAAD(Buffer.from('123', 'ascii')));
            });

            describe("GCM mode", () => {
                it("specific authentication tag lengths", () => {
                    for (const length of [0, 1, 2, 6, 9, 10, 11, 17]) {
                        assert.throws(() => {
                            const decrypt = crypto.createDecipheriv('aes-128-gcm',
                                'FxLKsqdmv0E9xrQh',
                                'qkuZpJWCewa6Szih');
                            decrypt.setAuthTag(Buffer.from('1'.repeat(length)));
                        });

                        assert.throws(() => {
                            crypto.createCipheriv('aes-256-gcm',
                                'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                                'qkuZpJWCewa6Szih',
                                {
                                    authTagLength: length
                                });
                        });

                        assert.throws(() => {
                            crypto.createDecipheriv('aes-256-gcm',
                                'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                                'qkuZpJWCewa6Szih',
                                {
                                    authTagLength: length
                                });
                        });
                    }
                });

                it("shorter authentication tags", () => {
                    const fullTag = '1debb47b2c91ba2cea16fad021703070';
                    for (const [authTagLength, e] of [[undefined, 16], [12, 12], [4, 4]]) {
                        const cipher = crypto.createCipheriv('aes-256-gcm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8', 'qkuZpJWCewa6Szih', {
                            authTagLength
                        });
                        cipher.setAAD(Buffer.from('abcd'));
                        cipher.update('01234567', 'hex');
                        cipher.final();
                        const tag = cipher.getAuthTag();
                        assert.equal(tag.toString('hex'), fullTag.substr(0, 2 * e));
                    }
                });

                it("manually restrict tag length", () => {
                    const decipher = crypto.createDecipheriv('aes-256-gcm',
                        'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8', 'qkuZpJWCewa6Szih', {
                        authTagLength: 8
                    });

                    assert.throws(() => {
                        // This tag would normally be allowed.
                        decipher.setAuthTag(Buffer.from('1'.repeat(12)));
                    });

                    // The Decipher object should be left intact.
                    decipher.setAuthTag(Buffer.from('445352d3ff85cf94', 'hex'));
                    const text = Buffer.concat([
                        decipher.update('3a2a3647', 'hex'),
                        decipher.final(),
                    ]);
                    assert.equal(text.toString('utf8'), 'node');
                });
            });

            it("invalid authentication tag length in CCM mode", () => {
                for (const authTagLength of [-1, true, false, NaN, 5.5]) {
                    assert.throws(() => {
                        crypto.createCipheriv('aes-256-ccm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S',
                            {
                                authTagLength
                            });
                    });

                    assert.throws(() => {
                        crypto.createDecipheriv('aes-256-ccm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S',
                            {
                                authTagLength
                            });
                    });

                    assert.throws(() => {
                        crypto.createCipher('aes-256-ccm', 'bad password', { authTagLength });
                    });

                    assert.throws(() => {
                        crypto.createDecipher('aes-256-ccm', 'bad password', { authTagLength });
                    });
                }

                for (const authTagLength of [0, 1, 2, 3, 5, 7, 9, 11, 13, 15, 17, 18]) {
                    assert.throws(() => {
                        crypto.createCipheriv('aes-256-ccm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S',
                            {
                                authTagLength
                            });
                    });

                    assert.throws(() => {
                        crypto.createDecipheriv('aes-256-ccm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S',
                            {
                                authTagLength
                            });
                    });

                    assert.throws(() => {
                        crypto.createCipher('aes-256-ccm', 'bad password', { authTagLength });
                    });

                    assert.throws(() => {
                        crypto.createDecipher('aes-256-ccm', 'bad password', { authTagLength });
                    });
                }
            });

            it("no authentication tag in CCM and OCB mode", () => {
                for (const mode of ['ccm', 'ocb']) {
                    assert.throws(() => {
                        crypto.createCipheriv(`aes-256-${mode}`,
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S');
                    }, {
                        message: `authTagLength required for aes-256-${mode}`
                    });

                    // CCM decryption and create(De|C)ipher are unsupported in FIPS mode.
                    assert.throws(() => {
                        crypto.createDecipheriv(`aes-256-${mode}`,
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S');
                    }, {
                        message: `authTagLength required for aes-256-${mode}`
                    });

                    assert.throws(() => {
                        crypto.createCipher(`aes-256-${mode}`, 'very bad password');
                    }, {
                        message: `authTagLength required for aes-256-${mode}`
                    });

                    assert.throws(() => {
                        crypto.createDecipher(`aes-256-${mode}`, 'very bad password');
                    }, {
                        message: `authTagLength required for aes-256-${mode}`
                    });
                }
            });

            describe("setAAD", () => {
                it("invalid plaintext length", () => {
                    const cipher = crypto.createCipheriv('aes-256-ccm',
                        'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                        'qkuZpJWCewa6S',
                        {
                            authTagLength: 10
                        });

                    for (const plaintextLength of [-1, true, false, NaN]) {
                        assert.throws(() => {
                            cipher.setAAD(Buffer.from('0123456789', 'hex'), { plaintextLength });
                        });
                    }
                });

                it("plaintext is too long", () => {
                    for (const ivLength of [13, 12]) {
                        const maxMessageSize = (1 << (8 * (15 - ivLength))) - 1;
                        const key = 'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8';
                        const cipher = () => crypto.createCipheriv('aes-256-ccm', key,
                            '0'.repeat(ivLength),
                            {
                                authTagLength: 10
                            });

                        assert.throws(() => {
                            cipher().setAAD(Buffer.alloc(0), {
                                plaintextLength: maxMessageSize + 1
                            });
                        });

                        const msg = Buffer.alloc(maxMessageSize + 1);
                        assert.throws(() => {
                            cipher().update(msg);
                        }, /Invalid message length/);

                        const c = cipher();
                        c.setAAD(Buffer.alloc(0), {
                            plaintextLength: maxMessageSize
                        });
                        c.update(msg.slice(1));
                    }
                });

                it("plaintext length has not been specified in CCM mode", () => {
                    assert.throws(() => {
                        const cipher = crypto.createCipheriv('aes-256-ccm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S',
                            {
                                authTagLength: 10
                            });
                        cipher.setAAD(Buffer.from('0123456789', 'hex'));
                    });

                    assert.throws(() => {
                        const cipher = crypto.createDecipheriv('aes-256-ccm',
                            'FxLKsqdmv0E9xrQhp0b1ZgI0K7JFZJM8',
                            'qkuZpJWCewa6S',
                            {
                                authTagLength: 10
                            });
                        cipher.setAAD(Buffer.from('0123456789', 'hex'));
                    });
                });

                it("final() throws in CCM mode when no authentication tag is provided", () => {
                    const key = Buffer.from('1ed2233fa2223ef5d7df08546049406c', 'hex');
                    const iv = Buffer.from('7305220bca40d4c90e1791e9', 'hex');
                    const ct = Buffer.from('8beba09d4d4d861f957d51c0794f4abf8030848e', 'hex');
                    const decrypt = crypto.createDecipheriv('aes-128-ccm', key, iv, {
                        authTagLength: 10
                    });

                    assert.throws(() => {
                        decrypt.setAAD(Buffer.from('63616c76696e', 'hex'), {
                            plaintextLength: ct.length
                        });
                        decrypt.update(ct);
                        decrypt.final();
                    });
                });

                it("setAuthTag does not throw in GCM mode when called after setAAD", () => {
                    const key = Buffer.from('1ed2233fa2223ef5d7df08546049406c', 'hex');
                    const iv = Buffer.from('579d9dfde9cd93d743da1ceaeebb86e4', 'hex');
                    const decrypt = crypto.createDecipheriv('aes-128-gcm', key, iv);
                    decrypt.setAAD(Buffer.from('0123456789', 'hex'));
                    decrypt.setAuthTag(Buffer.from('1bb9253e250b8069cde97151d7ef32d9', 'hex'));
                    assert.equal(decrypt.update('807022', 'hex', 'hex'), 'abcdef');
                    assert.equal(decrypt.final('hex'), '');
                });
            });

            it("IV length of 11 does not overflow max_message_size_", () => {
                const key = 'x'.repeat(16);
                const iv = Buffer.from('112233445566778899aabb', 'hex');
                const options = { authTagLength: 8 };
                const encrypt = crypto.createCipheriv('aes-128-ccm', key, iv, options);
                encrypt.update('boom');  // Should not throw 'Message exceeds maximum size'.
                encrypt.final();
            });

            it("authentication tag can be set at any point before final()", () => {
                const plain = Buffer.from('Hello world', 'utf8');
                const key = Buffer.from('0123456789abcdef', 'utf8');
                const iv = Buffer.from('0123456789ab', 'utf8');

                for (const mode of ['gcm', 'ocb']) {
                    for (const authTagLength of mode === 'gcm' ? [undefined, 8] : [8]) {
                        const cipher = crypto.createCipheriv(`aes-128-${mode}`, key, iv, {
                            authTagLength
                        });
                        const ciphertext = Buffer.concat([cipher.update(plain), cipher.final()]);
                        const authTag = cipher.getAuthTag();

                        for (const authTagBeforeUpdate of [true, false]) {
                            const decipher = crypto.createDecipheriv(`aes-128-${mode}`, key, iv, {
                                authTagLength
                            });
                            if (authTagBeforeUpdate) {
                                decipher.setAuthTag(authTag);
                            }
                            const resultUpdate = decipher.update(ciphertext);
                            if (!authTagBeforeUpdate) {
                                decipher.setAuthTag(authTag);
                            }
                            const resultFinal = decipher.final();
                            const result = Buffer.concat([resultUpdate, resultFinal]);
                            assert.deepEqual(result, plain);
                        }
                    }
                }
            });

            it("setAuthTag can only be called once", () => {
                const plain = Buffer.from('Hello world', 'utf8');
                const key = Buffer.from('0123456789abcdef', 'utf8');
                const iv = Buffer.from('0123456789ab', 'utf8');
                const opts = { authTagLength: 8 };

                for (const mode of ['gcm', 'ccm', 'ocb']) {
                    const cipher = crypto.createCipheriv(`aes-128-${mode}`, key, iv, opts);
                    const ciphertext = Buffer.concat([cipher.update(plain), cipher.final()]);
                    const tag = cipher.getAuthTag();

                    const decipher = crypto.createDecipheriv(`aes-128-${mode}`, key, iv, opts);
                    decipher.setAuthTag(tag);
                    assert.throws(() => {
                        decipher.setAuthTag(tag);
                    });
                    // Decryption should still work.
                    const plaintext = Buffer.concat([
                        decipher.update(ciphertext),
                        decipher.final(),
                    ]);
                    assert.deepEqual(plain, plaintext);
                }
            });

            describe("chacha20-poly1305", () => {
                it("rejects invalid IV lengths", () => {
                    const valid = {
                        algo: 'chacha20-poly1305',
                        key: '808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f',
                        iv: '070000004041424344454647',
                        plain: '4c616469657320616e642047656e746c656d656e206f662074686520636c6173' +
                            '73206f66202739393a204966204920636f756c64206f6666657220796f75206f' +
                            '6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73' +
                            '637265656e20776f756c642062652069742e',
                        plainIsHex: true,
                        aad: '50515253c0c1c2c3c4c5c6c7',
                        ct: 'd31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5' +
                            'a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e06' +
                            '0b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fa' +
                            'b324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d265' +
                            '86cec64b6116',
                        tag: '1ae10b594f09e26a7e902ecbd0600691',
                        tampered: false,
                    };

                    // Invalid IV lengths should be detected:
                    // - 12 and below are valid.
                    // - 13-16 are not detected as invalid by some OpenSSL versions.
                    check(13);
                    check(14);
                    check(15);
                    check(16);
                    // - 17 and above were always detected as invalid by OpenSSL.
                    check(17);

                    function check(ivLength) {
                        const prefix = ivLength - valid.iv.length / 2;
                        assert.throws(() => crypto.createCipheriv(
                            valid.algo,
                            Buffer.from(valid.key, 'hex'),
                            Buffer.from(H(prefix) + valid.iv, 'hex')
                        ));

                        function H(length) { return '00'.repeat(length); }
                    }

                    const key = Buffer.alloc(32);
                    const iv = Buffer.alloc(12);

                    for (const authTagLength of [0, 17]) {
                        assert.throws(() => {
                            crypto.createCipheriv('chacha20-poly1305', key, iv, { authTagLength });
                        });
                    }
                });

                it("respect the authTagLength option and should not require the authentication tag before calls to update() during decryption", () => {
                    const key = Buffer.alloc(32);
                    const iv = Buffer.alloc(12);

                    for (let authTagLength = 1; authTagLength <= 16; authTagLength++) {
                        const cipher =
                            crypto.createCipheriv('chacha20-poly1305', key, iv, { authTagLength });
                        const ciphertext = Buffer.concat([cipher.update('foo'), cipher.final()]);
                        const authTag = cipher.getAuthTag();
                        assert.equal(authTag.length, authTagLength);

                        // The decipher operation should reject all authentication tags other than
                        // that of the expected length.
                        for (let other = 1; other <= 16; other++) {
                            const decipher = crypto.createDecipheriv('chacha20-poly1305', key, iv, {
                                authTagLength: other
                            });
                            // ChaCha20 is a stream cipher so we do not need to call final() to obtain
                            // the full plaintext.
                            const plaintext = decipher.update(ciphertext);
                            assert.equal(plaintext.toString(), 'foo');
                            if (other === authTagLength) {
                                // The authentication tag length is as expected and the tag itself is
                                // correct, so this should work.
                                decipher.setAuthTag(authTag);
                                decipher.final();
                            } else {
                                // The authentication tag that we are going to pass to setAuthTag is
                                // either too short or too long. If other < authTagLength, the
                                // authentication tag is still correct, but it should still be rejected
                                // because its security assurance is lower than expected.
                                assert.throws(() => {
                                    decipher.setAuthTag(authTag);
                                });
                            }
                        }
                    }
                });

                it("default to an authTagLength of 16", () => {
                    const rfcTestCases = TEST_CASES.filter(({ algo, tampered }) => {
                        return algo === 'chacha20-poly1305' && tampered === false;
                    });
                    assert.equal(rfcTestCases.length, 1);

                    const [testCase] = rfcTestCases;
                    const key = Buffer.from(testCase.key, 'hex');
                    const iv = Buffer.from(testCase.iv, 'hex');
                    const aad = Buffer.from(testCase.aad, 'hex');

                    for (const opt of [
                        undefined,
                        { authTagLength: undefined },
                        { authTagLength: 16 },
                    ]) {
                        const cipher = crypto.createCipheriv('chacha20-poly1305', key, iv, opt);
                        const ciphertext = Buffer.concat([
                            cipher.setAAD(aad).update(testCase.plain, 'hex'),
                            cipher.final(),
                        ]);
                        const authTag = cipher.getAuthTag();

                        assert.equal(ciphertext.toString('hex'), testCase.ct);
                        assert.equal(authTag.toString('hex'), testCase.tag);

                        const decipher = crypto.createDecipheriv('chacha20-poly1305', key, iv, opt);
                        const plaintext = Buffer.concat([
                            decipher.setAAD(aad).update(ciphertext),
                            decipher.setAuthTag(authTag).final(),
                        ]);

                        assert.equal(plaintext.toString('hex'), testCase.plain);
                    }
                });

                it("without setting authTag on decypher", () => {
                    const rfcTestCases = TEST_CASES.filter(({ algo, tampered }) => {
                        return algo === 'chacha20-poly1305' && tampered === false;
                    });
                    assert.equal(rfcTestCases.length, 1);

                    const [testCase] = rfcTestCases;
                    const key = Buffer.from(testCase.key, 'hex');
                    const iv = Buffer.from(testCase.iv, 'hex');
                    const aad = Buffer.from(testCase.aad, 'hex');
                    const opt = { authTagLength: 16 };

                    const cipher = crypto.createCipheriv('chacha20-poly1305', key, iv, opt);
                    const ciphertext = Buffer.concat([
                        cipher.setAAD(aad).update(testCase.plain, 'hex'),
                        cipher.final(),
                    ]);
                    const authTag = cipher.getAuthTag();

                    assert.equal(ciphertext.toString('hex'), testCase.ct);
                    assert.equal(authTag.toString('hex'), testCase.tag);

                    const decipher = crypto.createDecipheriv('chacha20-poly1305', key, iv, opt);
                    decipher.setAAD(aad).update(ciphertext);

                    assert.throws(() => {
                        decipher.final();
                    });
                });
            });

            it("CCM cipher without data should not crash", () => {
                const algo = 'aes-128-ccm';
                const key = Buffer.alloc(16);
                const iv = Buffer.alloc(12);
                const opts = { authTagLength: 10 };

                for (const cipher of [
                    crypto.createCipher(algo, 'foo', opts),
                    crypto.createCipheriv(algo, key, iv, opts),
                ]) {
                    assert.throws(() => {
                        cipher.final();
                    });
                }
            });
        });

        it("padding", () => {
            // Input data.
            const ODD_LENGTH_PLAIN = 'Hello node world!';
            const EVEN_LENGTH_PLAIN = 'Hello node world!AbC09876dDeFgHi';

            const KEY_PLAIN = 'S3c.r.e.t.K.e.Y!';
            const IV_PLAIN = 'blahFizz2011Buzz';

            const CIPHER_NAME = 'aes-128-cbc';

            const ODD_LENGTH_ENCRYPTED = '7f57859550d4d2fdb9806da2a750461a9fe77253cd1cbd4b07beee4e070d561f';

            const EVEN_LENGTH_ENCRYPTED = '7f57859550d4d2fdb9806da2a750461ab46e71b3d78ebe2d9684dfc87f7575b988' +
                '6119866912cb8c7bcaf76c5ebc2378';

            const EVEN_LENGTH_ENCRYPTED_NOPAD = '7f57859550d4d2fdb9806da2a750461ab46e71b3d78ebe2d9684dfc87f7575b9';

            function enc(plain, pad) {
                const encrypt = crypto.createCipheriv(CIPHER_NAME, KEY_PLAIN, IV_PLAIN);
                encrypt.setAutoPadding(pad);
                let hex = encrypt.update(plain, 'ascii', 'hex');
                hex += encrypt.final('hex');
                return hex;
            }

            function dec(encd, pad) {
                const decrypt = crypto.createDecipheriv(CIPHER_NAME, KEY_PLAIN, IV_PLAIN);
                decrypt.setAutoPadding(pad);
                let plain = decrypt.update(encd, 'hex');
                plain += decrypt.final('latin1');
                return plain;
            }

            assert.equal(enc(ODD_LENGTH_PLAIN, true), ODD_LENGTH_ENCRYPTED);
            assert.equal(enc(EVEN_LENGTH_PLAIN, true), EVEN_LENGTH_ENCRYPTED);

            assert.throws(function () {
                enc(ODD_LENGTH_PLAIN, false);
            });

            assert.equal(enc(EVEN_LENGTH_PLAIN, false), EVEN_LENGTH_ENCRYPTED_NOPAD);

            assert.equal(dec(ODD_LENGTH_ENCRYPTED, true), ODD_LENGTH_PLAIN);
            assert.equal(dec(EVEN_LENGTH_ENCRYPTED, true), EVEN_LENGTH_PLAIN);

            assert.equal(dec(ODD_LENGTH_ENCRYPTED, false).length, 32);
            assert.equal(dec(EVEN_LENGTH_ENCRYPTED, false).length, 48);

            assert.throws(function () {
                assert.equal(dec(EVEN_LENGTH_ENCRYPTED_NOPAD, true), EVEN_LENGTH_PLAIN);
            });

            assert.equal(dec(EVEN_LENGTH_ENCRYPTED_NOPAD, false), EVEN_LENGTH_PLAIN);
        });

        it("padding in aes256", () => {
            const iv = Buffer.from('00000000000000000000000000000000', 'hex');
            const key = Buffer.from('0123456789abcdef0123456789abcdef' +
                '0123456789abcdef0123456789abcdef', 'hex');

            function encrypt(val, pad) {
                const c = crypto.createCipheriv('aes256', key, iv);
                c.setAutoPadding(pad);
                return c.update(val, 'utf8', 'latin1') + c.final('latin1');
            }

            function decrypt(val, pad) {
                const c = crypto.createDecipheriv('aes256', key, iv);
                c.setAutoPadding(pad);
                return c.update(val, 'latin1', 'utf8') + c.final('utf8');
            }

            let plaintext = '0123456789abcdef0123456789abcdef'; // Multiple of block size
            let encrypted = encrypt(plaintext, false);
            let decrypted = decrypt(encrypted, false);
            assert.equal(decrypted, plaintext);

            plaintext = '0123456789abcdef0123456789abcde'; // not a multiple
            encrypted = encrypt(plaintext, true);
            decrypted = decrypt(encrypted, true);
            assert.equal(decrypted, plaintext);
        });

        it("aes wrap", () => {
            const datas = [
                {
                    algorithm: 'aes128-wrap',
                    key: 'b26f309fbe57e9b3bb6ae5ef31d54450',
                    iv: '3fd838af4093d749',
                    text: '12345678123456781234567812345678',
                    enc: '612abece03d514c6a81b7a21f8895061877c43c4064bb89120bf80185809642d341ea88ff92bdf03'
                },
                {
                    algorithm: 'id-aes128-wrap-pad',
                    key: 'b26f309fbe57e9b3bb6ae5ef31d54450',
                    iv: '3fd838af',
                    text: '12345678123456781234567812345678123',
                    enc: 'dabf2d9db41ada96198435b0a3fe5b168959c19fff959a487953d60be8eb45c09f87e1ef609111b9a61db8e2be326c9b'
                },
                {
                    algorithm: 'aes192-wrap',
                    key: '40978085d68091f7dfca0d7dfc7a5ee76d2cc7f2f345a304',
                    iv: '3fd838af4093d749',
                    text: '12345678123456781234567812345678',
                    enc: 'e24c00fb63fe165ba5e180c786c3349f938081a1acbb48926f7f8219bd912fc41bf1d844b14e1472'
                },
                {
                    algorithm: 'id-aes192-wrap-pad',
                    key: '40978085d68091f7dfca0d7dfc7a5ee76d2cc7f2f345a304',
                    iv: '3fd838af',
                    text: '12345678123456781234567812345678123',
                    enc: 'ae32ae4c8914774fd6cc304ce26ee050aea98fb8189a4c808ce984d3debb1e79611792be5108a3355167eecb0900a72e'
                },
                {
                    algorithm: 'aes256-wrap',
                    key: '29c9eab5ed5ad44134a1437fe2e673b4d88a5b7c72e68454fea08721392b7323',
                    iv: '3fd838af4093d749',
                    text: '12345678123456781234567812345678',
                    enc: '83e4569782f43a2af901c4c371ffbc0dcf6ea5ce3e189b2420e7535318fc7243f84220074a7c61a1'
                },
                {
                    algorithm: 'id-aes256-wrap-pad',
                    key: '29c9eab5ed5ad44134a1437fe2e673b4d88a5b7c72e68454fea08721392b7323',
                    iv: '3fd838af',
                    text: '12345678123456781234567812345678123',
                    enc: '24ae1865a97f596c45a2594c565f2e55ffc874890b36526040ae520fffe33115ddbdcfe4ef4716bc40d64643ea5bbf8c'
                },
            ];

            datas.forEach((data) => {
                const cipher = crypto.createCipheriv(
                    data.algorithm,
                    Buffer.from(data.key, 'hex'),
                    Buffer.from(data.iv, 'hex'));
                const ciphertext = cipher.update(data.text);
                assert.equal(ciphertext.toString('hex'), data.enc);

                const decipher = crypto.createDecipheriv(
                    data.algorithm,
                    Buffer.from(data.key, 'hex'),
                    Buffer.from(data.iv, 'hex'));
                const msg = decipher.update(ciphertext, 'buffer', 'utf8');

                assert.equal(msg, data.text);
            });
        });

        it("des3 wrap", () => {
            const data = {
                key: Buffer.from('3c08e25be22352910671cfe4ba3652b1220a8a7769b490ba', 'hex'),
                iv: Buffer.alloc(0),
                plaintext: '32|RmVZZkFUVmpRRkp0TmJaUm56ZU9qcnJkaXNNWVNpTTU*|iXmckfRWZBG' +
                    'WWELweCBsThSsfUHLeRe0KCsK8ooHgxie0zOINpXxfZi/oNG7uq9JWFVCk70gfzQH8ZU' +
                    'JjAfaFg**'
            };

            const cipher = crypto.createCipheriv('des3-wrap', data.key, data.iv);
            const ciphertext = cipher.update(data.plaintext, 'utf8');

            const decipher = crypto.createDecipheriv('des3-wrap', data.key, data.iv);
            const msg = decipher.update(ciphertext, 'buffer', 'utf8');

            assert.equal(msg, data.plaintext);
        });

        it("wrong encoding", () => {
            const createCipher = () => {
                return crypto.createCipheriv('aes-256-cbc', crypto.randomBytes(32), crypto.randomBytes(16));
            };

            {
                const cipher = createCipher();
                cipher.update('test', 'utf-8', 'utf-8');
                assert.throws(() => cipher.update('666f6f', 'hex', 'hex'));
            }

            {
                const cipher = createCipher();
                cipher.update('test', 'utf-8', 'utf-8');
                assert.throws(() => cipher.final('hex'));
            }

            {
                const cipher = createCipher();
                cipher.update('test', 'utf-8', 'utf-8');
                assert.throws(() => cipher.final('bad2'));
            }

            {
                const cipher = createCipher();
                assert.throws(() => cipher.update('test', 'utf-8', 'bad3'));
            }
        });
    });

    it("diffie-hellman", () => {
        const dh_datas = [
            {
                "k1": {
                    "kty": "EC",
                    "crv": "secp256k1",
                    "x": "zjs2JIs0H5B3YpmXeQ8H7gBwjTWAhMBZ2f7yNPLunws",
                    "y": "2eaKLGlhQ4_FaTdTzADBkh87-JTgbP5jRyawEHSOTRM",
                    "d": "Befeh4WP_5-z_fujSCNK4B_cgM7cxe0saXc1Xk2H7_E"
                },
                "k2": {
                    "kty": "EC",
                    "crv": "secp256k1",
                    "x": "rnh1Pdyngihl7H9lLo0LXrKaFt80xMP-47jH3v6MQSw",
                    "y": "NIYva2P56-m_BMyIR5syIxwNYrWej-B56r6UUNwYzaQ",
                    "d": "wHgJZW2jg20Y7_p3XD0Wt6r8dNSUIS4ur7f9A0G-tWI"
                },
                "secret": "35b422687250a0d3f88de9b00fe3f129484af3fceadb44a8ee405c067840d1e2"
            },
            {
                "k1": {
                    "kty": "EC",
                    "crv": "P-256",
                    "x": "K-6GqkKg66r2RA7Q9fs3DUY-s3fgawg7A_vg4EqkN4g",
                    "y": "WNK3kl4Y1pQSqUbCU4u2RkO61B31lAAnXn6jvf4oi9A",
                    "d": "Bd7agtBTsSBIFG1ADu2tTFV40SPjl2p2pBZoZ0lnp-I"
                },
                "k2": {
                    "kty": "EC",
                    "crv": "P-256",
                    "x": "3p-DI6u8YwjBd1qWYcI7FS0xTmrrfo2toQLPvOY9AUY",
                    "y": "tjupBbC_Lo530taw0-5xJdr1cNE3NPpfLRQSUbBCDeI",
                    "d": "gw1AThs5T00yPbjD0ZlCUt5bo2nJc4rSuLnURyQE8_U"
                },
                "secret": "9a2f77996770755afeb373e67445550a632edb2001cbb0265c206ecd2a7de557"
            },
            {
                "k1": {
                    "kty": "EC",
                    "crv": "P-384",
                    "x": "nIjPfmf4vyEDi4g7MszBTHpl6la5Ng19bpLUAOizlTvFPovCmRQOlQL7154z_0PO",
                    "y": "JRT1IUEeKeNoxYv0LIux9PLsNGDPqxF6ybhALi_ajNREQhwOgH06uHnxfyFiZ9Gp",
                    "d": "gugR5r6MmJkUJZwJyRp0UoUutVRjFc8yYkJ52h0J0T0ZUVrGyw2WN5NSunlKoJF0"
                },
                "k2": {
                    "kty": "EC",
                    "crv": "P-384",
                    "x": "KTQGykVJIW5gkBio-FbEGIGq1Wlmd9SqPjRGSegk1YizS1-7eDNEs4aIrViun_13",
                    "y": "WnlAagWZRcNsAes3Kzb3L60cIS-XAY9RR9YyDwiw2233Gw748dj0Nmypdb8lbuwf",
                    "d": "Anu79INCjmJjwq9DEUxs62a-6wOYV4ajiAKW52lGYJKZvP5EevgfNMSLJIA14G9h"
                },
                "secret": "47a2b06932a91234a69f3c7ffe93d7b09d8c20122f6a0503b0a6aabf5635b0e8db60a63ab68eb73f52adac3507158b42"
            },
            {
                "k1": {
                    "kty": "EC",
                    "crv": "P-521",
                    "x": "AeltsZkWzF18bjSeMmzfbC7ixAnkO_b7YHe0hv1FL7qEqV2gwk7zn2LT4ibbkYuznEBcmw9Z-xwQMAmYlBX4Z-uB",
                    "y": "AOG_to96IsoeubLtO_W-24GEaq-EuSiq2Eg2bSDO5P9M2ZoccUQKpi61YlTCzkXgw6wamjoU6A5TVkZtcEZ6XMuF",
                    "d": "AGob8ojvPw-4h9onkp72xbjeWXo5iySH-v4Mj6B2qa0SSORbabj-6F_YBniTLcY6i-GUurFhG8mSRb12GH8d5x-S"
                },
                "k2": {
                    "kty": "EC",
                    "crv": "P-521",
                    "x": "AOuFvJm0Z-YKJ_ZdVmEGM6bHIs079JSL5KMj-QgbdxH5uDAugPgo8GgZ4JRuVHwhW9eaB5_H90Bqy92G27tvFKnR",
                    "y": "AHrHWFOY2c9lErQE2rIHjJ9F8LZ8uIT_iFZIcXTOoE7iQ0fnw1J6eea_C553SgmGqQH19JiGt_Lv0uhNtrPsl_i5",
                    "d": "AWUUBbXdRJbLvpvZ_ImewAkqWYPrrBTPb38UZ7F1hjWGSorP_keFdn2q8VOYjlgtuT-1zx331wu_7Cf1SrfVOM0X"
                },
                "secret": "009142f3d12b0d9906e4c4ab13e1286d28e0f1b2f50a5bac74bbf371ad85fa790a5a3abdb121e6ebc1f84e7d761a48c0b39aa15204ddf80e47626d4915c704f19691"
            },
            {
                "k1": {
                    "kty": "EC",
                    "crv": "SM2",
                    "x": "gTYUCa7XW13FXcmkngTuvAQia9mriVnWoNzh5FmYxO0",
                    "y": "6wjKH9g5Uw2OEhOPSlCAjs3osul8J75v6s9r8Z-uz24",
                    "d": "CPj44rkASC_FiY__rgAfJLZDG60cl_dEP-fD70ahnxI"
                },
                "k2": {
                    "kty": "EC",
                    "crv": "SM2",
                    "x": "6ZIdJNvyQGIY_cEhaozczcASCg36IsZb9cCNI1wcprU",
                    "y": "mF9ncL0bB9EwNCo4xLY7J_O3ypfucMZl4R-HVZlBmAo",
                    "d": "ROIu2ngTt0xTpg5-6UQblqH1xgXJUPNkASIPSlq8Bos"
                },
                "secret": "a6584084f00dced610494f9b115d7d79aba3bdee18efff1d9e53e19c14029cb7"
            },
            {
                "k1": {
                    "kty": "OKP",
                    "crv": "X25519",
                    "x": "VaU3ox_yStPvIJrEWcyqW-_0A5GvDldFuqyDw0ZMNmA",
                    "d": "WC9svS01NT0c43L379u8hjKeJ0NpOlq42RE3BnJLKG8"
                },
                "k2": {
                    "kty": "OKP",
                    "crv": "X25519",
                    "x": "YxXJrr20buB3m71o30vKGVIOsQZSa0K3FbpnMgotBHk",
                    "d": "iLV3YXmDZSq_Py6brY0SrZKBh-5IvS146YpODGXQiGE"
                },
                "secret": "939a92329dd167724368a33a93747f0bf1647345847adf8dbf230ad320d1446a"
            },
            {
                "k1": {
                    "kty": "OKP",
                    "crv": "X448",
                    "x": "EhPFjMVfNe3LACYIJUUaKGln5GagUq2VWvzQj39hMvIKgECzAN8GrmZ4y8-5vluPxjLoKF3126o",
                    "d": "wKpwoZ56rx2-WwKcONj05H7e1h-16Fc-55DIqTpESpLwTBSOyAQqiWnhnL5BmG6qH4ORnylG7pY"
                },
                "k2": {
                    "kty": "OKP",
                    "crv": "X448",
                    "x": "8EPT5JHeEFo13msoX_-Nw0NipAz00dKsDqt2GmlXTfYkbZ3Ew_ML6MAA0nR1dGJ2KedRIIT-fGc",
                    "d": "bMz55cfk1gKw5eub7ukXB7Kkgmoeg0-nF6zMkm2HgfPNRmfofbV_M85r4zn4gG-HF8Hv-DBXLI8"
                },
                "secret": "b96a1f324cb226919afb51420d06cc24bf236b656d5dcc8974c04400ef6037c976c5bb7c3b42aa221eb45135426b843a5b935c00cb6694a6"
            }
        ]

        dh_datas.forEach(data => {
            var k1 = crypto.createPrivateKey({
                key: data.k1,
                format: 'jwk'
            });

            var k2 = crypto.createPrivateKey({
                key: data.k2,
                format: 'jwk'
            });

            var s1 = crypto.diffieHellman({
                privateKey: k1,
                publicKey: crypto.createPublicKey(k2)
            });

            var s2 = crypto.diffieHellman({
                privateKey: k2,
                publicKey: crypto.createPublicKey(k1)
            });

            assert.equal(s1.toString("hex"), data.secret);
            assert.equal(s2.toString("hex"), data.secret);
        });
    });

    describe('X509Certificate', () => {
        const cert = readKey('agent1-cert.pem');
        const key = readKey('agent1-key.pem');
        const ca = readKey('ca1-cert.pem');

        const privateKey = crypto.createPrivateKey(key);

        const subjectCheck = `C=US\nST=CA\nL=SF\nO=Joyent\nOU=Node.js\nCN=agent1\nemailAddress=ry@tinyclouds.org`;
        const issuerCheck = `C=US\nST=CA\nL=SF\nO=Joyent\nOU=Node.js\nCN=ca1\nemailAddress=ry@tinyclouds.org`;
        let infoAccessCheck = `OCSP - URI:http://ocsp.nodejs.org/\nCA Issuers - URI:http://ca.nodejs.org/ca.cert`;

        const der = Buffer.from(
            '308203e8308202d0a0030201020214147d36c1c2f74206de9fab5f2226d78adb00a42630' +
            '0d06092a864886f70d01010b0500307a310b3009060355040613025553310b3009060355' +
            '04080c024341310b300906035504070c025346310f300d060355040a0c064a6f79656e74' +
            '3110300e060355040b0c074e6f64652e6a73310c300a06035504030c036361313120301e' +
            '06092a864886f70d010901161172794074696e79636c6f7564732e6f72673020170d3232' +
            '303930333231343033375a180f32323936303631373231343033375a307d310b30090603' +
            '55040613025553310b300906035504080c024341310b300906035504070c025346310f30' +
            '0d060355040a0c064a6f79656e743110300e060355040b0c074e6f64652e6a73310f300d' +
            '06035504030c066167656e74313120301e06092a864886f70d010901161172794074696e' +
            '79636c6f7564732e6f726730820122300d06092a864886f70d01010105000382010f0030' +
            '82010a0282010100d456320afb20d3827093dc2c4284ed04dfbabd56e1ddae529e28b790' +
            'cd4256db273349f3735ffd337c7a6363ecca5a27b7f73dc7089a96c6d886db0c62388f1c' +
            'dd6a963afcd599d5800e587a11f908960f84ed50ba25a28303ecda6e684fbe7baedc9ce8' +
            '801327b1697af25097cee3f175e400984c0db6a8eb87be03b4cf94774ba56fffc8c63c68' +
            'd6adeb60abbe69a7b14ab6a6b9e7baa89b5adab8eb07897c07f6d4fa3d660dff574107d2' +
            '8e8f63467a788624c574197693e959cea1362ffae1bba10c8c0d88840abfef103631b2e8' +
            'f5c39b5548a7ea57e8a39f89291813f45a76c448033a2b7ed8403f4baa147cf35e2d2554' +
            'aa65ce49695797095bf4dc6b0203010001a361305f305d06082b06010505070101045130' +
            '4f302306082b060105050730018617687474703a2f2f6f6373702e6e6f64656a732e6f72' +
            '672f302806082b06010505073002861c687474703a2f2f63612e6e6f64656a732e6f7267' +
            '2f63612e63657274300d06092a864886f70d01010b05000382010100c3349810632ccb7d' +
            'a585de3ed51e34ed154f0f7215608cf2701c00eda444dc2427072c8aca4da6472c1d9e68' +
            'f177f99a90a8b5dbf3884586d61cb1c14ea7016c8d38b70d1b46b42947db30edc1e9961e' +
            'd46c0f0e35da427bfbe52900771817e733b371adf19e12137235141a34347db0dfc05579' +
            '8b1f269f3bdf5e30ce35d1339d56bb3c570de9096215433047f87ca42447b44e7e6b5d0e' +
            '48f7894ab186f85b6b1a74561b520952fea888617f32f582afce1111581cd63efcc68986' +
            '00d248bb684dedb9c3d6710c38de9e9bc21f9c3394b729d5f707d64ea890603e5989f8fa' +
            '59c19ad1a00732e7adc851b89487cc00799dde068aa64b3b8fd976e8bc113ef2',
            'hex');

        var x509;
        var ca_cert;

        it('new X509Certificate', () => {
            x509 = new crypto.X509Certificate(cert);
            ca_cert = new crypto.X509Certificate(ca);
        });

        it('chain', () => {
            const pems = cert + ca + ca1 + ca2 + ca3;

            var chain = new crypto.X509Certificate(pems);
            assert.equal(chain.pem, pems);

            var chain = new crypto.X509Certificate([cert + ca, ca1 + ca2 + ca3]);
            assert.equal(chain.pem, pems);

            var chain = new crypto.X509Certificate([cert, ca, ca1 + ca2 + ca3]);
            assert.equal(chain.pem, pems);

            var chain = new crypto.X509Certificate([cert, ca, "", ca1 + ca2 + ca3]);
            assert.equal(chain.pem, pems);

            var chain = new crypto.X509Certificate(["", cert, ca, ca1 + ca2 + ca3]);
            assert.equal(chain.pem, pems);
            assert.equal(chain.toString(), pems);

            assert.throws(() => {
                new crypto.X509Certificate("");
            });

            assert.throws(() => {
                new crypto.X509Certificate([]);
            });
        });

        it('ca', () => {
            assert(!x509.ca);
        });

        it('subject', () => {
            assert.equal(x509.subject, subjectCheck);
        });

        it('issuer', () => {
            assert.equal(x509.issuer, issuerCheck);
        });

        it('infoAccess', () => {
            assert.equal(x509.infoAccess, infoAccessCheck);
        });

        it('validFrom/validTo', () => {
            assert.equal(x509.validFrom, 'Sep  3 21:40:37 2022 GMT');
            assert.equal(x509.validTo, 'Jun 17 21:40:37 2296 GMT');
        });

        it('serialNumber', () => {
            assert.equal(x509.serialNumber, '147D36C1C2F74206DE9FAB5F2226D78ADB00A426');
        });

        it('fingerprints', () => {
            assert.equal(x509.fingerprint,
                '8B:89:16:C4:99:87:D2:13:1A:64:94:36:38:A5:32:01:F0:95:3B:53');
            assert.equal(x509.fingerprint256,
                '2C:62:59:16:91:89:AB:90:6A:3E:98:88:A6:D3:C5:58:58:6C:AE:FF:9C:33:' +
                '22:7C:B6:77:D3:34:E7:53:4B:05'
            );
            assert.equal(x509.fingerprint512,
                '0B:6F:D0:4D:6B:22:53:99:66:62:51:2D:2C:96:F2:58:3F:95:1C:CC:4C:44:' +
                '9D:B5:59:AA:AD:A8:F6:2A:24:8A:BB:06:A5:26:42:52:30:A3:37:61:30:A9:' +
                '5A:42:63:E0:21:2F:D6:70:63:07:96:6F:27:A7:78:12:08:02:7A:8B'
            );
        });

        it('publicKey', () => {
            assert(x509.publicKey);
            assert.equal(x509.publicKey.type, 'public');
        });

        it('raw', () => {
            assert.deepEqual(x509.raw, der);
        });

        it('toString', () => {
            assert.equal(x509.toString().replaceAll('\r\n', '\n'),
                cert.toString().replaceAll('\r\n', '\n'));
        });

        it('checkPrivateKey', () => {
            assert(x509.checkPrivateKey(privateKey));
            assert.throws(() => x509.checkPrivateKey(x509.publicKey));
        });

        it('checkIP', () => {
            assert.equal(x509.checkIP('127.0.0.1'), undefined);
            assert.equal(x509.checkIP('::'), undefined);

            assert.throws(() => x509.checkIP('[::]'));
        });

        it('checkHost', () => {
            assert.equal(x509.checkHost('agent1'), 'agent1');
            assert.equal(x509.checkHost('agent2'), undefined);

            assert.throws(() => x509.checkHost('agent\x001'));

            [
                'wildcards',
                'partialWildcards',
                'multiLabelWildcards',
                'singleLabelSubdomains',
            ].forEach((key) => {
                [1, '', null, {}].forEach((i) => {
                    assert.throws(() => x509.checkHost('agent1', { [key]: i }), {
                        code: 'ERR_INVALID_ARG_TYPE'
                    });
                });
            });
        });

        it('checkEmail', () => {
            assert.equal(x509.checkEmail('ry@tinyclouds.org'), 'ry@tinyclouds.org');
            assert.equal(x509.checkEmail('sally@example.com'), undefined);

            assert.throws(() => x509.checkEmail('not\x00hing'));
        });

        it('checkIssued/verify', () => {
            assert(x509.verify(ca_cert.publicKey));
            assert(!x509.verify(x509.publicKey));
        });

        it('verify', () => {
            assert(x509.verify(ca_cert.publicKey));
            assert(!x509.verify(x509.publicKey));
        });

        it('subjectAltName', () => {
            const expectedSANs = [
                'DNS:"good.example.com\\u002c DNS:evil.example.com"',
                'URI:http://example.com/',
                'URI:http://example.com/?a=b&c=d',
                'URI:"http://example.com/a\\u002cb"',
                'URI:http://example.com/a%2Cb',
                'URI:"http://example.com/a\\u002c DNS:good.example.com"',
                'DNS:"ex\\u00e4mple.com"',
                'DNS:"\\"evil.example.com\\""',
                'IP Address:8.8.8.8',
                'IP Address:8.8.4.4',
                'IP Address:<invalid length=5>',
                'IP Address:<invalid length=6>',
                'IP Address:A0B:C0D:E0F:0:0:0:7A7B:7C7D',
                'email:foo@example.com',
                'email:"foo@example.com\\u002c DNS:good.example.com"',
                'DirName:"L=Hannover\\u002cC=DE"',
                'DirName:"L=München\\u002cC=DE"',
                'DirName:"L=Berlin\\\\\\u002c DNS:good.example.com\\u002cC=DE"',
                'DirName:"L=Berlin\\\\\\u002c DNS:good.example.com\\u0000' +
                'evil.example.com\\u002cC=DE"',
                'DirName:"L=Berlin\\\\\\u002c DNS:good.example.com\\\\\\\\\\u0000' +
                'evil.example.com\\u002cC=DE"',
                'DirName:"L=Berlin\\u000d\\u000a\\u002cC=DE"',
                'DirName:"L=Berlin/CN=good.example.com\\u002cC=DE"',
                'Registered ID:1.2.840.113549.1.1.11',
                'Registered ID:1.3.9999.12.34',
                'othername:XmppAddr:abc123',
                'othername:"XmppAddr:abc123\\u002c DNS:good.example.com"',
                'othername:"XmppAddr:good.example.com\\u0000abc123"',
                'othername:<unsupported>',
                'othername:SRVName:abc123',
                'othername:<unsupported>',
                'othername:"SRVName:abc\\u0000def"',
            ];

            for (let i = 0; i < expectedSANs.length; i++) {
                const pem = readSync(`x509-escaping/alt-${i}-cert.pem`, 'utf8');
                const cert = new crypto.X509Certificate(pem);
                assert.equal(cert.subjectAltName, expectedSANs[i]);
            }
        });

        it('infoAccess', () => {
            const expectedInfoAccess = [
                'OCSP - URI:"http://good.example.com/\\u000a' +
                'OCSP - URI:http://evil.example.com/"',
                'CA Issuers - URI:"http://ca.example.com/\\u000a' +
                'OCSP - URI:http://evil.example.com"\n' +
                'OCSP - DNS:"good.example.com\\u000a' +
                'OCSP - URI:http://ca.nodejs.org/ca.cert"',
                '1.3.9999.12.34 - URI:http://ca.example.com/',
                'OCSP - othername:XmppAddr:good.example.com\n' +
                'OCSP - othername:<unsupported>\n' +
                'OCSP - othername:SRVName:abc123',
                'OCSP - othername:"XmppAddr:good.example.com\\u0000abc123"'
            ];

            for (let i = 0; i < expectedInfoAccess.length; i++) {
                const pem = readSync(`x509-escaping/info-${i}-cert.pem`, 'utf8');
                const expected = expectedInfoAccess[i];

                const cert = new crypto.X509Certificate(pem);
                assert.equal(cert.infoAccess, expected);
            }
        });

        describe("X509CertificateRequest", () => {
            it("create rsa", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({
                    key: pk,
                    subject: {
                        C: "CN",
                        O: "baoz.cn",
                        CN: "baoz.me"
                    }
                });
                assert.equal(req.pem, req2);
                assert.equal(req.toString(), req2);
            })

            it("create sm2", () => {
                var pk = crypto.createPrivateKey(sm2_pem);
                var req = crypto.createCertificateRequest({
                    key: pk,
                    subject: {
                        C: "CN",
                        O: "baoz.cn",
                        CN: "baoz.me"
                    }
                });
            })

            it("info", () => {
                var req = crypto.createCertificateRequest(req2);
                assert.equal(req.subject, "C=CN\nO=baoz.cn\nCN=baoz.me");
                assert.equal(req.publicKey.export({ type: 'spki', format: 'pem' }), pub_rsa4096_pem);
            });

            it("sm2 info", () => {
                var req = crypto.createCertificateRequest(sm2_req);
                assert.equal(req.subject, "C=CN\nO=baoz.cn\nCN=baoz.me");
                assert.equal(req.publicKey.export({ type: 'spki', format: 'pem' }), pub_sm2_pem);
            });

            it("issue", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({
                    key: pk,
                    subject: {
                        C: "CN",
                        O: "baoz.cn",
                        CN: "baoz.me"
                    }
                });

                var cert = req.issue({
                    key: pk,
                    issuer: {
                        commonName: 'ca.com',
                        countryName: 'US',
                        localityName: 'CA',
                        organizationName: 'example',
                        stateOrProvinceName: 'CA',
                    }
                });

                assert.equal(cert.issuer, "CN=ca.com\nC=US\nL=CA\nO=example\nST=CA");
            });

            it("ca", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({ key: pk });

                assert.deepEqual(req.issue({
                    key: pk,
                    ca: true
                }).ca, true);

                assert.deepEqual(req.issue({ key: pk }).ca, false);
            });

            it("pathlen", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({ key: pk });

                assert.deepEqual(req.issue({
                    key: pk,
                    pathlen: 100
                }).pathlen, 100);

                assert.deepEqual(req.issue({ key: pk }).pathlen, -1);
            });

            it("days", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({ key: pk });

                var cert = req.issue({
                    key: pk,
                    days: 100
                });

                assert.equal(new Date(cert.validTo) - new Date(cert.validFrom), 100 * 24 * 60 * 60 * 1000);
            });

            it("KeyUsage", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({
                    key: pk,
                    subject: {
                        C: "CN",
                        O: "baoz.cn",
                        CN: "baoz.me"
                    }
                });

                [
                    "digitalSignature",
                    "nonRepudiation",
                    "keyEncipherment",
                    "dataEncipherment",
                    "keyAgreement",
                    "keyCertSign",
                    "cRLSign",
                    "encipherOnly"
                ].forEach((k) => {
                    var cert = req.issue({
                        key: pk,
                        keyUsage: [k]
                    });

                    assert.deepEqual(cert.keyUsage, [k]);
                });
            });

            it("type", () => {
                var pk = crypto.createPrivateKey(rsa4096_pem);
                var req = crypto.createCertificateRequest({
                    key: pk,
                    subject: {
                        C: "CN",
                        O: "baoz.cn",
                        CN: "baoz.me"
                    }
                });

                [
                    "client",
                    "server",
                    "email",
                    "objsign",
                    "reserved",
                    "sslCA",
                    "emailCA",
                    "objCA"
                ].forEach((k) => {
                    var cert = req.issue({
                        key: pk,
                        type: [k]
                    });

                    assert.deepEqual(cert.type, [k]);
                });
            });

            describe("suites", () => {
                var fl = fs.readdir(path.join(__dirname, 'req_files'));
                fl.forEach((s) => {
                    if (s.match(/\.req/)) {
                        describe(s, () => {
                            var req;
                            it('load', () => {
                                req = crypto.createCertificateRequest(fs.readTextFile(path.join(__dirname, 'req_files', s)));
                            });

                            it('pem', () => {
                                assert.equal(req.pem, req);
                            });

                            it("import/export pem", () => {
                                var s = req.pem;

                                var req1 = crypto.createCertificateRequest(s);

                                assert.equal(req1.pem, s);
                                assert.equal(crypto.createCertificateRequest(s).pem, s);
                            });
                        });
                    }
                });
            });
        });
    });

    it('hkdf', () => {
        const kDerivedKeys = {
            short: '5040737377307264',
            long: '55736572732073686f756c64207069636b206c6f6e6720706173737068726' +
                '173657320286e6f74207573652073686f72742070617373776f7264732921',
            empty: ''
        };

        const kSalts = {
            normal: '536f6469756d2043686c6f7269646520636f6d706f756e64',
            empty: ''
        };

        const kInfos = {
            normal: '484b444620657874726120696e666f',
            empty: ''
        };

        const kDerivations = {
            short: {
                normal: {
                    'sha384': {
                        normal: '19ba74368e6b993390f27fe9a7d02bc338173f72be71a19fc744fcdb3fd4b84b',
                        empty: '97601f4e0c53a5d3f3a2810099bc6820ec50083434769b59fc24a417a9543734'
                    },
                    'sha512': {
                        normal: '4bbd6db2435fb696157f6089c977c3c73f3eac5ef3dd6baae604cb53bfbb153e',
                        empty: '2f3157e7fe0c10b01298c8f0886a90edcf80abdef5dbc1df2b1482532b52b934'
                    },
                    'sha1': {
                        normal: '05ad22ed2138c9600e4d9e2725ded301f5d287fbfb5702f999bc6536d3edef98',
                        empty: 'd51b6fb7e599ca30c5ee264593e4b85f2220c7c3ab003157bff8cb4f369c7560'
                    },
                    'sha256': {
                        normal: '2af5901e28849c28443857386aa1ac3bb127e92631c1c051482d6690941772b4',
                        empty: '9e4b719033742101e90f1ad61e2ff3b4256863667296d74389f1f02af2c4e6a6'
                    }
                },
                empty: {
                    'sha384': {
                        normal: 'fb482ff22c4f8d466c4dfe6e29f2cc2ecdabf5884328fbf08a738fd945f166cb',
                        empty: '1e023c17b340533ceaef39230cb8b3bbdbf663a13d6075d0dd326c049478fba5'
                    },
                    'sha512': {
                        normal: 'f17b5bdcd8d7d3d4601036a19436317d1644f9a4e0956efc0e372b83acdacfdb',
                        empty: 'c7b474942f31f83faf5d14731802b1bd49478549cb3a8f3dbfedc4d3209cf5b6'
                    },
                    'sha1': {
                        normal: 'c126f1e6f25a9de42cf7d427059a52ed9601f29a5815cbfbc64bc7f668c6a341',
                        empty: '3215c3f08de70549b051b7033745a8184f8cbaa6b1735330d2bcb6b16f4642ef'
                    },
                    'sha256': {
                        normal: '733c8b6bcfac875c7f08982a6e3ffb560acea6f165476eb83460b9353ed41dfe',
                        empty: 'c8e12774135305c9147f2cc4766e5ead25d8f457b9a1953d52677361ced558fb'
                    }
                }
            },
            long: {
                normal: {
                    'sha384': {
                        normal: 'f91571b521f7eef13e573aa46378659ef3b7f36ffdd1bb055db2cd77d260c467',
                        empty: '68af1c2cf6b9370d2054344798bdbb1847ccf407b7652b793dd136d4640e0348'
                    },
                    'sha512': {
                        normal: '710aae2fdf889e45fe0fb995b2c26b33eb988650ec0faef167028a7a6ccb3638',
                        empty: 'e5de568081c71e562750829871c342758104765ed6f306f0613c9d4bb336f2aa'
                    },
                    'sha1': {
                        normal: '7f957edcbce3cb0b70566e1eb60efd1e405a13304c661d3663778109bf06899c',
                        empty: '3062f3cf1a730b9cef51f02c1dfac85ed91e4b0065eb50ca9fd8b0107e728733'
                    },
                    'sha256': {
                        normal: '31b7d68530a863e717c081ca6917b68650b3dd9a29f30606e2cad199bec14d13',
                        empty: 'e579d1f9e7f08e6f990ffcfcce1ed201c5e37e62cdf606f0ba4aca80427fbc44'
                    }
                },
                empty: {
                    'sha384': {
                        normal: '619eb6f9287395bbd5ed6a67c968465ad82b6c559f3c38b604bbb08f58320b03',
                        empty: 'ff447b423d83fe76836c32337228b56b5bd9bf68d58e7dca4b7cca842a45e11a'
                    },
                    'sha512': {
                        normal: '133e8a7f7ff433690cc88432c2a338c277e5c13756ff878f46753fe6a564e3e5',
                        empty: 'de54f7eec80c9cc66d349fc987f80d461db2ef4ff4e18505d28bd80cb42c7d76'
                    },
                    'sha1': {
                        normal: 'adb93cdbce79b7d51159b6c0131a2b62f23828d26acd685e34c06535e6f77496',
                        empty: '47710d2a7507e05a1ddcc87a7c2f906177a266efb9e622510cccb3713cd08d58'
                    },
                    'sha256': {
                        normal: 'a401d7c9158a29e5c7193ab9730f0748851cc5baadb42cad024b6290fe213436',
                        empty: 'b4f7e7557674d501cbfbc0148ad800c0750189fe295a2aca5e1bf4122c85edf9'
                    }
                }
            },
            empty: {
                normal: {
                    'sha384': {
                        normal: '6a8632e486899dc264f1a1f920593f2880804e0e1adacf94eb4ed5e5f83d0d12',
                        empty: 'ea5012feb58751d5bc8eb64e0deacd597ed710c9f35258ae6b9a087aed0725ae'
                    },
                    'sha512': {
                        normal: 'c797e1d1f2cab7f28a5f43455c10597f943385edfb428cfe2b98bed4a955d7a1',
                        empty: 'e08cdcc4c5a6aa799d86bc03a95475276ebb801d9ade016e14a8fa5b64051651'
                    },
                    'sha1': {
                        normal: 'ab679e67bcb4305fee42ef940e509cddd406e3498f857418a979ab39cf315f51',
                        empty: 'fe422187188c8636d36daad58ef28431a433bf0fef72d1cae735a04bdbbeb9d3'
                    },
                    'sha256': {
                        normal: 'df92b9a9fa9c01b898ceeaa13134832e31cb1c081d16a5235c69d85651e317ac',
                        empty: 'e60d432b06ee889dfab7299a20ec23697531d119fcf766d0988d0acb0c00c7f7'
                    }
                },
                empty: {
                    'sha384': {
                        normal: 'eacb9d6670ff3b1904779a419101b1ffaabd6d6510bd50856801746a871f7b31',
                        empty: '470cc65387ca4a10c7a68a3b5148c8e513daa63101000739c4c6659b86118884'
                    },
                    'sha512': {
                        normal: '57039174f16f5418a86856daeb77f69d4b4d5000334b6dd1f4f4b3e7b3dcb9d3',
                        empty: '9d73c98e791e80ebe5b4cb45693aa32fdd44b5fa3edab3ec82f9d0f4d66905e2'
                    },
                    'sha1': {
                        normal: 'a1bdd8c332c6464a4bb6a2f231aec9a444237eabe04d2f55f2ab25d40c54ebee',
                        empty: '885fc029b3224b896e09e0bbe5eb347ec59e6827c8e857b394f54ff49b88a8f6'
                    },
                    'sha256': {
                        normal: 'b7b86e422ad1c8a571fda528da16a066f42486dd4056792fd93362088e5dd4c2',
                        empty: 'eb70f01dede9afafa449eee1b1286504e1f62388b3f7dd4f956697b0e828fe18'
                    }
                }
            }
        };

        for (var key in kDerivedKeys) {
            for (var salt in kSalts) {
                for (var digest in kDerivations[key][salt]) {
                    for (var info in kInfos) {
                        const result = crypto.hkdfSync(digest, Buffer.from(kDerivedKeys[key], 'hex'), Buffer.from(kSalts[salt], 'hex'), Buffer.from(kInfos[info], 'hex'), 32).hex();
                        assert.equal(result, kDerivations[key][salt][digest][info]);
                    }
                }
            }
        }
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
            assert.deepEqual(crypto.pbkdf2(t[0], t[1], t[2], t[3], 'sha1'), t[4]);
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

    describe("webcrypto", () => {
        it("global.crypto equal require('crypto').webcrypto", () => {
            assert.equal(global.crypto, require('crypto').webcrypto);
        });

        it("getRandomValues", () => {
            const arrays = [
                'Int8Array',
                'Int16Array',
                'Int32Array',
                'BigInt64Array',
                'Uint8Array',
                'Uint8ClampedArray',
                'Uint16Array',
                'Uint32Array',
                'BigUint64Array',
            ];

            for (const array of arrays) {
                const ctor = globalThis[array];

                assert.equal(global.crypto.getRandomValues(new ctor(8)).constructor, ctor);

                const maxlength = 65536 / ctor.BYTES_PER_ELEMENT;
                assert.throws(() => {
                    global.crypto.getRandomValues(new ctor(maxlength + 1))
                });

                assert.equal(global.crypto.getRandomValues(new ctor(0)).length, 0);
            }
        });

        it("randomUUID", () => {
            const iterations = 256;
            const uuids = new Set()
            function randomUUID() {
                const uuid = global.crypto.randomUUID();
                if (uuids.has(uuid)) {
                    throw new Error(`uuid collision ${uuid}`)
                }
                uuids.add(uuid);
                return uuid;
            }

            for (let i = 0; i < iterations; i++) {
                let value = parseInt(randomUUID().split('-')[2].slice(0, 2), 16);
                value &= 0b11110000;
                assert.equal(value, 0b01000000);
            }
        });

        describe("subtle", () => {
            it("digest", async () => {
                var sourceData = {
                    empty: Buffer.alloc(0),
                    short: Buffer.from([21, 110, 234, 124, 193, 76, 86, 203, 148, 219, 3, 10, 74, 157, 149, 255]),
                    medium: Buffer.from([182, 200, 249, 223, 100, 140, 208, 136, 183, 15, 56, 231, 65, 151, 177, 140, 184, 30, 30, 67, 80, 213, 11, 204, 184, 251, 90, 115, 121, 200, 123, 178, 227, 214, 237, 84, 97, 237, 30, 159, 54, 243, 64, 163, 150, 42, 68, 107, 129, 91, 121, 75, 75, 212, 58, 68, 3, 80, 32, 119, 178, 37, 108, 200, 7, 131, 127, 58, 172, 209, 24, 235, 75, 156, 43, 174, 184, 151, 6, 134, 37, 171, 172, 161, 147])
                };

                sourceData.long = Buffer.alloc(1024 * sourceData.medium.byteLength);
                for (var i = 0; i < 1024; i++) {
                    sourceData.long.set(sourceData.medium, i * sourceData.medium.byteLength);
                }

                var digestedData = {
                    "sha-1": {
                        empty: Buffer.from([218, 57, 163, 238, 94, 107, 75, 13, 50, 85, 191, 239, 149, 96, 24, 144, 175, 216, 7, 9]),
                        short: Buffer.from([201, 19, 24, 205, 242, 57, 106, 1, 94, 63, 78, 106, 134, 160, 186, 101, 184, 99, 89, 68]),
                        medium: Buffer.from([229, 65, 6, 8, 112, 235, 22, 191, 51, 182, 142, 81, 245, 19, 82, 104, 147, 152, 103, 41]),
                        long: Buffer.from([48, 152, 181, 0, 55, 236, 208, 46, 189, 101, 118, 83, 178, 191, 160, 30, 238, 39, 162, 234])
                    },
                    "sha-256": {
                        empty: Buffer.from([227, 176, 196, 66, 152, 252, 28, 20, 154, 251, 244, 200, 153, 111, 185, 36, 39, 174, 65, 228, 100, 155, 147, 76, 164, 149, 153, 27, 120, 82, 184, 85]),
                        short: Buffer.from([162, 131, 17, 134, 152, 71, 146, 199, 211, 45, 89, 200, 151, 64, 104, 127, 25, 173, 220, 27, 149, 158, 113, 161, 204, 83, 138, 59, 126, 216, 67, 242]),
                        medium: Buffer.from([83, 83, 103, 135, 126, 240, 20, 215, 252, 113, 126, 92, 183, 132, 62, 89, 182, 26, 238, 98, 199, 2, 156, 236, 126, 198, 193, 47, 217, 36, 224, 228]),
                        long: Buffer.from([20, 205, 234, 157, 199, 95, 90, 98, 116, 217, 252, 30, 100, 0, 153, 18, 241, 220, 211, 6, 180, 143, 232, 233, 207, 18, 45, 230, 113, 87, 23, 129])
                    },
                    "sha-384": {
                        empty: Buffer.from([56, 176, 96, 167, 81, 172, 150, 56, 76, 217, 50, 126, 177, 177, 227, 106, 33, 253, 183, 17, 20, 190, 7, 67, 76, 12, 199, 191, 99, 246, 225, 218, 39, 78, 222, 191, 231, 111, 101, 251, 213, 26, 210, 241, 72, 152, 185, 91]),
                        short: Buffer.from([107, 245, 234, 101, 36, 209, 205, 220, 67, 247, 207, 59, 86, 238, 5, 146, 39, 64, 74, 47, 83, 143, 2, 42, 61, 183, 68, 122, 120, 44, 6, 193, 237, 5, 232, 171, 79, 94, 220, 23, 243, 113, 20, 64, 223, 233, 119, 49]),
                        medium: Buffer.from([203, 194, 197, 136, 254, 91, 37, 249, 22, 218, 40, 180, 228, 122, 72, 74, 230, 252, 31, 228, 144, 45, 213, 201, 147, 154, 107, 253, 3, 74, 179, 180, 139, 57, 8, 116, 54, 1, 31, 106, 153, 135, 157, 39, 149, 64, 233, 119]),
                        long: Buffer.from([73, 244, 253, 179, 152, 25, 104, 249, 125, 87, 55, 15, 133, 52, 80, 103, 205, 82, 150, 169, 125, 209, 161, 142, 6, 145, 30, 117, 110, 150, 8, 73, 37, 41, 135, 14, 26, 209, 48, 153, 141, 87, 203, 251, 183, 193, 208, 158])
                    },
                    "sha-512": {
                        empty: Buffer.from([207, 131, 225, 53, 126, 239, 184, 189, 241, 84, 40, 80, 214, 109, 128, 7, 214, 32, 228, 5, 11, 87, 21, 220, 131, 244, 169, 33, 211, 108, 233, 206, 71, 208, 209, 60, 93, 133, 242, 176, 255, 131, 24, 210, 135, 126, 236, 47, 99, 185, 49, 189, 71, 65, 122, 129, 165, 56, 50, 122, 249, 39, 218, 62]),
                        short: Buffer.from([55, 82, 72, 190, 95, 243, 75, 231, 76, 171, 79, 241, 195, 188, 141, 198, 139, 213, 248, 223, 244, 2, 62, 152, 248, 123, 134, 92, 255, 44, 114, 66, 146, 223, 24, 148, 67, 166, 79, 244, 19, 74, 101, 205, 70, 53, 185, 212, 245, 220, 13, 63, 182, 117, 40, 0, 42, 99, 172, 242, 108, 157, 165, 117]),
                        medium: Buffer.from([185, 16, 159, 131, 158, 142, 164, 60, 137, 15, 41, 60, 225, 29, 198, 226, 121, 141, 30, 36, 49, 241, 228, 185, 25, 227, 178, 12, 79, 54, 48, 59, 163, 156, 145, 109, 179, 6, 196, 90, 59, 101, 118, 31, 245, 190, 133, 50, 142, 234, 244, 44, 56, 48, 241, 217, 94, 122, 65, 22, 91, 125, 45, 54]),
                        long: Buffer.from([75, 2, 202, 246, 80, 39, 96, 48, 234, 86, 23, 229, 151, 197, 213, 63, 217, 218, 166, 139, 120, 191, 230, 11, 34, 170, 184, 211, 106, 76, 42, 58, 255, 219, 113, 35, 79, 73, 39, 103, 55, 197, 117, 221, 247, 77, 20, 5, 76, 189, 111, 219, 152, 253, 13, 220, 188, 180, 111, 145, 173, 118, 182, 238])
                    },
                }

                for (var source in sourceData) {
                    for (var digest in digestedData) {
                        const sourceBuffer = sourceData[source];
                        const digestedBuffer = digestedData[digest][source];

                        const hash = await global.crypto.subtle.digest(digest, sourceBuffer);
                        assert.deepEqual(hash, digestedBuffer);
                    }
                }
            });
        });

        describe("generateKey", async () => {
            it("ECDSA", async () => {
                const key = await global.crypto.subtle.generateKey(
                    {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    },
                    true,
                    ["sign", "verify"]
                );

                assert.equal(key.privateKey.algorithm.name, "ECDSA");
                assert.equal(key.privateKey.algorithm.namedCurve, "P-256");
                assert.equal(key.publicKey.algorithm.name, "ECDSA");
                assert.equal(key.publicKey.algorithm.namedCurve, "P-256");

                assert.deepEqual(key.privateKey.usages, ["sign"]);
                assert.deepEqual(key.publicKey.usages, ["verify"]);

                assert.isTrue(key.privateKey.extractable);
                assert.isTrue(key.publicKey.extractable);
            });

            it("should not include 'verify' when it is not included in the keyUsages", async () => {
                const key = await global.crypto.subtle.generateKey(
                    {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    },
                    true,
                    ["sign"]
                );

                assert.deepEqual(key.privateKey.usages, ["sign"]);
                assert.deepEqual(key.publicKey.usages, []);
            });

            it("should set extractable of public key to true by default", async () => {
                const key = await global.crypto.subtle.generateKey(
                    {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    },
                    false,
                    ["sign", "verify"]
                );

                assert.isFalse(key.privateKey.extractable);
                assert.isTrue(key.publicKey.extractable);
            });

            it("should throw if the key type is not supported", async () => {
                assert.throws(async () => {
                    const key = await global.crypto.subtle.generateKey(
                        {
                            name: "ECDSA",
                            namedCurve: "P-256"
                        },
                        true,
                        ["sign", "verify", "encrypt"]
                    );
                });
            });

            it("should throw if 'sign' is not included in the keyUsages", async () => {
                assert.throws(async () => {
                    const key = await global.crypto.subtle.generateKey(
                        {
                            name: "ECDSA",
                            namedCurve: "P-256"
                        },
                        true,
                        ["verify"]
                    );
                });
            });

            it("should throw if namedCurve is not supported", async () => {
                assert.throws(async () => {
                    const key = await global.crypto.subtle.generateKey(
                        {
                            name: "ECDSA",
                            namedCurve: "P-128"
                        },
                        true,
                        ["verify"]
                    );
                });
            });
        });

        describe("exportKey/importKey", async () => {
            var test_keys = crypto.generateKeyPairSync("ec", {
                namedCurve: "P-256",
                publicKeyEncoding: {
                    format: "jwk"
                },
                privateKeyEncoding: {
                    format: "jwk"
                }
            });

            it("imoprt/export JWK", async () => {
                const importedPublicKey = await global.crypto.subtle.importKey("jwk", test_keys.publicKey, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["verify"]);

                const importedPrivateKey = await global.crypto.subtle.importKey("jwk", test_keys.privateKey, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["sign"]);

                const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);
                const exportedPrivateKey = await global.crypto.subtle.exportKey("jwk", importedPrivateKey);

                assert.deepEqual(exportedPublicKey, test_keys.publicKey);
                assert.deepEqual(exportedPrivateKey, test_keys.privateKey);
            });

            it("export pkcs8", async () => {
                const importedPrivateKey = await global.crypto.subtle.importKey("jwk", test_keys.privateKey, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["sign"]);

                const exportedPrivateKey = await global.crypto.subtle.exportKey("pkcs8", importedPrivateKey);

                const sk = crypto.createPrivateKey({
                    key: test_keys.privateKey
                });

                const exportedPrivateKey2 = sk.export({ format: "der", type: "pkcs8" });

                assert.deepEqual(exportedPrivateKey, exportedPrivateKey2);
            });

            it("import pkcs8", async () => {
                const sk = crypto.createPrivateKey({
                    key: test_keys.privateKey
                });

                const exportedPrivateKey2 = sk.export({ format: "der", type: "pkcs8" });

                const importedPrivateKey = await global.crypto.subtle.importKey("pkcs8", exportedPrivateKey2, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["sign"]);

                const exportedPrivateKey = await global.crypto.subtle.exportKey("jwk", importedPrivateKey);

                assert.deepEqual(exportedPrivateKey, test_keys.privateKey);
            });

            it("export spki", async () => {
                const importedPublicKey = await global.crypto.subtle.importKey("jwk", test_keys.publicKey, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["verify"]);

                const exportedPublicKey = await global.crypto.subtle.exportKey("spki", importedPublicKey);

                const pk = crypto.createPublicKey({
                    key: test_keys.publicKey
                });

                const exportedPublicKey2 = pk.export({ format: "der", type: "spki" });

                assert.deepEqual(exportedPublicKey, exportedPublicKey2);
            });

            it("import spki", async () => {
                const pk = crypto.createPublicKey({
                    key: test_keys.publicKey
                });

                const exportedPublicKey2 = pk.export({ format: "der", type: "spki" });

                const importedPublicKey = await global.crypto.subtle.importKey("spki", exportedPublicKey2, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["verify"]);

                const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);

                assert.deepEqual(exportedPublicKey, test_keys.publicKey);
            });

            it("export raw", async () => {
                const importedPublicKey = await global.crypto.subtle.importKey("jwk", test_keys.publicKey, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["verify"]);

                const exportedPublicKey = await global.crypto.subtle.exportKey("raw", importedPublicKey);

                const pk = crypto.createPublicKey({
                    key: test_keys.publicKey
                });

                const exportedPublicKey2 = pk.export({ format: "raw" });

                assert.deepEqual(exportedPublicKey, exportedPublicKey2);
            });

            it("import raw", async () => {
                const pk = crypto.createPublicKey({
                    key: test_keys.publicKey
                });

                const exportedPublicKey2 = pk.export({ format: "raw" });

                const importedPublicKey = await global.crypto.subtle.importKey("raw", exportedPublicKey2, {
                    name: "ECDSA",
                    namedCurve: "P-256"
                }, true, ["verify"]);

                const exportedPublicKey = await global.crypto.subtle.exportKey("jwk", importedPublicKey);

                assert.deepEqual(exportedPublicKey, test_keys.publicKey);
            });

            it("should throw if name is not matching", async () => {
                assert.throws(async () => {
                    await global.crypto.subtle.importKey("jwk", test_keys.publicKey, {
                        name: "Ed25519",
                        namedCurve: "P-256"
                    }, true, ["verify"]);
                });
            });

            it("should throw if namedCurve is not matching", async () => {
                assert.throws(async () => {
                    await global.crypto.subtle.importKey("jwk", test_keys.publicKey, {
                        name: "ECDSA",
                        namedCurve: "P-384"
                    }, true, ["verify"]);
                });
            });

            it("should throw if the key type is not supported", async () => {
                assert.throws(async () => {
                    await global.crypto.subtle.importKey("jwk", test_keys.publicKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["sign"]);
                });

                assert.throws(async () => {
                    await global.crypto.subtle.importKey("jwk", test_keys.privateKey, {
                        name: "ECDSA",
                        namedCurve: "P-256"
                    }, true, ["verify"]);
                });
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);