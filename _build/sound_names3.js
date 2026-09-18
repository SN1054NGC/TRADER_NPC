
const fs = require('fs');
function names(f, re) { const s = fs.readFileSync(f).toString('latin1'); return [...new Set([...s.matchAll(re)].map(m => m[0]))]; }
const HPP = 'D:/steam/steamapps/common/DayZ/Addons/sounds_hpp.pbo';
console.log('=== ВСЕ имена с Paper в sounds_hpp.pbo ===');
console.log(names(HPP, /[A-Za-z0-9_]{0,70}[Pp]aper[A-Za-z0-9_]{0,60}/g).sort().join('\n'));
const CHR = 'D:/steam/steamapps/common/DayZ/Addons/sounds_characters.pbo';
console.log('\n=== sounds_characters.pbo: наборы со словом paper ===');
console.log(names(CHR, /[A-Za-z0-9_]{0,70}[Pp]aper[A-Za-z0-9_]{0,60}/g).sort().slice(0, 40).join('\n'));
