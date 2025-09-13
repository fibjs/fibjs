console.log("test_async");
async function test_async()
{
    console.log("test_async return");
    return 100;
}

let k8s = await test_async();
console.log("test_async export");
export default k8s;
console.log("test_async export done");
