#!/usr/bin/env python3
"""Generate compact, original RGB565 fan-game sprites for PRG32.

The art is drawn procedurally for this project. It evokes the recognizable
Grendizer/Spazer visual vocabulary without copying production frames or scans.
"""
from pathlib import Path
from PIL import Image, ImageDraw

# RGB565 palette: transparency key remains white to match the existing game.
W=0xFFFF; K=0x0000; R=0xF800; DR=0x9000; B=0x001F; DB=0x0010; C=0x07FF
Y=0xFFE0; G=0x07E0; M=0xF81F; D=0x4208; S=0x8410; O=0xFD20; LG=0xC618

def blank(w,h): return [[W]*w for _ in range(h)]
def px(a,x,y,c):
    if 0<=y<len(a) and 0<=x<len(a[0]): a[y][x]=c
def rect(a,x,y,w,h,c):
    for yy in range(max(0,y),min(len(a),y+h)):
        for xx in range(max(0,x),min(len(a[0]),x+w)): a[yy][xx]=c
def line(a,x0,y0,x1,y1,c):
    dx=abs(x1-x0); sx=1 if x0<x1 else -1; dy=-abs(y1-y0); sy=1 if y0<y1 else -1; e=dx+dy
    while True:
        px(a,x0,y0,c)
        if x0==x1 and y0==y1: break
        e2=2*e
        if e2>=dy: e+=dy; x0+=sx
        if e2<=dx: e+=dx; y0+=sy

def robot_frame(variant=0, spazer=False, pose=0):
    w,h=(32,24) if spazer else (24,24); a=blank(w,h); cx=w//2
    # Gold horns/crown create the unmistakable silhouette while all pixels are original.
    line(a,cx-3,2,cx-8,0,Y); line(a,cx+3,2,cx+8,0,Y); px(a,cx-7,1,O); px(a,cx+7,1,O)
    rect(a,cx-4,1,8,5,DB); rect(a,cx-2,0,4,2,R); rect(a,cx-1,2,2,2,Y)
    rect(a,cx-4,4,8,2,LG); rect(a,cx-2,4,4,1,C)
    # Broad dark-blue shoulders, white forearms and red chest chevron.
    rect(a,cx-7,7,14,3,DB); rect(a,cx-9,8,3,4,B); rect(a,cx+6,8,3,4,B)
    rect(a,cx-5,8,10,9,B); rect(a,cx-3,9,6,5,R); px(a,cx-2,10,Y); px(a,cx+1,10,Y)
    rect(a,cx-2,13,4,2,LG); rect(a,cx-1,13,2,1,C)
    if pose==1:
        line(a,cx-8,10,cx-11,7,LG); line(a,cx+8,10,cx+11,7,LG); px(a,cx-11,6,Y); px(a,cx+11,6,Y)
    elif pose==2:
        line(a,cx-8,11,cx-11,14,LG); line(a,cx+8,11,cx+11,14,LG); px(a,cx-11,15,Y); px(a,cx+11,15,Y)
    else:
        rect(a,cx-9,11,3,5,LG); rect(a,cx+6,11,3,5,LG); px(a,cx-8,16,Y); px(a,cx+8,16,Y)
    # Split armored legs with red boots and light-blue highlights.
    rect(a,cx-5,16,4,6,DB); rect(a,cx+1,16,4,6,DB); px(a,cx-4,17,C); px(a,cx+3,17,C)
    rect(a,cx-6,21,5,2,R); rect(a,cx+1,21,5,2,R); px(a,cx-6,23,K); px(a,cx+5,23,K)
    if variant: px(a,cx-5,8,Y); px(a,cx+4,8,Y)
    if spazer:
        # Compact red/white flying saucer wraps around the robot without a full bitmap.
        line(a,1,14,8,11,R); line(a,30,14,23,11,R); rect(a,5,13,22,2,LG); rect(a,1,15,30,2,R)
        rect(a,4,17,24,2,DB); line(a,1,15,0,18,Y); line(a,30,15,31,18,Y); px(a,7,18,C); px(a,24,18,C)
    return a

