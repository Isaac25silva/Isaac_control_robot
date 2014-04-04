#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <dynamixel.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <cvaux.h>
#include <highgui.h>
#include <cxcore.h>
#include <time.h>
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <iostream>

#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY 123
//#define KEY 123
//#define MSG "Mensagem escrita na memoria comum por Isaac"

#define RESOLUCAO_X 640
#define RESOLUCAO_Y 480

#define TYPE_FUNCTION *(mem+1)// 0 - Nenhuma função
                               // 1 - Função Andar rápido
                               // 2 - Função Andar curto
                               // 3 - Função Virar
                               // 4 - Função Andar lateral direita
                               // 5 - Função Andar lateral esquerda
                               // 6 - Chute pé direito
//#define KEEP_WALKING_1 *(mem+2)
//#define KEEP_WALKING_2 *(mem+7)
//#define KEEP_TURN_RIGHT *(mem+8)
//#define KEEP_WALKING_3 *(mem+9)
//#define KEEP_WALKING_4 *(mem+10)


void PrintCommStatus(int CommStatus);
void PrintErrorCode(void);

int *mem ; //Variável que manipula memória compartilhada

//////global variables////////////////////////////////////////////////
int inclina = 55;
unsigned int StandupPos[22] = {304, 594, //01 , 02, (cabeça)
                478, 304, 409, //03, 04, 05, (braço direito)
                546, 650, 609, //06, 07, 08, (braço esquerdo)
                573, 835, //09, 10, (torso)
                613, 534, 501, 508, 492, 559, //11, 12, 13, 14, 15, 16 (perna direita)
                590, 509, 514, 506, 510, 461}; //17, 18, 19, 20, 21, 22 (perna esquerda)

unsigned int StandupPos_corrigido[22] = {304, 594, //01 , 02, (cabeça)
                            478, 274, 350, //03, 04, 05, (braço direito)
                            541, 675, 682, //06, 07, 08, (braço esquerdo)
                            571, 835, //09, 10, (torso)
                            585, 512, 498, 510, 498, 562, //11, 12, 13, 14, 15, 16 (perna direita)
                            601, 512, 512, 510, 508, 467}; //17, 18, 19, 20, 21, 22 (perna esquerda)

