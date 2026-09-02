#!/usr/bin/env python3
"""Generate compact, original RGB565 fan-game sprites for PRG32.

The art is drawn procedurally for this project using classic mecha conventions.
Armor contours, palettes and insignia are project-authored, without traced references.
"""
from pathlib import Path
from PIL import Image, ImageDraw

# RGB565 palette: transparency key remains white to match the existing game.
W=0xFFFF; K=0x0842; R=0xD986; DR=0x7805; B=0x32D5; DB=0x1129; C=0x6F5E
Y=0xFF0C; G=0x5D4C; M=0xAAB6; D=0x2948; S=0x7493; O=0xEBA7; LG=0xD73A

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

def poly(a, points, color):
    """Rasterize an integer armor contour offline; no runtime PRG32 calls or allocation."""
    mask = Image.new('1', (len(a[0]), len(a)))
    ImageDraw.Draw(mask).polygon(points, fill=1)
    for y in range(len(a)):
        for x in range(len(a[0])):
            if mask.getpixel((x, y)):
                a[y][x] = color


def panel(a, points, color):
    """Fill a closed armor plate and ink its edge; mutates only the supplied frame."""
    poly(a, points, color)
    for start, end in zip(points, points[1:] + points[:1]):
        line(a, *start, *end, K)


def flight_shell(a, step=3):
    """Layer original swept docking wings onto a 32x24 frame in four fixed stages."""
    span = (0, 8, 12, 15)[step]
    if not span:
        return
    for side in (-1, 1):
        def p(x, y):
            return (15 + side*x if side < 0 else 16 + side*x, y)
        panel(a, [p(3, 13), p(span, 10), p(span, 16), p(7, 20), p(2, 18)], R)
        line(a, *p(4, 14), *p(span-1, 12), LG)
        line(a, *p(5, 17), *p(span-1, 15), DR)
        line(a, *p(6, 19), *p(span-1, 17), DB)
        px(a, *p(7, 18), C)
    panel(a, [(10, 14), (21, 14), (23, 17), (18, 20), (13, 20), (8, 17)], DB)
    line(a, 11, 15, 20, 15, LG)
    line(a, 12, 17, 19, 17, B)
    px(a, 14, 18, C); px(a, 17, 18, C)


def robot_frame(variant=0, spazer=False, pose=0):
    """Author a 24px mecha pose with inked plates; preserve the game's frame envelopes."""
    a = blank(24, 24)
    # Split boots, hip joints and tapered greaves keep the pose readable at native size.
    for x in (7, 13):
        panel(a, [(x, 15), (x+3, 15), (x+4, 22), (x-1, 23), (x-1, 20)], DB)
        rect(a, x, 17, 2, 3, LG); px(a, x+2, 18, S)
        rect(a, x, 21, 3, 2, R); px(a, x, 21, O)
    panel(a, [(7, 12), (16, 12), (16, 16), (12, 18), (7, 16)], D)
    line(a, 9, 15, 14, 15, LG); rect(a, 11, 15, 2, 2, O)
    # Faceted shoulders and substantial gauntlets replace single-pixel stick arms.
    for side in (-1, 1):
        def p(x, y):
            return (11-x if side < 0 else 12+x, y)
        panel(a, [p(4, 7), p(7, 6), p(10, 9), p(7, 12), p(4, 10)], DB)
        line(a, *p(5, 8), *p(7, 7), B)
        y = (12, 7, 13)[pose]
        panel(a, [p(7, y), p(9, y-1), p(11, y+2), p(10, y+4), p(7, y+3)], S)
        line(a, *p(8, y), *p(9, y+2), LG)
        line(a, *p(8, y+3), *p(10, y+3), DB)
    panel(a, [(7, 7), (16, 7), (17, 11), (14, 15), (9, 15), (6, 11)], DB)
    # An original split diagonal breastplate surrounds a narrow central reactor.
    poly(a, [(7, 8), (10, 9), (10, 12), (8, 11)], R)
    poly(a, [(16, 8), (13, 9), (13, 12), (15, 11)], R)
    px(a, 8, 8, O); px(a, 15, 8, O)
    rect(a, 11, 9, 2, 4, S); rect(a, 11, 10, 2, 1, C if variant else LG)
    line(a, 9, 14, 14, 14, B)
    # Swept cheek fins, a split crest and recessed eyes suggest cel-drawn machinery.
    panel(a, [(8, 1), (15, 1), (17, 4), (14, 7), (9, 7), (6, 4)], DB)
    for side in (-1, 1):
        def p(x, y):
            return (11-x if side < 0 else 12+x, y)
        poly(a, [p(4, 3), p(8, 2), p(9, 0), p(8, 4), p(5, 5)], O)
        line(a, *p(5, 3), *p(8, 2), Y)
    rect(a, 10, 0, 1, 3, R); rect(a, 13, 0, 1, 3, R)
    line(a, 9, 3, 10, 3, C); line(a, 13, 3, 14, 3, C)
    poly(a, [(9, 5), (11, 4), (14, 5), (12, 7)], LG)
    px(a, 12, 5, S); px(a, 12, 6, D)
    if spazer:
        wide = blank(32, 24)
        for y, row in enumerate(a):
            wide[y][4:28] = row
        flight_shell(wide)
        return wide
    return a


