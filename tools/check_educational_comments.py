#!/usr/bin/env python3
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
failed=[]
sources = sorted(path for folder in ('src', 'tests') for path in (root/folder).rglob('*') if path.suffix in ('.c', '.h'))
for path in sources:
    rel = path.relative_to(root)
    lines=(root/rel).read_text().splitlines()
    for i,line in enumerate(lines):
        if not line.strip() or line.lstrip().startswith('// EDUCATIONAL:'):
            continue
        if i==0 or not lines[i-1].lstrip().startswith('// EDUCATIONAL:'):
            failed.append(f'{rel}:{i+1}')
if failed:
    print('Missing educational comments:', *failed, sep='\n  ')
    sys.exit(1)
print('Educational line-comment policy: PASS')
