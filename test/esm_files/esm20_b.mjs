// Module B: imports shared module
const shared = await import('./esm20_shared.mjs');

export const sharedInstanceId = shared.instanceId;
export const sharedValue = shared.value;
