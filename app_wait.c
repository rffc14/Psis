#define WAIT 2
#define QUIT 9
#define MSG_LIMIT 100
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

#include "clipboard.h"


int main(void){

	char buf[MSG_LIMIT];
	int option;
	int region = 2;
	void *pointer;
	int n;

	int clipboard_id = clipboard_connect("./");
	if(clipboard_id == -1){
		printf("Clipboard connection error\n");
		exit(-1);
	}


	while(1){
		printf("WAIT(2)? (Press 9 for exit)\n");
		scanf("%d", &option); // le opcao
         	while(getchar()!='\n'); //limpa o buffer

	
		if(option == WAIT){ 
			printf("Which region will you wait for?\n");
			scanf("%d", &region);

			pointer = buf;
			n = clipboard_wait(clipboard_id,region,pointer,sizeof(buf));
			if(n>0)	printf("Region %d changed: %s\n", region, (char *)pointer);
			else printf("WAIT ERROR\n");
	
		}else if(option==QUIT){
			clipboard_close(clipboard_id);
			exit(0);
		}else printf("Option error\n");
	}	
}
