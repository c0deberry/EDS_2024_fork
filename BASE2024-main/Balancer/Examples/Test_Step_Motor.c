
#include <stdint.h>
#include <stdbool.h>
#include <step.h>
#include <i2c.h>


void delayMillis(uint16_t delay);

uint8_t *convDecByteToHex(uint8_t byte);
uint8_t vmax;
uint8_t addr = 0xC2; 	// Schreibadresse des Motors
uint8_t geschw = 0x00;	// 0x0 bis 0xF

int main(void)
{
    stepMotorInit(0xC2, 0);

    /* Hauptprogramm: Endlosschleife */
    while(1)
    {
    	for(int i = 0; i < 500; ++i )
    	{
    	int ac_pos = getActualPosition(0xC2); // Auslesen der aktuellen Motorposition
    	setPosition(addr,  ac_pos+32767);

    	}
    	softStop(0xc2); 		// Stoppt den Aktuellen Befehl

    	setVmax(addr, geschw); // Stetzt die Geschwindigkeit des Steppers
    	geschw++;

    	if(geschw == 0xf)
    	{
    		geschw = 0x00;
    	}
    }
    return 0;
}


void delayMillis(uint16_t delay){
	uint16_t i = 0;
	for(; delay > 0; --delay)
	{
		for (i = 0; i < 1245; ++i)
		{
			;
		}
	}
}