def saucer(kind=0, anim=0):
    """Draw an elliptical scout with a recessed canopy and alternating engine lights."""
    a = blank(16, 16); col = [M, G, O, C][kind % 4]
    panel(a, [(5, 3), (10, 3), (12, 7), (3, 7)], DB)
    line(a, 6, 4, 9, 4, C); px(a, 5, 5, LG)
    panel(a, [(3, 6), (12, 6), (15, 9), (12, 12), (3, 12), (0, 9)], col)
    line(a, 4, 7, 11, 7, LG); line(a, 2, 9, 13, 9, DB)
    line(a, 4, 11, 11, 11, D)
    for x in (4, 11):
        px(a, x, 10, C if anim else O)
        if anim:
            line(a, x, 12, x, 14, O); px(a, x, 13, Y)
    return a


def enemy_robot(kind=0, anim=0):
    """Build four original 16px enemy classes with articulated two-frame motion."""
    a = blank(16, 16); col = [M, G, O, R][kind]
    if kind == 1:
        panel(a, [(4, 2), (10, 2), (12, 7), (3, 7)], DB)
        line(a, 5, 4, 9, 4, C)
        panel(a, [(3, 6), (11, 6), (14, 10), (1, 10)], col)
        line(a, 4, 7, 9, 7, LG)
        panel(a, [(2, 10), (13, 10), (15, 12), (13, 15), (2, 15), (0, 12)], D)
        line(a, 2, 12, 13, 12, K)
        for x in range(2+anim, 14, 3):
            px(a, x, 13, LG)
        return a
    for x, dy in ((4, anim), (10, 1-anim)):
        rect(a, x, 11, 3, 4, K); rect(a, x, 12, 2, 2, D)
        rect(a, x-1, 14-dy, 3, 1, col)
    panel(a, [(4, 6), (11, 6), (12, 10), (9, 13), (6, 13), (3, 10)], col)
    line(a, 5, 7, 7, 9, LG); line(a, 10, 7, 8, 9, D)
    rect(a, 7, 10, 2, 2, DB)
    panel(a, [(5, 2), (10, 2), (11, 5), (8, 7), (4, 5)], D)
    line(a, 6, 4, 9, 4, C)
    if kind == 0:
        line(a, 5, 2, 3, 0, col); line(a, 10, 2, 12, 0, col)
        for x in (2, 12):
            rect(a, x, 7, 2, 4, DB)
        line(a, 0, 4+anim, 2, 10, LG); line(a, 15, 4+anim, 13, 10, LG)
        px(a, 0, 4+anim, C); px(a, 15, 4+anim, C)
    elif kind == 2:
        poly(a, [(4, 4), (1, 0), (2, 5), (5, 6)], Y)
        poly(a, [(11, 4), (14, 0), (13, 5), (10, 6)], O)
        for x in (0, 12):
            panel(a, [(x, 7), (x+3, 7), (x+3, 11+anim), (x, 12+anim)], col)
            px(a, x+1, 8, LG)
    else:
        rect(a, 6, 1, 4, 1, R)
        for x in (0, 12):
            panel(a, [(x, 7+anim), (x+3, 7+anim), (x+3, 11), (x, 11)], D)
            line(a, x+1, 8+anim, x+2, 8+anim, LG)
            px(a, x+1, 10, O if anim else K)
    return a


def boss(kind=0, anim=0):
    """Author three distinct 48x24 adversaries: winged, carapaced, and siege armor."""
    a = blank(48, 24); col = [M, O, R][kind]
    for side in (-1, 1):
        def p(x, y):
            return (23-x if side < 0 else 24+x, y)
        if kind == 0:
            panel(a, [p(6, 8), p(18, 2), p(23, 3), p(20, 10), p(11, 16)], DB)
            poly(a, [p(9, 8), p(20, 4), p(16, 10), p(10, 12)], col)
            line(a, *p(11, 7), *p(20, 3), LG)
        elif kind == 1:
            panel(a, [p(6, 5), p(15, 3), p(21, 8), p(22, 16), p(12, 18)], col)
            line(a, *p(11, 5), *p(17, 7), Y)
            for offset in (0, 4):
                line(a, *p(16, 12+offset), *p(22, 16+offset), S)
                line(a, *p(22, 16+offset), *p(23, 19+offset), LG)
        else:
            panel(a, [p(8, 5), p(17, 1), p(21, 4), p(20, 13), p(10, 16)], DB)
            rect(a, *p(18 if side < 0 else 12, 3), 6, 3, S)
            line(a, *p(12, 4), *p(17, 4), LG)
            panel(a, [p(12, 9), p(22, 8), p(23, 15), p(14, 17)], col)
            line(a, *p(16, 11), *p(20, 11), O if anim else D)
        panel(a, [p(3, 16), p(9, 15), p(11, 22), p(3, 23)], DB)
        line(a, *p(5, 18), *p(7, 18), LG)
        line(a, *p(4, 22), *p(9, 22), col)
    panel(a, [(16, 7), (22, 5), (28, 6), (32, 10), (30, 17), (24, 20), (17, 17), (14, 11)], col)
    line(a, 17, 8, 20, 7, LG); line(a, 17, 15, 21, 18, D)
    line(a, 29, 9, 30, 14, DR)
    panel(a, [(20, 10), (27, 10), (28, 14), (24, 17), (19, 14)], DB)
    poly(a, [(22, 11), (25, 11), (26, 13), (24, 15), (21, 13)], C if anim else O)
    px(a, 23, 12, LG if anim else Y)
    panel(a, [(19, 2), (23, 0), (28, 2), (29, 6), (25, 8), (20, 6)], DB)
    line(a, 20, 4, 22, 4, C); line(a, 25, 4, 27, 4, C)
    poly(a, [(22, 6), (25, 6), (24, 8)], S)
    if kind == 1:
        line(a, 20, 2, 16, 0, Y); line(a, 27, 2, 31, 0, Y)
    return a


