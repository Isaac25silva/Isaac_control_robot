#include <stdio.h>
#include <stdlib.h>
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
#include <string>
#include <dynamixel.h>
//#include <mpi.h>

#define P_GOAL_POSITION_L	30
#define P_GOAL_POSITION_H	31
#define P_PRESENT_POSITION_L	36
#define P_PRESENT_POSITION_H	37
#define P_MOVING		46
#define DEFAULT_BAUDNUM		1 // 1Mbps

// Tamanho Padrão de Captura WebCam Robo- 640x480
#define RESOLUCAO_X 640
#define RESOLUCAO_Y 480
void PrintCommStatus(int CommStatus);
void PrintErrorCode(void);
void captura_pixel_hsv();
int head_follow_ball(float posx, float posy, bool *head_move1, bool *head_move2);
void search_ball(bool inicio);

int cont_search_ball;

int visao()
{

    bool head_move2 = false;
    bool head_move1 = false;
//    int i;
	int baudnum = DEFAULT_BAUDNUM; //velocidade de transmissao
	int index = 0;
	int deviceIndex = 0; //endereça USB
	int Moving, PresentPos;
    unsigned int lost_ball = 0; // Conta quantos frames a bola está perdida
    int saida = 0;

	//int CommStatus;
    //int contq =0;
    //double media=0;
    //float media2=0;

    CvFont font;
    double hScale=1.0;
    double vScale=1.0;
    int    lineWidth=1;
    int posX = 1;
    int posY = 1;
    bool inicio=1;
    bool call_search = 0;
    bool call_head = 0;
   cont_search_ball = 0;
//int Min_Matiz=0;
//int Min_Saturacao=0;
//int Min_Brilho=0;
//int Max_Matiz=360;
//int Max_Saturacao=360;
//int Max_Brilho=360;

cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

	// Tamanho Padrão de Captura WebCam Robo- 640x480
	CvSize tamanho = cvSize(RESOLUCAO_X,RESOLUCAO_Y); //320 240

    	// Abre o dispositivo de Captura "0" que é /dev/video0
  	CvCapture* captura = cvCaptureFromCAM( 0 );
//  	CvCapture* captura1 = cvCaptureFromCAM( 1 );

	if( !captura )
		{
	        fprintf( stderr, "ERRO Não há Captura na Camera 0/n" );
	        getchar();
	        return -1;
        }

	// Cria uma janela onde as imagens capturadas serão apresentadas
	cvNamedWindow( "Video1", CV_WINDOW_AUTOSIZE );
	// Cria uma janela com a conversão para o HSV
	// HSV é a abreviatura para o sistema de cores formadas pelas componentes:
	// Hue ( Matiz - Angulo do Hexagono)
	// Saturation ( Saturação - Quantidade de Cinza )
	// Value ( Valor - Brilho ).

	//cvNamedWindow( "Sistema de cores RGB - HSV", CV_WINDOW_AUTOSIZE );

//*****************************************************************************
	// Detectando a bola laranja do Flavio Tonidadel Estável camera do Robo sem detectar a amão
	CvScalar min = cvScalar(4, 160, 50, 0);//160
	CvScalar max = cvScalar(11, 250, 255, 0);

//r 80~110
//g 130~150
//b 40~70

    //dxl_write_word(2, P_GOAL_POSITION_L, 594);
    //dxl_write_word(1, P_GOAL_POSITION_L, 304);
    //dxl_write_word(1, P_GOAL_POSITION_L, dxl_read_word( 1, P_PRESENT_POSITION_L));
    usleep(1000000);
    //dxl_write_word(2, P_GOAL_POSITION_L, 682);

//*****************************************************************************

	IplImage*  Quadro_hsv  = cvCreateImage(tamanho, IPL_DEPTH_8U, 3);
	IplImage*  segmentada  = cvCreateImage(tamanho, IPL_DEPTH_8U, 1);

    while( 1 )
 	{
            //posX = RESOLUCAO_X/2;
            //posY = RESOLUCAO_Y/2;
        // Pegar um quadro
        IplImage* Quadro = cvQueryFrame( captura );
//        IplImage* Quadro2 = cvQueryFrame( captura0 );

//---------- Captura pixel-----------------------------------------------------
        //ponteiro para as cores da imagem
        //unsigned char* colorData = (unsigned char*)Quadro_hsv->imageData;

        //int pixel_center = (Quadro_hsv->width/2) * (Quadro_hsv->height/2) * 3;


        //printf("H=%3d  S=%3d  V=%3d\n", colorData[pixel_center], colorData[pixel_center+1], colorData[pixel_center+2]);

//-----------------------------------------------------------------------------

        if( !Quadro )
        {
                fprintf( stderr, "ERRO Quadro vazio...\n" );
                getchar();
                break;
        }

       	// Converter o espaço de cores para o HSV para ficar mais fácil filtrar as cores.
        cvCvtColor(Quadro, Quadro_hsv, CV_BGR2HSV);

        // Filtrar cores que não interessam.
        cvInRangeS(Quadro_hsv, min, max, segmentada);

        // Memoria para os circulos de Hough
        CvMemStorage* memoria = cvCreateMemStorage(0);

        // O detector de Hough trabalha melhor com alguma suavização da imagem.
	// O Thresholding é o metodo mais simples de segmentação de imagens.

        cvDilate(segmentada,segmentada,NULL,5);
        cvErode(segmentada,segmentada,NULL,5);
        cvSmooth( segmentada, segmentada, CV_GAUSSIAN, 9, 9 );
        cvSmooth( segmentada, segmentada, CV_GAUSSIAN, 9, 9 );
        CvSeq* circulos = cvHoughCircles(segmentada, memoria, CV_HOUGH_GRADIENT, 2, segmentada->height/4, 100, 50, 10, 400);// 200 100 10 400
                                                                                                                            //vilão 100 50 10 400
        std::string str;
        //static float mx = 0;
        //static float my = 0;
        //static float mxp = 0;
        //static float myp = 0;

        //float mr = 0;
        //static unsigned int cont = 0;

//static int t;
        for (int i = 0; i < circulos->total; i++)
        {
            float* p = (float*)cvGetSeqElem( circulos, i );

                //t++;
            //if (p[2] < 30)
            //{

//	cvCircle( Quadro, cvPoint(cvRound(p[0]),cvRound(p[1])), 3, CV_RGB(0,255,0), -1, 8, 0 );

//	cvCircle( Quadro, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(255,255,255), 2, 8, 0 );

            //}
        // Calculate the moments to estimate the position of the ball

            CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
            cvMoments(segmentada, moments, 1);

            // The actual moment values
            //double moment10 = cvGetSpatialMoment(moments, 1, 0);
            //double moment01 = cvGetSpatialMoment(moments, 0, 1);
            double area = cvGetCentralMoment(moments, 0, 0);

            //int lastX = posX;
            //int lastY = posY;

            //posX = moment10/area;
            //posY = moment01/area;

            posX = p[0]; // variavel que contem a posição da bola em X da tela
            posY = p[1]; // variavel que contem a posição da bola em Y da tela

            cvCircle( Quadro, cvPoint(cvRound(posX),cvRound(posY)), 3, CV_RGB(0,255,0), -1, 8, 0 );
            cvCircle( Quadro, cvPoint(cvRound(posX),cvRound(posY)), cvRound(p[2]), CV_RGB(255,255,255), 2, 8, 0 );


            //if(lastX>0 && lastY>0 && posX>0 && posY>0)
            //{
                // Draw a yellow line from the previous point to the current point
               // cvLine(Quadro, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,255,255), 2);

           // }

           // double distfix = 30;
           // int pfix = 80;
            //double Dw;

            lost_ball=0;
            cont_search_ball = 0;

            //---- Escreve na Tela a posição X da bola ---------------------------------------
            std::stringstream X_str ;
            X_str << p[0];                 // valor de X detectado pelo circulo de Hough
            std::string x = X_str.str();
            const char * cx = x.c_str();
            cvPutText (Quadro, cx ,cvPoint(100,300), &font, cvScalar(255,255,0));

            //---- Escrve a diferença entre a posição X da bola e o centro da tela------------
            std::stringstream posiX_str ;
            posiX_str << posX - RESOLUCAO_X/2; // diferença entre a bola e o centro da tela em x
            std::string posix = posiX_str.str();
            const char * posx = posix.c_str();
            cvPutText (Quadro, posx ,cvPoint(100,400), &font, cvScalar(255,255,0));

            char cD[10];
            sprintf(cD, "DifY %d", (posY - RESOLUCAO_Y/2));
            cvPutText (Quadro, cD ,cvPoint(100,450), &font, cvScalar(255,255,0));

            //---- Escreve na Tela a posição Y da bola ---------------------------------------
            std::stringstream Y_str ;
            Y_str << p[1];                // valor de Y detectado pelo circulo de Hough
            std::string y = Y_str.str();
            const char * cy = y.c_str();
            cvPutText (Quadro, cy ,cvPoint(200,300), &font, cvScalar(255,255,0));

            //---- Escreve na Tela o raio de pixels da bola ------------------------------------
            std::stringstream R_str ;
            R_str << p[2];                // valor de R detectado pelo circulo de Hough
            std::string r = R_str.str();
            const char * cr = r.c_str();
            cvPutText (Quadro, cr ,cvPoint(300,300), &font, cvScalar(255,255,0));


            std::stringstream A_str ;
            A_str << area;                // valor da area em pixels
            std::string A = A_str.str();
            const char * cA= A.c_str();
            cvPutText (Quadro, cA ,cvPoint(300,400), &font, cvScalar(255,255,0));
       }
            //std::stringstream B_str ;
            //B_str << cont;                // valor de R detectado pelo circulo de Hough
            //std::string B = B_str.str();
            //const char * cB= B.c_str();
            //cvPutText (Quadro, cB ,cvPoint(100,200), &font, cvScalar(255,255,0));

        if(lost_ball>20) //verifica se depois de 30 frames a bola está perdida no campo
        {
            //while(*comunica!=0);
            //if(*comunica)
                search_ball(inicio);//Procura a bola pelo campo
                saida = 0;
            //char cD[10];
            //sprintf(cD, "procurando a bola");
            cvPutText (Quadro, "Procurando a bola" ,cvPoint(150,450), &font, cvScalar(255,255,0));
            //call_search = 1;
            inicio = 0;
        }
        else
        {
            if(inicio==0)// Faz o robô parar a cabeça no instante que achou a bola que estava perdida
            {            // Sem esse comando o código não funciona porque ele não para a cabeça
                dxl_write_word(1, P_GOAL_POSITION_L, dxl_read_word( 1, P_PRESENT_POSITION_L));
                dxl_write_word(2, P_GOAL_POSITION_L, dxl_read_word( 2, P_PRESENT_POSITION_L));
            }

            inicio =1;
            //cont++;
            //mx += posX;
            //my += posY;

            //if(cont%10==0) // faz o calculo da média de 10 amostras
           // {
                //mx = mx/cont;
                //my = my/cont;
                //mxp = mx;
                //myp = my;
                //head_follow_ball((mx+posX)/2, (my+posY)/2); // Move a cabeça para seguir a bola
                //cont=0;
                //mx = 0;
                //my = 0;

            //}
            //while(*comunica!=0);
            //if(*comunica)
                if (head_follow_ball(posX, posY, &head_move1, &head_move2) == 1){// Move a cabeça para seguir a bola
               saida++;
                }

        }
        if(lost_ball<100)
            lost_ball++;

/*

            std::stringstream B_str ;
            B_str << (int)((mxp+posX)/2);                // valor de R detectado pelo circulo de Hough
            std::string B = B_str.str();
            const char * cB= B.c_str();
            cvPutText (Quadro, cB ,cvPoint(100,200), &font, cvScalar(255,255,0));


            //std::stringstream C_str ;
            //C_str << (int)my;                // valor de R detectado pelo circulo de Hough
            //sprintf(C_str, "%f", my);
            //std::string C = C_str.str();
            char cC[10];
            sprintf(cC, "%f", (myp+posY)/2);
            cvPutText (Quadro, cC ,cvPoint(200,200), &font, cvScalar(255,255,0));*/


	// Centro é 320 x 240 preciso fazer com que o servo motor se mova para cima, para baixo, para a esquerda ou para direita.
	// de modo que a imagem fique centralizada na bola:
	//  se x > 320 mova para a esquerda q graus até x estiver entre 321 e 325.
	//  se x < 320 mova para a direita q graus até x estiver entre 315 e 320.
	//  se y > 240 mova para baixo q graus até y estiver entre 241 e 245.
	//  se x < 240 mova para a direita q graus até x estiver entre 235 e 240.

	// Importante conhecer o tamanho da bola em pixels !!!
	// Assim poderemos saber a distancia usando uma regra de proporção simples.



        cvShowImage( "Sistema de cores RGB - HSV", Quadro_hsv); // Stream Original no Espaço de Cores HSV
        cvShowImage( "Filtragem das cores", segmentada ); // O stream depois da filtragem de cores
        cvShowImage( "Video1", Quadro ); // Stream Original com a bola detectada
//	cvShowImage( "Video2", Quadro2 ); // Stream Original com a bola detectada
		cvReleaseMemStorage(&memoria);

		//usleep(800000);

  	// O processamento é interrompido se a tecla ESC for pressionada, Tecla=0x10001B
	// Isso é feito removendo os bits de maior significancia usando o operador AND

            //head_follow_ball(posX, posY); // Move a cabeça para seguir a bola


        if( (cvWaitKey(10) & 255) == 27 || saida > 22 || cont_search_ball > 9 )
        {
        cvReleaseCapture( &captura );
//      cvReleaseCapture( &captura1 );
        cvDestroyWindow( "Video1" );
        cvDestroyWindow( "Filtragem das cores" );
        cvDestroyWindow( "Sistema de cores RGB - HSV" );
        if (cont_search_ball > 9){
        //dxl_write_word(2, P_GOAL_POSITION_L, 594);
        //dxl_write_word(1, P_GOAL_POSITION_L, 304);
        return 2;
        }
        else
        return 1;
        }

	}

     // Libera o dispositivo de captura
     cvReleaseCapture( &captura );
//     cvReleaseCapture( &captura1 );
     cvDestroyWindow( "Video1" );
     cvDestroyWindow( "Filtragem das cores" );
     cvDestroyWindow( "Sistema de cores RGB - HSV" );
     return 0;
}

