exports.rmFile = function (target) {
    try {
        fs.unlink(target)
        return true;
    } catch (error) {
        return false;
    }
}