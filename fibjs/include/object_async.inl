template<typename T1, typename T2>
static result_t asyncCall(AsyncQueue& q, result_t (*func)(T1 v1, T2&v2), T1& v1,
		T2& v2)
{
	class _t
	{
	public:
		static void _stub(AsyncCall* ac)
		{
			ac->hr = ((result_t (*)(T1 v1, T2&v2)) ac->args[0])(
					*(T1*) ac->args[1], *(T2*) ac->args[2]);
			ac->post();
		}
	};

	void* args[] =
	{ (void*) func, &v1, &v2 };
	AsyncCall ac(args, _t::_stub);

	q.put(&ac);
	ac.weak.wait();

	return ac.hr;
}
