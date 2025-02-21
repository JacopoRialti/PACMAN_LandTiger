/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "Map/map.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

const int freqs[8]={4240,3779,3367,3175,2834,2525,2249,2120};
extern volatile seconds; 
/* 
131Hz		k=4240 C3
147Hz		k=3779
165Hz		k=3367
175Hz		k=3175
196Hz		k=2834		
220Hz		k=2525
247Hz		k=2249
262Hz		k=2120 C4
*/

//const int freqs[8]={2120,1890,1684,1592,1417,1263,1125,1062};
/*
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/
const int melody[] = {
    0x7000, 0x6500, 0x6000, 0x6500, 0x7000, 0x7500, 0x8000, 0x7500, 
    0x7000, 0x6500, 0x7000, 0x6500, 0x6000, 0x5500, 0x6000, 0x6500, 
    0x7000, 0x7500, 0x8000, 0x8500, 0x8000, 0x7500, 0x7000, 0x6500, 
    0x7200, 0x6800, 0x6400, 0x6800, 0x7200, 0x7800, 0x8400, 0x7800, 
    0x7200, 0x6800, 0x6400, 0x6000, 0x6400, 0x6800, 0x7200, 0x7800
};
		
const int ghost_flee_sound[] = {
    0x4000, 0x4500, 0x5000, 0x5500, 0x5000, 0x4500, 
    0x4000, 0x3500, 0x3000, 0x3500, 0x4000, 0x4500, 
    0x4000, 0x3500, 0x3000, 0x3500
};
		const int pacman_death_sound[] = { 0x2000, 0x1500, 0x1000 };
		const int powerpill_sound[] = { 0x6000, 0x7000, 0x8000 };
		
		static int melody_index = 0;  // Indice della melodia corrente
		static int event_in_progress = 0;  // Flag per verificare se un evento è in corso
		static const int *current_sound = melody;  // Puntatore alla melodia corrente
		static int current_sound_length = sizeof(melody) / sizeof(melody[0]);  // Lunghezza della melodia corrente
		
		//volatile int pacman_death_event = 0;  // Evento morte di Pac-Man
		//volatile int powerpill_event = 0;     // Evento Powerpill

		extern int chase;
		static int last_seconds;  // Ultimo valore dei secondi aggiornato


void ADC_IRQHandler(void) {
	
	if (seconds == 60){
		last_seconds =	seconds;
	}
	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
  if(AD_current != AD_last){
		    if (AD_current != AD_last) {
        // Verifica se è necessario aggiornare la melodia
        if (seconds != last_seconds) {  // Solo se seconds è diverso da last_seconds
            last_seconds = seconds;    // Aggiorna il valore di last_seconds
            
            disable_timer(3);
            reset_timer(3);

            if (!chase) {
                current_sound = ghost_flee_sound;
                current_sound_length = sizeof(ghost_flee_sound) / sizeof(ghost_flee_sound[0]);
            } else {
                // Nessun evento: riprendi la melodia principale
                current_sound = melody;
                current_sound_length = sizeof(melody) / sizeof(melody[0]);
            }

            // Passa la nota corrente alla funzione del Timer3
            init_timer(3, 0, 0, 3, current_sound[melody_index]);
            enable_timer(3);

            // Incrementa l'indice della melodia corrente
            melody_index = (melody_index + 1) % current_sound_length;
        }
        
        // Memorizza l'ultimo valore dell'ADC
        AD_last = AD_current;
    }
}
}

