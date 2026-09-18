
// Строгое трёхстороннее сравнение: НАШЕ / UPSTREAM (Dr-Jones) / ВАНИЛЬ (установленный DayZ 1.29)
const fs = require('fs'), path = require('path');
const OURS   = 'D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot';
const THEIRS = 'D:/DAYZDISKP/_diag/upstream';
const VAN    = ['D:/steam/steamapps/common/DayZServer/dta/scripts',
                'D:/steam/steamapps/common/DayZ/dta/scripts'];

function walk(root, ext) {
  const out = [];
  (function w(d) {
    let es; try { es = fs.readdirSync(d, { withFileTypes: true }); } catch (e) { return; }
    for (const e of es) {
      const p = d + '/' + e.name;
      if (e.isDirectory()) { if (e.name === '.git') continue; w(p); }
      else if (!ext || ext.test(e.name)) out.push(p);
    }
  })(root);
  return out;
}
const isComment = s => s.startsWith('//') || s.startsWith('/*') || s.startsWith('*') || s.startsWith('*/');
const isBrace   = s => /^[{}();,]+$/.test(s);
function lines(f) {                       // значимые строки кода
  return fs.readFileSync(f, 'utf8').split(/\r?\n/).map(s => s.trim())
           .filter(s => s.length > 10 && !isComment(s) && !isBrace(s));
}
function allLines(f) { return fs.readFileSync(f, 'utf8').split(/\r?\n/).map(s => s.trim()).filter(s => s.length); }
const rel = (f, base, marker) => { const i = f.toLowerCase().indexOf(marker); return i < 0 ? path.basename(f).toLowerCase() : f.slice(i + marker.length).toLowerCase(); };
const base = f => path.basename(f).toLowerCase();
const pct  = (n, d) => d ? (Math.round(n * 1000 / d) / 10) + '%' : '-';
const L = []; const out = s => { L.push(s); console.log(s); };

// ---------- индексы ----------
const vanFiles = []; for (const v of VAN) vanFiles.push(...walk(v, /\.c$/));
const vanSet = new Set(), vanSets = [], vanByBase = new Map(), vanByRel = new Map();
{
  const per = VAN.map(v => { const s = new Set(); for (const f of walk(v, /\.c$/)) for (const l of lines(f)) s.add(l); return s; });
  vanSets.push(...per);
  for (const s of per) for (const l of s) vanSet.add(l);
  for (const f of vanFiles) {
    const b = base(f); if (!vanByBase.has(b)) vanByBase.set(b, []); vanByBase.get(b).push(f);
    const r = rel(f, f, 'scripts/'); if (!vanByRel.has(r)) vanByRel.set(r, []); vanByRel.get(r).push(f);
  }
}
const ourC   = walk(OURS, /\.c$/),  ourLay = walk(OURS, /\.layout$/);
const theC   = walk(THEIRS + '/scripts', /\.c$/), theLay = walk(THEIRS, /\.layout$/);
const ourSet = new Set(), theSet = new Set();
for (const f of ourC)  for (const l of lines(f)) ourSet.add(l);
for (const f of theC)  for (const l of lines(f)) theSet.add(l);

out('ДЕРЕВЬЯ');
out('  ваниль .c: ' + vanFiles.length + ' (' + vanSet.size + ' значимых строк, DayZ 1.29 server+client)');
out('  upstream .c: ' + theC.length + ' (' + theSet.size + ' строк)   upstream .layout: ' + theLay.length);
out('  наши .c: ' + ourC.length + ' (' + ourSet.size + ' строк)   наши .layout: ' + ourLay.length);
out('');

// ---------- ЧАСТЬ 1: НАШЕ против ВАНИЛИ ----------
out('=== ЧАСТЬ 1. НАШ ПРОЕКТ против ВАНИЛИ ===');
let oTot = 0, oVan = 0, oVanStrict = 0;
for (const f of ourC) for (const l of lines(f)) { oTot++; if (vanSet.has(l)) { oVan++; if (vanSets.every(s => s.has(l))) oVanStrict++; } }
out('  наши значимые строки .c: ' + oTot);
out('  есть в ванили (хоть в одном дереве): ' + oVan + '  ' + pct(oVan, oTot));
out('  есть в обоих деревьях ванили (server И client): ' + oVanStrict + '  ' + pct(oVanStrict, oTot));
out('  наших собственных (нет в ванили): ' + (oTot - oVan) + '  ' + pct(oTot - oVan, oTot));

