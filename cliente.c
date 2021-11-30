#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <ncurses.h>

#define PORT 22000

void xprintf(int socket,int salto);
void nprintf(int socket, WINDOW *pun);
void mprintf(int socket,int col, int fil,int color,int estilo);
void mcprintf(int socket,int col, int fil,int color,int estilo, int tam);
int returnmscanf(int socket,int col, int fil);
void mscanf(int socket,int col, int fil);
int rscanf(int socket,int col, int fil);
void xscanf(int socket);
void limpia(int col, int fila);
void imprimeBus(int clientSocket, int col, int fila, int color);
void principal(int socket);
void opcion(int clientSocket,WINDOW *punt_ven,int color);

int main(){
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	char cadena[200];
	char captcha[6];
	char resp[2];
	char ncliente[2];
	
	int xmax,ymax;
	
	 WINDOW *punt_ven;

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
	initscr();//Se inicia el modo ncurse
	
	getmaxyx(stdscr,ymax,xmax);
 	
 	if (has_colors())
   		start_color();//Inicia modo color

 	init_pair(1,COLOR_RED,COLOR_GREEN);//Paleta de colores

		init_pair(2,COLOR_BLUE,COLOR_GREEN);
		init_pair(3,COLOR_BLACK,COLOR_WHITE);
		init_pair(4,COLOR_WHITE,COLOR_BLACK);
		init_pair(5,COLOR_BLUE,COLOR_WHITE);
		init_pair(6,COLOR_RED,COLOR_WHITE);
		init_pair(7,COLOR_MAGENTA,COLOR_WHITE);
		init_pair(8,COLOR_WHITE,COLOR_BLUE);
		init_pair(9,COLOR_WHITE,COLOR_RED);
		init_pair(10,COLOR_WHITE,COLOR_MAGENTA);
		bkgd(COLOR_PAIR(3));
		principal(clientSocket);
		endwin();
		system("clear");
}



void principal(int clientSocket)
{
	char cliente[25];
	char buffer[2];
	int opc;
	char resp[2];
	WINDOW *punt_ven;
	int color_menu;

	attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
 	//Configuracion de subventana para la bienvenida
 	punt_ven=subwin(stdscr,3,72,1,20);
 	box(punt_ven,ACS_VLINE,ACS_HLINE);

 	//Se recibe mensaje 
	nprintf(clientSocket,punt_ven);
	attroff(COLOR_PAIR(4)| A_BOLD);
	
	
 	//Recibe peticion de nombre
 	mprintf(clientSocket,12,30,3,0);
 	mscanf(clientSocket,12,50);
 	
 	//REcibe validacion de captcha
	mprintf(clientSocket,14,30,3,0);
	mcprintf(clientSocket,14,60,4,1,6);//Aqui se manda el captcha


	mprintf(clientSocket,16,30,3,0);
	mscanf(clientSocket,16,50);
	recv(clientSocket, resp, 1, 0);
	//Validacion socket
	while(atoi(resp) != 0){
		bzero(resp,sizeof(resp));
		limpia(16,30);
		mprintf(clientSocket,16,30,3,0);
		getch();

		mprintf(clientSocket,16,30,3,0);
		mscanf(clientSocket,16,50);
		recv(clientSocket, resp, 1, 0);

	
	}
		bzero(resp,sizeof(resp));

	//Verificacion correcta
	mprintf(clientSocket,18,30,3,0); 
	getch();
	do
	{
 		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu
		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"------------MENU------------");
 		//Se recibe mensaje 
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,20);
		opc=0;
		mprintf(clientSocket,10,10,3,1);//Pregunta la linea
		//Muetra las lineas
		mprintf(clientSocket,12,10,8,1);
		mprintf(clientSocket,14,10,9,1);
		mprintf(clientSocket,16,10,10,1);
		mprintf(clientSocket,18,10,4,1);
		mprintf(clientSocket,20,10,3,1);//su respuesta es
		opc=rscanf (clientSocket,20,25); // Se lee la repuesta
		color_menu=opc+4;
		if(opc>0 && opc<4)
			opc=1;
		switch(opc)
		{
			case 1: mprintf(clientSocket,22,10,1,1);
					getch();
					opcion(clientSocket,punt_ven,color_menu);
					break;
			case 4: mprintf(clientSocket,22,30,1,1);
					clear();
					wrefresh(punt_ven);
					delwin(punt_ven);
					break;
			default:mprintf(clientSocket,22,10,1,1);
		}
		getch();
	}while(opc!=4);
	close(clientSocket);
}

