#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY 123 // Usado como chave para memoria compartilhada ---

#define TYPE_FUNCTION *(mem+1) // 0 - Nenhuma função
                               // 1 - Função Andar rápido
                               // 2 - Função Andar curto
                               // 3 - Função Virar
                               // 4 - Função Andar lateral direita
                               // 5 - Função Andar lateral esquerda
                               // 6 - Chute pé direito
//#define KEEP_WALKING_1 *(mem+2)
#define FINISH_ACTION *(mem+3)
#define RETORNO_VISAO *(mem+4)
#define DISTANCIA_BOLA *(mem+5)
#define ANGULO_BOLA *(mem+6)
#define SERVO1_POSITION *(mem+7)
#define SERVO2_POSITION *(mem+8)

using namespace std;

int *mem ; //Variável que manipula memória compartilhada

int main()
{

    // --- Variaveis usada para memoria compartilhada -----
    int shmid ;  // identificador da memoria comum //
    int size = 1024 ;
    char *path="nome_de_arquivo_existente" ;
    int flag = 0;
    //-----------------------------------------------------

    cout << "Decision Running" << endl;

     //
     // Recuperando ou criando uma memoria compartilhada-------------------
     //

     if (( shmid = shmget(ftok(path,(key_t)KEY), size,0)) == -1)
     {
          perror("Erro no shmget") ;
          printf("\nMemória será criada\n");
         //return(1) ;
        if (( shmid = shmget(ftok(path,(key_t)KEY), size, IPC_CREAT|IPC_EXCL|SHM_R|SHM_W)) == -1)
        {
            perror("Erro no shmget") ;
            return(1) ;
        }

     }

     printf("Sou o processo com pid: %d \n",getpid()) ;
     printf("Identificador do segmento recuperado: %d \n",shmid) ;
     printf("Este segmento e associado a chave unica: %d\n",
                   ftok(path,(key_t)KEY)) ;
    //
    // acoplamento do processo a zona de memoria
    //recuperacao do pornteiro sobre a area de memoria comum
    //
     if ((mem = (int*)shmat (shmid, 0, flag)) == (int*)-1){
          perror("acoplamento impossivel") ;
          return (1) ;
     }
    *mem = 0;
     //---------------------------------------------------------------------

	//printf("%d", TYPE_FUNCTION);
	//TYPE_FUNCTION = 1;
	//KEEP_WALKING = 1;
	//sleep(18);
	//KEEP_WALKING = 0;
	TYPE_FUNCTION = 0;

while(1)
{

/*
if(RETORNO_VISAO == 1)
{
printf("bola encontrada | dist: %d | Angulo %d\n", DISTANCIA_BOLA, ANGULO_BOLA);
RETORNO_VISAO = 0;
}
if(RETORNO_VISAO == 2)
{
printf("Bola perdida\n");
RETORNO_VISAO = 0;
}
*/


//retorno_visao = visao();// Entra no programa da visão e recebe retorno se achou a bola

if (RETORNO_VISAO == 1) // Se retornou 1, então o robo encontrou a bola e está com a câmera centralizado nela
{

//printf("  Servo1: %d - Servo2: %d\n",dxl_read_word( 1, P_PRESENT_POSITION_L), dxl_read_word( 2, P_PRESENT_POSITION_L));

   //float dist_bola = distancia_bola(); // Calcula a distância que o robo está da bola
        if (DISTANCIA_BOLA > 50)
        {
            if(ANGULO_BOLA > 100)
            {
                TYPE_FUNCTION = 3; //virar(0);//100
            }
            else
            {
                if(DISTANCIA_BOLA > 200)
                    TYPE_FUNCTION = 1; //andar(dist_bola/0.095);//0.95

                if(DISTANCIA_BOLA <= 200)
                    TYPE_FUNCTION = 2; //andar_curto(dist_bola/0.04);
            }

        }

        //posição exata de chute servo 1: 537 / servo 2: 627
        if ((SERVO1_POSITION >= 540 && SERVO1_POSITION <= 578)
             && (SERVO2_POSITION >= 518 && (SERVO2_POSITION <= 560)))
        {
            TYPE_FUNCTION = 6; //chutepedireito();
        }

        if ((SERVO1_POSITION >= 540 && SERVO1_POSITION <= 577)
            && (SERVO2_POSITION < 518)) // verifica se necessita andar lateral
        {
            TYPE_FUNCTION = 4; //andar_lateral_direita(1);
        }

        if ((SERVO1_POSITION >= 540 && SERVO1_POSITION <= 577)
             && (SERVO2_POSITION > 560)) // verifica se necessita andar lateral
        {
            TYPE_FUNCTION = 5; //andar_lateral_esquerda(1);
        }

        if ((SERVO2_POSITION >= 518 && (SERVO2_POSITION <= 560))
            && DISTANCIA_BOLA<=50 && (SERVO1_POSITION < 540))
        {
            TYPE_FUNCTION = 2; //andar_curto(1);
        }

printf("bola encontrada | dist: %d | Angulo %d | Function_select %d\n", DISTANCIA_BOLA, ANGULO_BOLA, TYPE_FUNCTION);
RETORNO_VISAO = 0;
}

if (RETORNO_VISAO == 2)
{
    printf("Não achou bola\n");
    //dxl_write_word(2, P_GOAL_POSITION_L, 594);
    //dxl_write_word(1, P_GOAL_POSITION_L, 304);
    //for (int i = 0; i<3 ; i++)
        TYPE_FUNCTION = 3;//virar(3);
RETORNO_VISAO = 0;
}
/**/
}
    return 0;
}
