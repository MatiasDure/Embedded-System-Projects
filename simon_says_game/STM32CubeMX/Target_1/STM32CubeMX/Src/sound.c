#include "sound.h"
#include "delay.h"

Note hedwigTheme[] = {
    {B_NOTE_4, 400}, 
    {E_NOTE_5, 600}, 
    {G_NOTE_5, 200},   
    {FSHARP_NOTE_5, 400},   
    {E_NOTE_5, 800},   
    {B_NOTE_5, 400},   
    {A_NOTE_5, 1000},  
    {FSHARP_NOTE_5, 1000},  
    {E_NOTE_5, 600},   
    {G_NOTE_5, 200},   
    {FSHARP_NOTE_5, 400},   
    {DSHARP_NOTE_5, 600},   
    {F_NOTE_5, 400},   
    {B_NOTE_4, 1400},  
		//---------------
		{B_NOTE_4, 400},   
    {E_NOTE_5, 600},   
    {G_NOTE_5, 200},   
    {FSHARP_NOTE_5, 400},   
    {E_NOTE_5, 800},   
    {B_NOTE_5, 400},   
    {D_NOTE_6, 800},  
    {CSHARP_NOTE_6, 400},  
    {C_NOTE_6, 800},  
    {GSHARP_NOTE_5, 400},  
    {C_NOTE_6, 600},  
    {B_NOTE_5, 200}, 
    {ASHARP_NOTE_5, 400},   
    {ASHARP_NOTE_4, 800},   
    {G_NOTE_5, 400},  
    {E_NOTE_5, 1400},  
		// --------------
		{G_NOTE_5, 400},    
    {B_NOTE_5, 800},   
    {G_NOTE_5, 400},   
    {B_NOTE_5, 800},   
    {G_NOTE_5, 400},    
    {C_NOTE_6, 800},   
    {B_NOTE_5, 400},   
    {ASHARP_NOTE_5, 800},    
    {F_NOTE_5, 400},   
    {G_NOTE_5, 600},   
    {B_NOTE_5, 200},   
    {ASHARP_NOTE_5, 400},   
    {ASHARP_NOTE_4, 800},   
    {B_NOTE_4, 400},    
    {B_NOTE_5, 1400},   
		// ---------------
		{G_NOTE_5, 400},    
    {B_NOTE_5, 800},   
    {G_NOTE_5, 400},   
    {B_NOTE_5, 800},   
    {G_NOTE_5, 400},   
    {D_NOTE_6, 800},  
    {CSHARP_NOTE_6, 400},   
    {C_NOTE_6, 800},   
    {GSHARP_NOTE_5, 400},   
    {C_NOTE_6, 600},
    {B_NOTE_5, 200},   
    {ASHARP_NOTE_5, 400},  
    {ASHARP_NOTE_4, 800},  
    {G_NOTE_5, 400},   
    {E_NOTE_5, 1400},   
};

const uint8_t hedwigThemeLength = sizeof(hedwigTheme) / sizeof(hedwigTheme[0]);

