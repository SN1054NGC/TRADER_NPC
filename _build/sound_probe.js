
const fs = require('fs');
const dirs = ['D:/steam/steamapps/common/DayZ/Addons', 'D:/steam/steamapps/common/DayZ/dta'];
const want = ['coin', 'money', 'cash', 'ruble', 'banknote', 'bank_note', 'papers', 'paper'];
const files = [];
for (const d of dirs) { let es = []; try { es = fs.readdirSync(d); } catch (e) { continue; }
  for (const n of es) { if (!/\.pbo$/i.test(n)) continue; if (!/sound|scripts|languagecore|core|gui/i.test(n)) continue; files.push(d + '/' + n); } }
console.log('сканирую PBO: ' + files.length);
for (const f of files) {
  let buf; try { buf = fs.readFileSync(f); } catch (e) { continue; }
  const found = new Map();
  const s = buf.toString('latin1').toLowerCase();
  for (const w of want) {
    let i = -1, n = 0; const samples = [];
    while ((i = s.indexOf(w, i + 1)) >= 0) {
      n++;
      if (samples.length < 4) {
        let a = Math.max(0, i - 28), b = Math.min(buf.length, i + 28);
        samples.push(buf.toString('latin1', a, b).replace(/[^\x20-\x7e]/g, '.'));
      }
      if (n > 400) break;
    }
    if (n) found.set(w, { n, samples });
  }
  if (found.size) {
    console.log('\n=== ' + f.split('/').pop() + ' (' + Math.round(buf.length / 1048576) + 'MB) ===');
    for (const [w, v] of found) { console.log('  ' + w + ': ' + v.n + ' совпадений'); for (const sm of v.samples.slice(0, 2)) console.log('      ...' + sm + '...'); }
  }
}
