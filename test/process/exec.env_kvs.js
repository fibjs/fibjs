const assert = require('assert');

if (process.platform === 'win32') {
    [
        "SYSTEMROOT",
        "SystemRoot",
        "TEMP",
        "TMP",
        // "CommonProgramFiles",
        "CommonProgramFiles(x86)",
        "CommonProgramW6432",
        // "ProgramFiles",
        "ProgramFiles(x86)",
        "ProgramW6432",
    ].forEach(key => {
        console.log(`process.env['${key}']=${process.env[key]}`)
    })
}

if (process.platform === 'darwin') {
    console.log(`process.env.HOME=${process.env.HOME}`)
    console.log(`process.env.TMPDIR=${process.env.TMPDIR}`)
}

if (process.platform === 'linux') {
    console.log(`process.env.HOME=${process.env.HOME}`)
    console.log(`process.env.TMPDIR=${process.env.TMPDIR}`)
}