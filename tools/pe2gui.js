var fs = require("fs");

console.log("convert windows console app to gui mode.");

var DF_DosMagic = new Buffer("MZ");
var DF_PEMagic = new Buffer([0x50, 0x45, 0, 0]);

// offset of IMAGE_DOS_HEADER.e_lfanew
const DF_NewHeaderOffset = 0x3C;

// 4("PE00") + 20(sizeof(IMAGE_FILE_HEADER))
// + offset of IMAGE_OPTIONAL_HEADER.Subsystem
const DF_SubsytemOffset = 0x5C;

function IsCorrectExeFile(f) {
	// check "MZ"
	var buf = f.read(DF_DosMagic.length);
	if (buf === null || !buf.equals(DF_DosMagic))
		return false;

	f.seek(DF_NewHeaderOffset, fs.SEEK_SET);

	buf = f.read(4);
	var offset = buf.readUInt32LE();

	f.seek(offset, fs.SEEK_SET);

	// check "PE\0\0"
	var buf = f.read(DF_PEMagic.length);
	if (buf === null || !buf.equals(DF_PEMagic))
		return false;

	return true;
}

function SetSubsystem(f, bConsole) {

	f.seek(DF_NewHeaderOffset, fs.SEEK_SET);

	var buf = f.read(4);
	var offset = buf.readUInt32LE();

	f.seek(offset + DF_SubsytemOffset, fs.SEEK_SET);

	f.write(new Buffer([bConsole ? 3 : 2, 0]));
}

var f = fs.open(argv[2], 'r+');

if (!IsCorrectExeFile(f))
	console.error("not execute file");
else SetSubsystem(f, false);