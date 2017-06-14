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

describe('rados', () => {
	it('create cluster', () => {
		if (win) return;
		try {
			radosCluster = new rados.Rados(ceph_cluster, ceph_username, ceph_config);
		} catch (e) {
			radosCluster = null;
			console.error(e.toString());
		}
	});

	it('connect', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosCluster.connect();
		});
	});

	it('create pool', () => {
		if (win) return;
		if (!radosCluster) return;
		try {
			radosCluster.deletePool(poolN);
		} catch (e) {
			console.error(e.toString());
		}
		assert.doesNotThrow(() => {
			radosCluster.createPool(poolN);
		})
	});

	it('list pool', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(radosCluster.listPool().indexOf(poolN) > -1);
	});

	it('delete pool', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosCluster.deletePool(poolN);
		});
		assert.ok(radosCluster.listPool().indexOf(poolN) == -1);
	});

	it('create ioctx', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosCluster.createPool(poolN);
			radosIoctx = radosCluster.createIoCtx(poolN);
		});
	});

	it('open radosStream', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosStream = radosIoctx.open(oid);
		});
	});

	it('write', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosStream.write("write test");
		});
	});

	it('seek & read', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.equal(radosStream.read(), null);
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "write test");

	});

	it('seek & readAll', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.equal(radosStream.read(), null);
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.readAll().toString(), "write test");
	});

	it('create snapshot', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosIoctx.createSnap(snapshot);
		});
	});

	it('append', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosStream.append("write test");
		});
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "write testwrite test");
	});

	it('rollback snapshot', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosIoctx.rollbackSnap(oid, snapshot);
		})
		radosStream.seek(0, fs.SEEK_SET);
		assert.equal(radosStream.read().toString(), "write test");
	});

	it('remove snapshot', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosIoctx.removeSnap(snapshot);
		});
		assert.throws(() => {
			radosIoctx.rollbackSnap(oid, snapshot);
		})
	});

	it('writeFull & seek & tell & rewind', () => {
		if (win) return;
		if (!radosCluster) return;
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
		if (!radosCluster) return;
		assert.equal(radosStream.size(), 14);
		assert.equal(radosStream.stat().size, 14);
	});

	it('truncate', () => {
		if (win) return;
		if (!radosCluster) return;
		radosStream.truncate(12);
		assert.equal(radosStream.size(), 12);
		radosStream.truncate(16);
		assert.equal(radosStream.size(), 16);
	});

	it('copyTo & close RadosStream', () => {
		if (win) return;
		if (!radosCluster) return;
		var oid1 = "foo1";
		var stm = radosIoctx.open(oid1);
		radosStream.seek(0, fs.SEEK_SET);
		radosStream.copyTo(stm);
		stm.seek(0, fs.SEEK_SET);
		assert.equal(stm.size(), 16);
		assert.equal(stm.read().toString(), "writeFull te\u0000\u0000\u0000\u0000");
		assert.doesNotThrow(() => {
			stm.close();
		})
		assert.throws(() => {
			stm.read();
		})
	});

	it('flush', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosStream.flush();
		})
	});

	it('listOids', () => {
		if (win) return;
		if (!radosCluster) return;
		var oid1 = "foo1";
		assert.ok(radosIoctx.listOids().indexOf(oid) > -1);
		assert.ok(radosIoctx.listOids().indexOf(oid1) > -1);
	});

	it('listOids with regexp', () => {
		if (win) return;
		if (!radosCluster) return;
		var oid1 = "foo1";
		var reg = re.compile("foo\\d");
		assert.ok(radosIoctx.listOids(reg).indexOf(oid) == -1);
		assert.ok(radosIoctx.listOids(reg).indexOf(oid1) > -1);
	});

	it('set xattr', () => {
		if (win) return;
		if (!radosCluster) return;
		var key = "a";
		var val = "b";
		assert.doesNotThrow(() => {
			radosIoctx.setXattr(oid, key, val);
		})
	});

	it('get xattr', () => {
		if (win) return;
		if (!radosCluster) return;
		var key = "a";
		var val = "b";
		assert.equal(radosIoctx.getXattr(oid, key), val);
	});

	it('rm xattr', () => {
		if (win) return;
		if (!radosCluster) return;
		var key = "a";
		assert.doesNotThrow(() => {
			radosIoctx.rmXattr(oid, key);
		});
		assert.throws(() => {
			radosIoctx.getXattr(oid, key);
		});
	});

	it('get attrs', () => {
		if (win) return;
		if (!radosCluster) return;
		radosIoctx.setXattr(oid, 'a', 'A');
		radosIoctx.setXattr(oid, 'b', 'B');
		assert.deepEqual(radosIoctx.getXattrs(oid), {
			a: 'A',
			b: 'B'
		})
	});

	it('remove oid', () => {
		if (win) return;
		if (!radosCluster) return;
		var oid1 = "foo1";
		assert.doesNotThrow(() => {
			radosIoctx.remove(oid1);
			radosIoctx.remove(oid);
		})
		assert.ok(radosIoctx.listOids().indexOf(oid1) == -1);
		assert.ok(radosIoctx.listOids().indexOf(oid) == -1);
	});

	it('destroy ioctx', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosIoctx.destroy();
		})
	});

	it('shutdown cluster', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosCluster.shutdown();
		})
	});
});

// test.run(console.DEBUG);