var str = "";

function read_(n) {
    process.stdin.read(10, (err, data) => {
        str += data.toString();
        if (str.length == 100)
            process.stdout.write(str);
    });
}

for (var i = 0; i < 10; i++)
    read_(i);