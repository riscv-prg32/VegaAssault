#!/usr/bin/env python3
import json,struct,sys
from pathlib import Path
H=struct.Struct("<4sHHHHHHIIIIII"); S=struct.Struct("<IIIIHBB"); I=struct.Struct("<HBBBBBB"); T=struct.Struct("<II"); E=struct.Struct("<BBBB")
def a4(b):
 while len(b)%4:b.append(0)
def main():
 p=Path(sys.argv[1]); out=Path(sys.argv[2]); c=json.loads(p.read_text()); data=bytearray(); sd=bytearray()
 for s in c.get("samples",[]):
  r=(p.parent/s["file"]).read_bytes(); o=len(data); data.extend(r); f=1 if s.get("loop",False) else int(s.get("flags",0)); le=int(s.get("loop_end",len(r) if f else 0)); sd.extend(S.pack(o,len(r),int(s.get("loop_start",0)),le,int(s.get("base_note",60)),f,0))
 ids=bytearray()
 for i in c.get("instruments",[]): ids.extend(I.pack(int(i.get("sample_id",0)),int(i.get("default_volume",255)),int(i.get("default_pan",0))&255,int(i.get("attack",0)),int(i.get("decay",0)),int(i.get("sustain",255)),int(i.get("release",0))))
 td=bytearray(); ev=bytearray(); cmds={"NOTE_ON":1,"NOTE_OFF":2,"SET_VOLUME":3,"SET_PAN":4,"SET_TEMPO":5,"PLAY_SAMPLE":6,"JUMP":7,"END":255}
 for tr in c.get("tracks",[]):
  o=len(ev)//4; es=tr.get("events",[])
  for e in es: ev.extend(E.pack(int(e.get("delta",e.get("delta_ticks",0)))&255, cmds.get(str(e.get("command","END")).upper(),int(e.get("command",255)) if isinstance(e.get("command"),int) else 255)&255,int(e.get("arg0",0))&255,int(e.get("arg1",0))&255))
  td.extend(T.pack(o,len(es)))
 b=bytearray(b"\0"*H.size); so=len(b);b.extend(sd);a4(b);io=len(b);b.extend(ids);a4(b);to=len(b);b.extend(td);a4(b);eo=len(b);b.extend(ev);a4(b);do=len(b);b.extend(data);a4(b);b[:H.size]=H.pack(b"AUD0",1,H.size,len(c.get("samples",[])),len(c.get("instruments",[])),len(c.get("tracks",[])),0,so,io,to,eo,do,len(b));out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(b);print(len(b))
if __name__=="__main__": main()
