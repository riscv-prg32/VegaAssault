#!/usr/bin/env python3
import argparse, json, hashlib, shutil, zipfile
from pathlib import Path
p=argparse.ArgumentParser()
p.add_argument('--esp32c6')
p.add_argument('--qemu')
p.add_argument('--version',default='1.0.0')
p.add_argument('--out',default='dist/grendizer-vega-assault-86-store-1.0.0.zip')
a=p.parse_args()
root=Path(__file__).resolve().parents[1]
manifest=json.loads((root/'store/manifest.template.json').read_text())
manifest['version']=a.version
arch=[]; files=[]
for aid,src in [('esp32c6',a.esp32c6),('qemu',a.qemu)]:
    if src:
        s=Path(src)
        if not s.is_file(): raise SystemExit(f'missing {aid} cartridge: {s}')
        if s.stat().st_size>131072: raise SystemExit(f'{aid} exceeds 128 KiB: {s.stat().st_size} bytes')
        name=f"grendizer-vega-assault-86-{aid}.prg32"
        arch.append({'id':aid,'file':name}); files.append((s,name))
if not arch: raise SystemExit('provide at least --esp32c6 or --qemu with a real .prg32 artifact')
manifest['architectures']=arch
out=root/a.out; out.parent.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED) as z:
    z.writestr('manifest.json',json.dumps(manifest,indent=2,ensure_ascii=False)+'\n')
    z.write(root/'store/icon.png','icon.png')
    z.write(root/'store/splash.png','splash.png')
    for src,name in files:z.write(src,name)
sha=hashlib.sha256(out.read_bytes()).hexdigest()
(out.with_suffix(out.suffix+'.sha256')).write_text(f'{sha}  {out.name}\n')
print(out); print('sha256',sha)
