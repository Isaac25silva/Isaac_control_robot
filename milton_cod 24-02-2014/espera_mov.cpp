#include <dynamixel.h>
#include <unistd.h>

//extern int *mem;

inline void espera_mov()
{
    for(unsigned short int i=3;i<23;i++)
    {
        while(dxl_read_byte( i, P_MOVING ) != 0)
            usleep(2000); //Espera 2 milisegundos evitando uso excessivo do processador
    }
}


void espera_mov_ID(int id)
{
    while(dxl_read_byte( id, P_MOVING ) != 0)
        usleep(2000); //Espera 2 milisegundos evitando uso excessivo do processador
}
