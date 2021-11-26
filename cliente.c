#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 22009

int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	char cadena[200];
	char captcha[6];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error en conexion.\n");
		exit(1);
	}
	printf("[+]socket de cliente creado.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error en conexion.\n");
		exit(1);
	}
	printf("[+]Conectado al servidor.\n");
	//Se recibe mensaje
	recv(clientSocket/*Socket de donde viene el mensaje*/, cadena/*cadena donde guardarlo*/, 200/*Memoria maxima para guardar*/, 0);
	//Se muestra mensaje
	printf("\t%s\n", cadena);

	printf("Client: \t");
	scanf("%s", &buffer[0]);
	send(clientSocket, buffer, strlen(buffer), 0);
	bzero(cadena, sizeof(cadena));

	recv(clientSocket/*Socket de donde viene el mensaje*/, captcha/*cadena donde guardarlo*/, 6/*Memoria maxima para guardar*/, 0);

	//Se muestra mensaje
	printf("\t%s\n", captcha);
	printf("Client: \t");
	scanf("%s", &buffer[0]);
	send(clientSocket, buffer, 6, 0);
	
	while(strcmp(buffer,captcha) != 0){
				bzero(cadena, sizeof(cadena));
				recv(clientSocket/*Socket de donde viene el mensaje*/, cadena/*cadena donde guardarlo*/, 200/*Memoria maxima para guardar*/, 0);
				printf("\t%s\n", cadena);
				printf("Client: \t");
				scanf("%s", &buffer[0]);
				bzero(cadena, sizeof(cadena));
				send(clientSocket, buffer, 6, 0);
				
			}
	recv(clientSocket/*Socket de donde viene el mensaje*/, cadena/*cadena donde guardarlo*/, 200/*Memoria maxima para guardar*/, 0);
	printf("\t%s\n", cadena);
	bzero(cadena, sizeof(cadena));

	recv(clientSocket/*Socket de donde viene el mensaje*/, cadena/*cadena donde guardarlo*/, 200/*Memoria maxima para guardar*/, 0);
	//Se muestra mensaje
	printf("\t%s\n", cadena);
	printf("Client: \t");
	scanf("%s", &buffer[0]);
	send(clientSocket, buffer, strlen(buffer), 0);

	bzero(cadena, sizeof(cadena));

	recv(clientSocket/*Socket de donde viene el mensaje*/, cadena/*cadena donde guardarlo*/, 200/*Memoria maxima para guardar*/, 0);
	//Se muestra mensaje
	printf("\t%s\n", cadena);

	



	return 0;
}
