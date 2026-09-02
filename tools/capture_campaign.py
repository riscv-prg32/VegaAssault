#!/usr/bin/env python3
"""Render labeled campaign fixtures using actual game code and the PRG32 renderer.

Fixtures select scenes directly for visual QA; they are not playthrough evidence.
"""
import io
import os
from pathlib import Path
import subprocess
from PIL import Image, ImageDraw
from capture_screen import ROOT, FRAMEWORK, renderer_source


def main():
    out = ROOT / 'build/campaign-capture'
    out.mkdir(parents=True, exist_ok=True)
    (out / 'capture_renderer.h').write_text(renderer_source())
    source = (ROOT / 'tests/capture_game.c').read_text().split('int main(void){')[0]
    source = source.replace('../src/game.c', str(ROOT / 'src/game.c'))
    source += '''
#include <stdlib.h>
int main(int argc,char **argv){
int scene=argc>1?atoi(argv[1]):0;grendizer_c_init();new_game();
if(scene<3){timer=scene*55;state=ST_LAUNCH;}
else if(scene<8){stage=scene-2;start_stage_intro();begin_play();}
else if(scene==8){stage=5;start_stage_intro();begin_boss();}
else if(scene==9){stage=5;state=ST_WIN;timer=40;}
else if(scene<14){stage=scene-8;start_stage_intro();begin_transform();timer=80;}
else if(scene<16){stage=5;state=ST_COSMO;timer=scene==14?60:150;}
else if(scene<18){stage=5;start_stage_intro();begin_play();stage_scroll=scene==16?600:1200;}
else{stage=5;state=ST_ENDING;timer=scene==18?60:180;}
grendizer_c_draw();printf("P6\\n320 200\\n255\\n");
for(int y=0;y<200;y++)for(int x=0;x<320;x++){
uint16_t p=g_fb[(y+PRG32_VIEWPORT_Y)*320+x];
putchar(((p>>11)&31)*255/31);putchar(((p>>5)&63)*255/63);putchar((p&31)*255/31);
}
return ferror(stdout)?1:0;
}
'''
    source = '\n'.join('// EDUCATIONAL: Host-only campaign fixture exercises the actual portable renderer.\n'+line
                       if line.strip() and not line.lstrip().startswith('//') else line
                       for line in source.splitlines())
    cfile = out / 'capture.c'
    cfile.write_text(source)
    executable = out / 'capture'
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c99', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', '-I', str(out),
                    '-I', str(FRAMEWORK / 'components/prg32/include'),
                    '-I', str(FRAMEWORK / 'components/prg32_audio/include'),
                    str(cfile), '-o', str(executable)], check=True)
    names = ['Hangar opens', 'Takeoff', 'Climb', 'Earth defense', 'Double Spazer',
             'Marine Spazer', 'Drill Spazer', 'Lunar Spazer and escorts', 'Vega Moon base boss', 'Victory',
             'Double docking', 'Marine docking', 'Drill docking', 'Special Cosmo', 'Cosmo assembly', 'Cosmo Moon launch',
             'Lunar robot and escorts', 'Lunar combined form', 'Moon base destruction', 'Flight home']
    sheet = Image.new('RGB', (640, 2200), '#101827')
    draw = ImageDraw.Draw(sheet)
    for scene, name in enumerate(names):
        screen = Image.open(io.BytesIO(subprocess.check_output([str(executable), str(scene)])))
        screen.save(out / f'{scene}.png')
        x, y = (scene % 2)*320, (scene // 2)*220
        sheet.paste(screen, (x, y+20))
        draw.text((x+8, y+4), name, fill='white')
    target = ROOT / 'assets/campaign-scenes.png'
    sheet.save(target)
    print(target)


if __name__ == '__main__':
    main()
