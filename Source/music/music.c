
#include "LPC17xx.h"
#include "music.h"

#include "../timer/timer.h"
#include <stdio.h>


void playNote(NOTE note)
{
	if(note.freq != pause)
	{
		reset_timer(2);
		init_timer(2, 0, 0, 3, note.freq); // , SRImatchReg = 3 (interrupt + reset)
		enable_timer(2);
	}
	reset_timer(3);
	init_timer(3, 0, 0, 3, note.duration); //  SRImatchReg = 3 (interrupt + reset)
	enable_timer(3);
}

BOOL isNotePlaying(void)
{
	return ((LPC_TIM2->TCR != 0) || (LPC_TIM3->TCR != 0));
}