def support_frame(kind):
    """Original 32x24 support craft; four old docking slots keep storage constant."""
    a = blank(32, 24)
    for side in (-1, 1):
        def p(x, y):
            return (15 + side*x if side < 0 else 16 + side*x, y)
        # Double: swept red wings; Marine: blue fins; Drill: twin gold augers.
        color = (R, B, Y, R)[kind]
        panel(a, [p(2, 9), p(14, 3 if kind == 0 else 8), p(15, 17), p(5, 21)], color)
        line(a, *p(5, 12), *p(13, 10), LG)
        panel(a, [p(8, 7), p(11, 7), p(12, 21), p(7, 21)], DB)
        if kind >= 2:
            panel(a, [p(9, 0), p(13, 12), p(5, 12)], Y)
            for y in (6, 9): line(a, *p(7, y), *p(11, y), S)
        if kind in (1, 3):
            panel(a, [p(3, 13), p(15, 18), p(9, 23), p(2, 20)], B)
        rect(a, *p(9, 19), 2, 3, C)
    panel(a, [(15, 2), (19, 10), (19, 20), (12, 20), (12, 10)], LG)
    rect(a, 14, 7, 3, 5, C)
    return a


assets=[]
for i in range(2): assets.append((f'grendizer_{i}',robot_frame(variant=i&1,pose=i)))
assets.append(('grendizer_spazer',robot_frame(spazer=True,pose=1)))
solo = blank(32, 24)
flight_shell(solo)
panel(solo, [(12, 5), (19, 5), (22, 14), (9, 14)], B)
rect(solo, 14, 7, 4, 4, C)
assets.append(('solo_spazer', solo))
for i, name in enumerate(('double_spazer', 'marine_spazer', 'drill_spazer', 'all_spazers')):
    assets.append((name, support_frame(i)))
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
# Store images are promotional compositions, never represented as gameplay captures.
icon=Image.new('RGB',(128,128),(3,8,24))
di=ImageDraw.Draw(icon)
di.ellipse((7,7,120,120),fill=(12,27,48),outline=(45,99,125),width=2)
di.line((8,118,118,8),fill=(27,57,78),width=2)
paste_sprite(icon,robot_frame(pose=1),4,4,5)
icon.save('store/icon.png')

splash=Image.new('RGB',(320,200),(2,5,18)); ds=ImageDraw.Draw(splash)
for x,y in [(15,20),(44,58),(285,31),(252,78),(130,18),(210,45),(307,131),(197,117),(18,162)]:
    ds.point((x,y),fill=(180,220,255))
ds.ellipse((219,48,352,181),fill=(8,25,52),outline=(31,73,105))
# Scale the default pixel lettering with nearest-neighbor sampling for a clear title.
lettering=Image.new('RGBA',(110,14),(0,0,0,0))
ImageDraw.Draw(lettering).text((0,0),'VEGA ASSAULT',fill=(255,210,85,255))
splash.paste(lettering.resize((220,28),Image.Resampling.NEAREST),(77,12),lettering.resize((220,28),Image.Resampling.NEAREST))
ds.text((78,43),'ORIGINAL PIXEL ART / PRG32',fill=(133,183,207))
paste_sprite(splash,robot_frame(spazer=True,pose=1),16,61,4)
paste_sprite(splash,boss(0),191,66,2)
paste_sprite(splash,enemy_robot(1),210,132,2)
paste_sprite(splash,saucer(anim=1),267,131,2)
ds.line((16,174,303,174),fill=(45,99,125))
ds.text((17,181),'UNOFFICIAL EDUCATIONAL FAN GAME',fill=(188,210,225))
splash.save('store/splash.png')
print('assets',len(assets),'indexed_sprite_bytes',sum(len(a)*len(a[0]) for _,a in assets),'palette_bytes',len(palette)*2,'decode_buffer_bytes',48*24*2)
