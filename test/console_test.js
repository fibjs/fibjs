var test = require("test");
test.setup();

var os = require('os');

describe("console", () => {
    it("add", () => {
        console.use({
            type: "console"
        });

        if (process.platform === 'win32') {
            assert.throws(() => {
                console.use("syslog");
            });
        } else {
            console.use("syslog");

            console.use(["console", {
                type: "syslog"
            }]);
        }

        console.reset();
    });

    it("too many logger", () => {
        console.use(["console", "console", "console", "console",
            "console", "console", "console", "console", "console", "console"
        ]);

        assert.throws(() => {
            console.use("console");
        });

        console.reset();
    });

    it("log levels", () => {
        console.use({
            type: "console",
            levels: [console.DEBUG, console.ERROR]
        });

        // console.use({
        //     type: "console",
        //     levels: [console.DEBUG, {}]
        // });

        // console.use({
        //     type: "console",
        //     levels: [console.DEBUG, NaN]
        // });

        console.use({
            type: "console",
            levels: [console.DEBUG, '1']
        });

        assert.throws(() => {
            console.use({
                type: "console",
                levels: [console.DEBUG, 100]
            });
        });

        console.reset();
    });

    it("add file logger", () => {
        console.use("console");

        console.use({
            type: "file",
            path: "test_log",
            split: "day",
            count: 12
        });

        assert.throws(() => {
            console.use({
                type: "file"
            });
        });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                count: 10
            });
        }, "Missing split mode.");

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                count: 1
            });
        }, "Too few file count.");

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                count: 129
            });
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "day"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "hour"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "minute"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "1k"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "10k"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "100k"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "100m"
        });

        console.use({
            type: "file",
            path: "test_log",
            split: "100g"
        });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "100n"
            });
        });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "100n"
            });
        });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "1000k"
            });
        });

        assert.throws(() => {
            console.use({
                type: "file",
                path: "test_log",
                split: "k"
            });
        });

        console.reset();
    });

    it("console.assert is assert", () => {
        assert.equal(console.assert, assert);
    });

    it("fix: eval scriptname crash", () => {
        eval('console.log("Rock Lee")');
    })
});

require.main === module && test.run(console.DEBUG);