const rows = [];
for (const f of ourC) {
  const b = base(f), r = rel(f, f, 'scripts/');
  const cand = vanByRel.get(r) || vanByBase.get(b) || [];
  const mine = lines(f);
  let hit = 0, tot = mine.length;
  if (cand.length) { const cs = new Set(); for (const c of cand) for (const l of lines(c)) cs.add(l); for (const l of mine) if (cs.has(l)) hit++; }
  const share = tot ? hit / tot : 0;
  let kind;
  if (!cand.length) kind = 'NEW (нет ванильного файла)';
  else if (share >= 0.95) kind = 'КОПИЯ ванили';
  else if (share >= 0.5)  kind = 'копия ванили + правки';
  else if (share >= 0.15) kind = 'сильно переписан';
  else kind = 'почти полностью наше';
  rows.push({ r, tot, hit, share, kind, samePath: !!(vanByRel.get(r) || []).length });
}
const byKind = {};
for (const x of rows) { byKind[x.kind] = byKind[x.kind] || { n: 0, lines: 0 }; byKind[x.kind].n++; byKind[x.kind].lines += x.tot; }
out('  --- по файлам (' + rows.length + ') ---');
for (const k of Object.keys(byKind).sort((a, b) => byKind[b].n - byKind[a].n)) out('    ' + k + ': файлов ' + byKind[k].n + ', строк ' + byKind[k].lines);
const twins = rows.filter(x => x.hit > 0 && x.share >= 0.15).sort((a, b) => b.share - a.share);
out('  --- файлы, реально происходящие от ванильных (' + twins.length + ') ---');
for (const x of twins.slice(0, 25)) out('    ' + pct(x.hit, x.tot).padStart(6) + '  ' + x.r + (x.samePath ? '' : '   [путь в ванили другой]'));
out('');

// ---------- ЧАСТЬ 2: UPSTREAM против ВАНИЛИ ----------
out('=== ЧАСТЬ 2. ПРОЕКТ Dr-Jones (upstream) против ВАНИЛИ ===');
let tTot = 0, tVan = 0;
for (const f of theC) for (const l of lines(f)) { tTot++; if (vanSet.has(l)) tVan++; }
out('  значимые строки .c: ' + tTot);
out('  ванильные: ' + tVan + '  ' + pct(tVan, tTot));
out('  собственная работа автора: ' + (tTot - tVan) + '  ' + pct(tTot - tVan, tTot));
const uRows = [];
for (const f of theC) {
  const b = base(f), r = rel(f, f, 'scripts/');
  const cand = vanByRel.get(r) || vanByBase.get(b) || [];
  const mine = lines(f); let hit = 0;
  if (cand.length) { const cs = new Set(); for (const c of cand) for (const l of lines(c)) cs.add(l); for (const l of mine) if (cs.has(l)) hit++; }
  uRows.push({ r, tot: mine.length, hit, share: mine.length ? hit / mine.length : 0, kind: cand.length ? (hit / (mine.length || 1) >= 0.95 ? 'КОПИЯ ванили' : hit / (mine.length || 1) >= 0.5 ? 'копия + правки' : hit / (mine.length || 1) >= 0.15 ? 'сильно переписан' : 'почти полностью своё') : 'NEW (нет ванильного файла)' });
}
const uKind = {};
for (const x of uRows) { uKind[x.kind] = uKind[x.kind] || { n: 0, lines: 0 }; uKind[x.kind].n++; uKind[x.kind].lines += x.tot; }
for (const k of Object.keys(uKind).sort((a, b) => uKind[b].n - uKind[a].n)) out('    ' + k + ': файлов ' + uKind[k].n + ', строк ' + uKind[k].lines);
const uTwins = uRows.filter(x => x.hit > 0 && x.share >= 0.15).sort((a, b) => b.share - a.share);
out('  --- файлы автора, происходящие от ванильных (' + uTwins.length + ') ---');
for (const x of uTwins.slice(0, 20)) out('    ' + pct(x.hit, x.tot).padStart(6) + '  ' + x.r);
out('');

