// EDUCATIONAL: Use assertions to report violated behavioral invariants in a host process.
#include <assert.h>
// EDUCATIONAL: Print a concise host test result.
#include <stdio.h>
// EDUCATIONAL: Include the cartridge translation unit to inspect fixed internal state without changing its ABI.
#include "../src/game.c"
// EDUCATIONAL: Mock input and count portable audio/draw effects without requiring firmware.
static uint32_t test_input;
// EDUCATIONAL: Record music events and boss draws for pause verification.
static int audio_events, boss_draws, sample_events, sprite_draws;
// EDUCATIONAL: Fixed voice snapshots expose sustain, rest, dynamics and replay without real audio hardware.
static int note_count[3], note_active[3], note_volume[3];
// EDUCATIONAL: Hash ordered note events to compare complete deterministic phrases without allocating a log.
static uint32_t note_hash;
// EDUCATIONAL: Precondition: host test sets input; no state change; O(1); replaces portable input read.
uint32_t prg32_input_read(void){return test_input;}
// EDUCATIONAL: Precondition: valid voice parameters; records one effect; O(1); mocks portable note-on.
void prg32_audio_note_on(uint8_t c,uint8_t i,uint8_t n,uint8_t v){assert(c<3&&i<3&&n>0&&n<128&&v>0);note_count[c]++;note_active[c]=1;note_volume[c]=v;note_hash=note_hash*33u+c;note_hash=note_hash*33u+n;note_hash=note_hash*33u+v;audio_events++;}
// EDUCATIONAL: Precondition: valid channel; records one effect; O(1); mocks portable note-off.
void prg32_audio_note_off(uint8_t c){assert(c<3);note_active[c]=0;audio_events++;}
// EDUCATIONAL: Precondition: sample arguments from game; no sound output; O(1); mocks panned sample playback.
int prg32_audio_play_sample_pan(uint16_t s,uint8_t v,uint16_t p,int8_t pan){(void)s;(void)v;(void)p;assert(pan>=-64&&pan<=63);sample_events++;return 0;}
// EDUCATIONAL: Precondition: RGB565 color; no framebuffer effect; O(1); mocks portable clear.
void prg32_gfx_clear(uint16_t c){(void)c;}
// EDUCATIONAL: Precondition: integer rectangle and color; no framebuffer effect; O(1); mocks portable rectangle.
void prg32_gfx_rect(int x,int y,int w,int h,uint16_t c){(void)x;(void)y;(void)w;(void)h;(void)c;}
// EDUCATIONAL: Precondition: valid string; no output; O(1); mocks portable text.
void prg32_gfx_text8(int x,int y,const char*s,uint16_t f,uint16_t b){(void)x;(void)y;(void)s;(void)f;(void)b;}
// EDUCATIONAL: Precondition: sprite pointer; no output; O(1); mocks portable 16x16 sprite.
void prg32_sprite_draw_16x16(int x,int y,const uint16_t*s){(void)x;(void)y;(void)s;}
// EDUCATIONAL: Precondition: sprite pointer; no output; O(1); mocks portable 24x24 sprite.
void prg32_sprite_draw_24x24(int x,int y,const uint16_t*s){(void)x;(void)y;(void)s;}
// EDUCATIONAL: Precondition: valid frame; counts boss sprites; O(1); mocks portable frame rendering.
void prg32_sprite_draw_frame(int x,int y,int w,int h,const uint16_t*s,uint32_t f,uint16_t t){(void)x;(void)y;(void)h;(void)s;(void)f;(void)t;if(w==48)boss_draws++;sprite_draws++;}
// EDUCATIONAL: Precondition: none; resets cartridge and mocked input; bounded by entity capacity; calls game init and new-game audio.
static void reset(void){test_input=0;grendizer_c_init();new_game();begin_play();old_input=0;}
// EDUCATIONAL: Precondition: host mocks installed; exercises all stage phrases and silence with bounded loops; changes game/music snapshots and calls the sequencer.
static void test_music(void){
// EDUCATIONAL: Reserve deterministic replay snapshots and bounded loop counters on the stack.
int st,i,voice;uint32_t first;
// EDUCATIONAL: Verify every stage independently because each has different chord and melody data.
for(st=1;st<=STAGE_COUNT;st++){
// EDUCATIONAL: Start a full phrase in combat with a clean event checksum and voice counters.
reset();stage=st;music_reset();note_hash=0;for(voice=0;voice<3;voice++)note_count[voice]=0;
// EDUCATIONAL: Run exactly one phrase and require a clean wrap to the initial subdivision.
for(i=0;i<448;i++)music_update();assert(music_step==0&&music_tick==0);
// EDUCATIONAL: Arpeggio and bass retain distinct rhythms, and deliberate melody rests reduce its note count.
assert(note_count[1]==48&&note_count[2]==24&&note_count[0]>24&&note_count[0]<32);
// EDUCATIONAL: Repeat from reset and compare every emitted note, channel and velocity in order.
first=note_hash;music_reset();note_hash=0;for(i=0;i<448;i++)music_update();assert(note_hash==first);
// EDUCATIONAL: End per-stage sequence coverage.
}
// EDUCATIONAL: Hold the opening call across two subdivisions, then articulate repeated pitches and the pickup.
reset();music_reset();note_count[0]=0;for(i=0;i<14;i++)music_update();assert(note_active[0]&&note_count[0]==1);
// EDUCATIONAL: The next two subdivisions retrigger the chant while the bass remains active.
for(i=0;i<14;i++)music_update();assert(note_active[0]&&note_active[2]&&note_count[0]==3);
// EDUCATIONAL: The sixth subdivision ends the call; the following pickup starts a fresh lead note.
for(i=0;i<21;i++)music_update();assert(!note_active[0]);for(i=0;i<7;i++)music_update();assert(note_active[0]);
// EDUCATIONAL: Attract must lower every voice relative to its combat level.
state=ST_ATTRACT;music_reset();for(i=0;i<7;i++)music_update();assert(note_volume[0]<112&&note_volume[1]<84&&note_volume[2]<104);
// EDUCATIONAL: Game-over must stay silent over a complete phrase duration.
music_off();state=ST_OVER;i=audio_events;for(voice=0;voice<448;voice++)music_update();assert(audio_events==i&&!note_active[0]&&!note_active[1]&&!note_active[2]);
// EDUCATIONAL: End soundtrack regression checks without requiring a target audio device.
}
// EDUCATIONAL: Precondition: host execution; changes test/game state and prints results; bounded loops; all ABI effects are mocked.
int main(void){
// EDUCATIONAL: Reserve snapshot counters for pause and deterministic replay assertions.
int i,f,t,r,m,events;uint32_t seed;Star snapshot[MAX_STARS];
// EDUCATIONAL: Verify soundtrack phrase timing, sustain, rests, dynamics and deterministic replay.
test_music();
// EDUCATIONAL: Enter combat and pause with active cooldown and invulnerability.
reset();screw_cd=5;harken_cd=9;thunder_cd=12;invuln=50;timer=29;test_input=PRG32_BTN_START;grendizer_c_update();assert(state==ST_PAUSE);
// EDUCATIONAL: Snapshot simulation clocks, random seed, and audio count after pause entry.
f=frame;t=timer;r=stage_scroll;m=music_tick;seed=rng;events=audio_events;
// EDUCATIONAL: Wait longer than damage protection; pause must freeze all sampled state.
test_input=0;for(i=0;i<200;i++)grendizer_c_update();assert(frame==f&&timer==t&&stage_scroll==r&&music_tick==m&&rng==seed&&audio_events==events);assert(invuln==50&&screw_cd==5&&harken_cd==9&&thunder_cd==12);
// EDUCATIONAL: Resume without resetting the combat timer.
test_input=PRG32_BTN_START;grendizer_c_update();assert(state==ST_PLAY&&timer==29);
// EDUCATIONAL: The paused boss remains visible beneath the overlay.
begin_boss();previous_state=ST_BOSS;state=ST_PAUSE;boss_draws=0;grendizer_c_draw();assert(boss_draws==1);
// EDUCATIONAL: A lethal final-boss hit and hostile shot coincide; victory must win this frame transaction.
reset();stage=STAGE_COUNT;begin_boss();boss_hp=1;lives=1;shot_add(W_SCREW,boss_x,boss_y+7,0,-7,1,35);enemy_fire(px+4,py,0,3,0);grendizer_c_update();assert(state==ST_ENDING&&lives==1);
// EDUCATIONAL: Losing the last life to the final escaping enemy must not start a boss.
reset();clear_entities();lives=1;stage_kills=18;spawn_enemy(0,1,40,150);grendizer_c_update();assert(state==ST_OVER&&lives==0);
// EDUCATIONAL: Harken reverses once then keeps moving inward on following frames.
reset();clear_entities();shot_add(W_HARKEN,100,150,2,0,2,28);update_shots();assert(shots[0].dx==-2);update_shots();assert(shots[0].dx==-2&&shots[0].x==100);
// EDUCATIONAL: A crawler at the right edge must turn back into the viewport.
reset();clear_entities();spawn_enemy(0,2,296,30);enemies[0].dx=1;update_enemies();assert(enemies[0].dx==-1);
// EDUCATIONAL: An 80-tick cadence must not fire at phase 16, unlike an invalid bit mask.
reset();clear_entities();spawn_enemy(0,1,40,30);enemies[0].phase=15;update_enemies();assert(!eshots[0].active);enemies[0].phase=79;update_enemies();assert(eshots[0].active);
// EDUCATIONAL: START remains detectable while the player holds A on the title.
reset();state=ST_TITLE;old_input=PRG32_BTN_A;test_input=PRG32_BTN_A|PRG32_BTN_START;grendizer_c_update();assert(state==ST_LAUNCH);
// EDUCATIONAL: Capture initial star state, then disturb the attract loop before restarting.
reset();new_game();for(i=0;i<MAX_STARS;i++)snapshot[i]=stars[i];seed=rng;for(i=0;i<77;i++)update_stars();frame=1234;new_game();assert(frame==0&&rng==seed);
// EDUCATIONAL: Identical new games must restore the same star positions and speeds.
for(i=0;i<MAX_STARS;i++)assert(stars[i].x==snapshot[i].x&&stars[i].y==snapshot[i].y&&stars[i].s==snapshot[i].s);
// EDUCATIONAL: Long boss motion remains bounded even when periodic vertical steps accumulate.
reset();stage=2;begin_boss();for(i=0;i<10000;i++){frame++;update_boss();assert(boss_y>=24&&boss_y<=80);}
// EDUCATIONAL: Boot must enter attract and accept two distinct START presses to begin a run.
grendizer_c_init();test_input=PRG32_BTN_START;grendizer_c_update();assert(state==ST_TITLE);test_input=0;grendizer_c_update();test_input=PRG32_BTN_START;grendizer_c_update();assert(state==ST_LAUNCH);
// EDUCATIONAL: Each stage introduction and docking sequence completes within its documented tick budget.
test_input=0;for(i=0;i<179;i++){grendizer_c_update();assert(state==ST_LAUNCH);}grendizer_c_update();assert(state==ST_STAGE_INTRO);for(i=0;i<76;i++)grendizer_c_update();assert(state==ST_PLAY);stage=2;start_stage_intro();for(i=0;i<76;i++)grendizer_c_update();assert(state==ST_TRANSFORM);for(i=0;i<121;i++)grendizer_c_update();assert(state==ST_PLAY&&spazer_mode);
// EDUCATIONAL: Clamp movement to the playfield and verify each standard weapon maps to its input.
reset();px=3;py=116;update_play_input(PRG32_BTN_LEFT|PRG32_BTN_UP);assert(px==3&&py==116);update_play_input(PRG32_BTN_A);assert(shots[0].kind==W_SCREW);clear_entities();update_play_input(PRG32_BTN_B);assert(shots[0].kind==W_HARKEN&&shots[1].kind==W_HARKEN);clear_entities();energy=5;update_play_input(PRG32_BTN_A|PRG32_BTN_B);assert(energy==2&&shots[0].kind==W_THUNDER&&shots[1].kind==W_THUNDER);
// EDUCATIONAL: Every boss advances to the following stage or victory through a real projectile collision.
for(i=1;i<=STAGE_COUNT;i++){reset();stage=i;begin_boss();boss_hp=1;shot_add(W_SCREW,boss_x,boss_y+7,0,-7,1,35);grendizer_c_update();assert(state==(i==STAGE_COUNT?ST_ENDING:i==4?ST_COSMO:ST_STAGE_INTRO));}
// EDUCATIONAL: Each support mission selects its own craft, reaches combat and safely renders a paused boss.
for(i=2;i<=STAGE_COUNT;i++){reset();stage=i;start_stage_intro();assert(spazer_mode==1);for(t=0;t<76;t++)grendizer_c_update();assert(state==(i==5?ST_PLAY:ST_TRANSFORM));grendizer_c_draw();if(i<5)for(t=0;t<121;t++)grendizer_c_update();assert(state==ST_PLAY);grendizer_c_draw();begin_boss();for(t=0;t<448;t++){music_update();update_boss();}assert(boss_y>=24&&boss_y<=80);if(i==5)assert(boss_x==136&&boss_y==32);previous_state=ST_BOSS;state=ST_PAUSE;grendizer_c_draw();}
// EDUCATIONAL: Distinct immutable vehicle frames ensure every supporting craft remains reachable.
stage=2;assert(support_sprite()==double_spazer);stage=3;assert(support_sprite()==marine_spazer);stage=4;assert(support_sprite()==drill_spazer);stage=5;assert(support_sprite()==all_spazers);
// EDUCATIONAL: Special Cosmo blocks combat until all 240 assembly/launch ticks have elapsed.
reset();stage=4;next_stage();assert(stage==5&&state==ST_COSMO);for(i=0;i<239;i++){grendizer_c_update();grendizer_c_draw();assert(state==ST_COSMO);}grendizer_c_update();assert(state==ST_STAGE_INTRO);
// EDUCATIONAL: Ending remains animated for 240 ticks, ignores action input, then allows victory recovery.
next_stage();assert(state==ST_ENDING);test_input=PRG32_BTN_A;for(i=0;i<239;i++){grendizer_c_update();grendizer_c_draw();assert(state==ST_ENDING);}grendizer_c_update();assert(state==ST_WIN);test_input=0;
// EDUCATIONAL: Lunar escorts create a real projectile without charging energy, and pause freezes form progression.
reset();stage=5;stage_scroll=0;clear_entities();energy=5;grendizer_c_update();assert(shots[0].active&&shots[0].damage==1&&energy==5);stage_scroll=600;test_input=PRG32_BTN_START;grendizer_c_update();assert(state==ST_PAUSE);test_input=0;for(i=0;i<121;i++)grendizer_c_update();assert(stage_scroll==600);grendizer_c_draw();
// EDUCATIONAL: Player damage blinking must never hide the three independent lunar NPC escorts.
reset();stage=5;clear_entities();invuln=4;sprite_draws=0;grendizer_c_draw();assert(sprite_draws==3);invuln=0;sprite_draws=0;grendizer_c_draw();assert(sprite_draws==4);
// EDUCATIONAL: Every stage permits reversible manual forms with stable speed, weapon power and detached-NPC behavior.
for(i=1;i<=STAGE_COUNT;i++){
// EDUCATIONAL: Each stage starts combined and moves four pixels horizontally and three vertically.
reset();stage=i;start_stage_intro();begin_play();clear_entities();px=100;py=140;update_play_input(PRG32_BTN_RIGHT|PRG32_BTN_UP);assert(px==104&&py==137&&spazer_mode==1);
// EDUCATIONAL: DOWN+A consumes the chord, changes form once and leaves energy, shots and position untouched.
events=sample_events;update_play_input(PRG32_BTN_DOWN|PRG32_BTN_A);assert(spazer_mode==0&&px==104&&py==137&&free_shots()==MAX_SHOTS&&energy==5&&sample_events==events+1);update_play_input(PRG32_BTN_DOWN|PRG32_BTN_A);assert(sample_events==events+1);
// EDUCATIONAL: Robot motion is slower on both axes, and all three weapons have doubled damage.
update_play_input(PRG32_BTN_RIGHT|PRG32_BTN_UP);assert(px==106&&py==136);fire_screw();assert(shots[0].damage==2);clear_entities();fire_harken();assert(shots[0].damage==4&&shots[1].damage==4);clear_entities();fire_thunder();assert(shots[0].damage==8&&shots[1].damage==8);
// EDUCATIONAL: Detached support is visible even while the player blinks and fires a weak energy-free NPC shot.
clear_entities();invuln=4;sprite_draws=0;grendizer_c_draw();assert(sprite_draws==(i==5?4:1));frame=44;stage_scroll=1;energy=5;test_input=0;grendizer_c_update();assert(shots[0].active&&shots[0].damage==1&&energy==5);
// EDUCATIONAL: UP+A reconnects, removes detached support and restores lower damage without resetting cooldowns.
clear_entities();screw_cd=harken_cd=thunder_cd=0;update_play_input(PRG32_BTN_UP|PRG32_BTN_A);assert(spazer_mode==1&&free_shots()==MAX_SHOTS);fire_screw();assert(shots[0].damage==1);clear_entities();fire_harken();assert(shots[0].damage==2);clear_entities();fire_thunder();assert(shots[0].damage==4);
// EDUCATIONAL: Thunder retains priority for A+B with a vertical direction; it does not change form.
clear_entities();energy=5;thunder_cd=0;update_play_input(PRG32_BTN_A|PRG32_BTN_B|PRG32_BTN_DOWN);assert(spazer_mode==1&&energy==2&&shots[0].kind==W_THUNDER);
// EDUCATIONAL: Finish per-stage form checks.
}
// EDUCATIONAL: Pause ignores transform chords, and a full pool safely omits detached support shots.
reset();state=ST_PAUSE;previous_state=ST_PLAY;test_input=PRG32_BTN_DOWN|PRG32_BTN_A;grendizer_c_update();assert(spazer_mode==1);state=ST_PLAY;spazer_mode=0;frame=44;test_input=0;for(i=0;i<MAX_SHOTS;i++)shot_add(W_SCREW,100,150,0,0,1,100);energy=5;grendizer_c_update();assert(energy==5&&free_shots()==0);
// EDUCATIONAL: Both terminal screens recover to title on a fresh START press.
state=ST_OVER;old_input=0;test_input=PRG32_BTN_START;grendizer_c_update();assert(state==ST_TITLE);state=ST_WIN;old_input=0;grendizer_c_update();assert(state==ST_TITLE);
// EDUCATIONAL: A full shot pool must reject all weapons without charging resources or emitting a firing sound.
reset();for(i=0;i<MAX_SHOTS;i++)shots[i].active=1;events=sample_events;fire_screw();fire_harken();fire_thunder();assert(energy==5&&screw_cd==0&&harken_cd==0&&thunder_cd==0&&sample_events==events);
// EDUCATIONAL: One free slot cannot fit a paired attack; it must remain available with energy unchanged.
shots[4].active=0;fire_harken();fire_thunder();assert(!shots[4].active&&energy==5&&harken_cd==0&&thunder_cd==0&&sample_events==events);
// EDUCATIONAL: Two nonadjacent slots accept both Thunder projectiles and charge exactly once.
shots[9].active=0;fire_thunder();assert(shots[4].active&&shots[9].active&&shots[4].kind==W_THUNDER&&shots[9].kind==W_THUNDER&&energy==2&&thunder_cd==44&&sample_events==events+1);
// EDUCATIONAL: Every wave must include the saucer and all four robot families shown by the attract screen.
reset();r=0;for(i=0;i<MAX_ENEMIES;i++)r|=1<<enemies[i].kind;assert(r==31);
// EDUCATIONAL: A gunner fires at its scheduled tick while remaining in the enemy playfield.
clear_entities();spawn_enemy(0,4,40,30);enemies[0].phase=63;update_enemies();assert(eshots[0].active&&eshots[0].dy>0);
// EDUCATIONAL: A blade at the edge must turn inward instead of drifting outside the viewport.
clear_entities();spawn_enemy(0,1,8,30);enemies[0].dx=-1;enemies[0].phase=31;update_enemies();assert(enemies[0].x>=8&&enemies[0].dx==1);
// EDUCATIONAL: Report successful regression checks to the test runner.
puts("Gameplay regression tests: PASS");return 0;
// EDUCATIONAL: End the host regression entry point.
}
