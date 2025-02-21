/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

#include "../Map/map.h"
#include "CAN/CAN.h"
#include "music/music.h"




/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;

extern char led_value;
extern volatile finished;

volatile int gamePaused = 1;  // Inizia in modalità pausa
//extern volatile int lastDirection;
extern volatile int joyStickDirection;
//extern volatile startedd;

extern volatile uint16_t score; 
extern volatile uint8_t seconds;
extern volatile uint8_t lifes;


#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1

extern int chase;
extern volatile int finished;


//SHORTENING UNDERTALE: TOO MANY REPETITIONS
NOTE pacman_song[] = 
{
   //1
	 {C4,time_semicroma},
	 {C5,time_semicroma},
	 {G4,time_semicroma}, 
	 {E4,time_semicroma},
	 {C5,time_semicroma},
	 {G4,time_semicroma},
	 {E4,time_croma},
	 //2
	 {CS4,time_semicroma},
	 {CS5,time_semicroma},
	 {GS4,time_semicroma}, 
	 {E4,time_semicroma},
	 {CS5,time_semicroma},
	 {GS4,time_semicroma},
	 {E4,time_croma},
	 	 //3
	 {C4,time_semicroma},
	 {C5,time_semicroma},
	 {G4,time_semicroma}, 
	 {E4,time_semicroma},
	 {C4,time_semicroma},
	 {G4,time_semicroma},
	 {E4,time_croma},
	 //4
	 {FS4,time_biscroma},//sol diesis
	 {F4,time_biscroma},
	 {GS4,time_biscroma},
	 {GS4,time_biscroma},//la diesis
	 {A4,time_biscroma},
	 {AS4,time_biscroma},
	 {AS4,time_biscroma},//si diesis
	 {A4,time_biscroma},
	 {C5,time_semicroma},
	 {C5,time_semicroma}

};

NOTE chase_song[] = {
    // 1
    {C4, time_semicroma}, {E4, time_semicroma}, {G4, time_semicroma}, {C5, time_semicroma},
    {G4, time_semicroma}, {E4, time_semicroma}, {C4, time_semicroma}, {pause, time_semicroma},

    // 2
    {D4, time_semicroma}, {F4, time_semicroma}, {A4, time_semicroma}, {D5, time_semicroma},
    {A4, time_semicroma}, {F4, time_semicroma}, {D4, time_semicroma}, {pause, time_semicroma},

    // 3
    {E4, time_semicroma}, {G4, time_semicroma}, {B4, time_semicroma}, {E5, time_semicroma},
    {B4, time_semicroma}, {G4, time_semicroma}, {E4, time_semicroma}, {pause, time_semicroma},

    // 4
    {C4, time_semicroma}, {E4, time_semicroma}, {G4, time_semicroma}, {C5, time_semicroma},
    {D5, time_semicroma}, {B4, time_semicroma}, {G4, time_semicroma}, {E4, time_semicroma},

    // 5
    {F4, time_biscroma}, {G4, time_biscroma}, {A4, time_biscroma}, {B4, time_biscroma},
    {C5, time_biscroma}, {D5, time_biscroma}, {E5, time_biscroma}, {pause, time_biscroma}
};

NOTE end_song[] = 
{
    // Parte 1 - Apertura trionfale
    {C4, time_semicroma}, 
    {E4, time_semicroma}, 
    {G4, time_semicroma}, 
    {C5, time_semicroma}, 
    {pause, time_semicroma}, 
    {G4, time_semicroma}, 
    {E4, time_semicroma}, 
    {C4, time_croma}, 
    
    // Parte 2 - Scala discendente
    {G4, time_semicroma}, 
    {F4, time_semicroma}, 
    {E4, time_semicroma}, 
    {D4, time_semicroma}, 
    {pause, time_semicroma}, 
    {C4, time_croma},

    // Parte 3 - Chiusura lenta
    {C4, time_semicroma}, 
    {E4, time_semicroma}, 
    {G4, time_semicroma}, 
    {C4, time_croma},
    {pause, time_semicroma}, 
    {C4, time_croma}
};


