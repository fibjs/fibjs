var test = require("test");
test.setup();

var rados = require('rados');
var os = require('os');
var fs = require('fs');
var re = require('re');

var win = os.type() == "Windows";

var ceph_cluster = 'ceph';
var ceph_username = 'client.admin';
var ceph_config = '/etc/ceph/ceph.conf';
var radosCluster, radosIoctx, radosStream;
var poolN = "radospool";
var oid = "foo";
var snapshot = "s";

describe('fs', () => {
	it('create cluster', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosCluster = new rados.Rados(ceph_cluster, ceph_username, ceph_config);
		});
	});

	it('connect', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosCluster.connect();
		});
	});

	it('create pool', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosCluster.createPool(poolN);
		})
	});

	it('list pool', () => {
		if (win) return;
		assert.ok(radosCluster.listPool().indexOf(poolN) > -1);
	});

	it('delete pool', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosCluster.deletePool(poolN);
		});
		assert.ok(radosCluster.listPool().indexOf(poolN) == -1);
	});

	it('create ioctx', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosIoctx = radosCluster.createIoctx(poolN1);
		});
	});

	it('open radosStream', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosStream = radosIoctx.open(oid);
		});
	});


	it('write & seek & read & readAll', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosStream.write("write test");
		});
		assert.equal(radosStream.read(), null);

		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "write test");
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.readAll().toString(), "write test");
	});

	it('create snapshot', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosIoctx.createSnap(snapshot);
		});
	});

	it('append', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosStream.append("write test");
		});
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "write testwrite test");
	});

	it('rollback snapshot', () => {
		if (win) return;
		assert.doesNotThrow(( => {
			radosIoctx.rollbackSnap(oid, snapshot);
		}))
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "write test");
	});

	it('remove snapshot', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosIoctx.removeSnap(snapshot);
		});
		assert.throws(() => {
			radosIoctx.rollbackSnap(oid, snapshot);
		})
	});

	it('writeFull & seek & tell & rewind', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosStream.writeFull("writeFull test");
		});
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "writeFull test");
		assert.equal(radosStream.tell(), 14);
		radosStream.rewind();
		assert.equal(radosStream.tell(), 0);
	});

	it('size & stat', () => {
		if (win) return;
		assert.equal(radosStream.size(), 14);
		assert.equal(radosStream.stat().size, 14);
	});

	it('truncate', () => {
		if (win) return;
		radosStream.truncate(12);
		assert.equal(radosStream.size(), 12);
		radosStream.truncate(16);
		assert.equal(radosStream.size(), 16);
	});

	it('copyTo & close RadosStream', () => {
		if (win) return;
		var oid1 = "foo1";
		var stm = radosIoctx.open(oid1);
		radosStream.seek(0, fs.SEEK_SET);
		radosStream.copyTo(stm);
		stm.seek(0, fs.SEEK_SET);
		assert.equal(stm.size(), 16);
		assert.equal(stm.read().toString(), "writeFull te");
		assert.doesNotThrow(() => {
			stm.close();
		})
		assert.throws(() => {
			stm.read();
		})
	});

	it('flush', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosStream.flush();
		})
	});

	it('listOids', () => {
		if (win) return;
		var oid1 = "foo1";
		assert.ok(radosIoctx.listOids().indexOf(oid) > -1);
		assert.ok(radosIoctx.listOids().indexOf(oid1) > -1);
	});

	it('listOids with regexp', () => {
		if (win) return;
		var oid1 = "foo1";
		var reg = re.compile("foo\/d");
		assert.ok(radosIoctx.listOids(reg).indexOf(oid) == -1);
		assert.ok(radosIoctx.listOids(reg).indexOf(oid1) > -1);
	});

	it('remove oid', () => {
		if (win) return;
		var oid1 = "foo1";
		assert.doesNotThrow(() => {
			radosIoctx.remove(oid1);
		})
		assert.ok(radosIoctx.listOids().indexOf(oid1) == -1);
	});

	it('set xattr', () => {
		if (win) return;
		var key = "a";
		var val = "b";
		assert.doesNotThrow(() => {
			radosIoctx.setXattr(oid, key, key, val);
		})
	});

	it('get xattr', () => {
		if (win) return;
		var key = "a";
		var val = "b";
		assert.equal(radosIoctx.getXattr(oid, key), val);
	});

	it('rm xattr', () => {
		if (win) return;
		var key = "a";
		assert.doesNotThrow(() => {
			radosIoctx.rmXattr(oid, key);
		});
		assert.throws({
			radosIoctx.getXattr(oid, key);
		});
	});

	it('get attrs', () => {
		if (win) return;
		radosIoctx.setXattr(oid, 'a', 'A');
		radosIoctx.setXattr(oid, 'b', 'B');
		assert.deepEqual(radosIoctx.getXattrs(), {
			a: 'A',
			b: 'B'
		})
	});

	it('destroy ioctx', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosIoctx.destroy();
		})
	});

	it('shutdown cluster', () => {
		if (win) return;
		assert.doesNotThrow(() => {
			radosCluster.shutdown();
		})
	});
});

// test.run(console.DEBUG);