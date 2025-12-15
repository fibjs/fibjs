// Module B: imports shared module
console.log('[esm20_b] Starting');

const shared = await import('./esm20_shared.mjs');

console.log('[esm20_b] Got shared, instanceId:', shared.instanceId);

export const sharedInstanceId = shared.instanceId;
export const sharedValue = shared.value;