def saucer(kind=0, anim=0):
    a=blank(16,16); col=[M,G,O,C][kind%4]
    rect(a,6,3,4,2,C); rect(a,4,5,8,2,DB); rect(a,2,7,12,2,col); rect(a,0,9,16,2,DB)
    rect(a,2,11,12,2,col); px(a,1,8,Y); px(a,14,8,Y); px(a,7,6,LG); px(a,8,6,LG)
    if anim: px(a,4,13,O); px(a,11,13,O); px(a,5,14,Y); px(a,10,14,Y)
    return a

def enemy_robot(kind=0, anim=0):
    a=blank(16,16); c=[M,G,O,R][kind%4]; sh=[DR,D,DR,DB][kind%4]
    if kind==0: # blade robot
        line(a,4,4,2,1,Y); line(a,11,4,13,1,Y); rect(a,5,3,6,4,c); rect(a,4,7,8,6,sh)
        line(a,4,8,0,6,LG); line(a,11,8,15,6,LG); rect(a,5,13,2,3,c); rect(a,9,13,2,3,c)
    elif kind==1: # crawler
        rect(a,4,3,8,3,DB); rect(a,3,6,10,5,c); rect(a,1,10,14,4,D); rect(a,2,11,12,2,K)
        px(a,6,4,C); px(a,9,4,C); px(a,1,13,S); px(a,14,13,S)
    elif kind==2: # horn beast
        line(a,5,4,2,0,Y); line(a,10,4,13,0,Y); rect(a,5,3,6,5,c); rect(a,3,8,10,5,DB)
        rect(a,1,9,3,3,c); rect(a,12,9,3,3,c); rect(a,3,13,3,3,c); rect(a,10,13,3,3,c)
    else: # gunner
        rect(a,4,2,8,5,DB); px(a,6,4,C); px(a,9,4,C); rect(a,3,7,10,6,c)
        rect(a,0 if not anim else 1,8,4,2,Y); rect(a,12 if not anim else 11,8,4,2,Y); rect(a,5,13,2,3,DB); rect(a,9,13,2,3,DB)
    if anim: px(a,7,1,Y); px(a,8,1,Y)
    return a

def boss(kind=0, anim=0):
    a=blank(48,24); c=[M,O,R][kind]; sh=[DR,DR,DB][kind]
    # Three bosses share a large arcade silhouette but differ in weapons and cores.
    line(a,8,7,3,3,Y); line(a,39,7,44,3,Y); rect(a,8,6,32,11,sh); rect(a,12,4,24,12,c)
    rect(a,17,1,14,5,DB); rect(a,21,0,6,3,Y); rect(a,20,8,8,5,C); rect(a,22,9,4,3,LG)
    rect(a,2,9,8,5,DB); rect(a,38,9,8,5,DB); rect(a,14,17,7,6,DB); rect(a,27,17,7,6,DB)
    if kind==1:
        line(a,9,8,0,5,Y); line(a,38,8,47,5,Y); rect(a,20,18,8,5,G); px(a,23,20,C); px(a,24,20,C)
    if kind==2:
        rect(a,4,3,8,4,R); rect(a,36,3,8,4,R); line(a,4,3,0,0,Y); line(a,43,3,47,0,Y); rect(a,21,13,6,8,Y)
    if anim: rect(a,17,6,4,2,LG); rect(a,27,6,4,2,LG); px(a,19,6,C); px(a,28,6,C)
    return a

def transform_frame(step):
    a=blank(32,24); rob=robot_frame(pose=step%3)
    for y in range(24):
        for x in range(24):
            if rob[y][x]!=W: a[y][x+4]=rob[y][x]
    if step>=1:
        span=6+step*5; x0=max(0,16-span); x1=min(31,16+span)
        line(a,x0,14,4,12,R); line(a,x1,14,27,12,R); rect(a,x0,15,x1-x0+1,2,LG)
    if step>=2: rect(a,3,17,26,2,DB); px(a,5,18,C); px(a,26,18,C)
    if step>=3: line(a,1,15,0,19,Y); line(a,30,15,31,19,Y)
    return a

assets=[]
for i in range(3): assets.append((f'grendizer_{i}',robot_frame(variant=i&1,pose=i)))
assets.append(('grendizer_spazer',robot_frame(spazer=True,pose=1)))
for i in range(4): assets.append((f'transform_{i}',transform_frame(i)))
for k in range(4):
    for an in range(2): assets.append((f'enemy{k}_{an}',enemy_robot(k,an)))
