#include "gettftp.h"

int main(int argc, char* argv[]){
	
	// Question 1
	// On vérifie que l'on a deux arguments en plus du programme à lancer
	if(argc==3){
		printf("Host : %s\nFile : %s\n", argv[1], argv[2]);
	}else{
		printf("Use : ./gettftp host file\n");
		exit(EXIT_FAILURE);
	}

	// Question 2
	struct addrinfo hints;
	struct addrinfo *results;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	
	// On spécifie le protocole que l'on veut utiliser pour la connexion
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = 0;
	
	int o = getaddrinfo(argv[1], "1069", &hints, &results);
	addrerror(o);
	
	// On vérifie ce que nous ressort la fonction, les deux premiers entiers représent bien le port (4*256 +445 = 1069) et les 4 suivant sont bien l'host
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
	printf("socket fd : %d\n\n", sock);
	
	// Question 4
	// On créé pour commencer la requête a envoyer au serveur
	int lenRRQ = 2 + strlen(argv[2]) + 2 + strlen("octet");
	char RRQ[lenRRQ];
	sprintf(RRQ, "01%s0octet0", argv[2]);
	printf("RRQ : %s\n", RRQ);
	
	int nSend = sendto(sock, RRQ, lenRRQ, 0, results->ai_addr, results->ai_addrlen);
	printf("Nombre de bits envoyé : %d\n", nSend);

	char buf[516];
	char ACK[5];
	int nRecv;
	
	//Tant que l'on reçoit des données de 512 bits du serveur, on demande la suivante et on fait son acquitement
	do{
		nRecv = recvfrom(sock, buf, sizeof(buf), 0, results->ai_addr, &(results->ai_addrlen));
		printf("Nombre de bits reçu : %d\n", nRecv);
		
		sprintf(ACK, "04%c%c", buf[2], buf[3]);
		nSend = sendto(sock, ACK, 4, 0, results->ai_addr, results->ai_addrlen);
	} while(nRecv == 516);
	
	close(sock);
	
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
		perror("socket ");
		exit(EXIT_FAILURE);
	}else{
		perror("socket ");
	}
}