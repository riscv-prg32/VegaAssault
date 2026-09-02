// EDUCATIONAL: Use host stdout to emit the rendered RGB image.
#include <stdio.h>
// EDUCATIONAL: Validate that the scripted inputs reach live gameplay.
#include <assert.h>
// EDUCATIONAL: Execute the actual cartridge implementation and its generated sprites.
#include "../src/game.c"
// EDUCATIONAL: Include rasterization functions and the font extracted from the configured PRG32 checkout.
#include "capture_renderer.h"
// EDUCATIONAL: Hold the scripted portable input bits for the next simulation tick.
static uint32_t capture_input;
// EDUCATIONAL: Precondition: scripted input set; reads only; O(1); replaces the host input service.
uint32_t prg32_input_read(void){return capture_input;}
// EDUCATIONAL: Precondition: single-threaded capture; no shared display; O(1); replaces the host graphics lock.
void prg32_gfx_lock(void){}
// EDUCATIONAL: Precondition: single-threaded capture; no shared display; O(1); replaces the host graphics unlock.
void prg32_gfx_unlock(void){}
// EDUCATIONAL: Precondition: valid channel parameters; intentionally silent capture; O(1); replaces portable note-on.
void prg32_audio_note_on(uint8_t c,uint8_t i,uint8_t n,uint8_t v){(void)c;(void)i;(void)n;(void)v;}
// EDUCATIONAL: Precondition: valid channel; intentionally silent capture; O(1); replaces portable note-off.
void prg32_audio_note_off(uint8_t c){(void)c;}
// EDUCATIONAL: Precondition: valid sample parameters; intentionally silent capture; O(1); replaces portable sample playback.
int prg32_audio_play_sample_pan(uint16_t s,uint8_t v,uint16_t p,int8_t pan){(void)s;(void)v;(void)p;(void)pan;return 0;}
// EDUCATIONAL: Precondition: upstream renderer available; changes game/framebuffer and writes stdout; O(ticks*entity capacity+pixels); uses actual game entry points.
int main(void){
// EDUCATIONAL: Reserve counters and initialize the cartridge exactly as the host would.
int tick,x,y;grendizer_c_init();
// EDUCATIONAL: Two separated START presses navigate attract and title; then hold A during combat without editing game state.
for(tick=0;tick<292;tick++){capture_input=(tick==0||tick==2)?PRG32_BTN_START:(tick>=260?PRG32_BTN_A:0);grendizer_c_update();grendizer_c_draw();}
// EDUCATIONAL: Require a live first-stage scene instead of accidentally saving a menu or terminal screen.
assert(state==ST_PLAY&&stage==1&&lives>0);
// EDUCATIONAL: Write a portable RGB image header at the native 320x200 viewport resolution.
printf("P6\n320 200\n255\n");
// EDUCATIONAL: Crop only the host top/bottom bands and expand RGB565 channel values to RGB888.
for(y=0;y<200;y++)for(x=0;x<320;x++){uint16_t p=g_fb[(y+PRG32_VIEWPORT_Y)*320+x];putchar(((p>>11)&31)*255/31);putchar(((p>>5)&63)*255/63);putchar((p&31)*255/31);}
// EDUCATIONAL: Report a write error to the capture driver.
return ferror(stdout)?1:0;
// EDUCATIONAL: End the reproducible capture program.
}
