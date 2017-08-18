var path = require('path');

process.chdir(path.join(__dirname, '..'));

var describe = process.open('git', ['describe']).readLines();
var info = (/^(v[\d\.]+)-(\d+)-g([a-f\d]+)/g).exec(describe);

var logs = process.open('git', [
    'log',
    '--pretty=format:%H-%s(%an)',
    '-' + (new Number(info[2] - 1))
]).readLines();

var commits = [];
var changes = {};

logs.forEach(log => {
    var log_info = /^([a-f\d]+)-\s*(([^\,\:]*)(\s*,\s*([^\:]*))?\s*:)?\s*(.*)$/.exec(log);

    var hash = log_info[1].substr(0, 10);
    var module = log_info[3];
    var type = log_info[5];
    var type = '';

    if (log_info[3]) {
        if (log_info[5])
            type = `**${log_info[3]}, ${log_info[5]}**: `;
        else
            type = `**${log_info[3]}**: `;
    } else
        log_info[3] = '~';

    var group;
    if (log_info[5]) {
        group = changes[log_info[5]];
        if (group === undefined)
            group = changes[log_info[5]] = {};
    } else {
        group = changes['others'];
        if (group === undefined)
            group = changes['others'] = {};
    }

    var module = group[log_info[3]];
    if (module === undefined)
        module = group[log_info[3]] = [];

    module.push(log_info[6]);

    commits.push(`* [[\`${hash}\`](https://github.com/fibjs/fibjs/commit/${hash})] - ${type}${log_info[6]}`);
});


function out_changes(key, name) {
    if (name === undefined)
        name = key;

    var group = changes[key];

    if (group === undefined)
        return;

    console.log(`* **${name}** :`);

    var keys = Object.keys(group);

    keys.sort((a, b) => {
        return a.toLowerCase() > b.toLowerCase() ? 1 : -1;
    });

    keys.forEach(k => {
        var chgs = group[k];
        if (k === '~') {
            console.log(`    * ` + chgs.join('\n    * '));
        } else {
            if (chgs.length === 1)
                console.log(`    * ${k} - ${chgs[0]}`);
            else
                console.log(`    * ${k}:\n      - ` + chgs.join('\n      - '));
        }
    });

    console.log();

    delete changes[key];
}

out_changes('feat', 'feature');
out_changes('break', 'breakchange');
out_changes('bugfix');
out_changes('refactor');

for (var k in changes)
    out_changes(k, k);

console.log("### Commits");
console.log(commits.join('\n'));