
#include "clip_lib.h"	
int rec_d=0;
int rec_u=0;
struct sockaddr_in main_sync_addr;
sem_t *sem0;
sem_t *sem1;
sem_t *sem2;
sem_t *sem3;
sem_t *sem4;
sem_t *sem5;
sem_t *sem6;
sem_t *sem7;
sem_t *sem8;
sem_t *sem9;
sem_t *stop_u;
sem_t *stop_d;
int reg;
int app = 0;
int clips_up=0;  //cada clipboard regista a quem está ligado
int clips_down=0; // para cima e para baixo na árvore
int clip_id;
int status[10];
int countsent=0;
char data[REGIONS][MSG_LIMIT];


int main(int argc, char *argv[]){

	int ConnectedMode=OFF;
	int sock_net;
	int i, nbytes;
	for (i=0;i<10;i++){
		status[i]=UPDATED;
	}
	signal(SIGINT, ctrl_c_callback_handler);
	display_data(data);
	ConnectedMode = check_mode(argc, argv);
	pthread_t thread_sem;
	

	
	pthread_t thread_id[2];
	pthread_create(&(thread_id[0]), NULL, listen_local,NULL);
	pthread_create(&(thread_id[1]), NULL, listen_remote,NULL);
	if(ConnectedMode == ON){
		clips_up = 1;
		printf("Connecting to %s port %s:\n\n",argv[2],argv[3]);

		int option, region;
		DATA remote_data;

		struct sockaddr_in server_addr;

		int sockin_fd= socket(AF_INET, SOCK_STREAM, 0);
		int nbytes;


		if (sockin_fd == -1){
			perror("socket: ");
			exit(-1);
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port= htons(atoi(argv[3]));
		inet_aton(argv[2], &server_addr.sin_addr);
	
		if( -1 == connect(sockin_fd, 
			(const struct sockaddr *) &server_addr, 
			sizeof(server_addr))){
				printf("Remote connecting error\n");
				unlink("./CLIPBOARD_SOCKET");
				exit(-1);
		}
		printf("Initial update\n");
		nbytes = recv(sockin_fd, &data, sizeof(char)*MSG_LIMIT*REGIONS, 0);
		while(nbytes == -1){
			printf("Waiting to read...");
			nbytes = recv(sockin_fd, &data, sizeof(char)*MSG_LIMIT*REGIONS, 0); 
		}//Initial regions update

		nbytes = recv(sockin_fd, &main_sync_addr, sizeof(main_sync_addr), 0);
		while(nbytes == -1){
			printf("Waiting to read...");
			nbytes = recv(sockin_fd, &main_sync_addr, sizeof(main_sync_addr), 0); 
		}
		
		pthread_t recv_id;
		pthread_t send_id;
		pthread_create(&recv_id, NULL, d_recvt, sockin_fd);
		pthread_create(&send_id, NULL, d_sendt, sockin_fd);

		display_data(data);
		//pthread_create(&thread_sem, NULL, sems,0);

		pthread_join(recv_id, NULL);
		pthread_cancel(send_id);
	}
	if(ConnectedMode == OFF){
		int ret = fork();
		if(ret == 0) sync();

		struct sockaddr_un local_addr;
		struct sockaddr_un client_addr;
		socklen_t size_addr;
	
		int SYNC_SOCKET = socket(AF_UNIX, SOCK_STREAM, 0);
	
		if (SYNC_SOCKET == -1){ //validaçao de erro
			perror("socket: ");
			exit(-1);
		}
	
		local_addr.sun_family = AF_UNIX;
		sprintf(local_addr.sun_path, "./SYNC_SOCKET");

		int err = bind(SYNC_SOCKET, (struct sockaddr *)&local_addr, sizeof(local_addr));
		if(err == -1) {
			perror("Bind: ");
			unlink("/SYNC_SOCKET");
			unlink("./CLIPBOARD_SOCKET");
			exit(-1);
		}
	
		printf("sync ligado e binded\n");
	
		if(listen(SYNC_SOCKET, 2) == -1) { //quantas posições de espera usamos??
			perror("listen");
			unlink("/SYNC_SOCKET");
			unlink("./CLIPBOARD_SOCKET");
			exit(-1);
		}
		
		int client_fd = accept(SYNC_SOCKET, (struct sockaddr *) & client_addr, &size_addr);

		if(client_fd == -1) {
			unlink("/SYNC_SOCKET");
			unlink("./CLIPBOARD_SOCKET");
			perror("accept do sync");    
			exit(-1);
		}

		nbytes = recv(client_fd, &main_sync_addr, sizeof(main_sync_addr), 0);
		while(nbytes == -1)
			nbytes = recv(client_fd, &main_sync_addr, sizeof(main_sync_addr), 0);
			
		if(nbytes == 0){ //ligação terminada pelo cliente
			printf("O SYNC MORREU\n");
		}


		display_data(data);
		//pthread_create(&thread_sem, NULL, sems,0);
	}


	pthread_join(thread_id[0],NULL); // a main thread espera pela listen local	
	pthread_join(thread_id[1],NULL);
	
	return 0;
	
}









void * d_recvt(void *client_fd){

	//sem0 = sem_open(SEM0,O_CREAT,0666,1);
	DATA remote_data;
	int nbytes,i;
	int check_recv, option;

	printf("\nORA VIVA d_recvt!!\n\n");

	while(1){
		
		check_recv = recv(client_fd, &remote_data, sizeof(remote_data), 0);
		if (check_recv == -1){
			perror("Receiving remote data: ");
			exit(0);
		}

		

		if(check_recv == 0){ //ligação terminada pelo clipboard remoto que se ligou
			printf("Clipboard remoto desconectado.\n");
			clips_up = 0;
			pthread_exit(0);
		}
		rec_d=1;
		printf("remote data: %s\n region: %d\n", remote_data.characters, remote_data.region );
		reg = remote_data.region;
		printf("testingggg\n");
		strcpy(data[reg],remote_data.characters);
		printf("Updated with remote data\n");	
		countsent = clips_down;
		display_data(data);
		if(clips_down == 0)
			status[reg] = UPDATED;
		else{
			status[reg] = NOT_UPDATED;
			
			rec_d = 0;
			for(i = 0; i < clips_down; i++)	sem_post(stop_u);
			printf("MAIS QUE 1!!!\n\n");
			while(status[reg]==NOT_UPDATED);
		}
		rec_d=0;
	}
}
void *d_sendt(void *client_fd){

	//sem0 = sem_open(SEM0,O_CREAT,0666,1);
	char namesemd[100];
	sprintf(namesemd, "/semd_%d", getpid());
	stop_d = sem_open(namesemd,O_CREAT,0666,0);
	int nbytes;
	int check_recv, option;
	DATA remote_data;
	printf("ORA VIVA d_sendt!!\n\n");

	while(1){
			printf("EU CONTINUO CÁ!");
			sem_wait(stop_d);
			printf("after wait\n");
			if (status[reg]==NOT_UPDATED && rec_d == 0 && clips_up == 1){
				printf("VOU ENVIAR CENAS\n\n");
				strcpy(remote_data.characters, data[reg]);
				remote_data.region=reg;
		   
				nbytes=send(client_fd, &remote_data, sizeof(remote_data), 0);

				if(nbytes<=0) printf("UPS!\n");
				else{
					status[reg] == UPDATED;
					printf("ENVIADO E UPDATADO!\n\n");
				}

	}}
}


/*void * sems (void *arg){
	sem0 = sem_open(SEM0,O_CREAT,0666,1);
	sem1 = sem_open(SEM1,O_CREAT,0666,1);
	sem2 = sem_open(SEM2,O_CREAT,0666,1);
	sem3 = sem_open(SEM3,O_CREAT,0666,1);
	sem4 = sem_open(SEM4,O_CREAT,0666,1);
	sem5 = sem_open(SEM5,O_CREAT,0666,1);
	sem6 = sem_open(SEM6,O_CREAT,0666,1);
	sem7 = sem_open(SEM7,O_CREAT,0666,1);
	sem8 = sem_open(SEM8,O_CREAT,0666,1);
	sem9 = sem_open(SEM9,O_CREAT,0666,1);
	
	
	while(1){
		
		
		//if (all[0]=UPDATED){
			//sleep(2);
			sem_post(sem0);
		//}
		/*
		if (all1=UPDATED){
			sem_post(sem1);
		}
		if (all2=UPDATED){
			sem_post(sem2);
		}
		if (all3=UPDATED){
			sem_post(sem3);
		}
		if (all4=UPDATED){
			sem_post(sem4);
		}
		if (all5=UPDATED){
			sem_post(sem5);
		}
		if (all6=UPDATED){
			sem_post(sem6);
		}
		if (all7=UPDATED){
			sem_post(sem7);
		}
		if (all8=UPDATED){
			sem_post(sem8);
		}
		if (all9=UPDATED){
			sem_post(sem9);
		}
		
		sem_close(sem0);
		sem_close(sem1);
		sem_close(sem2);
		sem_close(sem3);
		sem_close(sem4);
		sem_close(sem5);
		sem_close(sem6);
		sem_close(sem7);
		sem_close(sem8);
		sem_close(sem9);
		pthread_exit(0);
	
	}
}*/
void * listen_local(void *arg){

	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	socklen_t size_addr;

	pthread_t thread_local_id[10];
	
	int CLIPBOARD_SOCKET = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if (CLIPBOARD_SOCKET == -1){ //validaçao de erro
		perror("socket: ");
		exit(-1);
	}

	local_addr.sun_family = AF_UNIX;
	sprintf(local_addr.sun_path, "./CLIPBOARD_SOCKET");

	int err = bind(CLIPBOARD_SOCKET, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("Bind: ");
		exit(-1);
	}
	
	printf("Socket created and binded, READY TO RECEIVE local connections! \n");
	
	if(listen(CLIPBOARD_SOCKET, 2) == -1) { //quantas posições de espera usamos??
		perror("listen");
		exit(-1);
	}
	
	while (1){ //thread princiapl fica bloqueada no accept para ligar a mais apps

	int client_fd = accept(CLIPBOARD_SOCKET, (struct sockaddr *) & client_addr, &size_addr);

	if(client_fd == -1) {
		perror("accept");    
		exit(-1);
	}
		printf("client_fd=%d",client_fd);
		pthread_create(&(thread_local_id[app]), NULL, new_app, client_fd);
		 // prever o que acontece quando uma app desliga e deixa disponível uma vaga na variavel app
	}
}

void * listen_remote(void *arg){

	struct sockaddr_in local_addr;
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	pthread_t thread_rem[10];
	int sock_fd= socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1){
    	perror("Remote socket: ");
    	exit(-1);
   	}
	
	local_addr.sin_family = AF_INET;
	srandom(getpid());
	int port=random()%(4000+1-2000)+2000;
  	local_addr.sin_port= htons(port);
	local_addr.sin_addr.s_addr= INADDR_ANY; // Um endereço qualquer 

	int err = bind(sock_fd,(struct sockaddr *)&local_addr, sizeof(local_addr));

	if(err == -1) {
    	perror("Remote socket bind");
    	exit(-1);
  	}
	
	printf(" socket created and binded, ready to receive remote connections!\n");
	printf("Working port: %d\n", port);

	if(listen(sock_fd, 5) == -1) {
		perror("listen");
		exit(-1);
	}

	while(1){
	int client_fd= accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr); //bloqueia até ter clientes
	
	if(client_fd == -1) {
		perror("accept");    
		exit(-1);
	}
	
	pthread_create(&(thread_rem[clips_down]), NULL, new_rem_clip, client_fd);
	}
}
void ctrl_c_callback_handler(int signum){
	printf("Caught signal Ctr-C\n");
	char namesemup[100];
	sprintf(namesemup, "/semd_%d", getpid());
	char namesemd[100];
	sprintf(namesemd, "/semup_%d", getpid());
	unlink("./CLIPBOARD_SOCKET");
	unlink("/SYNC_SOCKET");
	sem_close(stop_u);
	sem_close(stop_d);
	sem_unlink(namesemup);
	sem_unlink(namesemd);
	//sem_unlink(STOP_U);
	//sem_unlink(STOP_D);
	sem_unlink("/sem0");
	sem_unlink("/sem1");
	sem_unlink("/sem2");
	sem_unlink("/sem3");
	sem_unlink("/sem4");
	sem_unlink("/sem5");
	sem_unlink("/sem6");
	sem_unlink("/sem7");
	sem_unlink("/sem8");
	sem_unlink("/sem9");
	

	exit(0);
}

