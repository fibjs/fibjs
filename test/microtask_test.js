var test = require("test");
test.setup();

const coroutine = require("coroutine");

describe("microtask", () => {
    it("resolve", () => {
        var flag = false;
        var ev = new coroutine.Event();

        async function test1() {
        }

        async function test2() {
            await test1();
            flag = true;
            ev.set();
        }

        test2();
        ev.wait();

        assert.equal(flag, true);
    });

    it("reject", () => {

        async function test1() {
            throw "error"
        }

        var flag;
        var ev = new coroutine.Event();

        async function test2() {
            try {
                await test1();
            } catch (e) {
                flag = e;
            }

            ev.set();

        }

        test2();
        ev.wait();

        assert.equal(flag, "error");
    });

    it("async+promise resolve", () => {
        var flag = false;
        var ev = new coroutine.Event();

        async function sleep(ms) {
            return new Promise(resolve => setTimeout(resolve, ms));
        }

        async function test2() {
            await sleep(10);
            flag = true;
            ev.set();
        }

        test2();
        ev.wait();

        assert.equal(flag, true);
    });

    it("async+promise reject", () => {
        var flag;
        var ev = new coroutine.Event();

        async function sleep(ms) {
            return new Promise((resolve, reject) => setTimeout(() => {
                reject("timeout error");
            }, ms));
        }

        async function test2() {
            try {
                await sleep(10);
            } catch (e) {
                flag = e;
            }
            ev.set();
        }

        test2();
        ev.wait();

        assert.equal(flag, "timeout error");
    });
});

require.main === module && test.run(console.DEBUG);
