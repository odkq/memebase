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

#define INPUT_TIMEOUT 500000
#define FIXED_PREFIX "line="

int parse_environment(char **envp, char** keys, char** values, int* content_length, char* remote_address);
int is_input_ready(void);
int print_variable(char *key, char *value);

char *url_encode(char *str);
char *url_decode(char *str);
