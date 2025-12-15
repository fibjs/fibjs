// Module A: imports shared module
console.log('[esm20_a] Starting');

const shared = await import('./esm20_shared.mjs');

console.log('[esm20_a] Got shared, instanceId:', shared.instanceId);

export const sharedInstanceId = shared.instanceId;
export const sharedValue = shared.value;