Note piratesTheme[] = {
		{D_NOTE_5, 400}, // d
    {D_NOTE_5, 600}, // d
    {D_NOTE_5, 200},   // d 
    {E_NOTE_5, 200},   // e
    {F_NOTE_5, 400},   // f
    {F_NOTE_5, 600},   // f
    {F_NOTE_5, 200},  // f
    {G_NOTE_5, 200},  // g
    {E_NOTE_5, 400},   // e
    {E_NOTE_5, 600},   // e
    {D_NOTE_5, 200},   // d
    {C_NOTE_5, 200},   // c
    {C_NOTE_5, 200},   // c
    {D_NOTE_5, 1000},  // d
		//---------------
		{D_NOTE_5, 400}, // d
    {D_NOTE_5, 600}, // d
    {D_NOTE_5, 200},   // d 
    {E_NOTE_5, 200},   // e
    {F_NOTE_5, 400},   // f
    {F_NOTE_5, 600},   // f
    {F_NOTE_5, 200},  // f
    {G_NOTE_5, 200},  // g
    {E_NOTE_5, 400},   // e
    {E_NOTE_5, 600},   // e
    {D_NOTE_5, 200},   // d
    {C_NOTE_5, 200},   // c
    {C_NOTE_5, 800},   // c
		//---------------
		{D_NOTE_5, 400}, // d
    {D_NOTE_5, 600}, // d
    {D_NOTE_5, 200},   // d 
    {F_NOTE_5, 200},   // f
    {G_NOTE_5, 400},   // g
    {G_NOTE_5, 600},   // g
    {G_NOTE_5, 200},  // g
    {A_NOTE_5, 200},  // a
    {ASHARP_NOTE_5, 400},   // Bb
    {ASHARP_NOTE_5, 400},   // Bb
    {A_NOTE_5, 200},   // A
    {G_NOTE_5, 200},   // G
    {A_NOTE_5, 200},   // A
		{D_NOTE_5, 600},   // D
		//---------------
		{F_NOTE_5, 400}, // f
    {F_NOTE_5, 400}, // f
    {G_NOTE_5, 400},   // g 
    {A_NOTE_5, 200},   // a
    {D_NOTE_5, 600},   // d
    {D_NOTE_5, 200},   // d
    {F_NOTE_5, 200},  // f
    {E_NOTE_5, 400},  // e
    {E_NOTE_5, 600},   // e
    {F_NOTE_5, 200},   // f
    {D_NOTE_5, 200},   // d
    {E_NOTE_5, 1400},   // e
    //---------------
		{F_NOTE_5, 400}, // f
    {F_NOTE_5, 400}, // f
    {G_NOTE_5, 400},   // g 
    {A_NOTE_5, 200},   // a
    {D_NOTE_5, 600},   // d
    {D_NOTE_5, 200},   // d
    {F_NOTE_5, 200},  // f
    {E_NOTE_5, 400},  // e
    {E_NOTE_5, 600},   // e
    {D_NOTE_5, 200},   // d
    {C_NOTE_5, 200},   // c
    {D_NOTE_5, 1400},   // d
};

const uint8_t pirateThemeLength = sizeof(piratesTheme) / sizeof(piratesTheme[0]);

void PlaySingleNote(TIM_TypeDef *timer, Note noteToPlay) {
	Timer_updateFreq(timer, noteToPlay.frequency, 50);
	newDelay_ms(noteToPlay.duration);
}

static void ResetSoundState(Sound_TypeDef *soundState) {
	soundState->index = 0;
	soundState->state = PLAY;
}

// needs to be a statemachine to not block
void PlaySequence(TIM_TypeDef *timer, Sound_TypeDef *soundState, Note *sequenceArr, uint8_t length, uint8_t loop) {
	switch(soundState->state) {
		case PLAY:
			soundState->lastTimestamp = getSysTickCounter();
			Timer_updateFreq(timer, sequenceArr[soundState->index].frequency, 50);
			soundState->state = PLAYING;
			break;
		case PLAYING:
			if(!hasDelayElapsed(soundState->lastTimestamp, sequenceArr[soundState->index].duration)) break;
			soundState->state = TRANSITIONING;
			break;
		case TRANSITIONING:
			// went through all the notes
			if(++soundState->index == length) {
				soundState->state = STOPPED;
				break;
			}
			soundState->state = PLAY;
			break;
		case STOPPED:
			if(!loop) soundState->state = OFF;
			ResetSoundState(soundState);
			break;
		case OFF:
			// no more playing
			break;
		default:
			break;
	}	
}

void TurnOffSound(TIM_TypeDef *timer) {
	Timer_disablePWM(timer);
}

void TurnOnSound(TIM_TypeDef *timer) {
	Timer_enablePWM(timer);
}