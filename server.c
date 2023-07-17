#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib") /**Biblioteca Winsock*/
#define PORT 8888
#define MESSAGESIZE 255
#define MAXCONECTIONS 10

int acabar=1,x=1,conexoes,palitos,jogadores=0,jogada[MAXCONECTIONS]= {0,0,0,0,0,0,0,0,0,0},recomecar=1, pontos[MAXCONECTIONS]= {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},palpite[MAXCONECTIONS]= {0,0,0,0,0,0,0,0,0,0};
SOCKET new_socket[MAXCONECTIONS];

void connection(int nroconnections){
    int y=1,aux;
    char message[MESSAGESIZE];
    pontos[nroconnections]=palitos;
    if(x>0){
        strcpy(message,"\nAguardando jogadores. . .\n");
        send(new_socket[nroconnections] , message , strlen(message), 0);
    }
    while(x!=0){
    }
    strcpy(message,"\n\nTodos jogadores ja se conectaram\n");
    send(new_socket[nroconnections] , message , strlen(message), 0);
    aux=jogadores;
    while(acabar){
        _sleep(1000);
        jogadores=aux;
        if(acabar!=0){
            x=nroconnections+1;
            sprintf(message,"\n\n\nJOGADOR: %d\nVOCE POSSUI %d PALITOS",x,pontos[nroconnections]);
            send(new_socket[nroconnections] , message , strlen(message), 0);
        }
        do{
            if(acabar!=0){
                strcpy(message,"\nQuantos palitos voce ira colocar na mao:");
                send(new_socket[nroconnections] , message , strlen(message), 0);
                recv(new_socket[nroconnections],message,MESSAGESIZE,0);
                jogada[nroconnections]=atoi(message);
                if(jogada[nroconnections]<0){
                    strcpy(message,"\nA jogada nao pode ser negativa");
                    send(new_socket[nroconnections] , message , strlen(message), 0);
                }
                else if(jogada[nroconnections]>pontos[nroconnections]){
                    sprintf(message,"\nJogada invalida!!! \nVoce possui %d palitos",pontos[nroconnections]);
                    send(new_socket[nroconnections] , message , strlen(message), 0);
                }
            }
            else
                jogada[nroconnections]=1;
        }while((jogada[nroconnections]<0)|| (jogada[nroconnections]>pontos[nroconnections]));

        if(acabar!=0){
            do{
                strcpy(message,"\nQual seu palpite para o total de palitos:");
                send(new_socket[nroconnections] , message , strlen(message), 0);
                recv(new_socket[nroconnections],message,MESSAGESIZE,0);
                palpite[nroconnections]=atoi(message);
                if(palpite[nroconnections]<0){
                    strcpy(message,"\nO palpite nao pode ser negativo");
                    send(new_socket[nroconnections] , message , strlen(message), 0);
                }
            }while(palpite[nroconnections]<0);
            jogadores--;
            strcpy(message,"\nAguardando jogadores. . .");
            send(new_socket[nroconnections] , message , strlen(message), 0);
            while(jogadores>0){
                if(acabar==0)
                    jogadores=0;
            }
        }
        jogadores=-1;
        validar(nroconnections);
    }
    _sleep(500);
    strcpy(message,"\n\n\n\n\n\nA PARTIDA ACABOU, DIGITE 1 PARA JOGAR NOMAVENTE");
    send(new_socket[nroconnections] , message , strlen(message), 0);
    recv(new_socket[nroconnections],message,MESSAGESIZE,0);
    pontos[nroconnections]=atoi(message);
    if(pontos[nroconnections]==1){
        acabar=1;
        x=0;
        connection(nroconnections);
    }
    else
        recomecar=0;
}

void validar(int nroconnections){
    int soma=0,i, x;
    char message[MESSAGESIZE];
    for(i=0; i<conexoes; i++){
        soma = soma + jogada[i];
    }
    if(soma == palpite[nroconnections]){
        strcpy(message,"\nVOCE ACERTOU O PALPITE");
        send(new_socket[nroconnections] , message , strlen(message), 0);
        pontos[nroconnections]--;
        for(i=0; i<conexoes; i++){
            if(i != nroconnections){
                x = nroconnections+1;
                sprintf(message,"\nO JOGADOR %d ACERTOU",x);
                send(new_socket[i] , message , strlen(message), 0);
            }
        }
    }
    else{
        strcpy(message,"\nVOCE ERROU O PALPITE");
        send(new_socket[nroconnections] , message , strlen(message), 0);
    }
    for(i=0; i<conexoes; i++){
         if(pontos[i] == 0){
                if(i!=nroconnections){
                    x=i+1;
                    sprintf(message,"\n\nO JOGADOR %d GANHOU PARTIDA!!!",x);
                    send(new_socket[nroconnections] , message , strlen(message), 0);
                }
                else if(i==nroconnections){
                    strcpy(message,"\n\nVOCE GANHOU A PARTIDA!!!");
                    send(new_socket[nroconnections] , message , strlen(message), 0);
                }
                acabar=0;
        }
    }
}

int main(int argc , char *argv[]){
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server , client;
    int c, i, nroconnections=0, resp;
    char message[MESSAGESIZE];
    printf("\nInicializando biblioteca Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("\nBiblioteca Winsock inicializada.");
    printf("\nInicializando socket...");
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET){
        printf("\nNao e possivel inicializar o socket: %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("\nSocket inicializado.");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
    printf("\nConstruindo socket...");
    if(bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR){
        printf("\nNao e possivel construir o socket: %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("\nConstrucao realizada.");
    listen(s,3);
    puts("\nAguardando por conexoes de entrada...");
    c = sizeof(struct sockaddr_in);
    while(x>0){
        new_socket[nroconnections] = accept(s, (struct sockaddr *)&client, &c);
        if (new_socket == INVALID_SOCKET){
            printf("\nConexao n�o aceita. Codigo de erro: %d", WSAGetLastError());
            exit(EXIT_FAILURE);

        }
        if(nroconnections==0){
            /**primeira conexão*/
            do{
                strcpy(message,"\nQUAL O TOTAL DE JOGADORES?\t\t\t");
                send(new_socket[nroconnections] , message , strlen(message), 0);
                recv(new_socket[nroconnections],message,MESSAGESIZE,0);
                x=atoi(message);
            }
            while(x<2 || x>10);
            conexoes=x;
            do{
                strcpy(message,"\nQUAL O NUMERO DE PALITOS?\t\t");
                send(new_socket[nroconnections] , message , strlen(message), 0);
                recv(new_socket[nroconnections],message,MESSAGESIZE,0);
                palitos=atoi(message);
            }
            while(palitos<=2 || palitos>10);
        }
        puts("\nConex�o aceita.");
        printf("\nDados do cliente - IP: %s -  Porta: %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));
        _beginthread(connection,NULL,nroconnections);
        nroconnections++;
        jogadores++;
        x--;
    }
    while(recomecar){
    }
    for(i=0; i<conexoes; i++){
        strcpy(message,"\n\n\n\n\n\nUM OU MAIS JOGADORES NAO JOGARAM NOVAMENTE, A PARTIDA SERA FINALIZADA");
        send(new_socket[i] , message , strlen(message), 0);
    }
    _sleep(2000);
    strcpy(message,"99999");
    for(i=0; i<conexoes; i++){
        send(new_socket[nroconnections] , message , strlen(message), 0);
    }
    /**Finalizacao do socket*/
    closesocket(s);
    /**Finaliza��o da biblioteca*/
    WSACleanup();
}
