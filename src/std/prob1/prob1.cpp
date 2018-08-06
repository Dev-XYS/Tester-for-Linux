#include <cstdio>

using namespace std;

void dfs(int x)
{
	if (x > 0) dfs(x - 1);
}

int main()
{
	freopen("prob1.in", "r", stdin);
	freopen("prob1.out", "w", stdout);
	int x;
	scanf("%d", &x);
	printf("%d\n", x);
	if (x == 3) *((int*)0) = 0;
	if (x == 4) for (int i = 0; i < 1000000000; i++) ;
	if (x == 5) for (int i = 0; i < 100000000; i++) { int *a = new int; a = 0; }
	if (x == 6) dfs(1000000);
	return 0;
}
