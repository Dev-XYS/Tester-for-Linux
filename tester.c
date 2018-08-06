#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#define SYS(fmtstr, ...) (sprintf(__buf__, fmtstr, __VA_ARGS__), system(__buf__))

#define AC 1
#define WA 2
#define RE 3
#define TLE 4
#define MLE 5

char __buf__[256];

int problem_count;
struct problem
{
	char name[32];
	int testcase_count, time_limit, memory_limit;
	char checker[32];
}problems[16];

struct result
{
	int verdict;
	struct rusage usage;
};

void load_config(const char *conf)
{
	FILE *f = fopen(conf, "r");
	fscanf(f, "%d", &problem_count);
	for (int i = 1; i <= problem_count; i++)
	{
		fscanf(f, "%s%d%d%d%s", problems[i].name , &problems[i].testcase_count , &problems[i].time_limit , &problems[i].memory_limit , problems[i].checker);
	}
	fclose(f);
}

int check(const char *chkr, const char *outf, const char *ansf)
{
	return SYS("%s %s %s >/dev/null 2>&1", chkr, outf, ansf) == 0 ? AC : WA;
}

/* func run() 
 * cmd  - Command to run contestants' program.
 * tlim - Time limit in seconds.
 * mlim - Memory limit megabytes.
 * chkr - Checker.
 * outf - Output file.
 * ansf - Answer file.
 */
struct result run(const char *cmd, int tlim, int mlim, const char *chkr, const char *outf, const char *ansf)
{
	int pid = fork();
	if (pid > 0)
	{
		int status;
		struct rusage usage;
		wait4(pid, &status, 0, &usage);
		int v = 0;
		if (usage.ru_maxrss > mlim * 1024)
		{
			v = MLE;
		}
		else if (WIFEXITED(status))
		{
			v = check(chkr, outf, ansf);
		}
		else if (WTERMSIG(status) == SIGXCPU)
		{
			v = TLE;
		}
		else
		{
			v = RE;
		}
		return (struct result){v, usage};
	}
	else
	{
		setrlimit(RLIMIT_CPU, &(struct rlimit){tlim, tlim + 1});
		chdir("tmp");
		execl(cmd, NULL);
	}
}

void show_verdict(int verdict)
{
	printf("  ");
	switch (verdict)
	{
	case AC:
		printf(" \033[1;32mAC\033[0m");
		break;
	case WA:
		printf(" \033[1;31mWA\033[0m");
		break;
	case RE:
		printf(" \033[1;33mRE\033[0m");
		break;
	case TLE:
		printf("\033[1;35mTLE\033[0m");
		break;
	case MLE:
		printf("\033[1;35mMLE\033[0m");
		break;
	default:
		printf(" \033[1;41;37m??\033[0m");
	}
}

void test_one_case(const char *contestant, int probi, int tci)
{
	SYS("cp data/%s/%s%d.in tmp/%s.in >/dev/null 2>&1", problems[probi].name, problems[probi].name, tci, problems[probi].name);
	
	char outf[256], ansf[256];
	sprintf(outf, "tmp/%s.out", problems[probi].name);
	sprintf(ansf, "data/%s/%s%d.ans", problems[probi].name, problems[probi].name, tci);
	struct result res = run("./_EXECUTABLE_", problems[probi].time_limit, problems[probi].memory_limit, problems[probi].checker, outf, ansf);
	
	if (res.verdict != TLE)
	{
		printf("  %4ldms", res.usage.ru_utime.tv_sec * 1000 + res.usage.ru_utime.tv_usec / 1000);
	}
	else
	{
		printf("        ");
	}
	printf(" %7ldKB", res.usage.ru_maxrss);
	show_verdict(res.verdict);
	printf("\n");
	
	SYS("rm %s.in %s.out >/dev/null 2>&1", problems[probi].name, problems[probi].name);
}

void test_one_contestant_one_problem(const char *contestant, int probi)
{
	system("mkdir tmp >/dev/null 2>&1");
	printf("Compiling...");
	if (SYS("g++ src/%s/%s/%s.cpp -o tmp/_EXECUTABLE_ >/dev/null 2>&1", contestant, problems[probi].name, problems[probi].name) == 0)
	{
		printf("  \033[1;32mOK\033[0m\n\n");
	}
	else
	{
		printf("  \033[1;31mFailed\033[0m\n");
		return;
	}
	for (int i = 1; i <= problems[probi].testcase_count; i++)
	{
		printf("Testcase #%2d", i);
		test_one_case(contestant, probi, i);
	}
	system("rm _EXECUTABLE_ >/dev/null 2>&1");
}

void test_one_contestant(const char *contestant)
{
	system("mkdir tmp >/dev/null 2>&1");
	printf("\n>>> %s\n\n", contestant);
	for (int i = 1; i <= problem_count; i++)
	{
		test_one_contestant_one_problem(contestant, i);
	}
	printf("\n");
}

void test_all()
{
	system("mkdir tmp >/dev/null 2>&1");
	system("ls src >tmp/_CONTESTANT_LIST_");
	FILE *f = fopen("tmp/_CONTESTANT_LIST_", "r");
	char contestant[256];
	while (fscanf(f, "%[^\n] ", contestant) > 0)
	{
		test_one_contestant(contestant);
	}
}

int main()
{
	printf("Tester for Linux (c) Dev-XYS 2018\n\n");
	printf("Make sure you have run 'ulimit -s unlimited' before starting the tester.\n\n");
	while (1)
	{
		printf("TESTER$ ");
		char cmd[256];
		scanf("%s", cmd);
		if (strcmp(cmd, "load") == 0)
		{
			char conf[256];
			scanf("%s", conf);
			load_config(conf);
		}
		else if (strcmp(cmd, "test") == 0)
		{
			test_all();
		}
		else if (strcmp(cmd, "test1c") == 0)
		{
			char contestant[256];
			scanf("%s", contestant);
			test_one_contestant(contestant);
		}
		else if (strcmp(cmd, "test1c1p") == 0)
		{
			char contestant[256];
			int probi;
			scanf("%s%d", contestant, &probi);
			test_one_contestant_one_problem(contestant, probi);
		}
		else
		{
			printf("?\n");
		}
	}
	return 0;
}
