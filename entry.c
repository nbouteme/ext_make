#define _GNU_SOURCE
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <gnumake.h>

int plugin_is_GPL_compatible;

char *title = 0;
int first = 0;

typedef struct
{
	int curr_n;
	int width;
	int max;
}				t_progress_bar;

t_progress_bar pb;
char *print_tail_imp(int width);

char *init_progress_bar(const char *nm, unsigned argc, char **argv)
{
	pb.max = atoi(argv[0]);
	pb.width = atoi(argv[1]);
	pb.curr_n = 0;
	printf("\033[?25l");
	return (0);
}

char *gen_progress_bar(const char *nm, unsigned argc, char **argv)
{
	char *tb;
	char *buf;
	static int once = 0;

	++pb.curr_n;

	float per_cov = ((float)pb.curr_n / pb.max);
	if (per_cov > 1.0f)
		return (0);

	if (first == 1)
	{
		first = -1;
		printf("\033[31m%s\033[0m\n", title);
	}
	
	buf = calloc(1, pb.width * 4 + 1);

	int pbw = pb.width - 32;
	int i = 0;
	while (i < pbw * per_cov)
		strcat(buf, (i + 1 > pbw * per_cov + 0.5 ? "▓" : "█")), ++i;
	while (i < pbw)
		strcat(buf, "░"), ++i;

	printf("\033[9999D\033[31m│\033[0m%23.23s: %s %3.0f%%\033[31m│\033[0m",
		   argv[0], buf, per_cov * 100);
	if (!once)
	{
		once = 1;
		print_tail_imp(pb.width - 2);
		printf("\033[2A\033[99D");
	}
	free(buf);
	return (0);
}

char *shello(const char *nm, unsigned argc, char **argv)
{
	char *ret = gmk_alloc(2);
	ret[0] = ret[1] = 0;
	if (system(argv[0]))
		ret[0] = '1';
	return ret;
}

char *line_1(int wi, int tle)
{
	char *ret;
	char buf[(tle + 2) * 4 + 1];
	buf[0] = 0;
	strcat(buf, "╭");
	int i = 0;
	while (i < tle)
		strcat(buf, "─"), ++i;
	strcat(buf, "╮");
	++i;
	tle += 2;
	tle = wi / 2 - tle / 2;
	asprintf(&ret, "%*s%s%*s", tle - (i % 2 == 0), "", buf, tle + wi % 2, "");
	i += tle + tle + wi % 2;
	return ret;
}

char *line_3(int wi, int tle)
{
	char *ret;
	char buf[(tle + 2) * 3 + 1];
	buf[0] = 0;
	int i = 0;
	strcat(buf, "╰");
	while (i < tle)
		strcat(buf, "─"), ++i;
	strcat(buf, "╯");
	tle += 2;
	wi -= 2;
	tle = wi / 2 - tle / 2;
	asprintf(&ret, "│%*s%s%*s│", tle - (i % 2), "", buf, tle + wi % 2, "");
	return ret;
}

char *print_head(const char *nm, unsigned argc, char **argv)
{
	int width = atoi(argv[1]);
	int til = strlen(argv[0]) + 11;

	if (til >= width)
		return 0;

	char *l1 = line_1(width, strlen(argv[0]) + 8);

	title = calloc(1, width * 4 + 1);
	int i = 0;
	strcat(title, "╭"), ++i;
	while (i < width / 2 - til / 2)
		strcat(title, "─"), ++i;
	char buf[til * 4 + 6 + 9 + 1];
	sprintf(buf, "┤Projet: %s├", argv[0]);
	strcat(title, buf);
	i += strlen(buf) - 4;
	while (i < width - 1)
		strcat(title, "─"), ++i;
	strcat(title, "╮"), ++i;
	char *l2 = title;
	char *l3 = line_3(width, strlen(argv[0]) + 8);
	asprintf(&title, "%s\n%s\n%s", l1, l2, l3);
	free(l1);
	free(l2);
	free(l3);
	first = 1;
	return (0);
}

char *print_tail_imp(int width)
{
	char buf[width * 4 + 1];
	buf[0] = 0;
	int i = 0;
	while (i < width)
		strcat(buf, "─"), ++i;
	printf("\n\033[31m╰%s╯\033[0m\n", buf);
	return (0);
}

char *print_tail(const char *nm, unsigned argc, char **argv)
{
	if (first == -1)
	{
		first = 0;
		printf("\033[9999D\033[2B"), fflush(stdout);
	}
	printf("\033[?25h");
	return (0);
}

char *get_info(const char *nm, unsigned argc, char **argv)
{
	char *ret = strcpy(gmk_alloc(3), "54");
	char *tmp;
	if (strcmp(argv[0], "WIDTH") == 0)
	{
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) >= 0)
		{
			gmk_free(ret);
			asprintf(&tmp, "%d", w.ws_col);
			ret = strcpy(gmk_alloc(strlen(tmp) + 1), tmp);
			free(tmp);
		}
	}
	return ret;
}

int ext_make_gmk_setup()
{
	gmk_add_function("gen-pb", gen_progress_bar, 1, 1, 0);
	gmk_add_function("init-pb", init_progress_bar, 2, 2, 0);
	gmk_add_function("shello", shello, 1, 1, 0);
	gmk_add_function("print-head", print_head, 2, 2, 0);
	gmk_add_function("print-tail", print_tail, 1, 1, 0);
	gmk_add_function("get-term-info", get_info, 1, 1, 0);
	return (1);
}

int setup()
{
	return (ext_make_gmk_setup());
}
