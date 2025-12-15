// Entry point: imports A and B concurrently
// Both A and B import the same shared module

const [a, b] = await Promise.all([
    import('./esm20_a.mjs'),
    import('./esm20_b.mjs')
]);

export const a_instanceId = a.sharedInstanceId;
export const b_instanceId = b.sharedInstanceId;
export const a_value = a.sharedValue;
export const b_value = b.sharedValue;
export const same_instance = a.sharedInstanceId === b.sharedInstanceId;