void opcion(int clientSocket,WINDOW *punt_ven, int color)
{
	char band[2];
	char band_cantidad[2];
	do
	{
		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu

		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"---------MENU LINEA---------");
 	
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		//punt_ven=subwin(stdscr,3,30,1,20);
		mprintf(clientSocket,10,10,color,1);//elija un horatio
	
		mprintf(clientSocket,12,10,color,1);//HOrarios
		mprintf(clientSocket,13,10,color,1);//HOrarios
		mprintf(clientSocket,14,10,color,1);//HOrarios
		mprintf(clientSocket,15,10,color,1);//HOrarios
		mprintf(clientSocket,16,10,color,1);//HOrarios
		mprintf(clientSocket,17,10,color,1);//HOrarios
		mprintf(clientSocket,18,10,color,1);//HOrarios
		mprintf(clientSocket,19,10,color,1);//HOrarios
		mprintf(clientSocket,20,10,color,1);//HOrarios
		mprintf(clientSocket,21,10,color,1);//HOrarios

		mprintf(clientSocket,23,10,color,1);//Su repuesta es:
		mscanf (clientSocket,23,25);//Se manda respuesta
		recv(clientSocket,band,1,0);//Se recibe bandera
		if(atoi(band)!=0)
			mprintf(clientSocket,25,10,color,1);
	}while(atoi(band)!=0);

	mprintf(clientSocket,27,10,color,1);//Horario disponible
	mprintf(clientSocket,29,10,color,1);//El costo es 
	mprintf(clientSocket,31,10,color,1);//Niños menores
	do{
		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu

		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"---------MENU LINEA---------");
 	
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		mprintf(clientSocket,10,10,color,1);//Lugares disponible
		imprimeBus(clientSocket,12,10,color);
		mprintf(clientSocket,18,10,color,1);//Pregunta de cantidad
		mscanf(clientSocket,18,60);
		recv(clientSocket,band,1,0);//Se recibe bandera
		if(atoi(band)!=0)
			mprintf(clientSocket,20,10,color,1);//No hay suficiente asientos
	}while(atoi(band)!=0);
	recv(clientSocket,band_cantidad,1,0);
	mvprintw(1,1,band_cantidad);
	if(atoi(band_cantidad)>1)
	{
		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu
		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"---------MENU LINEA---------");
 	
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta

		mprintf(clientSocket,5,10,color,1);//Proporcione su edad
		mscanf(clientSocket,5,33);//Se lee la edad
		imprimeBus(clientSocket,7,10,color);
		do{
			mprintf(clientSocket,14,10,color,1);//propocione el umero de asiento
			mscanf(clientSocket,14,45);
			recv(clientSocket,band,1,0);
			if(atoi(band)!=0)
				mprintf(clientSocket,12,10,color,1);//No hay suficiente asientos
		}while(atoi(band)!=0);

		for(int i=0;i<atoi(band_cantidad)-1;i++)
		{
			wrefresh(punt_ven);//Se refresca para mostrar
			clear();//Se limpia para psar al menu

			attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
			punt_ven=subwin(stdscr,3,30,1,40);
 			box(punt_ven,ACS_VLINE,ACS_HLINE);
 			mvwprintw(punt_ven,1,1,"---------MENU LINEA---------");
 	
 			attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta

			mprintf(clientSocket,5,10,color,1);//Proporcione nombre de compañante
			mscanf(clientSocket,5,50);//Se lee el nombre
			mprintf(clientSocket,7,10,color,1);//Se pide edad
			mscanf(clientSocket,7,28);//se lee la edad
			imprimeBus(clientSocket,9,10,color);
			
			do{
				mprintf(clientSocket,15,10,color,1);//Se pide numero de asiento
				mscanf(clientSocket,15,44);
				recv(clientSocket,band,1,0);
				if(atoi(band)!=0)
				mprintf(clientSocket,17,10,color,1);//No hay suficiente asientos
			}while(atoi(band)!=0);

		}
		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu

		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"-----------RECIBO-----------");
 	
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta

		mprintf(clientSocket,7,10,color,1);//Usted compro los sig boletos
		mprintf(clientSocket,9,10,color,1);//nombre de clientes
		mprintf(clientSocket,9,18,color,1);//nombre de clientes
		mprintf(clientSocket,11,10,color,1);//edad:
		mprintf(clientSocket,11,16,color,1);//edad:
			
		mprintf(clientSocket,13,10,color,1);//Asiento:
		mprintf(clientSocket,13,20,color,1);//Asiento:
			
		mprintf(clientSocket,15,10,color,1);//costo:
		mprintf(clientSocket,15,16,color,1);//costo:
			
		getch();

		for(int i=0;i<atoi(band_cantidad)-1;i++)
		{

			wrefresh(punt_ven);//Se refresca para mostrar
			clear();//Se limpia para psar al menu

			attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
			punt_ven=subwin(stdscr,3,30,1,40);
	 		box(punt_ven,ACS_VLINE,ACS_HLINE);
	 		
 			mvwprintw(punt_ven,1,1,"-----------RECIBO-----------");
 	
	 	
	 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta

			
			mprintf(clientSocket,7,10,color,1);//Usted compro los sig boletos
			mprintf(clientSocket,7,18,color,1);//nombre de clientes
			mprintf(clientSocket,9,10,color,1);//edad:
			mprintf(clientSocket,9,16,color,1);//edad:
			
			mprintf(clientSocket,11,10,color,1);//Asiento:
			mprintf(clientSocket,11,20,color,1);//Asiento:

			
			mprintf(clientSocket,13,10,color,1);//costo:
			mprintf(clientSocket,13,16,color,1);//costo:
			getch();
		}
		mprintf(clientSocket,15,10,color,1);//costo total:
		mprintf(clientSocket,15,26,color,1);//costo

		mprintf(clientSocket,17,10,color,1);//costo total:
		mprintf(clientSocket,17,36,color,1);//costo

		getch();

	}	
	else
	{
		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu

		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"---------MENU LINEA---------");
 	
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta

		mprintf(clientSocket,5,10,color,1);//Proporcione su edad
		mscanf(clientSocket,5,33);//Se lee la edad
		imprimeBus(clientSocket,7,10,color);
		//mprintf(clientSocket,7,10,color,1); //se muestra el bus
		do{
			mprintf(clientSocket,14,10,color,1);//propocione el umero de asiento
			mscanf(clientSocket,14,45);
			recv(clientSocket,band,1,0);
			if(atoi(band)!=0)
				mprintf(clientSocket,16,10,color,1);//asineto no disponible
		}while(atoi(band)!=0);

		wrefresh(punt_ven);//Se refresca para mostrar
		clear();//Se limpia para psar al menu

		attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
		punt_ven=subwin(stdscr,3,30,1,40);
 		box(punt_ven,ACS_VLINE,ACS_HLINE);
 		mvwprintw(punt_ven,1,1,"-----------RECIBO-----------");
 	
 		attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta

		mprintf(clientSocket,7,10,color,1);//Usted compro los sig boletos
		mprintf(clientSocket,9,10,color,1);//nombre de clientes
		mprintf(clientSocket,9,20,color,1);//nombre de clientes
		mprintf(clientSocket,11,10,color,1);//edad:
		mprintf(clientSocket,11,16,color,1);//edad:
			
		mprintf(clientSocket,13,10,color,1);//Asiento:
		mprintf(clientSocket,13,20,color,1);//Asiento:
			
		mprintf(clientSocket,15,10,color,1);//costo:
		mprintf(clientSocket,15,16,color,1);//costo:
		mprintf(clientSocket,17,10,color,1);//costo total:
		mprintf(clientSocket,17,26,color,1);//costo :

		mprintf(clientSocket,19,10,color,1);//costo total:
		mprintf(clientSocket,19,36,color,1);//costo
		getch();
	}	

	wrefresh(punt_ven);//Se refresca para mostrar
	clear();//Se limpia para psar al menu

	attron(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
	punt_ven=subwin(stdscr,3,30,1,40);
 	box(punt_ven,ACS_VLINE,ACS_HLINE);
 	mvwprintw(punt_ven,1,1,"-------Autobus actual-------");
 	
 	attroff(COLOR_PAIR(4)|A_BOLD); //Se activa esa paleta
 		
 	imprimeBus(clientSocket,5,10,color);
 	mprintf(clientSocket,15,26,color,1);//costo :
 	getch();
}

