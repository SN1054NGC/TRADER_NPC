
const fs = require('fs');
function strings(f, re) {
  const s = fs.readFileSync(f).toString('latin1');
  const out = new Set();
  for (const m of s.matchAll(re)) out.add(m[0]);
  return [...out];
}
const HPP = 'D:/steam/steamapps/common/DayZ/Addons/sounds_hpp.pbo';
const CHR = 'D:/steam/steamapps/common/DayZ/Addons/sounds_characters.pbo';
console.log('=== sounds_hpp.pbo: имена наборов/шейдеров (paper, pickUp, putDown, pickup) ===');
console.log(strings(HPP, /[A-Za-z][A-Za-z0-9_]{3,60}(SoundSet|Soundshader|soundSet|soundShader)/g).filter(x => /paper|pickup|pick|put|drop|inventory|hand/i.test(x)).sort().join('\n'));
console.log('\n=== sounds_hpp.pbo: все _SoundSet (первые 40) ===');
console.log(strings(HPP, /[A-Z][A-Za-z0-9_]{2,50}_SoundSet/g).sort().slice(0, 40).join('\n'));
console.log('\n=== sounds_characters.pbo: paper/coin-related ===');
console.log(strings(CHR, /[A-Za-z0-9_\\\\.]{4,80}paper[A-Za-z0-9_\\\\.]{0,40}/gi).sort().slice(0, 25).join('\n'));
console.log('\n=== sounds_characters.pbo: *SoundSet с inventory/pickup ===');
console.log(strings(CHR, /[A-Z][A-Za-z0-9_]{2,50}_SoundSet/g).filter(x => /pick|put|inventory|hand|item/i.test(x)).sort().slice(0, 30).join('\n'));
