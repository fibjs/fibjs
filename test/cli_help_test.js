// fibjs CLI help convention regression tests (plans/cli-help-convention.md).
//
// Spawns the binary and asserts the convention:
//   R1  `fibjs --help` lists the commands one line each, no command options
//   R2  `fibjs --<command> --help` / `-h` -> stdout, exit 0, no side effects
//   R3  argument errors -> stderr, exit 1, the same usage text
//   R5  option descriptions start at offset 30
//   R7  every embedded opt tool appears in the Commands section
//
// Everything is spawned through process.execPath, so the suite is fibjs only
// (under node the same arguments would run node itself).

var { describe, it, after } = require('node:test');
var assert = require('assert');
var path = require('path');
var fs = require('fs');
var os = require('os');
var child_process = require('child_process');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

// Keep in sync with plans/cli-help-convention.md §5 and with the Commands
// section of fibjs/src/base/options.cpp (R7).
const COMMANDS = ['init', 'install', 'test', 'check', 'cov-process', 'prof-process'];

// Options that belong to a command: none of them may be inlined in the top
// level help (R1).
const COMMAND_OPTIONS = [
    '--frozen-lockfile',
    '--omit=dev',
    '--test-name-pattern',
    '--test-reporter',
    '--allowJs',
    '--dry-run'
];

const COMMAND_POINTER = 'Run `fibjs --<command> --help` for the options of a command.';
const GLOBAL_POINTER = 'Run `fibjs --help` for the global options.';
const OPT_TOOLS_CPP = path.join(__dirname, '..', 'fibjs', 'src', 'base', 'opt_tools.cpp');

