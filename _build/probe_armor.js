
const fs=require('fs');
const roots=['D:/DAYZDISKP/DZ','D:/DAYZDISKP/bin'];
const files=[];
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(roots[0]);
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(roots[1]);
let armorBlock='', foodBlock='', lastFile='';
for(const f of files){
  const t=fs.readFileSync(f,'utf8');
  if(!armorBlock){
    const i=t.indexOf('class GlobalArmor');
    if(i>=0){ armorBlock='### '+(lastFile=f)+'\n'+t.slice(i,i+700); }
  }
  if(!foodBlock){
    const i=t.indexOf('class Nutrition');
    if(i>=0){ foodBlock='### '+f+'\n'+t.slice(Math.max(0,i-400),i+900); }
  }
  if(armorBlock&&foodBlock) break;
}
console.log('=== пример GlobalArmor ===');
console.log(armorBlock.slice(0,900));
console.log('');
console.log('=== пример Nutrition / стадии еды ===');
console.log(foodBlock.slice(0,1200));
