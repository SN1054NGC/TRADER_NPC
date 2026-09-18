
const fs = require('fs'), path = require('path');
const MODS = {
  'НАШ @sps_client_bot': 'D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot',
  '@sps_zmb_01': 'D:/steam/steamapps/common/DayZServer/@sps_zmb_01/addons/sps_zmb_01',
  '@sps_item': 'D:/steam/steamapps/common/DayZServer/@sps_item/addons/sps_item',
  '@sps_server_bot': 'D:/steam/steamapps/common/DayZServer/@sps_server_bot/addons',
};
function walk(root, ext) {
  const out = [];
  (function w(d) {
    let es; try { es = fs.readdirSync(d, { withFileTypes: true }); } catch (e) { return; }
    for (const e of es) {
      const p = d + '/' + e.name;
      if (e.isDirectory()) w(p);
      else if (ext.test(e.name) && !/\.bak$/i.test(e.name)) out.push(p);
    }
  })(root);
  return out;
}
function info(root) {
  const res = { files: [], modded: new Map(), cfgClasses: new Map(), strKeys: new Map(), layouts: [], scripts: [] };
  for (const f of walk(root, /\.(c|cpp|csv|layout)$/i)) {
    const rel = f.slice(root.length + 1).toLowerCase();
    res.files.push(rel);
    const txt = fs.readFileSync(f, 'utf8');
    if (/\.c$/i.test(f)) {
      res.scripts.push(rel);
      for (const m of txt.matchAll(/^\s*modded\s+class\s+([A-Za-z_]\w*)/gm)) if (!res.modded.has(m[1])) res.modded.set(m[1], rel);
    }
    if (/\.layout$/i.test(f)) res.layouts.push(rel);
    if (/\.cpp$/i.test(f)) {
      for (const m of txt.matchAll(/^\s*class\s+([A-Za-z_]\w*)/gm)) { const n = m[1]; if (!res.cfgClasses.has(n)) res.cfgClasses.set(n, rel); }
    }
    if (/stringtable\.csv$/i.test(f)) {
      for (const line of txt.split(/\r?\n/)) { const k = (line.match(/^"([^"]+)"/) || [])[1]; if (k && k.toLowerCase() !== 'language') if (!res.strKeys.has(k.toLowerCase())) res.strKeys.set(k.toLowerCase(), rel); }
    }
  }
  return res;
}
const infos = {};
for (const [n, p] of Object.entries(MODS)) { infos[n] = info(p); console.log(n + ': скриптов ' + infos[n].scripts.length + ', modded class ' + infos[n].modded.size + ', конфиг-классов ' + infos[n].cfgClasses.size + ', layout ' + infos[n].layouts.length + ', строк локализации ' + infos[n].strKeys.size); }
const ours = infos['НАШ @sps_client_bot'];
console.log('\n================ КОНФЛИКТЫ С НАШИМ МОДОМ ================');
for (const name of Object.keys(MODS)) {
  if (name.startsWith('НАШ')) continue;
  const o = infos[name];
  const out = [];
  const sameScript = ours.scripts.filter(f => o.scripts.includes(f));
  if (sameScript.length) out.push('  !!! ОДИНАКОВЫЕ ПУТИ СКРИПТОВ (' + sameScript.length + '): ' + sameScript.join(', '));
  const sameLayout = ours.layouts.filter(f => o.layouts.includes(f));
  if (sameLayout.length) out.push('  !!! ОДИНАКОВЫЕ layout: ' + sameLayout.join(', '));
  const sameModded = [...ours.modded.keys()].filter(k => o.modded.has(k));
  out.push('  modded class на тех же классах (' + sameModded.length + '): ' + (sameModded.join(', ') || 'нет'));
  const sameCfg = [...ours.cfgClasses.keys()].filter(k => o.cfgClasses.has(k));
  out.push('  конфиг-классы с теми же именами (' + sameCfg.length + '): ' + (sameCfg.join(', ') || 'нет'));
  const sameStr = [...ours.strKeys.keys()].filter(k => o.strKeys.has(k));
  out.push('  ключи локализации с теми же именами (' + sameStr.length + '): ' + (sameStr.slice(0, 20).join(', ') || 'нет'));
  console.log('\n' + name + ':');
  console.log(out.join('\n'));
  if (sameCfg.length) for (const k of sameCfg.slice(0, 10)) console.log('      ' + k + ': у нас ' + ours.cfgClasses.get(k) + ' | у них ' + o.cfgClasses.get(k));
}
// конфиг-классы всех модов между собой (не только с нашим)
console.log('\n================ ПЕРЕСЕЧЕНИЯ МЕЖДУ ТВОИМИ МОДАМИ ================');
const names = Object.keys(MODS);
for (let i = 0; i < names.length; i++) for (let j = i + 1; j < names.length; j++) {
  const a = infos[names[i]], b = infos[names[j]];
  const s = [...a.cfgClasses.keys()].filter(k => b.cfgClasses.has(k));
  const sc = a.scripts.filter(f => b.scripts.includes(f));
  if (s.length || sc.length) console.log(names[i] + ' <-> ' + names[j] + ': общих конфиг-классов ' + s.length + ' (' + s.slice(0, 12).join(', ') + '), общих путей скриптов ' + sc.length + (sc.length ? ' (' + sc.join(', ') + ')' : ''));
}
