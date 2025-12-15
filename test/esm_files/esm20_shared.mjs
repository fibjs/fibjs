// Shared module with async initialization
// Used to test concurrent dynamic imports

console.log('[esm20_shared] Loading started');

await new Promise(resolve => setTimeout(resolve, 10));

export const instanceId = Math.random().toString(36).slice(2, 8);
export const value = 'SHARED_VALUE';

console.log('[esm20_shared] Loading finished, instanceId:', instanceId);
