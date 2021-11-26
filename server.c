#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 22000

void capt(int );
void principal(int );
int main(){
	//Primer socket en llegar
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	//Siguientes sockets en llegar
	int newSocket;
	struct sockaddr_in newAddr;

	//Direccion?
	socklen_t addr_size;

	char buffer[1024];
	pid_t hijo;
	char captcha[7];
	char cadena[100];
	char nombre[25];
	short int opcion;

	//Se crea el socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error en la conexion.\n");
		exit(1);
	}
	printf("[+]Servidor Socket creado con exito.\n");


	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error en la vinculacion.\n");
		exit(1);
	}

	if(listen(sockfd, 10) == 0){
		printf("[+]Escuchando....\n");
	}else{
		printf("[-]Error en la vinculacion.\n");
	}


	while(1){
		//Se solicita la conexion
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		//Se manda mensaje de bienvenida
		send(newSocket, "BIENVENIDO AL PROGRAMA DE VENTA DE BOLETOS DE UNA CENTRAL DE AUTOBUSES\n Ingresa tu nombre: ", 200, 0);
		//send(newSocket/*socket a donde se manda el mensaje*/, "BIENVENIDO AL PROGRMA DE VENTA DE BOLETOS DE UNA CENTRAL DE AUTOBUSES\n Ingresa tu nombre: "/*Mensaje*/, strlen(cadena)/*Tamano de mensaje*/, 0/*Permisos*/);
		if(newSocket < 0){
			exit(1);
		}
		//Se muestra que se logro la conexion
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		//Esta parte aun no la entiendo pero se que nos permitira tener varios clientes a la ves
		switch(hijo = fork())
		{
			case -1: printf("\nNo se pudo crear el proceso hijo");
					 break;
			case 0: principal(newSocket);
					break;
			default: close(newSocket);
					 break;
		}

	}
	return 0;
}

void capt(int socket)
{
	char buffer[1024];
	char cadena[100];
	char captcha[7];
	captcha[0]='A'+(rand()%26);
	captcha[1]='A'+(rand()%26);
	captcha[2]='A'+(rand()%26);
	captcha[3]='a'+(rand()%26);
	captcha[4]='0'+(rand()%10);
	captcha[5]='0'+(rand()%10);
	strcpy(cadena,captcha);
	send(socket, "Verifique que no es un robot ", 200, 0);
	send(socket, cadena, 6, 0);
	send(socket, "\nIngrese la cadena: ", 200, 0);
	recv(socket, buffer, 6, 0);
	if(strcmp(buffer, cadena) != 0)
	{
		send(socket, "1", 1, 0);
		while(strcmp(buffer, cadena) != 0){
			bzero(buffer, sizeof(buffer));
			send(socket, "\nError de verficacion", 200, 0);
			send(socket, "\nIngrese la cadena: ", 200, 0);
			recv(socket, buffer, 6, 0);
			if(strcmp(buffer, cadena) != 0)
				send(socket, "1", 1, 0);
			else
				send(socket, "0", 1, 0);
		}
	}
	else
		send(socket, "0", 1, 0);
	bzero(buffer, sizeof(buffer));
}

void principal(int socket)
{
	char nombres[50][50];
	char buffer[1024];
	int opc;
	recv(socket, nombres[0], 25, 0);
	bzero(buffer, sizeof(buffer));
	//Funcion del captcha
	capt(socket);
	send(socket, "\nVerificacion correcta", 200, 0);
	send(socket, "\n¿Con que linea desea viajar? \n1.Linea Futura \n2.ADO \n3.Estrella Blanca", 200, 0);
	send(socket, "\nSu respuesta: ", 100, 0);
	recv(socket, buffer, 1, 0);
	opc=atoi(buffer);
	switch(opc){
		case 1: send(socket,"\n Eligio Linea Futura",100,0);
				break;
	}
	close(socket);
}