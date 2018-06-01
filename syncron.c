#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "clip_lib.h"	
#include "clipboard.h"
#include <time.h>
#define SEM0 "/sem0"
#define SEM1 "/sem1"
#define SEM2 "/sem2"
#define SEM3 "/sem3"
#define SEM4 "/sem4"
#define SEM5 "/sem5"
#define SEM6 "/sem6"
#define SEM7 "/sem7"
#define SEM8 "/sem8"
#define SEM9 "/sem9"
sem_t *sems[10];
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

void sync(void){
	

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



	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;

	int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if (sock_fd == -1){
		perror("socket: ");
		return sock_fd;
	}

	client_addr.sun_family = AF_UNIX;
	sprintf(client_addr.sun_path, "%ssocket_%d", "./", getpid());

	server_addr.sun_family = AF_UNIX;
	sprintf(server_addr.sun_path, "%sSYNC_SOCKET", "./");

	int err_c = connect(sock_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));
	if(err_c==-1){
		perror("Connecting: ");
		return -1;
	}

	//===========Comunicação com o clipboard em singel mode========================



	
	struct sockaddr_in sync_addr;
	socklen_t size_addr;
	
	int recv_sock= socket(AF_INET, SOCK_STREAM, 0);

	sync_addr.sin_family = AF_INET;
	srandom(getpid());
	int port=random()%(6000+1-4001)+4001;
  	sync_addr.sin_port= htons(port);
	sync_addr.sin_addr.s_addr= INADDR_ANY; 

	//=========Enviar o endereço do sync====================
	int n = send(sock_fd, &sync_addr, sizeof(sync_addr), 0);
	if (n<=0){
		printf("CLIPBOARD NOT AVAILABLE\n");
		return 0;
	}
	//======================================================

	int err = bind(recv_sock,(struct sockaddr *)&sync_addr, sizeof(sync_addr));

	if(err == -1) {
    	perror("Sync sock bind");
    	exit(-1);
  	}
	pthread_t thread_id[100];
	pthread_t control_id;
	mkfifo("WAITING_0", 0666);
	fd_0=open("WAITING_0", O_RDONLY);
	if(fd ==-1){
		perror("open ");
		exit(-1);
	}
	if(listen(recv_sock, 5) == -1) {
		perror("listen");
		exit(-1);
	}
	int cnt=0;
	pthread_create(&control_id, NULL, control, fd_0);
	while(1){
		int client_fd= accept(recv_sock, (struct sockaddr *) & client_addr, &size_addr); //bloqueia até ter clientes
	
		if(client_fd == -1) {
			perror("accept");    
			exit(-1);
		}
		
		pthread_create(&(thread_id[cnt]), NULL, new_sync_app, client_fd);
		cnt++;
	
	}		

}

void * new_sync_app (arg *client_fd){

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
	int nbytes;
	int reg_sync;
	int permission = 0;
	pthread_id_np_t id;
	fd=open("WAITING", O_WRONLY);
	int t_id;
	t_id=id;
	if(fd ==-1){
		perror("open ");
		exit(-1);
	}
	while(1){
		
		nbytes= recv(client_fd, &reg_sync, sizeof(reg_sync), 0);
		
		write(fd, &t_id, sizeof(t_id));
		
		if (reg_sync==0){
			sem_wait(sem0);
			//read(f_0, pthread_id_np_t);
			permission=1;
			nbytes = send((client_fd, &permission, sizeof(int), 0));
			sem_post(sem0);
		}
		
		if (reg_sync==1) sem_wait(sem1);
		if (reg_sync==2) sem_wait(sem2);
		if (reg_sync==3) sem_wait(sem3);
		if (reg_sync==4) sem_wait(sem4);
		if (reg_sync==5) sem_wait(sem5);
		if (reg_sync==6) sem_wait(sem6);
		if (reg_sync==7) sem_wait(sem7);
		if (reg_sync==8) sem_wait(sem8);
		if (reg_sync==9) sem_wait(sem9);


	

	}

}
void * control (arg *fd){
	
	
	

}