//=======================================================================================================================
//------------- Função que faz a cabeça centralizar a bola no vídeo------------------------------------------------------
int head_follow_ball(float posx, float posy, bool *head_move1, bool *head_move2)
{
    int pan = 0;
    int tilt = 0;
// Posição inicial da cabeça {304, 594} //01 , 02, cabeça

    dxl_write_word(1, MOVING_SPEED, 400);//300
    dxl_write_word(2, MOVING_SPEED, 400);//300


//------ Realiza o movimento do Pan -----------------------------------------------------------
    //------ Segue a bola para a esquerda do video -----------------------------------------
    if(posx<(RESOLUCAO_X/2)*0.95 && *head_move2==false)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, dxl_read_word( 2, P_PRESENT_POSITION_L)+( ((RESOLUCAO_X/2)-posx) * 0.285));
        //head_move = true;
    }

    //------ Segue a bola para a direita do video -----------------------------------------
    if(posx>(RESOLUCAO_X/2)*1.05 && *head_move2==false)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, dxl_read_word( 2, P_PRESENT_POSITION_L)-( (posx-(RESOLUCAO_X/2)) * 0.285));
        //dxl_write_word(2, P_GOAL_POSITION_L, dxl_read_word( 2, P_PRESENT_POSITION_L)-(((RESOLUCAO_X/2)-posx) * 0.30) );
        //head_move = true;
    }

    // Para a cabeça se chegou na posição desejada ----------------------------------------
    if(posx>=(RESOLUCAO_X/2)*0.95 && posx<=(RESOLUCAO_X/2)*1.05){
        dxl_write_word(2, P_GOAL_POSITION_L, dxl_read_word( 2, P_PRESENT_POSITION_L));
        pan = 1;
    }

    if(dxl_read_byte( 2, P_MOVING ))
        *head_move2 = true;  // verifica se a cabeça está em movimento
    else
        *head_move2 = false; // verifica se a cabeça está em movimento