int main(int argc, char* argv[])
{

    // --- Variaveis usada para memoria compartilhada -----
    int shmid ;  // identificador da memoria comum //
    const unsigned short int size = 1024 ;
    char *path="nome_de_arquivo_existente" ;
    int flag = 0;
    //-----------------------------------------------------

     //int i;
	 unsigned int baudnum = DEFAULT_BAUDNUM; //velocidade de transmissao
	 unsigned int index = 0;
	 unsigned short int deviceIndex = 0; //endereça USB
	 //int Moving, PresentPos;
	 int CommStatus;
     char string[50]; //String usada para definir prioridade do programa
     //int retorno_visao = 0;


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

    system("echo 123456 | sudo -S chmod 777 /dev/ttyUSB*");//libera USB

    sprintf(string,"echo 123456 | sudo -S renice -20 -p %d", getpid()); // prioridade maxima do codigo
    system(string);//prioridade
	printf( "\nMiltonBot running...\n\n" );

	// ---- Open USBDynamixel -----------------------------------------------{
	if( dxl_initialize(deviceIndex, baudnum) == 0 )
	{
		printf( "Failed to open USB%dDynamixel!\n", deviceIndex );
		printf( "Press Enter key to terminate...\n" );
		getchar();
		return 0;
	}
	else
		printf( "Succeed to open USB%dDynamixel!\n", deviceIndex );


    if(dxl_read_byte( 20, 3 ) == 20)
        printf("USB okay");
    else
    {
        printf("USB wrong");
        dxl_terminate();
	    // ---- Open USBDynamixel again -------------------
	   if( dxl_initialize(deviceIndex+1, baudnum) == 0 )
	   {
            printf( "Failed to open USB%dDynamixel!\n", deviceIndex+1 );
            printf( "Press Enter key to terminate...\n" );
            getchar();
            return 0;
        }
        else
            printf( "Succeed to open USB%dDynamixel!\n", deviceIndex+1 );

    }
//-----------------------------------------------------------------------------}
    TYPE_FUNCTION = 0;

    robo_ereto_corrigido();

	while(1)
	{

	    usleep(1000); // Mantem o programa rodando numa frequencia de 1MHz

	    getchar();
	//
	//printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );

		//if(getchar() == 0x1b)
			//break;

       /*printf("Função: %d\n", TYPE_FUNCTION);

        switch(TYPE_FUNCTION)
        {
            case 1: { printf("Andar rápido\n"); andar_rapido(0); }
            case 2: { printf("Andar curto\n"); andar_curto(0); }
            case 3: { printf("Virar para direita\n"); virar(0); }
            case 4: { printf("Andar lateral esquerda\n"); andar_lateral_direita(0); }
            case 5: { printf("Andar lateral direita\n"); andar_lateral_esquerda(0); }
            case 6: { printf("Chute pé direito\n"); chutepedireito(); }
            default :{  }
        }*/

//robo_ereto_corrigido();
//sleep(3);
andar_rapido(0);
//andar_lateral_direita(1);
//andar_lateral_esquerda(1);
//virar(5);
//andar(2);
//andar_curto(3);
//levanta_abaixa_perna_esq();
//levantar_de_barriga();

//chutepedireito();
//levantar_de_barriga();
/*
retorno_visao = visao();// Entra no programa da visão e recebe retorno se achou a bola

if (retorno_visao == 1) // Se retornou 1, então o robo encontrou a bola e está com a câmera centralizado nela
{
printf("distancia: %2.2f |", distancia_bola());
printf("  virou %d",594-dxl_read_word( 2, P_PRESENT_POSITION_L));
printf("  Servo1: %d - Servo2: %d\n",dxl_read_word( 1, P_PRESENT_POSITION_L), dxl_read_word( 2, P_PRESENT_POSITION_L));

   float dist_bola = distancia_bola(); // Calcula a distância que o robo está da bola
        if (dist_bola > 0.05)
        {
            virar((594-dxl_read_word( 2, P_PRESENT_POSITION_L))/100);//100

            if(dist_bola > 0.20)
                andar(dist_bola/0.095);//0.95

            if(dist_bola <= 0.20)
                andar_curto(dist_bola/0.04);
            //robo_ereto_corrigido();

        }
        //posição exata de chute servo 1: 537 / servo 2: 627
        if ((dxl_read_word( 1, P_PRESENT_POSITION_L) >= 540 && dxl_read_word( 1, P_PRESENT_POSITION_L) <= 578)
             && (dxl_read_word( 2, P_PRESENT_POSITION_L) >= 518 && (dxl_read_word( 2, P_PRESENT_POSITION_L) <= 560)))
        {
            chutepedireito();
        }

        if ((dxl_read_word( 1, P_PRESENT_POSITION_L) >= 540 && dxl_read_word( 1, P_PRESENT_POSITION_L) <= 577)
             && (dxl_read_word( 2, P_PRESENT_POSITION_L) < 518)) // verifica se necessita andar lateral
        {
            andar_lateral_direita(1);
        }

        if ((dxl_read_word( 1, P_PRESENT_POSITION_L) >= 540 && dxl_read_word( 1, P_PRESENT_POSITION_L) <= 577)
             && (dxl_read_word( 2, P_PRESENT_POSITION_L) > 560)) // verifica se necessita andar lateral
        {
            andar_lateral_esquerda(1);
        }

        if ((dxl_read_word( 2, P_PRESENT_POSITION_L) >= 518 && (dxl_read_word( 2, P_PRESENT_POSITION_L) <= 560))
            && dist_bola<=0.05 && (dxl_read_word( 1, P_PRESENT_POSITION_L) < 540))
        {
            andar_curto(1);
        }


}

if (retorno_visao == 2)
{
    printf("Não achou bola");
    dxl_write_word(2, P_GOAL_POSITION_L, 594);
    dxl_write_word(1, P_GOAL_POSITION_L, 304);
    //for (int i = 0; i<3 ; i++)
    virar(3);
}
*/

    }
	// Close device
	dxl_terminate();

     // destruicao do segmento //
     if ((shmctl(shmid, IPC_RMID, NULL)) == -1){
          perror("Erro shmctl()") ;
          return(1) ;
     }

	printf( "Press Enter key to terminate...\n" );
	getchar();
	return 0;
}
//=================================================================================================================

//-----------------------------------------------------------------------------------------------------------------
// Print communication result
void PrintCommStatus(int CommStatus)
{
	switch(CommStatus)
	{
	case COMM_TXFAIL:
		printf("COMM_TXFAIL: Failed transmit instruction packet!\n");
		break;

	case COMM_TXERROR:
		printf("COMM_TXERROR: Incorrect instruction packet!\n");
		break;

	case COMM_RXFAIL:
		printf("COMM_RXFAIL: Failed get status packet from device!\n");
		break;

	case COMM_RXWAITING:
		printf("COMM_RXWAITING: Now recieving status packet!\n");
		break;

	case COMM_RXTIMEOUT:
		printf("COMM_RXTIMEOUT: There is no status packet!\n");
		break;

	case COMM_RXCORRUPT:
		printf("COMM_RXCORRUPT: Incorrect status packet!\n");
		break;

	default:
		printf("This is unknown error code!\n");
		break;
	}
}

// Print error bit of status packet
void PrintErrorCode()
{
	if(dxl_get_rxpacket_error(ERRBIT_VOLTAGE) == 1)
		printf("Input voltage error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_ANGLE) == 1)
		printf("Angle limit error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERHEAT) == 1)
		printf("Overheat error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_RANGE) == 1)
		printf("Out of range error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_CHECKSUM) == 1)
		printf("Checksum error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERLOAD) == 1)
		printf("Overload error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_INSTRUCTION) == 1)
		printf("Instruction code error!\n");
}
