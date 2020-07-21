var test = require("test");
test.setup();

var win = process.platform == 'win32';

if (win) {
    var registry = require('registry');

    describe("registry", () => {
        before(() => {
            try {
                registry.del(registry.CURRENT_USER, "Software\\fibjs_test");
            } catch (e) {}
        });

        it("get", () => {
            assert.equal(registry.get(registry.LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir"),
                process.env.ProgramFiles);
        });

        it("set: number", () => {
            registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\a", 100);
            assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\a"), 100);
        });

        it("set: string", () => {
            registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\s", "this is a string");
            assert.equal(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\s"), "this is a string");
        });

        it("set: multi string", () => {
            registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms", [
                "string1",
                "string2",
                "string3"
            ]);
            assert.deepEqual(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms"), [
                "string1",
                "string2",
                "string3"
            ]);
        });

        it("set: binary", () => {
            registry.set(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\b",
                new Buffer("this is binary data"));
            assert.deepEqual(registry.get(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\b"),
                new Buffer("this is binary data"));
        });

        it("list", () => {
            assert.deepEqual(registry.listSubKey(registry.CURRENT_USER, "Software\\fibjs_test"), ["test_key"]);
            assert.deepEqual(registry.listValue(registry.CURRENT_USER, "Software\\fibjs_test\\test_key").sort(), [
                "a",
                "b",
                "ms",
                "s"
            ]);
        });

        it("del", () => {
            registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key\\ms");
            assert.deepEqual(registry.listValue(registry.CURRENT_USER, "Software\\fibjs_test\\test_key").sort(), [
                "a",
                "b",
                "s"
            ]);

            registry.del(registry.CURRENT_USER, "Software\\fibjs_test\\test_key");
            assert.deepEqual(registry.listSubKey(registry.CURRENT_USER, "Software\\fibjs_test"), []);
        });
    });

    // winreg test
    var winreg = require('winreg');
    describe("winreg", () => {

        var testkey1 = 'HKEY_CURRENT_USER\\Software\\fibjs\\test';
        var testkey2 = 'hkcu/software/fibjs/test';

        before(() => {
            try {
                winreg.delKey(testkey1);
            } catch (e) {}
        });

        after(() => {
           winreg.delKey('HKCU/Software/fibjs');
        });

        it('set_dword', () => {
        	winreg.set_dword(testkey1, 'dword1', 0xABCD);
        	assert.deepEqual(winreg.get(testkey2, 'dword1'), 0xABCD);
        });

        it('set_qword', () => {
        	winreg.set_qword(testkey1, 'qword1', 0xABCDEFn);
        	assert.deepEqual(winreg.get(testkey2, 'qword1'), 0xABCDEFn);
        });

        it('set_sz', () => {
        	winreg.set_sz(testkey1, 'sz1', 'sz value');
        	assert.deepEqual(winreg.get(testkey2, 'sz1'), 'sz value');
        });

        it('set_sz : "/ \\" in value name', () => {
        	winreg.set_sz(testkey1, 'sz\\a\\b\\c\\d', 'sz value');
        	assert.deepEqual(winreg.get(testkey2, 'sz\\a\\b\\c\\d'), 'sz value');
        	winreg.set_sz(testkey1, 'sz/a/b/c/d'    , 'sz value');
        	assert.deepEqual(winreg.get(testkey2, 'sz/a/b/c/d'), 'sz value');
        });

        it('set_expand_sz', () => {
        	winreg.set_expand_sz(testkey1, 'expand_sz1', 'expand_sz;%path%');
        	assert.deepEqual(winreg.get(testkey2, 'expand_sz1'), 'expand_sz;%path%');
        });

        it('set_multi_sz', () => {
        	winreg.set_multi_sz(testkey1, 'multi_sz1', ['x','y','z']);
        	assert.deepEqual(winreg.get(testkey2, 'multi_sz1'), ['x','y','z']);
        });

        it('set_binary', () => {
        	winreg.set_binary(testkey1, 'binary1', Buffer.from('binary data...'));
        	assert.deepEqual(winreg.get(testkey2, 'binary1'), Buffer.from('binary data...'));
        });

        it('listValueName', () => {
        	var a1 = winreg.listValueName(testkey2); a1.sort();
        	var a2 = ['dword1', 'qword1', 'sz1', 'sz\\a\\b\\c\\d', 'sz/a/b/c/d', 'expand_sz1', 'multi_sz1', 'binary1'].sort();
        	assert.deepEqual(a1, a2);
        });

        it('create SubKey', () => {
        	winreg.set_sz(testkey1+'\\sub1', 'name1', 'value1');
        	assert.deepEqual(winreg.get(testkey2+'/sub1', 'name1'), 'value1');

        	winreg.set_sz(testkey1+'\\sub1\\sub11', 'name11', 'value11');
        	assert.deepEqual(winreg.get(testkey2+'/sub1/sub11', 'name11'), 'value11');

        	winreg.set_sz(testkey1+'\\sub2', '', '');
        	assert.deepEqual(winreg.get(testkey2+'/sub2', ''), '');
        });

        it('listSubKey', () => {
        	var a1 = winreg.listSubKey(testkey2); a1.sort();
        	var a2 = ['sub1', 'sub2'].sort();
        	assert.deepEqual(a1, a2);
        });

        it('delValue', () => {
          winreg.delValue(testkey2 +'/sub1', 'name1');
        	assert.deepEqual(winreg.listValueName(testkey2 + '/sub1'), []);
        });

        it('delKey', () => {
          winreg.delKey(testkey2+'/sub1');
        	assert.deepEqual(winreg.listSubKey(testkey2), ['sub2']);
        });
    });
}

require.main === module && test.run(console.DEBUG);