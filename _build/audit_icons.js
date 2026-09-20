
const fs=require('fs');
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
const ADDON='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/';
const sets={};
for(const f of fs.readdirSync('D:/DAYZDISKP/gui/imagesets')){
  if(!f.endsWith('.imageset')) continue;
  const t=fs.readFileSync('D:/DAYZDISKP/gui/imagesets/'+f,'utf8');
  const names=new Set();
  for(const m of t.matchAll(/ImageSetDefClass\s+(\w+)\s*\{/g)) names.add(m[1]);
  sets[f.replace('.imageset','')]=names;
}
let icons=0, tex=0, bad=0;
for(const f of fs.readdirSync(L).filter(x=>x.endsWith('.layout'))){
  const t=fs.readFileSync(L+f,'utf8');
  for(const m of t.matchAll(/image0\s+"set:([\w]+) image:([\w]+)"/g)){
    const set=m[1], img=m[2]; icons++;
    if(!sets[set]||!sets[set].has(img)){ console.log('НЕТ ИКОНКИ '+set+'/'+img+'  ('+f+')'); bad++; }
  }
  for(const m of t.matchAll(/imageTexture\s+"([^"]+)"/g)){
    const p=m[1].replace(/^\{[^}]+\}/,'').replace(/\\/g,'/'); tex++;
    const cands=[ 'D:/DAYZDISKP/'+p, ADDON+p.replace(/^TRADER_NPC\//,''), 'D:/DAYZDISKP/gui/'+p.replace(/^gui\//,'') ];
    let ok=false; for(const c of cands){ if(fs.existsSync(c)){ ok=true; break; } }
    if(!ok){ console.log('НЕТ ТЕКСТУРЫ '+p+'  ('+f+')'); bad++; }
  }
}
console.log('ссылок на иконки imageset: '+icons+', текстур imageTexture: '+tex+', проблем: '+bad);
