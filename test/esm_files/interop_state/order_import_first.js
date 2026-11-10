// Order fixture, mirror image: this file is imported first, then required.
// Separate from order_require_first.js so its module cache is cold here too.
export let counter = 0;

export function bump() {
    counter++;
}

export default 'DEFAULT';
