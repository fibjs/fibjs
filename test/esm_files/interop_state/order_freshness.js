// Facade freshness fixture: required once, then mutated through its own
// function. The facade must forward to the namespace instead of holding a
// copy of the values, so a later read sees the new state.
export let counter = 0;

export function bump() {
    counter++;
}

export default 'DEFAULT';
