const fs = require('fs')
const path = require('path')
const crypto = require('crypto')

const MAX_SZ = 5 * 1024 * 1024;
/**
 * just one file
 */
exports.generateFakeMp4 = () => {
    const target = path.resolve(__dirname, './fake.mp4')
    const file = fs.openFile(target, "w")

    while (file.size() < MAX_SZ) {
        file.write(crypto.randomBytes(5 * 1024))
    }

    return { target }
}