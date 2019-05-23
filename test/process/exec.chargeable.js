var io = require('io')

var is = new io.BufferedStream(process.stdin)

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