// ---------- ЧАСТЬ 3: НАШЕ против UPSTREAM, БЕЗ ВАНИЛИ ----------
out('=== ЧАСТЬ 3. НАШЕ против UPSTREAM БЕЗ ВАНИЛИ (только неванильный код) ===');
const ourNonVan = new Set([...ourSet].filter(l => !vanSet.has(l)));
const theNonVan = new Set([...theSet].filter(l => !vanSet.has(l)));
out('  наш неванильный код: ' + ourNonVan.size + ' строк;  неванильный код автора: ' + theNonVan.size + ' строк');
let kept = 0, oursOwn = 0;
for (const l of ourNonVan) { if (theNonVan.has(l)) kept++; else oursOwn++; }
out('  из нашего неванильного: заимствовано у автора ' + kept + '  ' + pct(kept, ourNonVan.size));
out('  из нашего неванильного: написано нами ' + oursOwn + '  ' + pct(oursOwn, ourNonVan.size));
let dropped = 0, keptA = 0;
for (const l of theNonVan) { if (ourNonVan.has(l)) keptA++; else dropped++; }
out('  из авторского неванильного: сохранено нами ' + keptA + '  ' + pct(keptA, theNonVan.size));
out('  из авторского неванильного: выброшено/переписано ' + dropped + '  ' + pct(dropped, theNonVan.size));
out('  (контроль: kept ' + kept + ' == keptA ' + keptA + ')');
out('');
out('  --- файлы автора: что с ними стало (по basename) ---');
const ourByBase = new Map(); for (const f of ourC) ourByBase.set(base(f), f);
const ourByRel = new Map(); for (const f of ourC) ourByRel.set(rel(f, f, 'scripts/'), f);
const stat = { same: [], modified: [], dropped: [] };
for (const f of theC) {
  const b = base(f), r = rel(f, f, 'scripts/');
  const minePath = ourByRel.get(r) || ourByBase.get(b);
  if (!minePath) { stat.dropped.push({ r, tot: lines(f).length }); continue; }
  const a = new Set(lines(f)), c = new Set(lines(minePath));
  let inter = 0; for (const l of a) if (c.has(l)) inter++;
  (inter / (a.size || 1) >= 0.9 ? stat.same : stat.modified).push({ r, tot: a.size, inter, share: inter / (a.size || 1) });
}
out('    сохранено без изменений/почти: ' + stat.same.length + ' файлов');
out('    сохранено с правками: ' + stat.modified.length + ' файлов');
out('    отсутствует у нас: ' + stat.dropped.length + ' файлов (' + stat.dropped.reduce((s, x) => s + x.tot, 0) + ' авторских строк)');
out('    --- переписанные сильнее всего ---');
for (const x of stat.modified.sort((a, b) => a.share - b.share).slice(0, 12)) out('      ' + pct(x.inter, x.tot).padStart(6) + ' авторских строк сохранено  ' + x.r);
out('    --- отсутствуют у нас ---');
for (const x of stat.dropped.sort((a, b) => b.tot - a.tot).slice(0, 15)) out('      ' + String(x.tot).padStart(5) + ' строк  ' + x.r);
out('');
out('  --- наши .c файлы, которых нет у автора ---');
const added = ourC.filter(f => { const b = base(f), r = rel(f, f, 'scripts/'); return !theC.some(t => base(t) === b || rel(t, t, 'scripts/') === r); });
for (const f of added) out('    ' + String(lines(f).length).padStart(5) + ' строк  ' + rel(f, f, 'scripts/'));
out('');
out('  --- layout ---');
const theLayNames = theLay.map(f => base(f));
let lo = 0, lsame = 0;
for (const f of ourLay) { lo++; if (theLayNames.includes(base(f))) lsame++; }
out('    наших layout: ' + ourLay.length + ', из них с тем же именем у автора: ' + lsame + ' (файлов автора ' + theLay.length + ')');

fs.writeFileSync('D:/DAYZDISKP/@sps_client_bot/_build/log/compare3.md', L.join('\n'), 'utf8');
console.log('\n[report] _build/log/compare3.md');
