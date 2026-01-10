// Shared module with async initialization
// Used to test concurrent dynamic imports

await new Promise(resolve => setTimeout(resolve, 10));

export const instanceId = Math.random().toString(36).slice(2, 8);
export const value = 'SHARED_VALUE';
