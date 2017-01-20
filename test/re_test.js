var test = require("test");
test.setup();

var re = require("re");

describe("re", () => {

    function re_create() {
        var regex = re.compile("fibjs/([^/]*)/fibjs/([a-z]+)"),
            regexg = re.compile("fibjs/([^/]*)/fibjs/([a-z]+)", "g"),
            regexi = re.compile("fibjs/([^/]*)/fibjs/([a-z]+)", "i"),
            regexm = re.compile("^([a-z0-9]+)$", "gm"),
            regexgim = re.compile("fibjs", "gim");
        return [regex, regexg, regexi, regexm, regexgim];
    }

    it("re compile", () => {
        var r = re_create();
        assert.isFalse(r[0].ignoreCase);
        assert.isFalse(r[0].global);

        assert.isFalse(r[1].ignoreCase);
        assert.isTrue(r[1].global);

        assert.isTrue(r[2].ignoreCase);
        assert.isFalse(r[2].global);

        assert.isTrue(r[3].multiline);

        assert.isTrue(r[4].ignoreCase);
        assert.isTrue(r[4].global);
        assert.isTrue(r[4].multiline);
    });

    it("regex exec", () => {
        var r = re_create();
        var str = "fibjs/ab/fibjs/cd/fibjs/ef/fibjs/gh";

        assert.deepEqual(r[0].exec(str), ["fibjs/ab/fibjs/cd", "ab", "cd"]);

        assert.deepEqual(r[1].exec(str), ["fibjs/ab/fibjs/cd", "ab", "cd"]);
        r[1].lastIndex = 0;
        assert.deepEqual(r[1].exec(str), ["fibjs/ab/fibjs/cd", "ab", "cd"]);

        str = "Fibjs/ab/Fibjs/cd";
        assert.deepEqual(r[2].exec(str), ["Fibjs/ab/Fibjs/cd", "ab", "cd"]);

        str = "fibjs1\nfibjs2\nfibjs3\n";
        assert.deepEqual(r[3].exec(str), ["fibjs1", "fibjs1"]);
        assert.deepEqual(r[3].exec(str), ["fibjs2", "fibjs2"]);
        assert.deepEqual(r[3].exec(str), ["fibjs3", "fibjs3"]);

    });

    it("regex test", () => {
        var r = re_create();
        var str = "fibjs/ab/fibjs/cd/fibjs/ef/fibjs/gh";
        assert.isTrue(r[0].test(str));
        assert.isTrue(r[1].test(str));

        str = "Fibjs/ab/Fibjs/cd";
        assert.isTrue(r[2].test(str));

        str = "fibjs1\nfibjs2\nfibjs3\n";
        assert.isTrue(r[3].test(str));
    });
});
