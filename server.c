#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

//Memoria compartida
#include <sys/mman.h>
#include <sys/shm.h>


#define PORT 22000

typedef struct{
	int horarios[10];
	int bus[10][10];
}linea;
void capt(int );
void principal(int ,linea *);
void opcion(int ,linea *,int ,char *);
void inicia(linea *);
int Hdispo(linea *,int,int);
int Adispo(linea *,int,int ,int );
void reservar(linea *,int ,int ,int );
int cuenta(linea *,int,int);
void imprimeBus(linea *,int ,int ,int);
void refrescar(linea *,int ,int);
int main(){
	//Primer socket en llegar
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	//Siguientes sockets en llegar
	int newSocket;
	struct sockaddr_in newAddr;

	//Direccion?
	socklen_t addr_size;
	pid_t hijo;
	

//---------------------------------Memoria compartida--------------------------------------
	key_t Clave;
  int Id_Memoria;
  linea *buses = NULL;

  Clave = ftok ("/bin/ls", 33);//se genera la clase
  if (Clave == -1)
  {
  	printf("\nNo consigo clave para memoria compartida");
  	exit(0);
  }
  //Se obtiene el espacio de memoria
  Id_Memoria = shmget (Clave, sizeof(linea)*3, 0777 | IPC_CREAT);
  if (Id_Memoria == -1)
  {
  	printf("\nNo consigo Id para memoria compartida");
    exit (0);
  }
  //Se enlazan
  buses = (linea *)shmat (Id_Memoria, (char *)0, 0);
  if (buses == NULL)
  {
  	printf("No consigo memoria compartida");
  	exit (0);
  }
  //------------------------------------------------------------------------------------

	inicia(buses);
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
			case 0: principal(newSocket,buses);
					break;
			default: close(newSocket);
					 break;
		}

	}	

	//Memoria compartida
	shmdt ((char *)buses);
  shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);

	return 0;
}

