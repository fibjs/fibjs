const envKeys = require('./const.env_keys');

if (process.platform === 'win32') {
    envKeys.win32.forEach(key => {
        console.log(`process.env['${key}']=${process.env[key] || ''}`)
    })
}

if (process.platform === 'darwin') {
    console.log(`process.env.HOME=${process.env.HOME || ''}`)
    console.log(`process.env.TMPDIR=${process.env.TMPDIR || ''}`)
}

if (process.platform === 'linux') {
    console.log(`process.env.HOME=${process.env.HOME || ''}`)
    console.log(`process.env.TMPDIR=${process.env.TMPDIR || ''}`)
}