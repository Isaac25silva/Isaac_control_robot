//##########################################################
//##                      R O B O T I S                   ##
//##          ReadWrite Example code for Dynamixel.       ##
//##                                           2009.11.10 ##
//##########################################################
#include <stdio.h>
//#include <string>
//#include <iostream>
//#include <fstream>
#include <termio.h>
#include <unistd.h>
#include <dynamixel.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <termios.h>

//using namespace cv;
//using namespace std;

// Control table address
#define P_GOAL_POSITION_L	30
#define P_GOAL_POSITION_H	31
#define P_PRESENT_POSITION_L	36
#define P_PRESENT_POSITION_H	37
#define P_MOVING		46

// Defulat setting
#define DEFAULT_BAUDNUM		1 // 1Mbps
#define DEFAULT_ID1		1
#define DEFAULT_ID2		2
#define DEFAULT_ID4		4
#define DEFAULT_ID5		5
#define DEFAULT_ID6		6
#define DEFAULT_ID7		7
#define DEFAULT_ID8		8
#define DEFAULT_ID9		9
#define DEFAULT_ID10		10
#define DEFAULT_ID11		11
#define DEFAULT_ID12		12
#define DEFAULT_ID13		13
#define DEFAULT_ID14		14
#define DEFAULT_ID10		10
#define DEFAULT_ID15		15
#define DEFAULT_ID16		16
#define DEFAULT_ID17		17
#define DEFAULT_ID18		18
#define DEFAULT_ID19		19
#define DEFAULT_ID20		20

void PrintCommStatus(int CommStatus);
void PrintErrorCode(void);

//int OpenAdrPort (char* sPortNumber);
//int WriteAdrPort(char* psOutput);
//int ReadAdrPort(char* psResponse, int iMax);
//void CloseAdrPort(void);
//int getProcIdByName(string procName);



int main()
{
    int i;
	int baudnum = DEFAULT_BAUDNUM;
	int GoalPos[2] = {0, 1023};
	//int GoalPos[2] = {0, 4095}; // for Ex series
	int index = 0;
	int deviceIndex = 0;
	int Moving, PresentPos;
	int CommStatus;

	system("/home/humanoid/Documents/visao/bin/Debug/visao");
system("echo 123456 | sudo -S chmod 777 /dev/ttyUSB*");//USB
//system("./home/humanoidfei/Documents/visao/bin/Debug/visao");


    //id_t PID = getpid();
    char string[50];
    sprintf(string,"echo 123456 | sudo -S renice -20 -p %d", getpid());
    system(string);

  //  char command[50];
  //  sprintf(command, "echo 123456 | sudo -S renice 0 -p %d", getProcIdByName("pulseaudio"));
  //  system(command);
    //int PID = popen("pidof pulseaudio","r");
    //system("echo 123456 | sudo -S renice 0 1533");
	//setpriority(PRIO_PROCESS, getpid(), -20);

	printf( "\n\nRx-28 example for Linux\n\n" );
	///////// Open USB2Dynamixel ////////////
	if( dxl_initialize(deviceIndex, baudnum) == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		printf( "Press Enter key to terminate...\n" );
		getchar();
		return 0;
	}
	else
		printf( "Succeed to open USB2Dynamixel!\n" );

	while(1)
	{
		printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
		if(getchar() == 0x1b)
			break;

	//	// Write goal position
		for(i = 1;i<21;i++)
        dxl_write_word( i, P_GOAL_POSITION_L, GoalPos[index] );
       dxl_write_word( 3, 32, 0);//usleep(1000000);
					if( index == 0 )
						index = 1;
					else
						index = 0;










	/*	do
		{
			// Read present position
			//for(i=1;i<11;i++)
			PresentPos = dxl_read_word( i, P_PRESENT_POSITION_L ); //usleep(1000000);
			CommStatus = dxl_get_result();

			if( CommStatus == COMM_RXSUCCESS )
			{
				printf( "%d   %d\n",GoalPos[index], PresentPos );
				PrintErrorCode();
			}
			else
			{
				PrintCommStatus(CommStatus);
				break;
			}
			// Check moving done
			//for(i=1;i<11;i++)
			Moving = dxl_read_byte( i, P_MOVING );
			CommStatus = dxl_get_result();

			if( CommStatus == COMM_RXSUCCESS )
			{
				if( Moving == 0 )
				{
					// Change goal position
					if( index == 0 )
						index = 1;
					else
						index = 0;
				}

				PrintErrorCode();
			}
			else
			{
				PrintCommStatus(CommStatus);
				break;
			}
		}while(Moving == 1);*/
		}
	//}
	// Close device
	dxl_terminate();
	printf( "Press Enter key to terminate...\n" );
	getchar();
	return 0;
}
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

/*int getProcIdByName(string procName)
{
    int pid = -1;

    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
                ifstream cmdFile(cmdPath.c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    // Compare against requested process name
                    if (procName == cmdLine)
                        pid = id;
                }
            }
        }
    }

    closedir(dp);

    return pid;
}*/
