/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../led/led.h"

#include "Map/map.h"
#include "RIT/RIT.h"

#define ScoreX 100
#define ScoreY 0

extern volatile  timeGenerationPP;
extern volatile nPills;
extern volatile lastDirection; 
extern uint16_t score;
extern volatile gamePaused;
extern volatile powerPills;
volatile uint8_t seconds; 
volatile int finished;

extern int chase;
extern int secondsChase;
extern int secondsRegeneration ;
extern int eatenGhost ; 

extern int rowGhost;
extern int colGhost;


uint16_t SinTable[45] =                                       /* ?????                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};



/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char led_value;					/* defined in funct_led								*/
int countPac =0;
int targetGhost = 3;

void TIMER0_IRQHandler (void)
{


	/* Match register 0 interrupt service routine */
	if (LPC_TIM0->IR & 01)
		
	{
		//printa secodi 
		seconds -= 1;
		if (seconds <= 0){
			terminateGame();
		}

		if (seconds == 40){
		//reset_timer(2);
		//disable_timer(2);
		//init_timer(2, 0, 0, 3, 0x7F2815); 		// 0x7F2815 1/3 sec
		//enable_timer(2);
		targetGhost=2;
		}
		
		if (seconds == 20){
		//reset_timer(2);
		//disable_timer(2);
		//init_timer(2, 0, 0, 3, 0x5F5E10); 		//	0x5F5E10 1/4 sec
		//enable_timer(2);
		targetGhost=1;
		}
		
		if (chase == 0){
			secondsChase--; 
		}
		
		if(secondsChase <= 0){
			chase = 1;
			secondsChase = 10;
		}
		if (eatenGhost){
			clearCell(rowGhost, colGhost);
			secondsRegeneration--;
		}
		if (secondsRegeneration <=0){
			rowGhost = 15;
			colGhost = 15;
			drawGhost(rowGhost, colGhost);
			secondsRegeneration = 3;
			eatenGhost = 0;
			chase=1;
		}
		
		if (!finished){
			if (seconds < 50 && nPills !=0 ){
				generatePowerPills(map);
			}
			if(nPills == powerPills +20){
				while (powerPills > 0){
				generatePowerPills(map);
				}
			}
		}
		/*
		char secondsStr[20];
		sprintf(secondsStr, "        :   ");// La stringa per il punteggio (assumiamo che il punteggio non superi i 20 caratteri)
		disable_RIT();
		GUI_Text(0, 0, (uint8_t *)secondsStr,0xFFFF, 0x0000);
		enable_RIT();
		sprintf(secondsStr, "Seconds :%d", seconds);
		disable_RIT();
		GUI_Text(0, 0, (uint8_t *)secondsStr,0xFFFF, 0x0000);
		enable_RIT();
		*/
		
		
		LPC_TIM0->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 02)
  {
	
		LPC_TIM0->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 4)
  {
		
		LPC_TIM0->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 8)
  {
	 
		LPC_TIM0->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
	if (LPC_TIM1->IR & 01)
	{
		//printPause(gamePaused);
		movePacman();
		countPac++;
		if (countPac == targetGhost){
			if(!eatenGhost){
			disable_RIT();
			updateGhost();
			enable_RIT();
			}
			countPac=0;
		}
					

		//char scoreStr[20];  // La stringa per il punteggio (assumiamo che il punteggio non superi i 20 caratteri)
		//sprintf(scoreStr, "Score: %d", score);
		//GUI_Text(ScoreX, ScoreY, (uint8_t *)scoreStr,0xFFFF, 0x0000);
		
		LPC_TIM1->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM1->IR & 02)
  {
	
		LPC_TIM1->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM1->IR & 4)
  {
		
		LPC_TIM1->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM1->IR & 8)
  {
	 
		LPC_TIM1->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
	if (LPC_TIM2->IR & 01)
	{	
		/*if(!eatenGhost){
		disable_RIT();
		updateGhost();
		enable_RIT();
		}*/
		
		static int sineticks=0;
		/* DAC management */	
		static int currentValue; 
		currentValue = SinTable[sineticks];
		currentValue -= 410;
		currentValue /= 1;
		currentValue += 410;
		LPC_DAC->DACR = currentValue <<6;
		sineticks++;
		if(sineticks==45) sineticks=0;

		
		
		LPC_TIM2->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 02)
  {
	
		LPC_TIM2->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 4)
  {
		
		LPC_TIM2->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 8)
  {
	 
		LPC_TIM2->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer3_IRQHandler
**
** Descriptions:		Timer/Counter 3 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
  /* Match register 0 interrupt service routine */
	if (LPC_TIM3->IR & 01)
	{
		disable_timer(2);
		disable_timer(3);
		
		LPC_TIM3->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM3->IR & 02)
  {
	
		LPC_TIM3->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM3->IR & 4)
  {
		
		LPC_TIM3->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM3->IR & 8)
  {
	 
		LPC_TIM3->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}



/******************************************************************************
**                            End Of File
******************************************************************************/
