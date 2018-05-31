#include "clipboard.h"
#include <pthread.h>

char data[REGIONS][MSG_LIMIT] = {{'0'},{'0'},{'0'},{'0'},{'0'},{'0'},{'0'},{'0'},{'0'},{'0'}};


int fetch_backup(char *IP, char *backup_data[REGIONS][MSG_LIMIT])
{
	
	struct sockaddr_in server_addr;

	int sockin_fd= socket(AF_INET, SOCK_STREAM, 0);

	int nbytes;
	
	if (sockin_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	server_addr.sin_family = AF_INET;

	server_addr.sin_port= htons(3010);
	inet_aton(IP, &server_addr.sin_addr);
	 
	
	if( -1 == connect(sockin_fd, 
			(const struct sockaddr *) &server_addr, 
			sizeof(server_addr))){
				printf("Error connecting\n");
				exit(-1);
	}

	nbytes = recv(sockin_fd, &*backup_data, sizeof(char)*MSG_LIMIT*REGIONS, 0); 
	while(nbytes == -1){
			
			printf("Waiting to read...");
			nbytes = recv(sockin_fd, &*backup_data, sizeof(char)*MSG_LIMIT*REGIONS, 0); 
	}
	
	return sockin_fd;


}

void display_data(char data[REGIONS][MSG_LIMIT])
{

	int i, j;
	
	printf("\n\n---- CLIPBOARD ----\n\n");
	printf("R\n");
	for(i = 0; i < REGIONS; i++)
		printf("%d: %s\n", i , data[i]);
	//printf("\n");


}


int clipboard_connect(char * clipboard_dir){
	
	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;

	int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	client_addr.sun_family = AF_UNIX;
	sprintf(client_addr.sun_path, "%ssocket_%d", clipboard_dir, getpid());

	server_addr.sun_family = AF_UNIX;
	sprintf(server_addr.sun_path, "%sCLIPBOARD_SOCKET", clipboard_dir);

	int err_c = connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));
	if(err_c==-1){
		perror("Connecting: ");
		exit(-1);
	}
	return sock_fd;
}



int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	
	int n;
	int option = COPY;
	DATA new_data;

	char *msg = malloc(count*sizeof(char));

	memcpy(msg, buf, count);

	printf("\nCLIPBOARD COPY\n"); // é para remover
	printf("Copying %s to region %d\n", msg, region);// é para remover

	strcpy(new_data.characters, msg);
	new_data.region = region;
	new_data.option = option;

	free(msg);

	n = send(clipboard_id, &new_data, sizeof(new_data), 0);
	if (n<0){
		exit(0);
	}else{
		return n;
	}
}


void update_backup(int sock_net, int region, char *newdata){

	int nbytes;
	
	DATA sent;
	
	strcpy(sent.characters, newdata);
	
	sent.region = region;
  
	nbytes = send(sock_net, &sent, sizeof(sent), 0);

	if (nbytes < 0){
		perror("write error");
		exit(-1);
	}

}

int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	
	int n;
	int option = PASTE;
	DATA new_data;
	char *msg = malloc(count*sizeof(char));

	new_data.region = region;
	new_data.option = option;
	
	printf("clipboard PASTE\n");

	n = send(clipboard_id, &new_data, sizeof(new_data), 0);
	if (n<0){
		perror("write error");
		exit(-1);
	}

			
	while(recv(clipboard_id, msg, count*sizeof(char), 0) == -1){
		printf("Waiting to read...");
	}

	memcpy(buf, msg, count);

	free(msg);
	
}

void clipboard_close(int clipboard_id){
	close(clipboard_id);
}
