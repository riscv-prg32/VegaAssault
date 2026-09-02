// EDUCATIONAL: Import the PRG32 or local sprite declarations required by the following translation unit.
#include "prg32.h"
// EDUCATIONAL: Import the PRG32 or local sprite declarations required by the following translation unit.
#include "sprites.h"
// EDUCATIONAL: Import the PRG32 or local sprite declarations required by the following translation unit.
#include "sprites.c"

// EDUCATIONAL: Documentation line retained to explain intent, provenance, or constraints.
/*
// EDUCATIONAL: Documentation line retained to explain intent, provenance, or constraints.
 * Vega Assault
// EDUCATIONAL: Documentation line retained to explain intent, provenance, or constraints.
 * Unofficial non-commercial fan-game teaching project.
// EDUCATIONAL: Documentation line retained to explain intent, provenance, or constraints.
 * Code, pixel art and soundtrack data in this package are original.
// EDUCATIONAL: Documentation line retained to explain intent, provenance, or constraints.
 */

// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_LAUNCH 9
// EDUCATIONAL: Special Cosmo assembly and departure are isolated from combat.
#define ST_COSMO 10
// EDUCATIONAL: The Moon-base destruction triggers a timed animated epilogue before victory.
#define ST_ENDING 11
// EDUCATIONAL: Five fixed missions share existing entity pools and three original music phrases.
#define STAGE_COUNT 5
// EDUCATIONAL: Logical viewport width matches the portable display ABI.
#define SW 320
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define SH 200
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define MAX_ENEMIES 10
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define MAX_SHOTS 12
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define MAX_ESHOTS 14
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define MAX_STARS 26

// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_ATTRACT 0
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_TITLE 1
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_STAGE_INTRO 2
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_TRANSFORM 3
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_PLAY 4
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_BOSS 5
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_OVER 6
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_WIN 7
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define ST_PAUSE 8

// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define W_SCREW 0
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define W_HARKEN 1
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define W_THUNDER 2

// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define SFX_BLAST 3
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define SFX_BOOM 4
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define SFX_HARKEN 5
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define SFX_THUNDER 6
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define SFX_DOCK 7

// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define INST_LEAD 0
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define INST_ARP 1
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define INST_BASS 2

// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define TRUE 1
// EDUCATIONAL: Define a compile-time constant so the runtime uses fixed memory and inexpensive integer comparisons.
#define FALSE 0

// EDUCATIONAL: Declare a compact fixed-size record used by the allocation-free entity model.
typedef struct { int x,y,dx,dy,kind,damage,life,active; } Shot;
// EDUCATIONAL: Declare a compact fixed-size record used by the allocation-free entity model.
typedef struct { int x,y,dx,dy,kind,hp,phase,anim,active; } Enemy;
// EDUCATIONAL: Declare a compact fixed-size record used by the allocation-free entity model.
typedef struct { int x,y,dx,dy,kind,active; } EShot;
// EDUCATIONAL: Declare a compact fixed-size record used by the allocation-free entity model.
typedef struct { int x,y,s; } Star;

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int state, previous_state, frame, timer, attract_page;
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int px,py,lives,score,stage,stage_kills,energy,invuln,combo;
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int screw_cd,harken_cd,thunder_cd,spazer_mode;
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int boss_x,boss_y,boss_dx,boss_hp,boss_max,boss_phase,boss_timer;
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int stage_scroll,music_tick,music_step;
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static uint32_t old_input,rng;
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static Shot shots[MAX_SHOTS];
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static Enemy enemies[MAX_ENEMIES];
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static EShot eshots[MAX_ESHOTS];
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static Star stars[MAX_STARS];

