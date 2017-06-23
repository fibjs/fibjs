var test = require("test");
test.setup();

var db = require('db');
var os = require('os');
var assert = require('assert');

describe("mongodb", () => {
    function clear(mdb) {
        try {
            mdb.test.drop();
        } catch (e) {}
    }

    it("open/close", () => {
        var mdb = db.open("mongodb://127.0.0.1/test");
        mdb.close();
        // var no1 = os.memoryUsage().nativeObjects.objects;
        // mdb.dispose();
        // assert.equal(no1 - 2, os.memoryUsage().nativeObjects.objects);
    })

    it("runCommand", () => {
        var mdb = db.open("mongodb://127.0.0.1/test");
        clear(mdb);

        mdb.test.insert({
            _id: 1,
            data: "data1"
        })

        assert.deepEqual(mdb.test.find({
            "_id": 1
        }).toArray()[0]["data"], "data1");

        var r = mdb.runCommand({
            findAndModify: "test",
            query: {
                _id: 1
            },
            update: {
                data: "new data1"
            }
        });

        assert.deepEqual(mdb.test.find({
            "_id": 1
        }).toArray()[0]["data"], "new data1");
    })

    it("collection & cursor", () => {
        var mdb = db.open("mongodb://127.0.0.1/test");
        clear(mdb);

        var datas = [];

        var i;

        for (i = 0; i < 30; i++) {
            datas[i] = {
                _id: i,
                data: 'data_' + i
            }
        }

        mdb.test.insert(datas);

        assert.deepEqual(mdb.test.find().toArray(), mdb.getCollection("test").find().toArray())

        assert.equal(mdb.test.find().skip(10).count(), 30);
        assert.equal(mdb.test.find().skip(10).size(), 20);

        assert.deepEqual(mdb.test.find().toJSON(), datas);
        assert.deepEqual(mdb.test.findOne({
            _id: 3
        }), datas[3]);
        assert.deepEqual(mdb.test.find().skip(10).toJSON(), datas.slice(10));
        assert.deepEqual(mdb.test.find().skip(10).limit(10).toJSON(), datas.slice(10, 20));

        datas[0].data = 'new data';
        mdb.test.save(datas[0]);

        assert.deepEqual(mdb.test.find().sort({
            _id: 1
        }).toJSON(), datas);

        var r = mdb.test.findAndModify({
            query: {
                _id: 10
            },
            update: {
                data: "new data1"
            }
        });
        assert.deepEqual(r.value, datas[10]);

        datas[10].data = "new data1";
        assert.deepEqual(mdb.test.find().sort({
            _id: 1
        }).toJSON(), datas);
    })

    it("callback", () => {
        var mdb = db.open("mongodb://127.0.0.1/test");
        clear(mdb);

        var datas = [];
        var conts = [];

        var i;

        for (i = 0; i < 30; i++) {
            datas[i] = {
                _id: i,
                data: 'data_' + i
            }
            conts[i] = 'data_' + i;
        }

        mdb.test.insert(datas);

        var res = mdb.test.find().map(function (elem) {
            return elem["data"];
        })

        assert.deepEqual(res, conts);

        i = 0;
        mdb.test.find().forEach(function (elem) {
            assert.equal(elem["data"], 'data_' + i++);
        })
    })

    it("index", () => {
        var mdb = db.open("mongodb://127.0.0.1/test");
        clear(mdb);

        var datas = [];

        var i;

        for (i = 0; i < 30; i++) {
            datas[i] = {
                _id: i,
                data: 'data_' + i
            }
        }

        mdb.test.insert(datas);

        mdb.test.ensureIndex({
            data: 1
        });

        // console.log(mdb.test.getIndexes());

        // assert.equal(mdb.test.getIndexes().toArray()[1]["name"], "data_1");
        mdb.test.reIndex();

        mdb.test.dropIndexes();
        // assert.equal(mdb.test.getIndexes().toArray()[1], null)
    })

    xit("GridFS", () => {
        var mdb = db.open("mongodb://127.0.0.1/test");
        mdb.fs.chunks.ensureIndex({
            files_id: 1,
            n: 1
        }, {
            unique: true
        });
        mdb.fs.chunks.reIndex();

        mdb.fs.files.ensureIndex({
            filename: 1
        }, {
            unique: true
        });

        mdb.fs.files.reIndex();


        var fs = mdb.fs;
        var data = new Buffer("this is a gridfs file.");

        fs.store("test.txt", data);
        assert.ok(fs.exists("test.txt"), "file created.");

        assert.deepEqual(fs.retrieve("test.txt").readAll(), data);

        fs.remove('test.txt');
        assert.ok(!fs.exists("test.txt"), "file removed.");
    })

    //assert.equal(fs.retrieve("test.txt").readAll(), data);
})

repl && test.run(console.DEBUG);