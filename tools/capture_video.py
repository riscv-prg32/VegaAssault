#!/usr/bin/env python3
"""Record native gameplay and synchronized stereo music into a short MP4.

Uses the actual game update/draw functions, upstream software renderer, and
explicit campaign scene fixtures and scripted combat input. Music uses the preview mixer model; SFX are omitted.
Requires ffmpeg on PATH or FFMPEG pointing to an executable.
"""
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import wave

from capture_screen import FRAMEWORK, ROOT, renderer_source
from preview_music import RATE, render

TICKS = 910


def main():
    ffmpeg = os.environ.get('FFMPEG') or shutil.which('ffmpeg')
    if not ffmpeg:
        raise SystemExit('Set FFMPEG to an ffmpeg executable or install ffmpeg.')
    output = ROOT / 'build/game-demo'
    output.mkdir(parents=True, exist_ok=True)
    (output / 'capture_renderer.h').write_text(renderer_source())
    source = (ROOT / 'tests/capture_game.c').read_text().split('int main(void){')[0]
    source = source.replace('../src/game.c', str(ROOT / 'src/game.c'))
    source = 'static int video_tick;\n' + source
    source = source.replace('(void)c;(void)i;(void)n;(void)v;',
                            'fprintf(stderr,"%d %u %u %u %u\\n",video_tick,c,i,n,v);')
    source = source.replace('(void)c;', 'fprintf(stderr,"%d %u 0 0 0\\n",video_tick,c);')
    source += '''
int main(void){
grendizer_c_init();
for(video_tick=0;video_tick<910;video_tick++){
int target=px,nearest=-1;
capture_input=0;
if(video_tick==0)state=ST_TITLE;
if(video_tick==30)new_game();
if(video_tick==210||video_tick==270||video_tick==330||video_tick==390||video_tick==600){
stage=video_tick==210?1:video_tick==270?2:video_tick==330?3:video_tick==390?4:5;
start_stage_intro();begin_play();invuln=1001;
}
if(video_tick==450){stage=4;next_stage();}
if(video_tick>=450&&video_tick<600)timer=(video_tick-450)*240/150;
if(video_tick>=600&&video_tick<810)stage_scroll=(video_tick-600)*10;
if(video_tick==750)begin_boss();
if(video_tick==809){boss_hp=1;shot_add(W_SCREW,boss_x,boss_y+7,0,-7,1,35);}
if(video_tick==810)assert(state==ST_ENDING);
if(video_tick>=810)timer=(video_tick-810)*240/100;
if(state==ST_PLAY||state==ST_BOSS){
invuln=1001;
capture_input=PRG32_BTN_A;
if(state==ST_BOSS)target=boss_x+12;
else for(int i=0;i<MAX_ENEMIES;i++){
if(enemies[i].active&&enemies[i].y>nearest){nearest=enemies[i].y;target=enemies[i].x;}
}
if(target>px+3)capture_input|=PRG32_BTN_RIGHT;
if(target<px-3)capture_input|=PRG32_BTN_LEFT;
if((video_tick%180)<12&&energy>=3)capture_input|=PRG32_BTN_B;
}
grendizer_c_update();grendizer_c_draw();
assert(state!=ST_OVER);
if(video_tick%30==0)fprintf(stderr,"STATE %d %d %d %d %d\\n",video_tick,state,stage,lives,score);
for(int y=0;y<200;y++)for(int x=0;x<320;x++){
uint16_t p=g_fb[(y+PRG32_VIEWPORT_Y)*320+x];
putchar(((p>>11)&31)*255/31);putchar(((p>>5)&63)*255/63);putchar((p&31)*255/31);
}
}
return ferror(stdout)?1:0;
}
'''
    # Annotate the generated host-only harness, including all scripted inputs.
    annotated = '\n'.join('// EDUCATIONAL: Demo capture executes game code and records frames/events through host mocks.\n' + line
                          if line.strip() and not line.lstrip().startswith('//') else line
                          for line in source.splitlines())
    cfile, executable = output / 'capture.c', output / 'capture'
    cfile.write_text(annotated)
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c99', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', '-I', str(output),
                    '-I', str(FRAMEWORK / 'components/prg32/include'),
                    '-I', str(FRAMEWORK / 'components/prg32_audio/include'),
                    str(cfile), '-o', str(executable)], check=True)
    raw = output / 'frames.rgb'
    with raw.open('wb') as frames, (output / 'events.log').open('w') as log:
        subprocess.run([str(executable)], stdout=frames, stderr=log, check=True)
    assert raw.stat().st_size == TICKS * 320 * 200 * 3
    lines = (output / 'events.log').read_text().splitlines()
    note_events = [list(map(int, line.split())) for line in lines if not line.startswith('STATE')]
    states = [line for line in lines if line.startswith('STATE')]
    print('\n'.join(states), flush=True)
    stereo, _ = render(note_events, TICKS)
    peak = max(map(abs, stereo))
    if sys.byteorder != 'little':
        stereo.byteswap()
    audio = output / 'soundtrack.wav'
    with wave.open(str(audio), 'wb') as wav:
        wav.setparams((2, 2, RATE, 0, 'NONE', 'not compressed'))
        wav.writeframes(stereo.tobytes())
    video = output / 'vega-assault-demo.mp4'
    # Integer scaling preserves the game's pixels; modest fades finish the clip.
    subprocess.run([ffmpeg, '-hide_banner', '-loglevel', 'warning', '-y',
                    '-f', 'rawvideo', '-pixel_format', 'rgb24', '-video_size', '320x200',
                    '-framerate', '1000/33', '-i', str(raw), '-i', str(audio),
                    '-vf', 'scale=960:600:flags=neighbor,fade=t=out:st=29.4:d=0.6',
                    '-af', 'afade=t=out:st=29.4:d=0.6',
                    '-c:v', 'libx264', '-crf', '18', '-preset', 'medium', '-pix_fmt', 'yuv420p',
                    '-c:a', 'aac', '-b:a', '160k', '-movflags', '+faststart',
                    '-t', '30', str(video)], check=True)
    (output / 'capture.json').write_text(json.dumps({
        'duration_seconds': 30, 'ticks': TICKS, 'size': [960, 600],
        'capture': 'native campaign montage with explicit scene fixtures and accelerated cinematic/form timing',
        'audio': 'synchronized C note events, software stereo mixer model, no SFX',
        'stereo_pcm_peak': peak, 'states': states,
    }, indent=2) + '\n')
    print(video)


if __name__ == '__main__':
    main()
