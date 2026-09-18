
const fs = require('fs');
const HPP = 'D:/steam/steamapps/common/DayZ/Addons/sounds_hpp.pbo';
const CHR = 'D:/steam/steamapps/common/DayZ/Addons/sounds_characters.pbo';
const SCR = 'D:/steam/steamapps/common/DayZ/dta/scripts.pbo';
function names(f, re) { const s = fs.readFileSync(f).toString('latin1'); return [...new Set([...s.matchAll(re)].map(m => m[0]))]; }
console.log('=== sounds_hpp.pbo: всё с "Paper" ===');
console.log(names(HPP, /[A-Za-z0-9_]{2,70}[Pp]aper[A-Za-z0-9_]{0,60}/g).sort().join('\n'));
console.log('\n=== sounds_hpp.pbo: putDown/putaway/pickUp (уникальные, без Bodyfall) ===');
console.log(names(HPP, /[A-Za-z0-9_]{4,60}(putDown|putdown|pickUp|pickup)[A-Za-z0-9_]{0,40}/g).filter(x => !/bodyfall|hand_/i.test(x)).sort().slice(0, 40).join('\n'));
console.log('\n=== sounds_characters.pbo: ogg с paper ===');
console.log(names(CHR, /[A-Za-z0-9_\\\\.-]{3,90}\.ogg/g).filter(x => /paper/i.test(x)).sort().join('\n'));
console.log('\n=== scripts.pbo: строки со Paper или SoundSet для денег ===');
const s = fs.readFileSync(SCR).toString('latin1');
console.log([...new Set([...s.matchAll(/[A-Za-z0-9_\\\\]{2,80}[Pp]aper[A-Za-z0-9_\\\\.]{0,40}/g)].map(m => m[0]))].sort().slice(0, 30).join('\n'));
console.log('\n=== есть ли где-то MoneyRuble звук в ваниле/предметах ===');
console.log(names('D:/steam/steamapps/common/DayZ/Addons/characters_items.pbo', /[A-Za-z0-9_]{2,60}(Money|money|Coin|coin)[A-Za-z0-9_]{0,40}/g).sort().slice(0, 20).join('\n') || '(нет совпадений)');
