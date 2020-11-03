console.log('process.env', process.env);

if (process.platform === 'win32') {
    console.log(`process.env['ProgramFiles(x86)']`, process.env['ProgramFiles(x86)'])
}