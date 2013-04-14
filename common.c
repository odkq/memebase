#define HOSTURL "/"
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <syslog.h>
#include <ctype.h>

#include "common.h"

static int process_variable(char *key, char *value, int* content_length, char* remote_address);
static char from_hex(char ch);
static char to_hex(char code);

int parse_environment(char **envp, char** keys, char** values, int* content_length, char* remote_address)
{
	int i, j, k;
	char *key;
	char *value;
	char *p;
	k = 0;
	for (i=0; envp[i]; i++) {
		j = 0;
		key = p = envp[i];
		do {
			if (*p == '=') {
				*p='\0';
				value = p+1;
				process_variable(key, value, content_length, remote_address);
				keys[k] = key;
				values[k] = value;
				if (k == 4096)
					return -1;
				k++;
				break;
			}
			if (*p == '\0') {
				value = NULL;	
			}
			if (j >= 2000) {
				return -1;
			}
			p++;
			j++;
		} while(1);
	}
	return k;
}

static int process_variable(char *key, char *value, int* content_length, char* remote_address)
{
	if (!strncmp(key, "CONTENT_LENGTH", 14))
		*content_length=atoi(value);
	if (!strncmp(key, "REMOTE_ADDR", 11)) 
		remote_address = value;
	return 0;
}

int print_variable(char *key, char *value)
{
	if (value == NULL)
		printf("<li>key \"%s\", value NULL</li>", key);
	else
		printf("<li>key \"%s\", value \"%s\"\n</li>", key, value);
	return 0;
}

int is_input_ready(void)
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = INPUT_TIMEOUT;
	retval = select(1, &rfds, NULL, NULL, &tv);
	if (retval == -1)
		return -1;
	else if (retval)
		return 1;
	else
		return 0;
}

static char from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

static char to_hex(char code)
{
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

char *url_encode(char *str)
{
	char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
			*pbuf++ = *pstr;
		else if (*pstr == ' ') 
			*pbuf++ = '+';
		else 
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

char *url_decode(char *str)
{
	char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
	while (*pstr) {
		if (*pstr == '%') {
		if (pstr[1] && pstr[2]) {
			*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
			pstr += 2;
		}
	} else if (*pstr == '+') { 
		*pbuf++ = ' ';
	} else {
		*pbuf++ = *pstr;
	}
	pstr++;
	}
	*pbuf = '\0';
	return buf;
}