// fibjs console.error colours its output, even into a pipe
const ANSI_RE = /\u001b\[[0-9;]*m/g;

function stripAnsi(s) {
    return s.replace(ANSI_RE, '');
}

function run(args, cwd) {
    var r = child_process.spawnSync(process.execPath, args, {
        encoding: 'utf8',
        cwd: cwd || __dirname,
        input: ''
    });

    return { code: r.status, stdout: r.stdout || '', stderr: r.stderr || '' };
}

// `fibjs --<command> --help` as the convention defines it, with the checks
// every command must satisfy (R2).
function usageOf(command) {
    var args = command ? ['--' + command, '--help'] : ['--help'];
    var r = run(args);

    assert.equal(r.code, 0, `${args.join(' ')}: exit code ${r.code}, stderr: ${r.stderr}`);
    assert.equal(r.stderr, '', `${args.join(' ')}: help must not write to stderr`);
    assert.ok(r.stdout.length > 0, `${args.join(' ')}: help must write to stdout`);

    return stripAnsi(r.stdout);
}

// Lines whose description does not start at offset 30 (R5). A name longer than
// 26 columns carries no description on its own line.
function misalignedLines(text, label) {
    var bad = [];

    text.split('\n').forEach((line, i) => {
        if (!/^ {2}[-a-zA-Z]/.test(line))
            return;

        var m = / {2,}/.exec(line.slice(2));
        if (m && 2 + m.index + m[0].length !== 30)
            bad.push(`${label}:${i + 1}: description at offset ` +
                `${2 + m.index + m[0].length}: ${JSON.stringify(line.slice(0, 60))}`);
    });

    return bad;
}

describe('cli help convention', { skip: !isFibjs }, () => {
    var scratch;

    function scratchDir() {
        if (scratch === undefined) {
            scratch = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-cli-help-'));
        }
        return scratch;
    }

    after(() => {
        if (scratch !== undefined)
            fs.rmSync(scratch, { recursive: true, force: true });
    });

    describe('R1: the top level help lists commands, not their options', () => {
        it('--help and -h are the same text, on stdout, exit 0', () => {
            var long = usageOf(undefined);
            var short = run(['-h']);

            assert.equal(short.code, 0, short.stderr);
            assert.equal(short.stderr, '');
            assert.equal(stripAnsi(short.stdout), long);
        });

        it('has an Options section, a Commands section and the pointer', () => {
            var help = usageOf(undefined);

            assert.ok(help.indexOf('\nOptions:\n') >= 0, 'Options section is missing');
            assert.ok(help.indexOf('\nCommands:\n') >= 0, 'Commands section is missing');
            assert.ok(help.indexOf(COMMAND_POINTER) >= 0, 'command pointer line is missing');
            assert.ok(help.indexOf('Usage: fibjs') === 0, 'help must start with the usage line');
        });

        it('lists every command of the convention', () => {
            var help = usageOf(undefined);

            COMMANDS.forEach(c => {
                var re = new RegExp('^  --' + c + '( |$)', 'm');
                assert.ok(re.test(help), `--${c} is not listed in the Commands section`);
            });
        });

        it('inlines no command option', () => {
            var help = usageOf(undefined);

            COMMAND_OPTIONS.forEach(opt => {
                assert.ok(help.indexOf(opt) < 0,
                    `the top level help inlines ${opt}, it belongs to a command`);
            });
        });

        it('lists every opt tool of the embedded table (R7)', () => {
            if (!fs.existsSync(OPT_TOOLS_CPP)) {
                console.log('note: ' + OPT_TOOLS_CPP +
                    ' not found, the embedded opt tool table was not cross checked');
                return;
            }

            var src = fs.readFileSync(OPT_TOOLS_CPP, 'utf8');
            var re = /\{"opt_tools\/([a-z0-9-]+)"/g;
            var help = usageOf(undefined);
            var names = [];
            var m;

            while ((m = re.exec(src)) !== null)
                names.push(m[1]);

            assert.ok(names.length > 0, 'no opt_tools entry found in ' + OPT_TOOLS_CPP);

            names.forEach(name => {
                assert.ok(help.indexOf('  --' + name + ' ') >= 0,
                    `opt tool --${name} is embedded but not listed in the top level help`);
            });
        });
    });

    describe('R2: every command has its own help', () => {
        COMMANDS.forEach(command => {
            it(`--${command} --help: stdout, exit 0, stderr empty`, () => {
                var usage = usageOf(command);

                assert.ok(usage.indexOf(`Usage: fibjs --${command}`) === 0,
                    `usage must start with "Usage: fibjs --${command}"`);
                assert.ok(usage.indexOf(GLOBAL_POINTER) >= 0,
                    `--${command} help must point at the global options`);
            });

            it(`--${command} -h is the same as --help`, () => {
                var short = run(['--' + command, '-h']);

                assert.equal(short.code, 0, short.stderr);
                assert.equal(short.stderr, '');
                assert.equal(stripAnsi(short.stdout), usageOf(command));
            });

            it(`--${command} --help writes nothing to disk`, () => {
                var dir = scratchDir();
                var before = fs.readdirSync(dir);
                var r = run(['--' + command, '--help'], dir);

                assert.equal(r.code, 0, r.stderr);
                assert.deepEqual(fs.readdirSync(dir), before,
                    `--${command} --help must not create files in the working directory`);
            });
        });

        it('help wins over the other arguments', () => {
            var install = run(['--install', '--save', '--help']);
            assert.equal(install.code, 0, install.stderr);
            assert.equal(stripAnsi(install.stdout), usageOf('install'));

            var test = run(['--test', '--test-name-pattern=x', '--help']);
            assert.equal(test.code, 0, test.stderr);
            assert.equal(stripAnsi(test.stdout), usageOf('test'));
        });

        it('-c is an alias of --check and reaches the same help', () => {
            var r = run(['-c', '--help']);

            assert.equal(r.code, 0, r.stderr);
            assert.equal(r.stderr, '');
            assert.equal(stripAnsi(r.stdout), usageOf('check'));
        });

        it('--check documents the fibjs wrapper before the compiler options', () => {
            var usage = usageOf('check');

            assert.ok(usage.indexOf('fibjs differences from tsc') > 0,
                'the fibjs wrapper notes are missing');
            assert.ok(usage.indexOf('tsc: The TypeScript Compiler') > 0,
                'the compiler option list is missing');
            assert.ok(usage.indexOf('fibjs differences from tsc') <
                usage.indexOf('tsc: The TypeScript Compiler'),
                'the fibjs wrapper notes must come before the compiler options');
        });
    });

    describe('R3: argument errors print the same usage on stderr', () => {
        // the usage as the error path prints it: reason line, then the usage
        function errorUsage(args) {
            var r = run(args);

            assert.equal(r.code, 1, `${args.join(' ')}: expected exit code 1, got ${r.code}`);
            assert.equal(r.stdout, '', `${args.join(' ')}: an error must not write to stdout`);
            assert.ok(r.stderr.length > 0, `${args.join(' ')}: an error must write to stderr`);

            var lines = stripAnsi(r.stderr).split('\n');
            lines.shift(); // the reason line
            return lines.join('\n').replace(/\n+$/, '\n');
        }

        it('--install with an unknown option', () => {
            assert.equal(errorUsage(['--install', '--bogus']), usageOf('install'));
        });

        it('--install with a missing value', () => {
            assert.equal(errorUsage(['--install', '--target']), usageOf('install'));
        });

        it('--init with an unknown option', () => {
            assert.equal(errorUsage(['--init', '--bogus']), usageOf('init'));
        });

        it('--cov-process without arguments', () => {
            assert.equal(errorUsage(['--cov-process']), usageOf('cov-process'));
        });

        it('--prof-process without arguments', () => {
            assert.equal(errorUsage(['--prof-process']), usageOf('prof-process'));
        });
    });

    describe('R5: option descriptions are aligned', () => {
        it('the top level help and every command help use the same column', () => {
            var bad = misalignedLines(usageOf(undefined), 'top');

            COMMANDS.forEach(command => {
                var usage = usageOf(command);

                // the compiler's own option list keeps its own layout
                if (command === 'check')
                    usage = usage.split('\ntsc: ')[0];

                bad = bad.concat(misalignedLines(usage, command));
            });

            assert.deepEqual(bad, [], 'misaligned descriptions:\n' + bad.join('\n'));
        });
    });
});
