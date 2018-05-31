#include "clipboard.h"


int main(){

	char buf[MSG_LIMIT];
	int option;
	int region;
	void *pointer;
	int n;

	//signal(SIGINT, ctrl_c_callback_handler);

	int clipboard_id = clipboard_connect("./");
	if(clipboard_id == -1){
		printf("Clipboard connection error");
		exit(-1);
	}

	printf("Press Enter\n");

	while(1){	
		while(getchar()!='\n'); //limpa o buffer
		printf("COPY (0) or PASTE (1)? (Press 9 for exit)\n");
		scanf("%d", &option); // le opcao
            	while(getchar()!='\n'); //limpa o buffer

		if (option == COPY){
			printf("\n\nYOU CHOSE COPY OPTION\n");
			printf("What do you want to copy?\n");

			fgets(buf, MSG_LIMIT, stdin);
			strtok(buf, "\n");
		
			printf("Which region?\n");
			scanf("%d", &region);
			if (region < 0 || region > 9){
				printf("Invalid region, should just write digit between 0 and 9\n");
			}
			else{
				pointer = buf;
				n=clipboard_copy(clipboard_id,region,pointer,sizeof(buf));
				if (n==0){
					printf("Clipboard not available.\n");
					exit(-1);
				}
			}
			
		}else if(option == PASTE){ 
			printf("\n\nYOU CHOSE PASTE OPTION\n");
			printf("Which region?\n");
			scanf("%d", &region);
			if (region < 0 || region > 9){
				printf("Invalid region, should just write digit between 0 and 9\n");
			}
			else
			{
				pointer = buf;
				clipboard_paste(clipboard_id,region,pointer,sizeof(buf));

				printf("String pasted on Clipboard: %s\n", (char *)pointer);

			}	
		}else if(option==QUIT){
			clipboard_close(clipboard_id);
			exit(0);
		}else{
			printf("Option error\n");
		}
	}	
}
