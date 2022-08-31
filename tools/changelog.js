var fs = require('fs');
var path = require('path');
var io = require('io');
var child_process = require('child_process');

const changelog = path.resolve(__dirname, '../CHANGELOG.md');
process.on('exit', () => {
    if (allOutputs.length) {
        child_process.run('git', 'checkout HEAD -- CHANGELOG.md'.split(' '))

        let content = fs.readFile(changelog);
        const d = new Date();
        const dateStr = `${d.getFullYear()}-${(d.getMonth() + 1 + '').padStart(2, '0')}-${(d.getDate() + '').padStart(2, '0')}`;
        const versionStr = '0.xx.0';
        content = [
            `## ${dateStr}, Version ${versionStr}`,
            `${allOutputs.join('\n')}\n`,
            content
        ].join('\n');
        fs.writeFile(changelog, content);
    }
})
let allOutputs = [];
function logToOutput(...msg) {
    if (process.env.CHANGELOG) {
        allOutputs = allOutputs.concat(msg.join(' '));
    }
    console.log(...msg);
}

process.chdir(path.join(__dirname, '..'));

function pick_subprocess_out_lines(popen_r) {
    var stdout = new io.BufferedStream(popen_r.stdout);

    return stdout.readLines().map(line => line.trimEnd());
}

var hash = pick_subprocess_out_lines(
    child_process.spawn('git', 'rev-list --tags=v[0-9]*.[0-9]*.[0-9]* --max-count=1'.split(' '))
)[0]
var describe = pick_subprocess_out_lines(child_process.spawn('git', ['describe', '--match', 'v[0-9]*.[0-9]*.[0-9]*', '--tags']));
var info = (/^(v[\d\.]+)-(\d+)-g([a-f\d]+)/g).exec(describe);

var logs = pick_subprocess_out_lines(
    child_process.spawn('git', [
        'log',
        '--pretty=format:%H-%s(%an)',
        '-' + (new Number(info[2] - 1))
    ])
);
if (process.platform === 'win32') {
    logs = logs.reduce((accu, line) => accu.concat(line.split('\n')), []);
}

var commits = [];
var changes = {};
var alias = {
    'feat': 'feature',
    'break': 'breakchange',
    'fixbug': 'bugfix'
};

logs.forEach(log => {
    if (!log) {
        throw new Error('[changelog] invalid log: ' + log);
    }
    var log_info = /^([a-f\d]+)-\s*(([^\,\:]*)(\s*,\s*([^\:]*))?\s*:)?\s*(.*)$/.exec(log);
    if (!log_info) {
        throw new Error('[changelog] parse log failed: ' + log);
    }

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
        var k = log_info[5];
        if (alias[k])
            k = alias[k];
        group = changes[k];
        if (group === undefined)
            group = changes[k] = {};
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

function out_changes(key) {
    var group = changes[key];

    if (group === undefined)
        return;

    logToOutput(`* **${key}** :`);

    var keys = Object.keys(group);

    keys.sort((a, b) => {
        return a.toLowerCase() > b.toLowerCase() ? 1 : -1;
    });

    keys.forEach(k => {
        var chgs = group[k];
        if (k === '~') {
            logToOutput(`    * ` + chgs.join('\n    * '));
        } else {
            if (chgs.length === 1)
                logToOutput(`    * ${k} - ${chgs[0]}`);
            else
                logToOutput(`    * ${k}:\n      - ` + chgs.join('\n      - '));
        }
    });

    logToOutput();

    delete changes[key];
}

out_changes('feature');
out_changes('breakchange');
out_changes('bugfix');
out_changes('refactor');

for (var k in changes)
    out_changes(k);

logToOutput("### Commits");
logToOutput(commits.join('\n'));