var test = require("test");
test.setup();

var rados = require('rados');
var os = require('os');
var fs = require('fs');

var win = os.type() == "Windows";

var ceph_cluster = 'ceph';
var ceph_username = 'client.admin';
var ceph_config = '/etc/ceph/ceph.conf';

describe('rados', () => {
	var radosCluster, radosIoctx, radosStream;
	var poolN = "radospool";
	var oid = "foo";
	var snapshot = "s";

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
		assert.ok(radosIoctx.listOids("foo\\d").indexOf(oid) == -1);
		assert.ok(radosIoctx.listOids("foo\\d").indexOf(oid1) > -1);
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

describe('rbd', () => {
	var radosCluster, radosIoctx, rbdImage;
	var imgName = "image";
	var imgName2 = "image2";
	var poolName = "rbdpool";
	var poolName2 = "rbdpool2";
	var snap = "snapshot"
	var snap2 = "snapshot2"

	before(() => {
		if (win) return;
		try {
			radosCluster = new rados.Rados(ceph_cluster, ceph_username, ceph_config);
			radosCluster.connect();
			radosCluster.createPool(poolName);
			radosCluster.createPool(poolName2);
			radosIoctx = radosCluster.createIoCtx(poolName);
		} catch (e) {
			if (radosCluster) {
				radosCluster.deletePool(poolName);
				radosCluster.deletePool(poolName2);
			}
			radosCluster = null;
			console.error(e.toString());
		}
	});

	after(() => {
		if (win) return;
		if (!radosCluster) return;
		radosIoctx.destroy();
		radosCluster.deletePool(poolName);
		radosCluster.deletePool(poolName2);
		radosCluster.shutdown();
	});

	it('version', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(/\d+\.\d+\.\d+/.test(radosIoctx.version()));
	});

	it('createImage', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			radosIoctx.createImage(imgName, 1024 * 1024);
		})
		assert.ok(radosIoctx.listImages().indexOf(imgName) > -1)
	});

	it('openImage', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			rbdImage = radosIoctx.openImage(imgName);
		})
	});

	it('createSnap & listSnaps', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.createSnap(snap);
		assert.equal(rbdImage.listSnaps().length, 1);
		assert.equal(rbdImage.listSnaps()[0], snap);
	});

	it('removeSnap', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.removeSnap(snap);
		assert.equal(rbdImage.listSnaps().length, 0);
	});

	it('rollbackSnap', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.write('1234');
		rbdImage.rewind();
		assert.equal(rbdImage.read(4).toString(), '1234');
		rbdImage.createSnap(snap);
		rbdImage.rewind();
		rbdImage.write('4321');
		rbdImage.rewind();
		assert.equal(rbdImage.read(4).toString(), '4321');
		rbdImage.rollbackSnap(snap);
		rbdImage.rewind();
		assert.equal(rbdImage.read(4).toString(), '1234');
	});

	it('protectSnap', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.protectSnap(snap);
		assert.throws(() => {
			rbdImage.removeSnap(snap);
		})
	});

	it('unprotectSnap', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.unprotectSnap(snap);
		assert.doesNotThrow(() => {
			rbdImage.removeSnap(snap);
		})
	});

	it('setSnap', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.rewind();
		rbdImage.write('1234');
		rbdImage.createSnap(snap);
		rbdImage.setSnap(snap);
		rbdImage.rewind();
		assert.throws(() => {
			rbdImage.write("1234");
		})
		rbdImage.setSnap();
		rbdImage.rewind();
		assert.doesNotThrow(() => {
			rbdImage.write("1234")
		})
	});

	it('isSnapProtected', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.protectSnap(snap);
		assert.equal(true, rbdImage.isSnapProtected(snap));
		rbdImage.unprotectSnap(snap);
		assert.equal(false, rbdImage.isSnapProtected(snap));
	});


	it('cloneImage', () => {
		if (win) return;
		if (!radosCluster) return;
		var ctx = radosCluster.createIoCtx(poolName2);
		rbdImage.protectSnap(snap);
		assert.doesNotThrow(() => {
			radosIoctx.cloneImage(imgName, snap, ctx, imgName2);
		})
		assert.equal(ctx.listImages().length, 1);
		assert.equal(ctx.listImages()[0], imgName2);
		ctx.destroy();
	});

	it('removeImage', () => {
		if (win) return;
		if (!radosCluster) return;
		var ctx = radosCluster.createIoCtx(poolName2);
		assert.doesNotThrow(() => {
			ctx.removeImage(imgName2);
		})
		assert.ok(ctx.listImages().length == 0);
		ctx.destroy();
	});

	it('renameImage', () => {
		if (win) return;
		if (!radosCluster) return;
		radosIoctx.renameImage(imgName, imgName2);
		assert.ok(radosIoctx.listImages().indexOf(imgName) < 0);
		assert.ok(radosIoctx.listImages().indexOf(imgName2) > -1);
		radosIoctx.renameImage(imgName2, imgName);
	});

	it('listImages', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.equal(radosIoctx.listImages().length, 1);
	});

	it('write', () => {
		if (win) return;
		if (!radosCluster) return;

		var arr = [];
		for (var i = 0; i < 1024; i++) {
			var k = i % 4;
			arr.push(0x31 + k);
		}
		var buf = new Buffer(arr);
		assert.doesNotThrow(() => {
			rbdImage.write(buf);
		});
	});

	it('seek & read', () => {
		if (win) return;
		if (!radosCluster) return;
		var arr = [];
		for (var i = 0; i < 1024; i++) {
			var k = i % 4;
			arr.push(0x31 + k);
		}
		var buf = new Buffer(arr);
		rbdImage.seek(0, fs.SEEK_SET);

		assert.equal(rbdImage.read(1024).toString(), buf.toString());
	});

	it('size & readAll', () => {
		if (win) return;
		if (!radosCluster) return;
		var size = rbdImage.size();
		rbdImage.seek(-4, fs.SEEK_END);
		rbdImage.write("1234");
		rbdImage.seek(-4, fs.SEEK_END);
		assert.equal(rbdImage.readAll().toString(), "1234");
	});

	it('copyTo', () => {
		if (win) return;
		if (!radosCluster) return;
		var io = require('io');
		var stm = new io.MemoryStream();
		rbdImage.seek(-4, fs.SEEK_END);
		rbdImage.copyTo(stm);
		stm.rewind();
		assert.equal(stm.readAll().toString(), "1234");
	});

	it('tell & rewind', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.seek(0, fs.SEEK_END);
		assert.equal(1024 * 1024, rbdImage.tell());
		rbdImage.rewind();
		assert.equal(0, rbdImage.tell());
	});

	it('stat', () => {
		if (win) return;
		if (!radosCluster) return;
		var stat = rbdImage.stat();
		var size = rbdImage.size();
		assert.equal(size, stat.size);
	});

	it('resize', () => {
		if (win) return;
		if (!radosCluster) return;
		rbdImage.resize(1024 * 2048);
		assert.equal(2048 * 1024, rbdImage.size());
		rbdImage.resize(512 * 1024);
		assert.equal(512 * 1024, rbdImage.size());
	});

	it('get_stripe_unit', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(typeof rbdImage.stripe_unit, "number");
	});

	it('get_stripe_count', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(typeof rbdImage.stripe_count, "number");
	});

	it('get_features', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(typeof rbdImage.features, "number");
	});

	it('get_create_timestamp', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(typeof rbdImage.create_timestamp, "object");
	});

	it('get_block_name_prefix', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.ok(typeof rbdImage.block_name_prefix, "string");
	});

	it('flush', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			rbdImage.flush();
		})
	});

	it('close', () => {
		if (win) return;
		if (!radosCluster) return;
		assert.doesNotThrow(() => {
			rbdImage.close();
		})
	});

});

repl && test.run(console.DEBUG);