for an in range(2): assets.append((f'saucer_{an}',saucer(0,an)))
for k in range(3):
    for an in range(2): assets.append((f'boss{k}_{an}',boss(k,an)))

# Emit line-by-line educational comments directly, preserving the repository teaching policy.
palette=sorted({v for _,a in assets for row in a for v in row})
assert len(palette)<=256, 'Sprite indices must fit in one byte'
indices={v:i for i,v in enumerate(palette)}
h_lines=[]; c_lines=[]
def hed(s): h_lines.extend([f'// EDUCATIONAL: This generated declaration supports compact RGB565 sprite rendering.',s])
def ced(s): c_lines.extend([f'// EDUCATIONAL: Generated palette/index data from tools/generate_assets.py reconstructs RGB565 pixels.',s])
hed('#pragma once'); hed('#include <stdint.h>'); ced('#include "sprites.h"')
hed(f'extern const uint16_t sprite_palette[{len(palette)}];')
ced(f'const uint16_t sprite_palette[{len(palette)}]={{'+','.join(f'0x{v:04x}' for v in palette)+'};')
for name,a in assets:
    h=len(a); w=len(a[0]); flat=[v for row in a for v in row]
    hed(f'extern const uint8_t {name}[{w*h}];')
    ced(f'const uint8_t {name}[{w*h}]={{')
    for i in range(0,len(flat),16): ced(' '+','.join(str(indices[x]) for x in flat[i:i+16])+',')
    ced('};')
Path('src/sprites.h').write_text('\n'.join(h_lines)+'\n'); Path('src/sprites.c').write_text('\n'.join(c_lines)+'\n')

# Sprite-sheet preview with labels; not included in the runtime cartridge.
scale=5; cellw=58; cellh=31; cols=4; rows=(len(assets)+cols-1)//cols
img=Image.new('RGB',(cellw*cols*scale,cellh*rows*scale),(7,10,18)); draw=ImageDraw.Draw(img)
def rgb565(v): return (((v>>11)&31)*255//31,((v>>5)&63)*255//63,(v&31)*255//31)
for idx,(name,a) in enumerate(assets):
    col=idx%cols; row=idx//cols; ox=(col*cellw+2)*scale; oy=(row*cellh+2)*scale
    for y,rr in enumerate(a):
        for x,v in enumerate(rr):
            if v!=W: draw.rectangle((ox+x*scale,oy+y*scale,ox+(x+1)*scale-1,oy+(y+1)*scale-1),fill=rgb565(v))
    draw.text((ox,oy+25*scale),name,fill=(150,200,255))
img.save('assets/sprite-sheet.png')

# Store icon and splash are derived from the same original pixel vocabulary.
def paste_sprite(dst,a,x,y,scale):
    d=ImageDraw.Draw(dst)
    for yy,row in enumerate(a):
        for xx,v in enumerate(row):
            if v!=W: d.rectangle((x+xx*scale,y+yy*scale,x+(xx+1)*scale-1,y+(yy+1)*scale-1),fill=rgb565(v))
icon=Image.new('RGB',(128,128),(3,8,24)); di=ImageDraw.Draw(icon); di.rectangle((2,2,125,125),outline=(0,150,255),width=2)
paste_sprite(icon,robot_frame(pose=1),16,5,4); di.text((20,105),'VEGA 86',fill=(255,210,40)); icon.save('store/icon.png')
splash=Image.new('RGB',(320,200),(2,5,18)); ds=ImageDraw.Draw(splash)
for x,y in [(15,20),(44,58),(285,31),(252,78),(130,18),(210,45)]: ds.point((x,y),fill=(180,220,255))
ds.ellipse((220,-35,355,100),fill=(8,35,95),outline=(20,100,220)); paste_sprite(splash,robot_frame(spazer=True,pose=1),24,35,6)
ds.text((120,40),'GRENDIZER',fill=(255,195,20)); ds.text((122,58),"VEGA ASSAULT '86",fill=(90,190,255)); ds.text((122,82),'PRG32 EDUCATIONAL FAN GAME',fill=(220,220,220)); ds.text((122,102),'PRESS START',fill=(255,255,255)); splash.save('store/splash.png')
print('assets',len(assets),'indexed_sprite_bytes',sum(len(a)*len(a[0]) for _,a in assets),'palette_bytes',len(palette)*2,'decode_buffer_bytes',48*24*2)
