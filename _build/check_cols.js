
const fs=require('fs');
const P='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/languagecore/stringtable.csv';
const lines=fs.readFileSync(P,'utf8').split('\n');
let bad=[], rows=0;
for(const l of lines){ if(!l.trim()) continue; rows++;
  // count fields properly: split on "," respecting the quoted format used here
  const f=l.split('","');
  if(f.length!==15) bad.push((l.match(/^"([^"]+)"/)||[])[1]+'='+f.length);
}
console.log('rows '+rows+'  wrong column count: '+(bad.length?bad.join(', '):'NONE (all 15)'));
const g=lines.filter(l=>/^"(tm_money|tm_trader|tm_cond_worn|tm_cargo_size)"/.test(l));
console.log(g.join('\n'));
