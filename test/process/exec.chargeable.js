var io = require('io')

var is = new io.BufferedStream(process.stdin)

/**
 * @why in platform win32, EOL may be one of [`\r\n`, `\n`];
 * 
 * @todo normalize behavior about `EOL` of
 *       BufferesStream **from stdio** in the future
 */
if (process.platform === 'win32') {
    is.EOL = '\n';
}

var input = null
while (true) {
    input = is.readLine()
    if (input === '.exit') {
        console.log('exit')
        break
    }

    if (input)
        console.log(`> your input is: ${input}`)
    else
        console.log(`> you input nothing`)
}