void capt(int socket)
{
	char buffer[1024];
	char cadena[100];
	char captcha[7];
	time_t t;
	srand((unsigned)time(&t));
	captcha[0]='A'+(rand()%26);
	captcha[1]='A'+(rand()%26);
	captcha[2]='A'+(rand()%26);
	captcha[3]='a'+(rand()%26);
	captcha[4]='0'+(rand()%10);
	captcha[5]='0'+(rand()%10);
	strcpy(cadena,captcha);
	send(socket, "Verifique que no es un robot ", 200, 0);
	send(socket, cadena, 6, 0);
	send(socket, "Ingrese la cadena:  ", 200, 0);
	recv(socket, buffer, 6, 0);
	if(strcmp(buffer, cadena) != 0)
	{
		send(socket, "1", 1, 0);
		while(strcmp(buffer, cadena) != 0){
			bzero(buffer, sizeof(buffer));
			send(socket, "Error de verficacion", 200, 0);
			send(socket, "Ingrese la cadena:  ", 200, 0);
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

void principal(int socket,linea *A)
{
	char cliente[200];//nombre del cliente
	char buffer[2];
	int opc;
  //Se manda mensaje de bienvenida
	send(socket, "BIENVENIDO AL PROGRAMA DE VENTA DE BOLETOS DE UNA CENTRAL DE AUTOBUSES", 200, 0);
	send(socket, "Ingresa tu nombre: ", 200, 0);
	recv(socket, cliente, 200, 0);
	bzero(buffer, sizeof(buffer));
	//Funcion del captcha
	capt(socket);
	send(socket, "Verificacion correcta", 200, 0);
	do
	{
		send(socket, "¿Con que linea desea viajar?", 200, 0);
		send(socket, "1.Linea Futura", 200, 0);
		send(socket, "2.ADO ", 200, 0);
		send(socket, "3.Estrella Blanca", 200, 0);
		send(socket, "4.Salir", 200, 0);
		send(socket, "Su respuesta: ", 100, 0);
		recv(socket, buffer, 1, 0);
		opc=atoi(buffer);
		switch(opc)
		{
			case 1: send(socket,"Eligio Linea Futura",200,0);
							opcion(socket,A,0,cliente);
							send(socket,"Gracias por comprar en Linea Futura :)",200,0);
							break;
			case 2: send(socket,"Eligio ADO",200,0);
							opcion(socket,A,1,cliente);
							send(socket,"Gracias por comprar en ADO :)",200,0);
							break;
			case 3: send(socket,"Eligio Estrella Blanca",200,0);
							opcion(socket,A,2,cliente);
							send(socket,"Gracias por comprar en Estrella Blanca :)",200,0);
							break;
			case 4: send(socket,"Gracias por usar el programa :)",200,0);
							break;
			default: send(socket,"Error, opcion invalida",200,0);
		}
		bzero(buffer, sizeof(buffer));
	}while(opc!=4);
	close(socket);
}

void opcion(int socket,linea *A,int n,char * clien)
{
	char amigos[9][200];
	char buffer[200];
	char aux[200];
	char letre[]="Proporcione nombre del acompañante  :";
	int hora,asien,band,cant,i,suma=0;
	int edad[10][3];
	char h1[]="1)7 a.m.  - Xalapa\0";
	char h2[]="2)8 a.m.  - Perote\0";
	char h3[]="3)9 a.m.  - Tlaxcala\0";
	char h4[]="4)10 p.m. - Veracruz\0";
	char h5[]="5)11 a.m. - Cuernavaca\0";
	char h6[]="6)1 p.m.  - Teziutlan\0";
	char h7[]="7)2 p.m.  - Guerrero\0";
	char h8[]="8)3 p.m.  - Jalisco\0";
	char h9[]="9)4 p.m.  - Monterrey\0";
	char h10[]="10)5 p.m. - Ciudad de Mexico\0";
	do
	{
		send(socket, "Elija un horario:\0", 200, 0);
		send(socket, h1, 200, 0);
		send(socket, h2, 200, 0);
		send(socket, h3, 200, 0);
		send(socket, h4, 200, 0);
		send(socket, h5, 200, 0);
		send(socket, h6, 200, 0);
		send(socket, h7, 200, 0);
		send(socket, h8, 200, 0);
		send(socket, h9, 200, 0);
		send(socket, h10, 200, 0);
		send(socket, "Su respuesta: \0", 200, 0);
		recv(socket, buffer, 1, 0);
		hora=atoi(buffer);
		bzero(buffer, sizeof(buffer));
		hora--;
		band=Hdispo(A,n,hora);
		if(band!=0)
		{
			send(socket,"1",1,0);
			send(socket, "Cupo lleno en ese horario", 200, 0);
		}
	}while(band!=0);
	send(socket,"0",1,0);
	send(socket, "Horario disponible", 200, 0);
	send(socket, "El costo del boleto es de $52", 200, 0);
	send(socket, "(Niños menores de 12 y Adultos mayores de 60 pagan mitad)", 200, 0);
	do
	{
		band=0;
		send(socket,"Estos son los lugares disponibles: ",200,0);
		imprimeBus(A,n,hora,socket);
		send(socket,"Cantidad de boletos que comprara (incluido usted): ",200,0);
		recv(socket, buffer, 1, 0);
		cant=atoi(buffer);
		bzero(buffer, sizeof(buffer));
		refrescar(A,n,hora);
		if(cant>cuenta(A,n,hora))
		{
			band=1;
			send(socket,"1",1,0);
			send(socket, "No hay suficientes asientos", 200, 0);
		}
	}while(band!=0);
	send(socket,"0",1,0);
	sprintf(aux,"%d",cant);
	send(socket,aux,1,0);

	if(cant>1)
	{
		send(socket, "Proporcione su edad: ", 200, 0);
		recv(socket, buffer, 4, 0);
		edad[0][0]=atoi(buffer);
		bzero(buffer, sizeof(buffer));
		if(edad[0][0]<12 || edad[0][0]>60)
			edad[0][1]=26;
		else
			edad[0][1]=52;
		imprimeBus(A,n,hora,socket);
		do
		{
			send(socket, "Proporcione el numero de asiento : ", 200, 0);
			recv(socket, buffer, 4, 0);
			edad[0][2]=atoi(buffer);
			bzero(buffer, sizeof(buffer));
			band=Adispo(A,n,hora,edad[0][2]-1);
			if(band!=0)
			{
				send(socket,"1",1,0);
				send(socket, "Asiento no disponible", 200, 0);
			}
		}while(band!=0);
		send(socket,"0",1,0);
		reservar(A,n,hora,edad[0][2]-1);
		for(i=0;i<cant-1;i++)
		{
			letre[37]=i+1+'0';
			letre[38]=':';
			send(socket, letre, 200, 0);
			recv(socket, amigos[i], 200, 0);
			send(socket, "Proporcione edad: ", 200, 0);
			recv(socket, buffer, 4, 0);
			edad[i+1][0]=atoi(buffer);
			bzero(buffer, sizeof(buffer));
			if(edad[i+1][0]<12 || edad[i+1][0]>60)
				edad[i+1][1]=26;
			else
				edad[i+1][1]=52;
			imprimeBus(A,n,hora,socket);
			do
			{
				send(socket, "Proporcione el numero de asiento : ", 200, 0);
				recv(socket, buffer, 4, 0);
				edad[i+1][2]=atoi(buffer);
				bzero(buffer, sizeof(buffer));
				band=Adispo(A,n,hora,edad[i+1][2]-1);
				if(band!=0)
				{
					send(socket,"1",1,0);
					send(socket, "Asiento no disponible", 200, 0);
				}
			}while(band!=0);
			send(socket,"0",1,0);
			reservar(A,n,hora,edad[i+1][2]-1);
		}
		send(socket,"Usted compro los siguientes boletos: ",200,0);
		send(socket,"Nombre: \0",200,0);
		send(socket,clien,200,0);
		send(socket,"Edad: ",200,0);
		sprintf(aux,"%d",edad[0][0]);
		//bzero(buffer, sizeof(buffer));
		send(socket,aux,200,0);
		send(socket,"Asiento: ",200,0);
		sprintf(aux,"%d",edad[0][2]);
		send(socket,aux,200,0);
		
		send(socket,"costo: ",200,0);
		sprintf(aux,"%d",edad[0][1]);
		send(socket,aux,200,0);
		
		suma+=edad[0][1];
		for(i=0;i<cant-1;i++)
		{
			send(socket,"Nombre: ",200,0);
			send(socket,amigos[i],200,0);
			send(socket,"Edad: ",200,0);
			sprintf(aux,"%d",edad[i+1][0]);
			send(socket,aux,200,0);
			send(socket,"Asiento: ",200,0);
			sprintf(aux,"%d",edad[i+1][2]);
			send(socket,aux,200,0);
			send(socket,"Costo: ",200,0);
			sprintf(aux,"%d",edad[i+1][1]);
			send(socket,aux,200,0);
			suma+=edad[i+1][1];
		}
		send(socket,"Costo total: $",200,0);
		sprintf(aux,"%d",suma);
		send(socket,aux,200,0);
		send(socket,"Con horario y destino: \0",200,0);
		switch(hora)
		{
			case 0: send(socket, h1, 200, 0);
							break;
			case 1: send(socket, h2, 200, 0);
							break;
			case 2: send(socket, h3, 200, 0);
							break;
			case 3: send(socket, h4, 200, 0);
							break;
			case 4: send(socket, h5, 200, 0);
							break;
			case 5: send(socket, h6, 200, 0);
							break;
			case 6: send(socket, h7, 200, 0);
							break;
			case 7: send(socket, h8, 200, 0);
							break;
			case 8: send(socket, h9, 200, 0);
							break;
			case 9: send(socket, h10, 200, 0);
							break;
		}
	}
	else
	{
		send(socket, "Proporcione su edad: ", 200, 0);
		recv(socket, buffer, 4, 0);
		edad[0][0]=atoi(buffer);
		bzero(buffer, sizeof(buffer));
		imprimeBus(A,n,hora,socket);
		if(edad[0][0]<12 || edad[0][0]>60)
			edad[0][1]=26;
		else
			edad[0][1]=52;
		do
		{
			send(socket, "Proporcione el numero de asiento : ", 200, 0);
			recv(socket, buffer, 4, 0);
			edad[0][2]=atoi(buffer);
			bzero(buffer, sizeof(buffer));
			band=Adispo(A,n,hora,edad[0][2]-1);
			sprintf(aux,"%d",band);
			send(socket,aux,1,0);
			if(band!=0)
				send(socket, "Asiento no disponible", 200, 0);
		}while(band!=0);
		reservar(A,n,hora,edad[0][2]-1);
		
		send(socket,"Usted compro el siguientes boleto: ",200,0);
		send(socket,"Nombre: \0",200,0);
	
		send(socket,clien,200,0);
		send(socket,"Edad: ",200,0);
		sprintf(aux,"%d",edad[0][0]);
		send(socket,aux,200,0);
	
		send(socket,"Asiento: ",200,0);
		sprintf(aux,"%d",edad[0][2]);
		send(socket,aux,200,0);
		
		send(socket,"costo: ",200,0);
		sprintf(aux,"%d",edad[0][1]);
		send(socket,aux,200,0);
		
		send(socket,"Costo total: $",200,0);
		sprintf(aux,"%d",edad[0][1]);
		send(socket,aux,200,0);

		send(socket,"Con horario y destino: \0",200,0);
		switch(hora)
		{
			case 0: send(socket, h1, 200, 0);
							break;
			case 1: send(socket, h2, 200, 0);
							break;
			case 2: send(socket, h3, 200, 0);
							break;
			case 3: send(socket, h4, 200, 0);
							break;
			case 4: send(socket, h5, 200, 0);
							break;
			case 5: send(socket, h6, 200, 0);
							break;
			case 6: send(socket, h7, 200, 0);
							break;
			case 7: send(socket, h8, 200, 0);
							break;
			case 8: send(socket, h9, 200, 0);
							break;
			case 9: send(socket, h10, 200, 0);
							break;
		}
		
	}
	//send(socket,"Autobus actual:",200,0);
	imprimeBus(A,n,hora,socket);
}

//Iniciaiza toda la estructura con 0
void inicia(linea *A)
{
	int h,i,j;
	for(h=0;h<3;h++)
	{
		for(i=0;i<10;i++)
		{
			A[h].horarios[i]=0;
			for(j=0;j<10;j++)
				A[h].bus[i][j]=0;
		}
	}
}

//Verifica si el horario esta disponible
int Hdispo(linea *A,int n,int h)
{
	return(A[n].horarios[h]);
}

//Verifica si un asiento esta disponble
int Adispo(linea *A,int n,int h,int a)
{
	return(A[n].bus[h][a]);
}

//Reserva un asiento
void reservar(linea *A,int n,int h,int a)
{
	A[n].bus[h][a]=1;
}

//Retorna el total de asientos libre
int cuenta(linea *A,int n,int h)
 {
 	int cont=0,i;
 	for(i=0;i<10;i++)
 	{
 		if(Adispo(A,n,h,i)==0)
 			cont++;
 	}
 	return cont;
 }

//Revisa todos los asientos y determina si hay libres o no
void refrescar(linea *A,int n,int h)
{
	int j=0;
	A[n].horarios[h]=1;
	while(A[n].horarios[h]==1 && j<10)
	{
		if(A[n].bus[h][j]==0)
			A[n].horarios[h]=0;
		else
			j++;
	}
}

//Imprime los asientos del autobus

void imprimeBus(linea *A,int n,int h,int socket)
{
	int i;
	int p[]={3,10,17,10,17,10,17,3,10,17};
	char l1[]={'1','.','[',' ',']',' ',' ','2','.','[',' ',']',' ',' ','3','.','[',' ',']',' ',' ','\0'};
	char l2[]={' ',' ',' ',' ',' ',' ',' ','4','.','[',' ',']',' ',' ','5','.','[',' ',']',' ',' ','\0'};
	char l3[]={' ',' ',' ',' ',' ','p','a','s','i','l','l','o','\0'};
	char l4[]={' ',' ',' ',' ',' ',' ',' ','6','.','[',' ',']',' ',' ','7','.','[',' ',']',' ',' ','\0'};
	char l5[]={'8','.','[',' ',']',' ',' ','9','.','[',' ',']',' ','1','0','.','[',' ',']',' ',' ','\0'};
	
	for(i=0;i<10;i++)
	{
		if(i<3)
		{
			if(A[n].bus[h][i]==0)
				l1[p[i]]='0';
			else
				l1[p[i]]='1';
		}
		else
		{
			if(i==3 || i==4)
			{
				if(A[n].bus[h][i]==0)
					l2[p[i]]='0';
				else
					l2[p[i]]='1';
			}
			else
			{
				if(i==5 || i==6)
				{
					if(A[n].bus[h][i]==0)
						l4[p[i]]='0';
					else
						l4[p[i]]='1';
				}
				else
				{
					if(A[n].bus[h][i]==0)
						l5[p[i]]='0';
					else
						l5[p[i]]='1';
				}
			}
		}
	}
	send(socket, l1, 200, 0);
	send(socket, l2, 200, 0);
	send(socket, l3, 200, 0);
	send(socket, l4, 200, 0);
	send(socket, l5, 200, 0);
}