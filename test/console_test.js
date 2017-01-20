var test = require("test");
test.setup();

var os = require('os');

describe("console", () => {
    it("add", () => {
        console.add({
            type: "console"
        });

        if (os.type == 'Windows') {
            assert.throws(() => {
                console.add("syslog");
            });
        } else {
            console.add("syslog");

            console.add(["console", {
                type: "syslog"
            }]);
        }

        console.reset();
    });

    it("too many logger", () => {
        console.add(["console", "console", "console", "console",
            "console", "console", "console", "console", "console", "console"
        ]);

        assert.throws(() => {
            console.add("console");
        });

        console.reset();
    });

    it("log levels", () => {
        console.add({
            type: "console",
            levels: [console.DEBUG, console.ERROR]
        });

        assert.throws(() => {
            console.add({
                type: "console",
                levels: [console.DEBUG, "other"]
            });
        });

        assert.throws(() => {
            console.add({
                type: "console",
                levels: [console.DEBUG, 100]
            });
        });

        console.reset();
    });

    it("add file logger", () => {
        console.add("console");

        console.add({
            type: "file",
            path: "test_log",
            split: "day",
            count: 12
        });

        assert.throws(() => {
            console.add({
                type: "file"
            });
        });

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                count: 10
            });
        }, "Missing split mode.");

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                count: 1
            });
        }, "Too few file count.");

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                count: 129
            });
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "day"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "hour"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "minute"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "1k"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "10k"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "100k"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "100m"
        });

        console.add({
            type: "file",
            path: "test_log",
            split: "100g"
        });

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                split: "100n"
            });
        });

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                split: "100n"
            });
        });

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                split: "1000k"
            });
        });

        assert.throws(() => {
            console.add({
                type: "file",
                path: "test_log",
                split: "k"
            });
        });

        console.reset();
    });

    it("fix: eval scriptname crash", () => {
        eval('console.log("Rock Lee")');
    })
});

//test.run(console.DEBUG);