void imprimeBus(int clientSocket, int col, int fila, int color){
	mprintf(clientSocket,col,fila,color,1);
	mprintf(clientSocket,col+1,fila,color,1);
	mprintf(clientSocket,col+2,fila,color,1);
	mprintf(clientSocket,col+3,fila,color,1);
	mprintf(clientSocket,col+4,fila,color,1);
}

/*funcion para imprimir con ncurses
//REcibe:columna,fila,,color y estilo*/
void mprintf(int socket,int col, int fil,int color,int estilo){
	char cadena[200];
	recv(socket, cadena, 200, 0);

		//Pareja de colores
		start_color();
 		init_pair(1,COLOR_RED,COLOR_GREEN);
		init_pair(2,COLOR_BLUE,COLOR_GREEN);
		init_pair(3,COLOR_BLACK,COLOR_WHITE);
		

		//estilos
		switch(estilo)
		{
			case 0: attron(COLOR_PAIR(color));
					mvprintw(col,fil,cadena);
					attroff(COLOR_PAIR(color));
					break;
			case 1: attron(COLOR_PAIR(color)|A_BOLD);
					mvprintw(col,fil,cadena);
					attroff(COLOR_PAIR(color)| A_BOLD);
					break;
			case 2: attron(COLOR_PAIR(color)|A_UNDERLINE);
					mvprintw(col,fil,cadena);
					attroff(COLOR_PAIR(color)| A_UNDERLINE);
					break;
		}
	
	bzero(cadena, sizeof(cadena));
}