void RIT_IRQHandler (void)
{	
	
	// Preparazione del messaggio CAN da inviare su CAN2

	//Copia il valore del countdown nel campo data (in formato little-endian)
	CAN_TxMsg.data[0] = (score & 0xFF);            // Byte meno significativo
	CAN_TxMsg.data[1] = (score >> 8) ;       // Byte successivo
	CAN_TxMsg.data[2] = lifes ;      // Terzo byte
	CAN_TxMsg.data[3] = seconds;      // Byte pi? significativo
	CAN_TxMsg.id = 2;  // ID del messaggio (ad esempio, 21)
	CAN_TxMsg.len = 4;  // Lunghezza dei dati (4 byte per countdown)
	CAN_TxMsg.format = STANDARD_FORMAT;  // Identificatore standard
	CAN_TxMsg.type = DATA_FRAME;  // Tipo di frame (DATA FRAME)
	// Invia il messaggio da CAN1 a CAN2
	CAN_wrMsg(1, &CAN_TxMsg);  // Trasmetti il messaggio da CAN1
	
	
	
	static int currentNote = 0;
	static int ticks = 0;
	NOTE *currentSong;
	int songLength;

	// Seleziona la canzone basandoti su "chase"
	if (finished) {
			// Riproduci la end_song se il gioco è terminato
			currentSong = end_song;
			songLength = sizeof(end_song) / sizeof(end_song[0]);
	} else if (chase) {
			// Riproduci pacman_song se chase è attivo
			currentSong = pacman_song;
			songLength = sizeof(pacman_song) / sizeof(pacman_song[0]);
	} else {
			// Riproduci chase_song altrimenti
			currentSong = chase_song;
			songLength = sizeof(chase_song) / sizeof(chase_song[0]);
	}

	// Gestisci la riproduzione delle note
	if (!isNotePlaying()) {
		++ticks;
		if (ticks == UPTICKS) {
			ticks = 0;
			playNote(currentSong[currentNote++ % songLength]);
		}
	}
	
	
	
	
	static int J_select = 0;
	static int J_down = 0;
	static int J_left = 0;
	static int J_right = 0;
	static int J_up = 0;
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick J_Select pressed p1.25*/
		
		J_select++;
		switch(J_select){
			case 1:
 				//code here
				break;
			default:
				break;
		}
	}
	else{
			J_select=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		
		J_down++;
		switch(J_down){
			case 1:
					
				joyStickDirection = 0; // in questo modo si muove solo quando viene chiamata l interruzione del timer e non ogli volta che c'e movimento del jooystick
				//movePacman(0); 
				//code here
				break;
			default:
				break;
		}
	}
	else{
			J_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick J_Left pressed p1.27*/
		J_left++;
		switch(J_left){
			case 1:
				//code here
				joyStickDirection = 1;
				//movePacman(1);
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick J_right pressed p1.28*/
		
		J_right++;
		switch(J_right){
			case 1:
				//code here
				joyStickDirection = 2;
			//movePacman(2);
				
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick J_up pressed p1.29*/
		
		J_up++;
		switch(J_up){
			case 1:
				//code here
				joyStickDirection = 3;
				//movePacman(3);
				break;
			default:
				break;
		}
	}
	else{
			J_up=0;
	}
	
	
	/* button management */
	if(down_0 != 0){
		down_0++; 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */			
			switch(down_0){
				case 2:				
				if (!finished){
					if (gamePaused) {
						gamePaused = 0;// Se il gioco è in pausa, riprendi

						// Rimuovi il messaggio "PAUSE"
						disable_RIT();
						GUI_Text(200, 0, (uint8_t *)"     ", 0xFFFF, 0x0000);  // Cancella messaggio
						enable_RIT();
						
						enable_timer(0);
						enable_timer(1);
						enable_timer(2);

						
						} else {
								// Se il gioco è attivo, metti in pausa
								gamePaused = 1;

								// Mostra il messaggio "PAUSE"
								disable_RIT();
								GUI_Text(200, 0, (uint8_t *)"PAUSE", 0xFFFF, 0x0000);  // Testo bianco con sfondo nero
								enable_RIT();
								// Disabilita i timer
								disable_timer(0);
								disable_timer(1);
								disable_timer(2);
						}
				}
				
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_1!=0){ 
		down_1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */			
			switch(down_1){
				case 2:				
					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_2 != 0){
		down_2++;	
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */			
			switch(down_2){
				case 2:		
					//code here
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
		
	}
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
