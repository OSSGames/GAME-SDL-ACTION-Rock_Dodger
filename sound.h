#ifdef RD_SOUND_OSS
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_PLAYING_SOUNDS 20
#define CONDERROR(a) if ((a)) {fprintf(stderr,"Error: %s\n",SDL_GetError());exit(1);}
#define NULLERROR(a) CONDERROR((a)==NULL)


// Storage for the sound
typedef struct sound_s {
	SDL_AudioSpec audiospec;
	Uint8 *sample;
	Uint32 length;
} sound_t, *sound_p;


// File names
char *filename[] = {
	//"/usr/share/games/Chromium-0.9/data/wav/exploBig.wav"
	"wav/booom.wav"
};
#define NUM_SAMPLES (sizeof(filename)/sizeof(filename[0]))


// Dynamic storage area for playing sounds
typedef struct playing_s {
	int active;
	sound_p sound;
	Uint32 position;
} playing_t, *playing_p;

// The array of sound effects.
// Initialised using the file name and otherwise blank records.
sound_t sound[NUM_SAMPLES];

// The array of currently-playing sounds
playing_t playing[MAX_PLAYING_SOUNDS];

SDL_Surface *screen;

SDL_AudioSpec audiospec_required, audiospec_desired;

void read_samples() {
	int i;
	SDL_AudioCVT cvt;
	Uint8 *buf;

	for (i=0; i<NUM_SAMPLES; i++) {

		// Read the sample files from the 'filename' element
		// of the sound[] array, into the 'sound' element.
		NULLERROR(SDL_LoadWAV(
				filename[i],
				&(sound[i].audiospec),
				&(sound[i].sample),
				&(sound[i].length)
		));

		// Create an audio conversion structure
		if (SDL_BuildAudioCVT(
				&cvt,
				sound[i].audiospec.format,
				sound[i].audiospec.channels,
				sound[i].audiospec.freq, 
				audiospec_required.format,
				audiospec_required.channels,
				audiospec_required.freq) < 0) {
			printf ("Can't construct Audio converter %s: %s\n", filename[i], SDL_GetError());
			exit(0);
		}

		// Allocate buffer for the sound
		cvt.len = sound[i].length;
		NULLERROR(buf = (Uint8 *) malloc(cvt.len * cvt.len_mult));
		
		// Copy sound samples to the new buffer
		memcpy(buf, sound[i].sample, sound[i].length);

		// Perform conversion
		cvt.buf = buf;
		CONDERROR(SDL_ConvertAudio(&cvt)<0);

		// Swap the converted data for the original
		SDL_FreeWAV(sound[i].sample);
		sound[i].sample = buf;
		sound[i].length *= cvt.len_mult;

	}

}

void audio_callback(void *userdata, Uint8 *audio, int length) {
	int i;

	// Clear the audio buffer
	memset(audio,0,length);

	// Mix each playing sound
	// [mostly cribbed from Programming Linux Games, John Hall]
	for (i=0; i<MAX_PLAYING_SOUNDS; i++) {
		if (playing[i].active) {

			Uint8 *sound_buf;
			Uint32 sound_len;

	    	/* Locate this sound's current buffer position. */
	    	sound_buf = playing[i].sound->sample;
	    	sound_buf += playing[i].position;

	    	/* Determine the number of samples to mix. */
	    	if ((playing[i].position + length) > playing[i].sound->length) {
				sound_len = playing[i].sound->length - playing[i].position;
	    	} else {
				sound_len = length;
	    	}

	    	/* Mix this sound into the stream. */
	    	SDL_MixAudio(audio, sound_buf, sound_len, 127);

	    	/* Update the sound buffer's position. */
	    	playing[i].position += length;

	    	/* Have we reached the end of the sound? */
	    	if (playing[i].position >= playing[i].sound->length) {
				// If it's a repeating sound, loop by resetting the pointer to 0
				if (0) {
					playing[i].position = 0;
				}
				else {
					playing[i].active = 0;	/* mark it inactive */
				}
				
	    	}

		}
	}

}


void init_sound() {

	// Request audio device
	audiospec_desired.freq = 44100;
	audiospec_desired.format = AUDIO_S16;
	audiospec_desired.samples = 4096;
	audiospec_desired.channels = 2;
	audiospec_desired.callback = audio_callback;
	audiospec_desired.userdata = NULL;

	if (SDL_OpenAudio( &audiospec_desired, &audiospec_required) < 0) {
		printf("Can't open audio mate.\n");
		exit(1);
	}

	read_samples();

	SDL_PauseAudio(0);
}

void play_sound(int i) {
	int slot;
	sound_p p;
	p = &sound[i];

	// Find a vacant slot
	for (slot=0; slot<MAX_PLAYING_SOUNDS && playing[slot].active; slot++)
		;
	// If there's lots of sounds playing, we're going to have to abandon this one
	if (slot>=MAX_PLAYING_SOUNDS) return;
		
	// Insert pointers to the sound, and activate the sound.
	SDL_LockAudio();
	playing[slot].active = 1;
	playing[slot].sound = p;
	playing[slot].position = 0;
	SDL_UnlockAudio();

}

#endif
