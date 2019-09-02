/**
 * @description run this script to generate env keys for your device.
 */
const fs = require('fs')
const path = require('path')

const keys = Object.keys(process.env)

fs.writeTextFile(
    path.resolve(__dirname, `process_env_keys.${process.platform}.json`),
    JSON.stringify(keys, null, '\t')
)