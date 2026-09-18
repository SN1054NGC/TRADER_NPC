
const fs = require('fs'), path = require('path');
const MODS = {
  'РќРђРЁ': 'D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot',
  'zmb_01': 'D:/steam/steamapps/common/DayZServer/@sps_zmb_01/addons/sps_zmb_01',
  'server_bot': 'D:/steam/steamapps/common/DayZServer/@sps_server_bot/addons',
};
function walk(root, ext) { const o = []; (function w(d) { let es; try { es = fs.readdirSync(d, { withFileTypes: true }); } catch (e) { return; } for (const e of es) { const p = d + '/' + e.name; if (e.isDirectory()) w(p); else if (ext.test(e.name) && !/\.bak$/i.test(e.name)) o.push(p); } })(root); return o; }
function blocks(txt) {                        // modded class X { ... } -> РјРµС‚РѕРґС‹
  const out = [];
  const re = /modded\s+class\s+([A-Za-z_]\w*)[^{]*\{/g; let m;
  while ((m = re.exec(txt))) {
    let depth = 1, i = re.lastIndex;
    while (i < txt.length && depth > 0) { const c = txt[i]; if (c === '{') depth++; else if (c === '}') depth--; i++; }
    const body = txt.slice(re.lastIndex, i - 1);
    const methods = [];
    for (const mm of body.matchAll(/override\s+[\w:<>\[\]]+\s+(\w+)\s*\(/g)) methods.push({ n: mm[1], super: false });
    for (const mm of body.matchAll(/override\s+[\w:<>\[\]]+\s+(\w+)\s*\(([^)]*)\)\s*\{/g)) {
      let d2 = 1, j = mm.index + mm[0].length;
      while (j < body.length && d2 > 0) { const c = body[j]; if (c === '{') d2++; else if (c === '}') d2--; j++; }
      const fnBody = body.slice(mm.index, j);
      const target = methods.find(x => x.n === mm[1]);
      if (target && /super\s*\./.test(fnBody)) target.super = true;
    }
    out.push({ cls: m[1], methods });
  }
  return out;
}
const data = {};
for (const [n, p] of Object.entries(MODS)) {
  data[n] = new Map();
  for (const f of walk(p, /\.c$/i)) for (const b of blocks(fs.readFileSync(f, 'utf8'))) {
    if (!data[n].has(b.cls)) data[n].set(b.cls, []);
    for (const mth of b.methods) data[n].get(b.cls).push({ ...mth, file: f.slice(p.length + 1) });
  }
}
const ours = data['РќРђРЁ'];
for (const other of ['zmb_01', 'server_bot']) {
  const th = data[other];
  const shared = [...ours.keys()].filter(k => th.has(k));
  console.log('=== РќРђРЁ vs ' + other + ': РѕР±С‰РёС… modded-РєР»Р°СЃСЃРѕРІ ' + shared.length + ' (' + shared.join(', ') + ') ===');
  for (const cls of shared) {
    const a = ours.get(cls), b = th.get(cls);
    const am = a.map(x => x.n), bm = b.map(x => x.n);
    const both = am.filter(x => bm.includes(x));
    console.log('  РєР»Р°СЃСЃ ' + cls + ':');
    console.log('    РЅР°С€Рё РјРµС‚РѕРґС‹: ' + [...new Set(am)].join(', '));
    console.log('    РёС… РјРµС‚РѕРґС‹:   ' + [...new Set(bm)].join(', '));
    if (both.length) {
      for (const mn of [...new Set(both)]) {
        const ma = a.find(x => x.n === mn), mb = b.find(x => x.n === mn);
        console.log('    !!! РћР‘Рђ РїРµСЂРµРѕРїСЂРµРґРµР»СЏСЋС‚ ' + mn + '  | РЅР°С€ super=' + (ma.super ? 'РґР°' : 'РќР•Рў') + ' (' + ma.file + ')  | РёС… super=' + (mb.super ? 'РґР°' : 'РќР•Рў') + ' (' + mb.file + ')');
      }
    } else console.log('    РїРµСЂРµСЃРµС‡РµРЅРёР№ РїРѕ РјРµС‚РѕРґР°Рј РЅРµС‚');
  }
  console.log('');
}

