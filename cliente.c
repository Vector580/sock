#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 22000
void xprintf(int socket,int salto);
void xscanf(int socket);
int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	char cadena[200];
	char captcha[6];
	char resp[2];


	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error en conexion.\n");
		exit(1);
	}
	printf("\n[+]socket de cliente creado.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error en conexion.\n");
		exit(1);
	}
	printf("\n[+]Conectado al servidor.\n");

	//Se recibe mensaje y proporcione su nombre
	xprintf(clientSocket,1);
	xscanf(clientSocket);


	//se pregunta por la validacion captcha
	xprintf(clientSocket,0);
	xprintf(clientSocket,0);
	xprintf(clientSocket,0);
	xscanf(clientSocket);
	recv(clientSocket, resp, 1, 0);
	//Validacion socket
	while(atoi(resp) != 0){
		bzero(resp,sizeof(resp));
		xprintf(clientSocket,0);
		xprintf(clientSocket,1);
		xscanf(clientSocket);
		recv(clientSocket, resp, 1, 0);
	}
	bzero(resp,sizeof(resp));
	//Verificacion correcta
	xprintf(clientSocket,1);
	//Menu de lineas
	xprintf(clientSocket,1);
	xprintf(clientSocket,0);
	xscanf(clientSocket);
	//Imprime la linea elegida
	xprintf(clientSocket,2);
	return 0;
}

void xprintf(int socket,int salto)
{
	char cadena[200];
	recv(socket, cadena, 200, 0);
	switch(salto)
	{
		case 0: printf("%s", cadena);
				break;
		case 1: printf("\n%s", cadena);
				break;
		case 2: printf("%s\n", cadena);
				break;
	}
	bzero(cadena, sizeof(cadena));
}

void xscanf(int socket)
{
	char buffer[1024];
	scanf("%s", &buffer[0]);
	send(socket, buffer, strlen(buffer), 0);
	bzero(buffer, sizeof(buffer));
}