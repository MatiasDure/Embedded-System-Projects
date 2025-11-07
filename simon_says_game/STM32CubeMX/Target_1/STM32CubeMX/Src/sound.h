#ifndef _SOUND_
#define _SOUND_

#include <stdint.h>

#include "timer.h"

// Octave 4
#define ASHARP_NOTE_4 466U
#define B_NOTE_4 494U

// Octave 5
#define A_NOTE_5 880U
#define B_NOTE_5 988U
#define ASHARP_NOTE_5 932U
#define C_NOTE_5 523U
#define D_NOTE_5 587U
#define DSHARP_NOTE_5 622U
#define E_NOTE_5 659U
#define F_NOTE_5 698U
#define FSHARP_NOTE_5 740U
#define G_NOTE_5 784U
#define GSHARP_NOTE_5 831U

// octave 6
#define C_NOTE_6 1047U
#define CSHARP_NOTE_6 1109U
#define D_NOTE_6 1175U

typedef enum {
	PLAY,
	PLAYING,
	TRANSITIONING,
	STOPPED,
	OFF
} SoundState;

typedef struct {
    uint16_t frequency;
    uint16_t duration;
} Note;

typedef struct {
	SoundState state;
	uint32_t lastTimestamp;
	uint8_t index;
} Sound_TypeDef;

// Used note sequence from: https://blogs.glowscotland.org.uk/sb/public/youthmusicinitiative/uploads/sites/3670/2020/09/09143506/Ipad-Bigger-Harry-Potter-Tune-Notes-Only.pdf
extern Note hedwigTheme[];
extern const uint8_t hedwigThemeLength;
// melody from: https://blogs.glowscotland.org.uk/sb/public/youthmusicinitiative/uploads/sites/3670/2020/09/09142521/Ipad-Pirates-of-the-Caribbean-Tune-Notes-Only.pdf
extern Note piratesTheme[];
extern const uint8_t pirateThemeLength;

void PlaySingleNote(TIM_TypeDef *timer, Note noteToPlay);
void PlaySequence(TIM_TypeDef *timer, Sound_TypeDef *soundState, Note *sequenceArr, uint8_t length, uint8_t loop);
void TurnOffSound(TIM_TypeDef *timer);
void TurnOnSound(TIM_TypeDef *timer);
#endif