// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#define grendizer_c_init game_init
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#define grendizer_c_update game_update
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#define grendizer_c_draw game_draw
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#include "../src/game.c"
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#undef grendizer_c_init
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#undef grendizer_c_update
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
#undef grendizer_c_draw
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
static int fixture_tick;
// EDUCATIONAL: Precondition: portable host boot; reset game and fixture clock; O(entity capacity), emits normal audio reset calls.
void grendizer_c_init(void){game_init();new_game();fixture_tick=0;}
// EDUCATIONAL: Precondition: initialized fixture; select scheduled scenes then execute a real update; bounded entity work, normal input/audio ABI effects.
void grendizer_c_update(void){
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
int t=fixture_tick++;
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
if(t==0||t==120||t==240||t==600){stage=t==0?2:t==120?3:t==240?4:5;start_stage_intro();begin_play();invuln=2000;}
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
if(t==360){stage=4;next_stage();}
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
if(t==960){stage=5;next_stage();}
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
// EDUCATIONAL: Exercise actual transformation chords during each combat environment, including detachment and reunion on the Moon.
if(t==30||t==150||t==270||t==720)update_play_input(PRG32_BTN_DOWN|PRG32_BTN_A);
// EDUCATIONAL: Recombination consumes the same player input chord used in the playable cartridge.
if(t==90||t==210||t==330||t==840)update_play_input(PRG32_BTN_UP|PRG32_BTN_A);
// EDUCATIONAL: Continue the unmodified game simulation after fixture inputs.
game_update();
// EDUCATIONAL: Target-only timed scene fixture; mutates game state for rendering/relocation verification, not a playthrough.
}
// EDUCATIONAL: Precondition: initialized game; render the selected scene; bounded sprite work, writes only through portable graphics calls.
void grendizer_c_draw(void){game_draw();}
