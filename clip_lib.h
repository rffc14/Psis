#define REGIONS 10
#define MSG_LIMIT 100
#define COPY 0
#define PASTE 1
#define UPDATE 5
#define QUIT 9
#define INVALID_OPTION 2
#define OUT 'O'
#define IN 'I'
#define ON 1
#define OFF 0
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <libio.h>


typedef struct data {
	char characters[MSG_LIMIT];
	int region;
	int option;
}DATA;