//---------------------------------------------------------------------------------------------


//------ Realiza o movimento do Tilt -----------------------------------------------------------
    //------ Segue a bola para a esquerda do video -----------------------------------------
    if(posy<(RESOLUCAO_Y/2)*0.95 && *head_move1==false)
    {
        dxl_write_word(1, P_GOAL_POSITION_L, dxl_read_word( 1, P_PRESENT_POSITION_L)-( ((RESOLUCAO_Y/2)-posy) * 0.285));
        //head_move = true;
    }

    //------ Segue a bola para a direita do video -----------------------------------------
    if(posy>(RESOLUCAO_Y/2)*1.05 && *head_move1==false)
    {
       dxl_write_word(1, P_GOAL_POSITION_L, dxl_read_word( 1, P_PRESENT_POSITION_L)+( (posy-(RESOLUCAO_Y/2)) * 0.285));
        //dxl_write_word(2, P_GOAL_POSITION_L, dxl_read_word( 2, P_PRESENT_POSITION_L)-(((RESOLUCAO_X/2)-posx) * 0.30) );
        //head_move = true;
    }

    // Para a cabeça se chegou na posição desejada ----------------------------------------
    if(posy>=(RESOLUCAO_Y/2)*0.95 && posy<=(RESOLUCAO_Y/2)*1.05){
        dxl_write_word(1, P_GOAL_POSITION_L, dxl_read_word( 1, P_PRESENT_POSITION_L));
        tilt = 1;
    }

    if(dxl_read_byte( 1, P_MOVING ))
        *head_move1 = true;  // verifica se a cabeça está em movimento
    else
        *head_move1 = false; // verifica se a cabeça está em movimento
