async function test_async()
{
    return 100;
}

let k8s = await test_async();
export default k8s;
