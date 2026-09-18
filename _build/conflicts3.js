
const fs = require('fs');
const MODS = {
  'OURS': 'D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot',
  'zmb_01': 'D:/steam/steamapps/common/DayZServer/@sps_zmb_01/addons/sps_zmb_01',
  'server_bot': 'D:/steam/steamapps/common/DayZServer/@sps_server_bot/addons',
};
function walk(root, ext) { const o = []; (function w(d) { let es; try { es = fs.readdirSync(d, { withFileTypes: true }); } catch (e) { return; } for (const e of es) { const p = d + '/' + e.name; if (e.isDirectory()) w(p); else if (ext.test(e.name) && !/\.bak$/i.test(e.name)) o.push(p); } })(root); return o; }
function blocks(txt) {
  const out = []; const re = /modded\s+class\s+([A-Za-z_]\w*)[^{]*\{/g; let m;
  while ((m = re.exec(txt))) {
    let depth = 1, i = re.lastIndex;
    while (i < txt.length && depth > 0) { const c = txt[i]; if (c === '{') depth++; else if (c === '}') depth--; i++; }
    const body = txt.slice(re.lastIndex, i - 1);
    const methods = new Map();
    for (const mm of body.matchAll(/(?:^|\n)\s*(?:override\s+)?(?:static\s+)?[\w:<>\[\]]+\s+(\w+)\s*\(([^)]*)\)\s*\{/g)) {
      let d2 = 1, j = mm.index + mm[0].length;
      while (j < body.length && d2 > 0) { const c = body[j]; if (c === '{') d2++; else if (c === '}') d2--; j++; }
      const fn = body.slice(mm.index, j);
      if (!methods.has(mm[1])) methods.set(mm[1], { super: /super\s*\./.test(fn), override: /override/.test(mm[0]), nonEmpty: !/^\{\s*\}$/.test(fn.slice(fn.indexOf('{')).trim()) });
    }
    out.push({ cls: m[1], methods });
  }
  return out;
}
const data = {};
for (const [n, p] of Object.entries(MODS)) {
  data[n] = new Map();
  for (const f of walk(p, /\.c$/i)) for (const b of blocks(fs.readFileSync(f, 'utf8'))) {
    if (!data[n].has(b.cls)) data[n].set(b.cls, new Map());
    for (const [k, v] of b.methods) { if (!data[n].get(b.cls).has(k)) data[n].get(b.cls).set(k, { ...v, file: f.slice(p.length + 1) }); }
  }
}
const ours = data['OURS'];
console.log('OURS modded classes: ' + ours.size);
for (const other of ['zmb_01', 'server_bot']) {
  const th = data[other];
  const shared = [...ours.keys()].filter(k => th.has(k));
  console.log('\n=== OURS vs ' + other + ': shared modded classes: ' + shared.length + ' (' + shared.join(', ') + ') ===');
  for (const cls of shared) {
    const a = ours.get(cls), b = th.get(cls);
    const both = [...a.keys()].filter(k => b.has(k));
    console.log('  ' + cls + ': ours=[' + [...a.keys()].join(',') + ']  theirs=[' + [...b.keys()].join(',') + ']');
    for (const mn of both) {
      const ma = a.get(mn), mb = b.get(mn);
      console.log('    !! BOTH define ' + mn + ' :: ours super=' + (ma.super ? 'YES' : 'NO') + ' (' + ma.file + ') | theirs super=' + (mb.super ? 'YES' : 'NO') + ' (' + mb.file + ')');
    }
  }
}
