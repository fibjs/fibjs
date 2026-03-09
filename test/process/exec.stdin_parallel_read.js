// Child process: read stdin using 10 parallel read(10) calls, collect all, then write once
var parts = new Array(10);
var count = 0;

function read_(index) {
    process.stdin.read(10, (err, data) => {
        parts[index] = data.toString();
        count++;
        if (count == 10)
            process.stdout.write(parts.join(''));
    });
}

for (var i = 0; i < 10; i++)
    read_(i);
