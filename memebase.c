/*
 * Memebase
 *
 * The dumbest cgi script in the world
 *
 * pablo.martin@acm.org, 2013
 *
 * Licensed under the GPLv3 for example
 */
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <common.h>
#include <sqlite3.h>

char *remote_address;
char *decoded_input_buffer;
char input_buffer[4096];
char output_buffer[4096]; /* line by line */
char *keys[4096];
char *values[4096];
int content_length = 0;

static void usage(void);
static int check_configuration(void);

static int add_meme(char *input_line)
{
	int r;

	r = 0;
	return r;
}

static void usage(void)
{
	printf("Usage: memebase [option]\n");
	printf("          --check-config: Load configuration file and connect to db\n");
	printf("                          returning 0 ($? -eq 0) on success\n");
	printf("\n");
	printf("          --input-from-stdin: Get input line from stdin bypassing CGI/POST\n");
	printf("                          parsing and redirecting all logs to stdout\n");
	printf("                          (only for debugging)\n");
}

int main(int argc, char **argv, char **envp)
{
	char input_buffer[1024];	/* Used only on debug i.e --input-from-stdin */
	int c, r, nvars, i;

	nvars = parse_environment(envp, keys, values, &content_length, remote_address);
	if (nvars < 0)
		return 0;
	if ((argc == 2) && (!strcmp(argv[1], "--check-config"))) {
		return check_configuration();
	} else if ((argc == 2) && (!strcmp(argv[1], "--input-from-stdin"))) {
		if (fgets(input_buffer, 1024, stdin) != NULL) {
			input_buffer[1023] = '\0';
			return add_meme(input_buffer);
		}
	} else if (argc == 2) {
		usage();
		return 0;
	}
	/* Regular CGI mode below */
	r = is_input_ready();
	if (content_length <= 5)
		r = 0;
	if (!r) {
		printf("content-type: text/html\n\n");
		printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0 Strict//EN\">\n");
		printf("<html>\n<head>\n");
		printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=iso-8859-1\" >\n");
		printf("<title>Memebase</title>\n");
		printf("</head><body><p>\n");
		printf("<center><h1>All your memebase is belong to us</h1></center>\n");
		printf("<p><small><center>Source code at ");
		printf("<a href=\"https://github.com/odkq/memebase\">\n");
		printf("https://github.com/odkq/memebase</a></center></small></p>\n");
		printf("<p><center><i>Please add your brave meme below\n");
		printf("</i></center></p>\n");
		/* url is defined upon calling gcc */
		printf("<center><form action=\"");
		printf(URL);
		printf("\" method=\"post\">");
		printf("URL: <input type=\"text\" name=\"url\" size=\"30\"/> ");
		printf("Comment: <input type=\"text\" name=\"comment\" size=\"30\"/><br />");
		printf("<input type=\"submit\" value=\"Submit\" /></center>");
		printf("\n<p>");
		printf("<hr>\n");
		printf("</p>\n");
		printf("</body></html>\n");
	} else if (r > 0) {
		for (i = 0; i < content_length; i++) {
			c = getchar();
			if (c == EOF) {
				// End of input too early
				printf("End of input too early\n");
				return -1;
			} else {
				input_buffer[i] = c;
			}
		}
		input_buffer[i] = '\0';
		decoded_input_buffer = url_decode(input_buffer);
			printf("content-type: text/plain\n\n");
			printf("decoded_input_buffer: %s\n", decoded_input_buffer);
		add_meme(decoded_input_buffer);
		free(decoded_input_buffer);
	}
	return 0;
}

int check_configuration(void)
{
	return 0;
}