void *new_app(void *client_fd){
	app++;
	DATA new_data;
	int check_recv, option;
	int nbytes;
	int sock_net, i;
	int permission = 0;
	char namesemd[100];
	struct timespec time;
	sprintf(namesemd, "/semd_%d", getpid());
	char namesemup[100];
	sprintf(namesemup, "/semup_%d", getpid());

	stop_u = sem_open(namesemup,O_CREAT,0666,0);
	stop_d = sem_open(namesemd,O_CREAT,0666,0);

	struct sockaddr_in sync_addr;

	int sockid = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockid == -1){
		perror("socket: ");
		exit(-1);
	}

	sync_addr.sin_family = AF_INET;
	sync_addr.sin_port = main_sync_addr.sin_port;
	sync_addr.sin_addr = main_sync_addr.sin_addr;

	if( -1 == connect(sockid, 
			(const struct sockaddr *) &sync_addr, 
			sizeof(sync_addr))){
				printf("SYNC connecting error\n");
				exit(-1);
	}
	


	printf("Aplicação local conectada.\n");
	int n;
	while(1){
		
		new_data.option = INVALID_OPTION; /* TO GO OUT WHEN NO NEW_DATA */
		
		check_recv = recv(client_fd, &new_data, sizeof(new_data), 0);
		while(check_recv == -1)
			check_recv = recv(client_fd, &new_data, sizeof(new_data), 0);
			
		if(check_recv == 0){ //ligação terminada pelo cliente
			printf("Aplicação local desconectada.\n");
			app--;
			pthread_exit(0);
		}
		
		option = new_data.option;

		if (option == COPY){
			
			reg = new_data.region;
			
			nbytes = send(sockid, &reg, sizeof(reg), 0);
			if (nbytes<=0){
				printf("SYNC AVAILABLE\n");
				return 0;
			}
			while(permission == 0){
				nbytes = recv(sockid, &permission, sizeof(permission), 0);
				if(nbytes == -1){
					printf("Waiting to read...");
					nbytes = recv(sockid, &permission, sizeof(permission), 0); 
				}
			}
			
			strcpy(data[reg],new_data.characters);
			status[reg]=NOT_UPDATED;
		
			rec_d = 0;
			rec_u = 0;
			countsent = clips_down;

			if(clips_up == 1)
				sem_post(stop_d);
			else {
				for(i = 0; i < clips_down; i++)
					sem_post(stop_u);
			}
				

			printf("\nMESSAGE RECEIVED: %s\n", new_data.characters);
			printf("COPY OPTION\n");
			printf("This is what we stored in region %d: %s \n", reg, data[reg]);	
			display_data(data);
		
		}else if(option == PASTE){
			reg = new_data.region;
			nbytes = send(client_fd, data[reg], sizeof(data[reg]), 0); 
			if(nbytes<0){
				perror("write error");
				exit(-1);
			}
		}
	}
}

