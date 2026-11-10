// An ES module that both exports a live binding and declares a default.
//
// The default export is what makes this file interesting: the CJS-interop
// facade only kicks in for modules that have one, so a module without a
// default is never wrapped and never exposes the regression these tests
// pin down.
export let counter = 0;

export function bump() {
    counter++;
}

export default 'DEFAULT';