// EDUCATIONAL: Original 32-note stage-one melody spans eight bars; zero represents a deliberate rest.
static const uint8_t lead1[32]={64,64,67,71,69,67,66,0,64,67,67,72,71,67,64,0,69,69,72,76,74,72,71,69,66,66,71,75,74,71,66,0};
// EDUCATIONAL: Stage two uses a minor-key answer phrase over its own matching chord roots.
static const uint8_t lead2[32]={69,69,72,76,74,72,71,0,65,69,69,72,76,72,69,0,67,67,72,76,79,76,74,72,71,71,74,79,77,74,71,0};
// EDUCATIONAL: Stage three resolves a rising minor melody through a major dominant chord.
static const uint8_t lead3[32]={67,67,70,74,72,70,69,0,63,67,67,70,74,70,67,0,65,65,69,72,77,72,69,65,66,66,69,74,72,69,66,0};
// EDUCATIONAL: Four chord roots per stage serve both the centered bass and the right-hand arpeggio.
static const uint8_t music_roots[3][4]={{52,48,45,47},{45,41,48,43},{43,39,41,38}};
// EDUCATIONAL: Major thirds are four semitones and minor thirds three, keeping every accompaniment in harmony.
static const uint8_t music_thirds[3][4]={{3,4,3,4},{3,4,4,4},{3,4,4,4}};
// EDUCATIONAL: Two-bar vocal-like rhythm maps subdivisions to note slots; 255 sustains the previous event.
static const uint8_t lead_rhythm[16]={0,255,1,2,3,255,255,4,5,255,255,255,6,255,7,255};
// EDUCATIONAL: Eight accompaniment positions encode chord tones; 255 marks a rest for a galloping string-like rhythm.
static const uint8_t arp_degrees[8]={0,2,255,1,2,3,255,2};

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static uint32_t rnd(void){ rng=rng*1664525u+1013904223u; return rng; }
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int hit(int ax,int ay,int aw,int ah,int bx,int by,int bw,int bh){return ax<bx+bw&&ax+aw>bx&&ay<by+bh&&ay+ah>by;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int panx(int x){int p=(x*127)/SW-64; if(p<-64)p=-64;if(p>63)p=63;return p;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void num(unsigned int v,char *o){char t[11];int n=0,i;if(!v){o[0]='0';o[1]=0;return;}while(v&&n<10){t[n++]=(char)('0'+v%10);v/=10;}for(i=0;i<n;i++)o[i]=t[n-1-i];o[n]=0;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void clear_entities(void){int i;for(i=0;i<MAX_SHOTS;i++)shots[i].active=0;for(i=0;i<MAX_ENEMIES;i++)enemies[i].active=0;for(i=0;i<MAX_ESHOTS;i++)eshots[i].active=0;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void music_off(void){prg32_audio_note_off(0);prg32_audio_note_off(1);prg32_audio_note_off(2);}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void music_reset(void){music_tick=0;music_step=0;music_off();}
// EDUCATIONAL: Precondition: initialized game and stage 1..5; advances only fixed music counters, emits portable note calls, uses O(1) time/memory and no RNG.
static void music_update(void){
// EDUCATIONAL: Select one immutable original melody without allocating or copying note data.
const uint8_t *lead=(stage-1)%3==0?lead1:((stage-1)%3==1?lead2:lead3);
// EDUCATIONAL: Local integer values describe chord position, note and performance dynamics.
int chord,root,third,degree,n,quiet,event;
// EDUCATIONAL: Game-over remains silent; the main update already freezes this function during pause.
if(state==ST_OVER)return;
// EDUCATIONAL: Seven game ticks form one subdivision, retaining the existing deterministic tempo.
if(++music_tick<7)return;
// EDUCATIONAL: Restart only the subdivision clock, allowing sustained voices to continue sounding.
music_tick=0;
// EDUCATIONAL: Each chord occupies sixteen subdivisions; a full phrase occupies sixty-four.
chord=music_step/16;
// EDUCATIONAL: Read the stage-specific root and third shared by all accompaniment notes.
root=music_roots[(stage-1)%3][chord];third=music_thirds[(stage-1)%3][chord];
// EDUCATIONAL: Attract mode reduces every part rather than leaving accompaniment at combat volume.
quiet=state==ST_ATTRACT;
// EDUCATIONAL: Unequal note lengths create short repeated calls followed by long answering notes.
event=lead_rhythm[music_step&15];
// EDUCATIONAL: A hold leaves the current voice untouched, including silence after a rest.
if(event!=255){
// EDUCATIONAL: Release only the lead before its next note or deliberate rest.
prg32_audio_note_off(0);
// EDUCATIONAL: Each two-bar chord contains eight melody events with independently authored pitch.
n=lead[chord*8+event];
// EDUCATIONAL: Zero is a rest, never a request to play MIDI note zero.
if(n)prg32_audio_note_on(0,INST_LEAD,(uint8_t)n,(uint8_t)(quiet?76:((music_step&7)==0?124:108)));
// EDUCATIONAL: Finish the melody event while bass can remain sustained.
}
// EDUCATIONAL: Give each two-bar call a breath before its long answer without cutting the syncopated pickup.
if((music_step&15)==6)prg32_audio_note_off(0);
// EDUCATIONAL: Renew the arpeggio on every subdivision using the instrument default stereo pan.
prg32_audio_note_off(1);
// EDUCATIONAL: Select a repeating chord tone rather than an unrelated fixed counter-line.
degree=arp_degrees[music_step&7];
// EDUCATIONAL: Omit two subdivisions per bar so the accompaniment breathes around the lead.
if(degree!=255){
// EDUCATIONAL: Place the arpeggio one octave above the chord root with the correct major or minor third.
n=root+12+(degree==1?third:(degree==2?7:(degree==3?12:0)));
// EDUCATIONAL: Accent alternating arpeggio notes gently and leave room for melody and effects.
prg32_audio_note_on(1,INST_ARP,(uint8_t)n,(uint8_t)(quiet?54:((music_step&3)==0?90:68)));
// EDUCATIONAL: End the accompaniment note or retain its explicit rest.
}
// EDUCATIONAL: Root/fifth half-bar pulses gain a short octave pickup at each bar end.
if((music_step&3)==0||(music_step&7)==7){
// EDUCATIONAL: Stop the previous bass note before alternating root and fifth.
prg32_audio_note_off(2);
// EDUCATIONAL: Alternate root and fifth one octave below the chord; all values fit MIDI byte range.
n=root-12+((music_step&7)==7?12:((music_step&4)?7:0));
// EDUCATIONAL: Keep bass centered through its instrument and quieter during attract mode.
prg32_audio_note_on(2,INST_BASS,(uint8_t)n,(uint8_t)(quiet?72:104));
// EDUCATIONAL: Finish the bass event without stopping the other voices.
}
// EDUCATIONAL: Wrap the eight-bar phrase with a bounded counter and no heap state.
music_step=(music_step+1)&63;
// EDUCATIONAL: End the constant-time three-voice sequencer.
}

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void init_stars(void){int i;for(i=0;i<MAX_STARS;i++){stars[i].x=(int)(rnd()%SW);stars[i].y=(int)(rnd()%SH);stars[i].s=1+(int)(rnd()%3u);}}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void update_stars(void){int i;for(i=0;i<MAX_STARS;i++){stars[i].y+=stars[i].s;if(stars[i].y>=SH){stars[i].y=18;stars[i].x=(int)(rnd()%SW);}}}

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void start_stage_intro(void){clear_entities();timer=0;state=ST_STAGE_INTRO;spazer_mode=1;px=148;py=164;stage_scroll=0;music_reset();}
// EDUCATIONAL: Precondition: initialization completed; resets run state, RNG and stars; O(entity capacities); resets music through portable note-off calls.
static void new_game(void){px=148;py=164;lives=3;score=0;stage=1;stage_kills=0;energy=5;invuln=0;combo=0;screw_cd=harken_cd=thunder_cd=0;spazer_mode=0;stage_scroll=0;frame=0;rng=0x4752454eu;init_stars();start_stage_intro();state=ST_LAUNCH;}

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void spawn_enemy(int slot,int kind,int x,int y){Enemy *e=&enemies[slot];e->x=x;e->y=y;e->kind=kind;e->dx=(rnd()&1u)?1:-1;e->dy=1;e->hp=1+kind/2+stage/3;e->phase=(int)(rnd()%64u);e->anim=0;e->active=1;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static int alive_enemies(void){int i,n=0;for(i=0;i<MAX_ENEMIES;i++)if(enemies[i].active)n++;return n;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void fill_wave(void){int i,k;for(i=0;i<MAX_ENEMIES;i++){k=(i+stage+stage_kills/4)%5;spawn_enemy(i,k,16+(i%5)*60,28+(i/5)*28);} }

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void enemy_fire(int x,int y,int dx,int dy,int kind){int i;for(i=0;i<MAX_ESHOTS;i++)if(!eshots[i].active){eshots[i].x=x;eshots[i].y=y;eshots[i].dx=dx;eshots[i].dy=dy;eshots[i].kind=kind;eshots[i].active=1;return;}}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void player_damage(void){if(invuln)return;lives--;invuln=75;combo=0;prg32_audio_play_sample_pan(SFX_BOOM,220,720,0);if(lives<=0){state=ST_OVER;timer=0;music_off();}}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void kill_enemy(Enemy *e){score+=120+e->kind*55+combo*10;if(combo<9)combo++;stage_kills++;if(energy<9)energy++;prg32_audio_play_sample_pan(SFX_BOOM,155,900,(int8_t)panx(e->x));e->active=0;}

// EDUCATIONAL: Precondition: initialized shot pool; reads slots without mutation or ABI calls; O(MAX_SHOTS); returns free capacity before a weapon commits.
static int free_shots(void){
// EDUCATIONAL: Count unused entries in the fixed pool; no dynamic allocation occurs.
int i,count=0;
// EDUCATIONAL: Every inactive entry can hold exactly one projectile.
for(i=0;i<MAX_SHOTS;i++)if(!shots[i].active)count++;
// EDUCATIONAL: The single-threaded update guarantees this capacity until the caller inserts its shots.
return count;
// EDUCATIONAL: End the capacity query.
}

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void shot_add(int kind,int x,int y,int dx,int dy,int damage,int life){int i;for(i=0;i<MAX_SHOTS;i++)if(!shots[i].active){shots[i].x=x;shots[i].y=y;shots[i].dx=dx;shots[i].dy=dy;shots[i].kind=kind;shots[i].damage=damage;shots[i].life=life;shots[i].active=1;return;}}
// EDUCATIONAL: Precondition: combat input phase; commits a complete attack only when capacity permits; O(MAX_SHOTS); changes shots/resources and plays portable SFX.
static void fire_screw(void){
// EDUCATIONAL: Reject the attack before side effects if its cooldown, energy or fixed-pool capacity forbids it.
if(screw_cd||free_shots()<1)return;
// EDUCATIONAL: Insert into guaranteed capacity; robot form doubles projectile damage while combined flight retains baseline damage.
shot_add(W_SCREW,px+10,py-5,0,-7,spazer_mode?1:2,35);
// EDUCATIONAL: Charge the successful attack after all its projectiles exist.
screw_cd=6;
// EDUCATIONAL: Emit one mono-compatible firing sound, with spatial pan where appropriate.
prg32_audio_play_sample_pan(SFX_BLAST,175,1200,(int8_t)panx(px));
// EDUCATIONAL: End the all-or-nothing weapon action.
}
// EDUCATIONAL: Precondition: combat input phase; commits a complete attack only when capacity permits; O(MAX_SHOTS); changes shots/resources and plays portable SFX.
static void fire_harken(void){
// EDUCATIONAL: Reject the attack before side effects if its cooldown, energy or fixed-pool capacity forbids it.
if(harken_cd||free_shots()<2)return;
// EDUCATIONAL: Insert into guaranteed capacity; robot form doubles projectile damage while combined flight retains baseline damage.
shot_add(W_HARKEN,px+4,py+2,-2,-5,spazer_mode?2:4,44);
// EDUCATIONAL: Insert into guaranteed capacity; robot form doubles projectile damage while combined flight retains baseline damage.
shot_add(W_HARKEN,px+16,py+2,2,-5,spazer_mode?2:4,44);
// EDUCATIONAL: Charge the successful attack after all its projectiles exist.
harken_cd=18;
// EDUCATIONAL: Emit one mono-compatible firing sound, with spatial pan where appropriate.
prg32_audio_play_sample_pan(SFX_HARKEN,190,900,(int8_t)panx(px));
// EDUCATIONAL: End the all-or-nothing weapon action.
}
// EDUCATIONAL: Precondition: combat input phase; commits a complete attack only when capacity permits; O(MAX_SHOTS); changes shots/resources and plays portable SFX.
static void fire_thunder(void){
// EDUCATIONAL: Reject the attack before side effects if its cooldown, energy or fixed-pool capacity forbids it.
if(thunder_cd||energy<3||free_shots()<2)return;
// EDUCATIONAL: Insert into guaranteed capacity; robot form doubles projectile damage while combined flight retains baseline damage.
shot_add(W_THUNDER,px+8,py-2,0,-9,spazer_mode?4:8,26);
// EDUCATIONAL: Insert into guaranteed capacity; robot form doubles projectile damage while combined flight retains baseline damage.
shot_add(W_THUNDER,px+12,py-2,0,-9,spazer_mode?4:8,26);
// EDUCATIONAL: Charge the successful attack after all its projectiles exist.
energy-=3;thunder_cd=44;
// EDUCATIONAL: Emit one mono-compatible firing sound, with spatial pan where appropriate.
prg32_audio_play_sample_pan(SFX_THUNDER,230,520,0);
// EDUCATIONAL: End the all-or-nothing weapon action.
}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void begin_transform(void){timer=0;state=ST_TRANSFORM;clear_entities();prg32_audio_play_sample_pan(SFX_DOCK,180,760,0);}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void begin_play(void){timer=0;state=ST_PLAY;stage_kills=0;fill_wave();}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void begin_boss(void){clear_entities();state=ST_BOSS;timer=0;boss_x=136;boss_y=32;boss_dx=stage==5?0:2;boss_phase=0;boss_timer=0;boss_max=24+stage*12;boss_hp=boss_max;}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void next_stage(void){if(stage>=STAGE_COUNT){clear_entities();state=ST_ENDING;timer=0;music_reset();}else{stage++;start_stage_intro();if(stage==5)state=ST_COSMO;}}

// EDUCATIONAL: Precondition: combat state; moves bounded shots and resolves enemy/boss damage; O(MAX_SHOTS*MAX_ENEMIES); emits portable hit sounds and returns immediately on stage completion.
static void update_shots(void){int i,j;for(i=0;i<MAX_SHOTS;i++){Shot *s=&shots[i];if(!s->active)continue;s->x+=s->dx;s->y+=s->dy;if(s->kind==W_HARKEN&&s->life==28)s->dx=-s->dx;s->life--;if(s->life<=0||s->y<18||s->x<-8||s->x>328){s->active=0;continue;}if(state==ST_BOSS&&hit(s->x,s->y,s->kind==W_THUNDER?8:5,8,boss_x,boss_y,48,24)){boss_hp-=s->damage;s->active=0;score+=30*s->damage;prg32_audio_play_sample_pan(SFX_BLAST,100,650,(int8_t)panx(boss_x));if(boss_hp<=0){score+=1500*stage;prg32_audio_play_sample_pan(SFX_BOOM,250,560,(int8_t)panx(boss_x));timer=0;next_stage();return;}continue;}for(j=0;j<MAX_ENEMIES;j++){Enemy *e=&enemies[j];if(e->active&&hit(s->x,s->y,6,8,e->x,e->y,16,16)){e->hp-=s->damage;s->active=0;if(e->hp<=0)kill_enemy(e);break;}}}}
// EDUCATIONAL: Precondition: combat state; moves hostile shots and applies player damage; O(MAX_ESHOTS); portable damage sound may play, and fatal damage ends processing.
static void update_eshots(void){int i;for(i=0;i<MAX_ESHOTS;i++){EShot *s=&eshots[i];if(!s->active)continue;s->x+=s->dx;s->y+=s->dy;if(s->kind==2&&((frame>>3)&1))s->x+=s->dx>=0?1:-1;if(s->x<-8||s->x>328||s->y>200){s->active=0;continue;}if(hit(s->x,s->y,4,6,px+4,py+3,16,19)){s->active=0;player_damage();if(state==ST_OVER)return;}}}

// EDUCATIONAL: Precondition: ST_PLAY; moves and fires enemies at integer tick periods; O(MAX_ENEMIES*MAX_ESHOTS); fatal escape damage stops wave progression and may emit portable audio.
static void update_enemies(void){int i;for(i=0;i<MAX_ENEMIES;i++){Enemy *e=&enemies[i];if(!e->active)continue;e->phase++;e->anim=(frame>>3)&1;switch(e->kind){case 0:e->x+=e->dx*2;if(e->x<8||e->x>296)e->dx=-e->dx;if((e->phase&63)==0)enemy_fire(e->x+7,e->y+12,0,3,0);break;case 1:e->y+=1;if((e->phase&31)==0)e->x+=e->dx*10;if(e->x<8){e->x=8;e->dx=1;}if(e->x>296){e->x=296;e->dx=-1;}if((e->phase%80)==0)enemy_fire(e->x+7,e->y+12,e->dx,3,1);break;case 2:e->x+=e->dx;if(e->x<8||e->x>296)e->dx=-e->dx;if((e->phase&15)==0)e->y+=2;if((e->phase%96)==0){enemy_fire(e->x+6,e->y+10,-1,3,2);enemy_fire(e->x+10,e->y+10,1,3,2);}break;case 3:e->x+=e->dx*2;if(e->x<6||e->x>298)e->dx=-e->dx;if((e->phase%48)==0){enemy_fire(e->x+4,e->y+10,-1,4,0);enemy_fire(e->x+12,e->y+10,1,4,0);}break;case 4:if((e->phase&7)==0)e->y++;if((e->phase&63)==0)enemy_fire(e->x+7,e->y+12,px>e->x?1:-1,3,1);break;default:e->y++;break;}if(e->y>150){e->active=0;player_damage();if(state==ST_OVER)return;}}if(alive_enemies()==0){if(stage_kills>=18){begin_boss();}else fill_wave();}}

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void boss_fire_pattern(void){int d;if(stage==1){enemy_fire(boss_x+24,boss_y+18,0,4,0);if((boss_phase&1)==0){enemy_fire(boss_x+8,boss_y+16,-1,3,1);enemy_fire(boss_x+40,boss_y+16,1,3,1);}}
// EDUCATIONAL: Handle the complementary branch of the preceding gameplay condition.
else if(stage==2){d=(boss_phase&1)?1:-1;enemy_fire(boss_x+24,boss_y+19,d,4,2);enemy_fire(boss_x+10,boss_y+17,-2,3,1);enemy_fire(boss_x+38,boss_y+17,2,3,1);}
// EDUCATIONAL: Handle the complementary branch of the preceding gameplay condition.
else{d=(boss_phase%5)-2;enemy_fire(boss_x+24,boss_y+19,d,4,2);enemy_fire(boss_x+24,boss_y+19,-d,3,2);if((boss_phase%3)==0){enemy_fire(boss_x+5,boss_y+14,-2,4,0);enemy_fire(boss_x+43,boss_y+14,2,4,0);}}boss_phase++;}
// EDUCATIONAL: Precondition: ST_BOSS; advances bounded boss position and firing timer; O(MAX_ESHOTS); creates hostile shots without direct ABI calls.
static void update_boss(void){boss_x+=boss_dx;if(boss_x<10||boss_x>262)boss_dx=-boss_dx;if(stage>=2&&stage<5&&((frame&63)==0))boss_y+=((boss_phase&1)?5:-5);if(boss_y<24)boss_y=24;if(boss_y>80)boss_y=80;if(++boss_timer>22-stage*3){boss_timer=0;boss_fire_pattern();}}

// EDUCATIONAL: Precondition: combat input; changes form, position and attacks; O(MAX_SHOTS), uses portable docking sound and weapon calls.
static void update_play_input(uint32_t in){int both=(in&PRG32_BTN_A)&&(in&PRG32_BTN_B),vertical=in&(PRG32_BTN_UP|PRG32_BTN_DOWN),speed=spazer_mode?4:2;
// EDUCATIONAL: A with exactly one vertical direction changes form; A+B retains Thunder priority and conflicting directions do not transform.
if((in&PRG32_BTN_A)&&!both&&(vertical==PRG32_BTN_UP||vertical==PRG32_BTN_DOWN)){int mode=vertical==PRG32_BTN_UP;if(mode!=spazer_mode){spazer_mode=mode;prg32_audio_play_sample_pan(SFX_DOCK,180,760,0);}return;}
// EDUCATIONAL: Combined flight moves four/three pixels per tick, while the stronger robot moves two/one.
if(in&PRG32_BTN_LEFT)px-=speed;
// EDUCATIONAL: Horizontal movement uses the same speed in either direction.
if(in&PRG32_BTN_RIGHT)px+=speed;
// EDUCATIONAL: Vertical speed is one pixel below horizontal speed in both forms.
if(in&PRG32_BTN_UP)py-=speed-1;
// EDUCATIONAL: Downward movement is bounded by the lower playfield clamp.
if(in&PRG32_BTN_DOWN)py+=speed-1;
// EDUCATIONAL: Keep both forms in the existing collision playfield and preserve standard weapon mappings.
if(px<3)px=3;
// EDUCATIONAL: The right clamp keeps the collision box inside the screen.
if(px>293)px=293;
// EDUCATIONAL: The upper clamp reserves the enemy combat area.
if(py<116)py=116;
// EDUCATIONAL: The lower clamp leaves space for the controls legend.
if(py>170)py=170;
// EDUCATIONAL: A+B takes weapon priority when no transform chord was consumed.
if(both)fire_thunder();else{if(in&PRG32_BTN_A)fire_screw();if(in&PRG32_BTN_B)fire_harken();}
// EDUCATIONAL: End the input transaction without allocating memory.
}

// EDUCATIONAL: Implement the PRG32 initialization entry point called once when the cartridge starts.
void grendizer_c_init(void){state=ST_ATTRACT;previous_state=ST_ATTRACT;frame=timer=attract_page=0;stage=1;rng=0x4752454eu;old_input=0;init_stars();music_reset();}

// EDUCATIONAL: Precondition: PRG32 calls once per tick after init; changes fixed game state, reads input and schedules audio; bounded by entity capacities.
void grendizer_c_update(void){
// EDUCATIONAL: Read portable button bits once; each newly pressed action is detected independently of other held buttons.
uint32_t in=prg32_input_read();
// EDUCATIONAL: Unsigned masks isolate rising edges, including START while A or B remains held.
uint32_t pressed=in&~old_input;
// EDUCATIONAL: Save input even while paused so held START cannot repeatedly toggle pause.
old_input=in;
// EDUCATIONAL: A paused tick changes no simulation timers, random state, entities, or music sequence.
if(state==ST_PAUSE){
// EDUCATIONAL: Resume the exact previous gameplay phase without resetting its timer.
if(pressed&PRG32_BTN_START)state=previous_state;
// EDUCATIONAL: Resume takes effect on the next simulation tick.
return;
// EDUCATIONAL: End the paused-state branch.
}
// EDUCATIONAL: Accept pause only in live combat before any simulation work occurs.
if((state==ST_PLAY||state==ST_BOSS)&&(pressed&PRG32_BTN_START)){
// EDUCATIONAL: Remember whether the paused scene contains enemies or a boss.
previous_state=state;
// EDUCATIONAL: Enter the frozen state.
state=ST_PAUSE;
// EDUCATIONAL: Release music voices through the portable audio ABI; the sequencer position is preserved.
music_off();
// EDUCATIONAL: Do not advance the world on the pause-entry tick.
return;
// EDUCATIONAL: End pause entry.
}
// EDUCATIONAL: Advance animation, state timing, deterministic stars and music for an active tick.
frame++;timer++;update_stars();music_update();
// EDUCATIONAL: Combat cooldowns count down only during active simulation.
if(screw_cd)screw_cd--;
// EDUCATIONAL: Harken cooldown limits paired projectile frequency.
if(harken_cd)harken_cd--;
// EDUCATIONAL: Thunder cooldown limits its energy attack frequency.
if(thunder_cd)thunder_cd--;
// EDUCATIONAL: Damage protection survives an arbitrarily long pause.
if(invuln)invuln--;
// EDUCATIONAL: Cycle instructional pages or enter the title on a fresh action press.
if(state==ST_ATTRACT){if(pressed&(PRG32_BTN_A|PRG32_BTN_B|PRG32_BTN_START)){state=ST_TITLE;timer=0;}else if(timer>280){timer=0;attract_page=(attract_page+1)%3;}return;}
// EDUCATIONAL: Start a reproducible run or return an idle title to the attract loop.
if(state==ST_TITLE){if(pressed&(PRG32_BTN_A|PRG32_BTN_B|PRG32_BTN_START))new_game();else if(timer>420){state=ST_ATTRACT;timer=0;}return;}
// EDUCATIONAL: Launch lasts 180 ticks; only after takeoff can the Earth introduction begin.
if(state==ST_LAUNCH){if(timer>=180)start_stage_intro();return;}
// EDUCATIONAL: Both noninteractive sequences finish after 240 ticks, with no hostile updates.
if(state==ST_COSMO||state==ST_ENDING){if(timer>=240){if(state==ST_COSMO)start_stage_intro();else{state=ST_WIN;timer=0;}}return;}
// EDUCATIONAL: Stage introductions transition to play or docking after their fixed duration.
if(state==ST_STAGE_INTRO){if(timer>75){if(stage>=2&&stage<5)begin_transform();else begin_play();}return;}
// EDUCATIONAL: Docking completes before any combat updates are allowed.
if(state==ST_TRANSFORM){if(timer>120)begin_play();return;}
// EDUCATIONAL: Terminal states wait for a new action and never process lingering collisions.
if(state==ST_OVER||state==ST_WIN){if(pressed&(PRG32_BTN_A|PRG32_BTN_B|PRG32_BTN_START)){state=ST_TITLE;timer=0;music_reset();}return;}
// EDUCATIONAL: Apply movement and weapon input to the current combat scene.
update_play_input(in);
// EDUCATIONAL: Lunar escorts fire staggered shots from the same fixed pool without consuming player energy.
if(stage==5&&stage_scroll%150==0){int escort=(stage_scroll/150)%3;shot_add(W_SCREW,72+escort*80,108,0,-7,1,35);}
// EDUCATIONAL: A detached Spazer follows the player and adds one weak supporting shot every 45 active ticks without energy cost.
if(!spazer_mode&&frame%45==0)shot_add(W_SCREW,px+12,104,0,-7,1,35);
// EDUCATIONAL: Resolve player attacks, which may end a stage or win the game.
update_shots();
// EDUCATIONAL: A stage transition ends this frame transaction before enemy damage can overwrite it.
if(state!=ST_PLAY&&state!=ST_BOSS)return;
// EDUCATIONAL: Advance hostile shots and apply bounded collision damage.
update_eshots();
// EDUCATIONAL: Lethal damage ends combat immediately.
if(state==ST_OVER)return;
// EDUCATIONAL: Update only the current combat phase.
if(state==ST_PLAY)update_enemies();else update_boss();
// EDUCATIONAL: Advance terrain only if combat remains active.
if(state==ST_PLAY||state==ST_BOSS)stage_scroll+=stage;
// EDUCATIONAL: End the fixed-memory frame transaction.
}

// EDUCATIONAL: One fixed RGB565 buffer holds the largest 48x24 sprite; it is reused synchronously without heap allocation.
static uint16_t sprite_pixels[48*24];
// EDUCATIONAL: Precondition: valid palette indices and dimensions up to 48x24; changes scratch pixels and framebuffer; O(w*h); calls portable sprite drawing.
static void sprite_draw(int x,int y,int w,int h,const uint8_t *indices){
// EDUCATIONAL: Decode one byte per source pixel to the RGB565 value expected by the portable graphics ABI.
int i;for(i=0;i<w*h;i++)sprite_pixels[i]=sprite_palette[indices[i]];
// EDUCATIONAL: The host consumes the decoded frame synchronously with white as the transparent key.
prg32_sprite_draw_frame(x,y,w,h,sprite_pixels,0,0xffff);
// EDUCATIONAL: The next sprite may safely overwrite the scratch buffer after the draw returns.
}

// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void stars_draw(void){int i;for(i=0;i<MAX_STARS;i++)prg32_gfx_rect(stars[i].x,stars[i].y,stars[i].s==3?2:1,1,PRG32_COLOR_WHITE);}
// EDUCATIONAL: Precondition: stage 1..5; returns immutable mission text; O(1), no state changes or ABI calls.
static const char mission_names[5][16]={"EARTH DEFENSE","DOUBLE SPAZER","MARINE SPAZER","DRILL SPAZER","VEGA MOON BASE"};
// EDUCATIONAL: Precondition: stage 1..5; compute a position-relative row address rather than a table of unrelocated pointers; O(1), no ABI or state effects.
static const char *mission_name(void){return mission_names[stage-1];}
// EDUCATIONAL: Precondition: stage 2..5; volatile local reads prevent GCC pointer lookup tables that retain link-time addresses on relocated hosts; O(1), no game-state or ABI effects.
static const uint8_t *support_sprite(void){volatile int mission=stage;if(mission==2)return double_spazer;if(mission==3)return marine_spazer;if(mission==4)return drill_spazer;return all_spazers;}
// EDUCATIONAL: Precondition: initialized stage; draws procedural terrain through portable rectangles; O(SW), framebuffer-only effects.
static void terrain_draw(void){int x,o=stage_scroll;
// EDUCATIONAL: Earth has green foothills; Double Spazer flies above bright cloud banks.
if(stage==1){prg32_gfx_rect(0,110,SW,90,0x03e0);for(x=-32;x<352;x+=32)prg32_gfx_rect(x-(o/2)%32,137-((x+o/3)&31),32,63,0x2104);}
// EDUCATIONAL: Double Spazer fights entirely in blue sky with small scrolling cloud banks.
else if(stage==2){prg32_gfx_rect(0,18,SW,182,0x32d5);for(x=-48;x<368;x+=80){prg32_gfx_rect(x-(o/3)%80,55+(x+48)%70,44,8,0xd73a);prg32_gfx_rect(x+8-(o/3)%80,49+(x+48)%70,24,6,0xd73a);}}
// EDUCATIONAL: Marine Spazer operates under a blue water surface with drifting bubbles.
else if(stage==3){prg32_gfx_rect(0,28,SW,172,0x0210);for(x=0;x<SW;x+=24)prg32_gfx_rect(x,40+(x*3+o)%130,2,3,PRG32_COLOR_CYAN);}
// EDUCATIONAL: Drill Spazer crosses a rocky tunnel, while the Moon exposes stars and a cratered surface.
else{if(stage==5){stars_draw();prg32_gfx_rect(268,40,16,16,0x32d5);prg32_gfx_rect(271,44,6,7,0x5d4c);}else{prg32_gfx_rect(0,18,SW,182,0x2104);prg32_gfx_rect(0,18,SW,28,0x7a20);for(x=0;x<320;x+=40)prg32_gfx_rect(x,40,12,10+(x%23),0x7a20);}for(x=0;x<SW;x+=32)prg32_gfx_rect(x,150+(x+o/4)%23,24,40,stage==5?0x7493:0x7a20);}
// EDUCATIONAL: End bounded terrain rendering without stored background bitmaps.
}
// EDUCATIONAL: Precondition: initialized launch timer; draws an original mountain waterfall hangar and takeoff; O(SW), portable graphics only, no simulation changes.
static void launch_draw(void){int x,h,y=140-(timer>45?(timer-45):0);
// EDUCATIONAL: Paint the sky and stepped mountain silhouette with constant-memory geometry.
prg32_gfx_rect(0,0,320,200,0x1129);for(x=0;x<320;x+=8){h=100-(x>160?x-160:160-x)/2;prg32_gfx_rect(x,170-h,8,h,0x2948);}
// EDUCATIONAL: The waterfall hides a dark hangar opening and a split launch door.
prg32_gfx_rect(144,78,32,100,0x6f5e);prg32_gfx_rect(136,130,48,36,0x0842);prg32_gfx_rect(0,178,320,22,0x03e0);
// EDUCATIONAL: Animate exhaust and vertical departure after the doors open; clipping is handled by the host sprite ABI.
if(timer>45)prg32_gfx_rect(155,y+22,10,8+(timer&7),PRG32_COLOR_YELLOW);
// EDUCATIONAL: Render the craft before both door leaves so the closed hangar conceals it.
sprite_draw(144,y,32,24,grendizer_spazer);prg32_gfx_rect(136,130,24-(timer<45?timer/2:22),36,0x7493);prg32_gfx_rect(160+(timer<45?timer/2:22),130,24-(timer<45?timer/2:22),36,0x7493);
// EDUCATIONAL: Identify the hidden base and launch event in the native eight-pixel font.
prg32_gfx_text8(64,18,"SECRET BASE - LAUNCH",PRG32_COLOR_YELLOW,0x1129);
// EDUCATIONAL: End the cinematic without allocating a framebuffer or consuming random numbers.
}
// EDUCATIONAL: Precondition: COSMO or ENDING with timer 0..239; animate convergence, departure or escape; O(stars+sprites), only portable framebuffer effects.
static void cinematic_draw(void){int d=timer<120?120-timer:0,y=timer>120?100-(timer-120):100;
// EDUCATIONAL: The distant Moon or Earth supplies a fixed destination while stars continue their deterministic motion.
stars_draw();prg32_gfx_rect(144,30,32,24,state==ST_COSMO?0x7493:0x32d5);
// EDUCATIONAL: Three craft converge from separate directions, then one Special Cosmo departs upward.
if(state==ST_COSMO){prg32_gfx_text8(64,8,"SPECIAL COSMO",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);if(timer<120){sprite_draw(144-d,100,32,24,double_spazer);sprite_draw(144+d,100,32,24,marine_spazer);sprite_draw(144,100+d/2,32,24,drill_spazer);}else{sprite_draw(144,y,32,24,all_spazers);prg32_gfx_rect(156,y+24,8,8+(timer&7),PRG32_COLOR_YELLOW);}prg32_gfx_text8(72,180,"LAUNCH TO THE MOON",PRG32_COLOR_CYAN,PRG32_COLOR_BLACK);}
// EDUCATIONAL: After destruction, expanding flashes surround ruins as the robot and all escorts fly home.
else{int i;prg32_gfx_text8(48,8,"MOON BASE DESTROYED",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);prg32_gfx_rect(0,165,320,35,0x2948);for(i=0;i<6;i++)prg32_gfx_rect(104+i*18,160-(timer+i*7)%30,8+(timer&7),8,PRG32_COLOR_YELLOW);sprite_draw(144,y,32,24,grendizer_spazer);sprite_draw(100,y+24,32,24,double_spazer);sprite_draw(188,y+24,32,24,marine_spazer);sprite_draw(144,y+48,32,24,drill_spazer);}
// EDUCATIONAL: Cinematic drawing never changes the clocks that control stage or victory transitions.
}
// EDUCATIONAL: Precondition: initialized player; renders generated craft and docked robot; O(sprite pixels), scratch/framebuffer changes via portable sprite ABI.
static void player_draw(void){if(!spazer_mode)sprite_draw(px,py,24,24,((frame>>3)%3==0)?grendizer_1:grendizer_0);else if(stage==1||stage==5)sprite_draw(px-4,py,32,24,grendizer_spazer);else{sprite_draw(px-4,py,32,24,support_sprite());sprite_draw(px,py-5,24,24,grendizer_0);}}
// EDUCATIONAL: Precondition: valid active enemy pointer; selects its animation and renders it; O(sprite pixels); changes scratch buffer and framebuffer through the portable sprite ABI.
static void enemy_draw(const Enemy *e){const uint8_t *s;if(e->kind==0)s=e->anim?saucer_1:saucer_0;else if(e->kind==1)s=e->anim?enemy0_1:enemy0_0;else if(e->kind==2)s=e->anim?enemy1_1:enemy1_0;else if(e->kind==3)s=e->anim?enemy2_1:enemy2_0;else s=e->anim?enemy3_1:enemy3_0;sprite_draw(e->x,e->y,16,16,s);}
// EDUCATIONAL: Precondition: initialized boss for stage 1..5; draws a 48x24 frame; O(sprite pixels); changes scratch buffer and framebuffer through the portable sprite ABI.
static void boss_draw(void){const uint8_t *s;if(stage==5){int x;prg32_gfx_rect(boss_x,boss_y+8,48,16,0x2948);for(x=0;x<48;x+=16){prg32_gfx_rect(boss_x+x,boss_y,12,24,0x7493);prg32_gfx_rect(boss_x+x+3,boss_y+6,6,4,PRG32_COLOR_RED);}prg32_gfx_rect(boss_x+18,boss_y+14,12,10,PRG32_COLOR_MAGENTA);return;}if(stage==1)s=((frame>>3)&1)?boss0_1:boss0_0;else if(stage==2)s=((frame>>3)&1)?boss1_1:boss1_0;else s=((frame>>3)&1)?boss2_1:boss2_0;sprite_draw(boss_x,boss_y,48,24,s);}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void shots_draw(void){int i;for(i=0;i<MAX_SHOTS;i++)if(shots[i].active){Shot*s=&shots[i];if(s->kind==W_SCREW){prg32_gfx_rect(s->x,s->y,4,8,PRG32_COLOR_YELLOW);prg32_gfx_rect(s->x-2,s->y+2,8,2,PRG32_COLOR_WHITE);}else if(s->kind==W_HARKEN){prg32_gfx_rect(s->x,s->y,7,2,PRG32_COLOR_CYAN);prg32_gfx_rect(s->x+2,s->y-2,3,6,PRG32_COLOR_WHITE);}else{prg32_gfx_rect(s->x,s->y,6,10,PRG32_COLOR_CYAN);prg32_gfx_rect(s->x+2,18,2,s->y-18,PRG32_COLOR_YELLOW);}}for(i=0;i<MAX_ESHOTS;i++)if(eshots[i].active){uint16_t c=eshots[i].kind==2?PRG32_COLOR_MAGENTA:PRG32_COLOR_RED;prg32_gfx_rect(eshots[i].x,eshots[i].y,4,6,c);}}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void hud_draw(void){char n[12];prg32_gfx_rect(0,0,SW,18,PRG32_COLOR_BLUE);prg32_gfx_text8(4,5,"GRENDIZER",PRG32_COLOR_YELLOW,PRG32_COLOR_BLUE);prg32_gfx_text8(82,5,"S",PRG32_COLOR_WHITE,PRG32_COLOR_BLUE);num((unsigned)score,n);prg32_gfx_text8(94,5,n,PRG32_COLOR_WHITE,PRG32_COLOR_BLUE);prg32_gfx_text8(195,5,"L",PRG32_COLOR_WHITE,PRG32_COLOR_BLUE);num((unsigned)lives,n);prg32_gfx_text8(207,5,n,PRG32_COLOR_YELLOW,PRG32_COLOR_BLUE);prg32_gfx_text8(224,5,"E",PRG32_COLOR_WHITE,PRG32_COLOR_BLUE);num((unsigned)energy,n);prg32_gfx_text8(236,5,n,PRG32_COLOR_CYAN,PRG32_COLOR_BLUE);prg32_gfx_text8(258,5,"STAGE",PRG32_COLOR_WHITE,PRG32_COLOR_BLUE);num((unsigned)stage,n);prg32_gfx_text8(306,5,n,PRG32_COLOR_YELLOW,PRG32_COLOR_BLUE);}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void title_draw(void){prg32_gfx_text8(116,32,"VEGA ASSAULT",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);sprite_draw(148,68,24,24,grendizer_0);prg32_gfx_text8(62,108,"A  SCREW CRUSHER",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(62,122,"B  DOUBLE HARKEN",PRG32_COLOR_CYAN,PRG32_COLOR_BLACK);prg32_gfx_text8(62,136,"A+B SPACE THUNDER",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);prg32_gfx_text8(74,160,"START 1 PLAYER",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(55,186,"UNOFFICIAL FAN ARCADE",0x8410,PRG32_COLOR_BLACK);}
// EDUCATIONAL: Keep this state or helper private to the cartridge translation unit to minimize the exported ABI surface.
static void attract_draw(void){if(attract_page==0){title_draw();prg32_gfx_text8(96,176,"INSERT COIN",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);}else if(attract_page==1){prg32_gfx_text8(105,30,"HOW TO PLAY",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);sprite_draw(30,62,32,24,grendizer_spazer);prg32_gfx_text8(80,64,"DESTROY VEGA FORCES",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(80,82,"BUILD ENERGY",PRG32_COLOR_CYAN,PRG32_COLOR_BLACK);prg32_gfx_text8(80,100,"UNLEASH SPACE THUNDER",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);prg32_gfx_text8(82,146,"A+B = 3 ENERGY",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);}
// EDUCATIONAL: Handle the complementary branch of the preceding gameplay condition.
else{prg32_gfx_text8(96,28,"ENEMY FILE",PRG32_COLOR_RED,PRG32_COLOR_BLACK);sprite_draw(45,72,16,16,enemy0_0);sprite_draw(110,72,16,16,enemy1_0);sprite_draw(175,72,16,16,enemy2_0);sprite_draw(240,72,16,16,enemy3_0);prg32_gfx_text8(34,100,"BLADE",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(99,100,"CRAWLER",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(171,100,"BEAST",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(230,100,"GUNNER",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(84,164,"PRESS START",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);}}

// EDUCATIONAL: Implement the PRG32 draw entry point: render the current state without changing gameplay semantics.
void grendizer_c_draw(void){int i;char n[12];prg32_gfx_clear(PRG32_COLOR_BLACK);if(state<=ST_TITLE)stars_draw();if(state==ST_LAUNCH){launch_draw();return;}if(state==ST_COSMO||state==ST_ENDING){cinematic_draw();return;}if(state==ST_ATTRACT){attract_draw();return;}if(state==ST_TITLE){title_draw();return;}if(state==ST_STAGE_INTRO){terrain_draw();prg32_gfx_text8(116,64,"STAGE",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);num((unsigned)stage,n);prg32_gfx_text8(168,64,n,PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(80,90,mission_name(),PRG32_COLOR_CYAN,PRG32_COLOR_BLACK);return;}if(state==ST_TRANSFORM){terrain_draw();prg32_gfx_text8(78,44,mission_name(),PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);sprite_draw(144,82,32,24,support_sprite());if(stage<5)sprite_draw(148,18+(timer<64?timer:64),24,24,grendizer_0);if(timer>96)prg32_gfx_text8(100,134,"DOCKING COMPLETE",PRG32_COLOR_CYAN,PRG32_COLOR_BLACK);return;}if(state==ST_OVER){prg32_gfx_text8(112,74,"GAME OVER",PRG32_COLOR_RED,PRG32_COLOR_BLACK);num((unsigned)score,n);prg32_gfx_text8(105,110,"SCORE",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(151,110,n,PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);prg32_gfx_text8(78,166,"PRESS START",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);return;}if(state==ST_WIN){prg32_gfx_text8(78,52,"MOON BASE DESTROYED",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);terrain_draw();prg32_gfx_rect(136,170,48,10,0x2948);for(i=0;i<3;i++)prg32_gfx_rect(138+i*16,158+((timer+i*5)&7),8,12,PRG32_COLOR_YELLOW);sprite_draw(144,86,32,24,all_spazers);prg32_gfx_text8(96,126,"EARTH IS SAFE",PRG32_COLOR_CYAN,PRG32_COLOR_BLACK);num((unsigned)score,n);prg32_gfx_text8(105,150,"SCORE",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);prg32_gfx_text8(151,150,n,PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);return;}terrain_draw();hud_draw();if(state==ST_BOSS||(state==ST_PAUSE&&previous_state==ST_BOSS)){boss_draw();prg32_gfx_text8(6,22,stage==5?"MOON BASE":stage==3?"SEA TYRANT":"VEGA BOSS",PRG32_COLOR_RED,PRG32_COLOR_BLACK);prg32_gfx_rect(130,23,(boss_hp*120)/boss_max,4,PRG32_COLOR_MAGENTA);}else for(i=0;i<MAX_ENEMIES;i++)if(enemies[i].active)enemy_draw(&enemies[i]);shots_draw();if(!spazer_mode)sprite_draw(px-4,90+((frame>>3)&7),32,24,(stage==1||stage==5)?solo_spazer:support_sprite());if(stage==5){sprite_draw(56,110,32,24,double_spazer);sprite_draw(136,106,32,24,marine_spazer);sprite_draw(216,110,32,24,drill_spazer);}if(!invuln||((invuln>>2)&1)==0)player_draw();if(state==ST_PAUSE){prg32_gfx_rect(104,82,112,30,PRG32_COLOR_BLACK);prg32_gfx_text8(132,94,"PAUSE",PRG32_COLOR_YELLOW,PRG32_COLOR_BLACK);}prg32_gfx_text8(4,189,"DOWN+A ROBOT  UP+A SPAZER",PRG32_COLOR_WHITE,PRG32_COLOR_BLACK);}