//---------------------------------------------------------------------------------------------
if (pan == 1 && tilt == 1)
return 1;
else
return 0;

}
//=======================================================================================================================


void search_ball(bool inicio)
{
    // Posição inicial da cabeça {304, 594} //01 , 02, cabeça
static unsigned int varredura=0;

        dxl_write_word(2, MOVING_SPEED, 150);//Seta as velocidades da cabeça
        dxl_write_word(1, MOVING_SPEED, 400);

    if(inicio)
        varredura--; // continua a varredura de onde parou

    if(varredura>9||varredura<1)
        varredura=0;

    if(dxl_read_byte( 2, P_MOVING )==0){
     varredura++;
     cont_search_ball++;
    }


    if(dxl_read_byte( 2, P_MOVING )==0 && varredura==8)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 1000);
        dxl_write_word(1, P_GOAL_POSITION_L, 304);
    }

    if(dxl_read_byte( 2, P_MOVING )==0 && varredura==7)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 150);
        dxl_write_word(1, P_GOAL_POSITION_L, 304);
    }


    if(dxl_read_byte( 2, P_MOVING )==0 && varredura==6)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 150);
        dxl_write_word(1, P_GOAL_POSITION_L, 435);
    }


    if(dxl_read_byte( 2, P_MOVING )==0 && varredura==5)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 1000);
        dxl_write_word(1, P_GOAL_POSITION_L, 435);
    }


    if(dxl_read_byte( 2, P_MOVING )==0 && varredura==4)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 1000);
        dxl_write_word(1, P_GOAL_POSITION_L, 550);
    }


    if(dxl_read_byte( 2, P_MOVING )==0 && varredura==3)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 150);
        dxl_write_word(1, P_GOAL_POSITION_L, 550);
    }

    if(dxl_read_byte( 1, P_MOVING )==0 && varredura==2)
    {
        dxl_write_word(2, P_GOAL_POSITION_L, 150);
        dxl_write_word(1, P_GOAL_POSITION_L, 304);
    }

    if(dxl_read_byte( 1, P_MOVING )==0 && varredura==1)
    {
        dxl_write_word(2, MOVING_SPEED, 700);
        dxl_write_word(1, MOVING_SPEED, 700);
        dxl_write_word(2, P_GOAL_POSITION_L, 594);
        dxl_write_word(1, P_GOAL_POSITION_L, 304);
    }


    //dxl_write_word(2, MOVING_SPEED, 20);//300
    //dxl_write_word(10, P_GOAL_POSITION_L, );


}

/*
void captura_pixel_hsv()
{

	CvSize tamanho = cvSize(320,240);


	    IplImage*  Quadro_hsv  = cvCreateImage(tamanho, IPL_DEPTH_8U, 3);

        if( !Quadro_hsv )
        	{
                fprintf( stderr, "ERRO Quadro vazio...\n" );
                getchar();
        	}

        //ponteiro para as cores da imagem
        unsigned char* colorData = (unsigned char*)Quadro_hsv->imageData;

        printf("%3d  %3d  %3d", colorData[0], colorData[1], colorData[2]);

}*/


//Comando para Compilar esse programa.
//g++ -I/usr/include/opencv Bola.cpp -o Bola -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
