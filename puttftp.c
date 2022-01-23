#include "puttftp.h"

int main(int argc, char* argv[]){
	
	// Question 1
	if(argc==3){
		printf("%s, %s\n", argv[1], argv[2]);
	}else{
		printf("Use : ./puttftp host file\n");
		exit(EXIT_FAILURE);
	}

	// Question 2
	struct addrinfo hints;
	struct addrinfo *results;
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = 0;
	
	int o = getaddrinfo(argv[1], "1069", &hints, &results);
	addrerror(o);

	printf("sa_family : %u\n",(results->ai_addr)->sa_family);
	printf("sa_data : ");
	int k;
	for(k=0;k<14;k++){
		printf("%u  ",((results->ai_addr)->sa_data)[k]);
	}
	printf("\n");
	
	// Question 3
	int sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	socketerror(sock);
	printf("socket fd: %d\n\n", sock);
	
	// Question 5
	// On indique au serveur que l'on va lui envoyer des données pour commencer
	int lenWRQ = 2 + strlen(argv[2]) + 2 + strlen("octet");
	char WRQ[lenWRQ];
	sprintf(WRQ, "02%s0octet0", argv[2]);
	printf("WRQ : %s\n", WRQ);
	
	int nSend = sendto(sock, WRQ, lenWRQ, 0, results->ai_addr, results->ai_addrlen);
	printf("Nombre de bits envoyé : %d\n", nSend);
	
	// On vérifie que le serveur acquitte notre demande
	char ack[4];
	int nRecv = recvfrom(sock, ack, sizeof(ack), 0, results->ai_addr, &(results->ai_addrlen));
	printf("Nombre de bits reçu : %d\n", nRecv);
	
	char msg[512];
	char DATA[516];
	int fd = open(argv[2], O_RDONLY);
	int nRead = read(fd, msg, 512);
	//Tant qu'on lit des bits du fichier à envoyer, on vérifie que le serveur acquitte de leur reception et on envoie les données suivantes
	while(nRead > 0){
		sprintf(DATA, "0301%s", msg);
		nSend = sendto(sock, DATA, 516, 0, results->ai_addr, results->ai_addrlen);
		printf("Nombre de bits envoyé : %d\n", nSend);
		
		nRecv = recvfrom(sock, ack, sizeof(ack), 0, results->ai_addr, &(results->ai_addrlen));
		printf("Nombre de bits reçu : %d\n, ack : %s", nRecv, ack);
		nRead = read(fd, msg, 512);
	} 
	
	close(sock);
	close(fd);
	
	return 1;
}

void addrerror(int o){
	if(o!=0){
		printf("getaddrinfo : %s\n", gai_strerror(o));
		exit(EXIT_FAILURE);
	}
}

void socketerror(int sock){
	if(sock==-1){
			perror("socket");
			exit(EXIT_FAILURE);
	}
}