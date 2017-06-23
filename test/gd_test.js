var test = require("test");
test.setup();

var gd = require('gd');
var fs = require('fs');

var vmid = require('coroutine').vmid;

describe("gd", () => {
    var img = gd.create(440, 240);

    var w = img.colorAllocate(0xffff);
    var blk = img.colorAllocate(0, 0, 0);
    var r = img.colorAllocate(255, 0, 0);
    var g = img.colorAllocate(0, 255, 0);
    var b = img.colorAllocate(0, 0, 255);

    function gd_test(fmt) {
        img.save(__dirname + '/test.pic' + vmid, fmt);
        var img1 = gd.load(__dirname + '/test.pic' + vmid);

        fs.unlink(__dirname + '/test.pic' + vmid);

        img1 = gd.load(img.getData(fmt));

        assert.equal(img1.width, 440);
        assert.equal(img1.height, 240);
        assert.equal(img1.format, fmt);
    }

    function sample_test(img, colors) {
        var points = [
            [30, 32],
            [366, 120],
            [364, 183]
        ]
        for (var i = 0; i < points.length; i++)
            assert.equal(img.getTrueColorPixel(points[i][0], points[i][1]), colors[i]);
    }

    it("rgb color", () => {
        assert.equal(gd.rgb(255, 0, 0), 0xff0000);
        assert.equal(gd.rgb(0, 255, 0), 0x00ff00);
        assert.equal(gd.rgb(0, 0, 255), 0x0000ff);

        assert.equal(gd.rgba(0, 255, 0, .5), 0x4000ff00);
    });

    it("hsl color", () => {
        assert.equal(gd.hsl(0, 1, .5), 0xff0000);
        assert.equal(gd.hsl(120, 1, .5), 0x00ff00);
        assert.equal(gd.hsl(240, 1, .5), 0x0000ff);

        assert.equal(gd.hsl(0, 0, .8), 0xcccccc);
        assert.equal(gd.hsl(210, .5, .6), 0x6699cc);

        assert.equal(gd.hsla(120, 1, .5, .5), 0x4000ff00);
    });

    it("hsb color", () => {
        assert.equal(gd.hsb(0, 1, .5), 0x800000);
        assert.equal(gd.hsb(120, 1, .5), 0x8000);
        assert.equal(gd.hsb(240, 1, .5), 0x000080);

        assert.equal(gd.hsb(0, 0, .8), 0xcccccc);
        assert.equal(gd.hsb(210, .5, .6), 0x264d99);

        assert.equal(gd.hsba(120, 1, .5, .5), 0x40008000);
    });

    it("color string", () => {
        assert.equal(gd.color("ff0000"), 0xff0000);
        assert.equal(gd.color("00ff00"), 0x00ff00);
        assert.equal(gd.color("0000ff"), 0x0000ff);

        assert.equal(gd.color("f00"), 0xff0000);
        assert.equal(gd.color("0f0"), 0x00ff00);
        assert.equal(gd.color("00f"), 0x0000ff);

        assert.equal(gd.color("#ff0000"), 0xff0000);
    });

    it("pixel", () => {
        img.setPixel(10, 10, w);
        assert.equal(img.getPixel(10, 10), w);
    });

    it("line", () => {
        img.line(10, 20, 30, 40, b);
        assert.equal(img.getPixel(11, 21), b);
    });

    it("rectangle", () => {
        img.rectangle(40, 40, 50, 50, r);
        img.filledRectangle(60, 40, 70, 50, img.colorAllocate(255, 255, 0));

        img.filledRectangle(20, 20, 70, 50, img.colorAllocateAlpha(0x3fffffff));
    });

    it("transparent", () => {
        img.transparent = blk;
        assert.equal(img.transparent, blk);
    });

    it("thickness", () => {
        img.setThickness(3);
        img.rectangle(60, 60, 70, 70, r);
    });

    it("polygon", () => {
        img.setThickness(1);
        img.openPolygon([
            [1, 1],
            [1, 10],
            [10, 15],
            [10, 20]
        ], b);
        img.polygon([
                [101, 101],
                [101, 110],
                [110, 115],
                [110, 100]
            ],
            b);
        img.filledPolygon(
            [
                [101, 1],
                [101, 10],
                [110, 15],
                [110, 10]
            ], b);
    });

    it("ellipse", () => {
        img.ellipse(80, 10, 30, 50, b);
        img.filledEllipse(80, 100, 30, 50, b);
    });

    it("arc", () => {
        img.arc(20, 70, 80, 60, 45, 90, b);
        img.filledArc(20, 140, 80, 60, 45, 90, b);
        img.filledArc(50, 140, 80, 60, 45, 90, b, gd.CHORD);
        img.filledArc(80, 140, 80, 60, 45, 90, b, gd.NOFILL);
        img.filledArc(110, 140, 80, 60, 45, 90, b, gd.EDGED);
        img.filledArc(140, 140, 80, 60, 45, 90, b, gd.EDGED | gd.NOFILL);
    });

    it("fill", () => {
        img.rectangle(70, 10, 120, 60, r);
        img.fill(80, 40, b);
    });

    it("fillToBorder", () => {
        img.rectangle(5, 30, 50, 80, r);
        img.fillToBorder(10, 40, r, b);
    });

    it("png", () => {
        gd_test(gd.PNG);
    });

    it("jpeg", () => {
        gd_test(gd.JPEG);
    });

    it("gif", () => {
        gd_test(gd.GIF);
    });

    it("tiff", () => {
        gd_test(gd.TIFF);
    });

    it("bmp", () => {
        gd_test(gd.BMP);
    });

    it("webp", () => {
        gd_test(gd.WEBP);
    });

    it("flip", () => {
        var img1 = img.resample(220, 120);
        img1.flip();
    });

    it("progressive", () => {
        var img = gd.create(100, 100);

        var data = img.getData(gd.JPEG);
        var img1 = gd.load(data);

        assert.equal(img1.format, gd.JPEG);
        assert.equal(img1.progressive, false);

        img.progressive = true;
        var data1 = img.getData(gd.JPEG);
        var img2 = gd.load(data1);

        assert.equal(img2.format, gd.JPEG);
        assert.equal(img2.progressive, true);
    });

    it("filter", () => {
        img = gd.load(__dirname + '/test.png');
        img.filter(gd.MEAN_REMOVAL);
        sample_test(img, [0xffffff, 0xff00, 0xffffff])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.EDGEDETECT);
        sample_test(img, [0x7fffff, 0x7f00, 0xff7fff])

        var img = gd.load(__dirname + '/test.png');
        img.filter(gd.EMBOSS);
        sample_test(img, [0x7fffff, 0xff7fff, 0x7f00])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.SELECTIVE_BLUR);
        sample_test(img, [0xfefdfd, 0xfe00, 0xd4fed4])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.GAUSSIAN_BLUR);
        sample_test(img, [0xffe9e9, 0x3dff3d, 0xa6ffa6])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.NEGATE);
        sample_test(img, [0x0, 0xff00ff, 0x2a002a])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.GRAYSCALE);
        sample_test(img, [0xffffff, 0x959595, 0xededed])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.SMOOTH, 10);
        sample_test(img, [0xffefef, 0x25ff25, 0xb8ffb8])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.BRIGHTNESS, 10);
        sample_test(img, [0xffffff, 0xaff0a, 0xdfffdf])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.CONTRAST, 10);
        sample_test(img, [0xe6e6e6, 0x18e618, 0xc4e6c4])

        img = gd.load(__dirname + '/test.png');
        img.filter(gd.COLORIZE, 10, 10, 10, 10);
        sample_test(img, [0xffffff, 0x9ff09, 0xdeffde])
    });

    it("affine", () => {
        var img = gd.load(__dirname + '/test.png');
        var affines = [2, 1, 1, 2, 1, 1];
        var img1 = img.affine(affines);
        assert.equal(img1.width, 1117);
        assert.equal(img1.height, 1126);
        sample_test(img1, [0xffffff, 0x7fffffff, 0xfefefe]);

        var img2 = img.affine(affines, 50, 50, 50, 50);
        assert.equal(img2.width, 151);
        assert.equal(img2.height, 151);
        sample_test(img2, [0xff0000, 0x0, 0x0]);
    });
});

repl && test.run(console.DEBUG);