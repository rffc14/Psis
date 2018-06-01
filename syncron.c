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

	



	
	struct sockaddr_in sync_addr;
	socklen_t size_addr;
	
	int recv_sock= socket(AF_INET, SOCK_STREAM, 0);

	sync_addr.sin_family = AF_INET;
	srandom(getpid());
	int port=random()%(6000+1-4001)+4001;
  	sync_addr.sin_port= htons(port);
	sync_addr.sin_addr.s_addr= INADDR_ANY; 

	
	int n = send(sock_fd, &sync_addr, sizeof(sync_addr), 0);
	if (n<=0){
		printf("CLIPBOARD NOT AVAILABLE\n");
		return 0;
	}


	int err = bind(recv_sock,(struct sockaddr *)&sync_addr, sizeof(sync_addr));

	if(err == -1) {
    	perror("Sync sock bind");
    	exit(-1);
  	}
	

	if(listen(recv_sock, 5) == -1) {
		perror("listen");
		exit(-1);
	}

	while(1){
		int client_fd= accept(recv_sock, (struct sockaddr *) & client_addr, &size_addr); //bloqueia até ter clientes
	
		if(client_fd == -1) {
			perror("accept");    
			exit(-1);
		}

		



	}		
}













}
