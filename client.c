#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define MESSAGESIZE 255
int fechar=1;
void receber(SOCKET s){
    int recv_size;
    char server_reply[MESSAGESIZE];
    while(fechar){
          /**Recebe a resposta do servidor**/
          recv_size = recv(s , server_reply , MESSAGESIZE , 0);
          if(recv_size>0){
              //printf("\nServer: ");
              /**Adicionar o \0 no final antes de imprimir*/
              server_reply[recv_size] = '\0';
              if(atoi(server_reply)==99999){
                fechar=0;
              }
              else
                puts(server_reply);
          }

    }

}



int main(int argc , char *argv[]) {
  WSADATA wsa;
  SOCKET s;
  struct sockaddr_in server;
  char message[MESSAGESIZE];
  int recv_size;
  char ip[16];

  printf("\nInicializando Winsock...");
  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
    printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
    exit(EXIT_FAILURE);
  }
  printf("Inicializado.\n");

  printf("\nInicializando Socket...");
  if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
    printf("\nNao e possivel criar o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }
  printf("\nSocket criado.");
  _beginthread(receber,NULL,s);

  printf(" \nQUAL O IP DO SERVIDOR ??\t");
  gets(ip);

  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_family = AF_INET;
  server.sin_port = htons( 8888 );

  if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0) {
    puts("\nErro de conex�o.");
    exit(EXIT_FAILURE);
  }
  puts("\nConectado");
  while(fechar){
      gets(message);
      if( send(s , message , strlen(message)+1, 0) < 0) {
        puts("\nEnvio falhou.");
        return 1;
      }
  }
}
