const assert = require('assert')

exports.assert_error_msg = (fn, msgOrErr) => {
    let msg = msgOrErr instanceof Error ? msgOrErr.message : msgOrErr
    let errored = false

    try {
        fn.apply(null)
    } catch (error) {
        errored = !!error;
        assert.strictEqual(error.message, msg)
    }

    assert.isTrue(errored)

    return errored;
}