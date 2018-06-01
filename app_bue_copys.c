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
#define SOCK_ADDRESS "/tmp/sock_16"
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

#include "clipboard.h"

int main(void){

	char buf[MSG_LIMIT];
	int i;
	int option = COPY;
	int region;
	int copys;
	void *pointer;
	int n;


	int clipboard_id = clipboard_connect("./");
	if(clipboard_id == -1){
		printf("Clipboard connection error\n");
		exit(-1);
	}

	while(1){	
		printf("COPY (0) QUANTAS VEZES? (Press 9 for exit)\n");
		scanf("%d", &copys); // le opcao
            	while(getchar()!='\n'); //limpa o buffer
		
		printf("Which region?\n");
		scanf("%d", &region);
			
		for(i = 0; i < copys; i++)
		{
			sprintf(buf,"%d", i);
			pointer = buf;
			n=clipboard_copy(clipboard_id,region,pointer,sizeof(buf));
			if (n==0) printf("COPY ERROR\n");
		}
	
	}	
}
