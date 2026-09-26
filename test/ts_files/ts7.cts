// Parameter properties cannot be erased: they have to be lowered into
// `this.x = x;` at the top of the constructor, which makes the stripped code
// longer than the source. Loading this file twice in one process has to give the
// same result both times.
class Counter {
    constructor(private readonly start: number, public step: number = 2) { }

    sum(): number {
        return this.start + this.step;
    }
}

exports.sum = new Counter(3).sum();
