#include <string>

#define TEST(t, n, d) \
    t v##n; \
    v##n = (d);

int t1(int a, int b)
{
	return a + b;
}

void t()
{
	TEST(int, 13, t1(1, 2));
}