/*funcion para imprimir con ncurses
//REcibe:columna,fila,,color y estilo*/
void mcprintf(int socket,int col, int fil,int color,int estilo, int tam){
	char cadena[200];
	recv(socket, cadena, tam, 0);

		//Pareja de colores
		start_color();
 		init_pair(1,COLOR_RED,COLOR_GREEN);
		init_pair(2,COLOR_BLUE,COLOR_GREEN);
		init_pair(3,COLOR_BLACK,COLOR_WHITE);
		

		//estilos
		switch(estilo)
		{
			case 0: attron(COLOR_PAIR(color));
					mvprintw(col,fil,cadena);
					attroff(COLOR_PAIR(color));
					break;
			case 1: attron(COLOR_PAIR(color)|A_BOLD);
					mvprintw(col,fil,cadena);
					attroff(COLOR_PAIR(color)| A_BOLD);
					break;
			case 2: attron(COLOR_PAIR(color)|A_UNDERLINE);
					mvprintw(col,fil,cadena);
					attroff(COLOR_PAIR(color)| A_UNDERLINE);
					break;
		}
	
	bzero(cadena, sizeof(cadena));
}


//Imprime en cajas de ncurses
//Solo lo uso para la bienvenida
void nprintf(int socket, WINDOW *pun)
{
	char cadena[200];
	recv(socket, cadena, 200, 0);
	
		mvwprintw(pun,1,1,cadena);
		

	bzero(cadena, sizeof(cadena));
}

//Leer en ncurses
void mscanf(int socket,int col, int fil)
{
	char buffer[1024];
	move(col,fil);
	scanw("%s",&buffer[0]);

	send(socket, buffer, strlen(buffer), 0);
}

//Leer en ncurses
int rscanf(int socket,int col, int fil)
{
	char buffer[1024];
	move(col,fil);
	scanw("%s",&buffer[0]);
	send(socket, buffer, strlen(buffer), 0);
	return atoi(buffer);
}
//Leer en ncurses
int returnmscanf(int socket,int col, int fil)
{
	char buffer[1024];
	int resp;
	move(col,fil);
	scanw("%s",&buffer[0]);
	resp=atoi(buffer);
	send(socket, buffer, strlen(buffer), 0);
	bzero(buffer, sizeof(buffer));
	return resp;
}



void limpia(int col, int fila)
{
int j,i;
 j=col;
  for (i=fila;i<=81;i++)
  {
   move(j,i);
   printw(" ");
  }
 
 }

