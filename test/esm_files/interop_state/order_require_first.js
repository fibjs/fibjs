// Order fixture: this file is required first, then imported.
// Kept separate from the other fixtures so the module cache is cold when the
// test starts - the point is what a cold require() leaves behind for import().
export let counter = 0;

export function bump() {
    counter++;
}

export default 'DEFAULT';