void *new_rem_clip(void *client_fd){
	

	int nbytes;
	
	clips_down++;
	countsent++;
	printf("Clipboard remoto conectado.\n");

	nbytes = send(client_fd, &data, sizeof(data), 0);
	if(nbytes<0){
		perror("Initial remote update: ");
		exit(-1);
	}
	
	nbytes = send(client_fd, &main_sync_addr, sizeof(main_sync_addr), 0);
	if(nbytes<0){
		perror("SYNC ESTUPIDO");
		exit(-1);
	}


	pthread_t recv_id;
	pthread_t send_id;
	pthread_create(&recv_id, NULL, up_recvt, client_fd);
	pthread_create(&send_id, NULL, up_sendt, client_fd);

	pthread_join(recv_id, NULL);
	pthread_cancel(send_id);

}
void * up_recvt(void * client_fd){
	char namesemup[100];
	sprintf(namesemup, "/semup_%d", getpid());
	stop_u = sem_open(namesemup,O_CREAT,0666,0);
	DATA remote_data;
	int check_recv, option;
	int nbytes, i;
	printf("ORA VIVA up_recvt!!\n\n");

	while(1){
		printf("DE NOVO AQUI!\n");
		check_recv = recv(client_fd, &remote_data, sizeof(remote_data), 0);
		if (check_recv == -1){
			perror("Reveiving remote data in UP: ");
			pthread_exit(0);
		}
		if(check_recv == 0){ //ligação terminada pelo clipboard remoto que se ligou
			printf("Clipboard remoto desconectado.\n");
			clips_down--;
			countsent--;
			pthread_exit(0);
		}
		rec_u=1;
		printf("Averificar se recebe\n");

		reg = remote_data.region;
		strcpy(data[reg],remote_data.characters);
		status[reg]=NOT_UPDATED;
		printf("Updated with remote data\n");	
		display_data(data);
		countsent = clips_down;
		if(clips_up == 1) { rec_d = 0; sem_post(stop_d);}
		else if(clips_down >= 1){
			rec_u = 0;
			for(i = 0; i < clips_down; i++)	sem_post(stop_u);
			printf("MAIS QUE 1!!!\n\n");
			
		}
		rec_u = 0;

		while(status[reg]==NOT_UPDATED);


	}
}
void * up_sendt(void * client_fd){

	DATA send_data;
	char namesemup[100];
	sprintf(namesemup, "/semup_%d", getpid());
	stop_u = sem_open(namesemup,O_CREAT,0666,0);

	int nbytes;
	int check_recv;
	printf("EU ESTOU AQUI up_sendt!!\n");
	while(1){
		sem_wait(stop_u);
		if (status[reg]==NOT_UPDATED && rec_u == 0 && clips_down > 0){
			strcpy(send_data.characters, data[reg]);
			send_data.region=reg;
			printf("as voltas\n");
			nbytes=send(client_fd, &send_data, sizeof(send_data), 0);
			if (nbytes==-1){
				perror("Sending: ");
			}
			printf("Enviados %d bytes\n",nbytes);
			// cena para averiguar que se mandou a todos os clips abaixo
			countsent--;
			printf("COUNTSENT%d\n", countsent);
			while(countsent>0);
			if(countsent == 0) status[reg] = UPDATED;
	

			printf("SAÍ DO CICLO!\n countsent - %d\n", countsent);
		}
	
	}
}
int check_mode(int argc, char *argv[])
{
	if (argc != 4)
		return OFF;
	else if(argc==4) /* CONNECTED MODE */
		return ON;

	return OFF;
}
char* makes_name_sem(char *sem){

	char *buf = malloc(40*sizeof(char));

	getcwd(buf, 40);
	
	char *buf2 = malloc(42*sizeof(char));

	memcpy(buf2, "--", 2);
	
	strcat(buf2,buf);
	free(buf);
	buf = malloc(100*sizeof(char));
	memcpy(buf, sem, sizeof(sem));
	strcat(buf, buf2);
	free(buf2);

	return buf;
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

















