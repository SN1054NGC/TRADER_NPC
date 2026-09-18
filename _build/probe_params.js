
const fs=require('fs');
const roots=['D:/DAYZDISKP/DZ','D:/DAYZDISKP/bin'];
const files=[];
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(roots[0]);
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(roots[1]);
const all=files.map(f=>fs.readFileSync(f,'utf8')).join('\n');
const keys=['GlobalArmor','armor =','armor=','weight =','weight=','itemSize','itemsCargoSize','heatIsolation','absorbency','visibilityModifier','Nutrition','energy =','water =','varQuantityMax','\\bcount =','healthLabels','DamageSystem','GlobalHealth','liquidType','foodStage','class Stage','MedicalItem','bloodRegeneration','shock','useAction'];
console.log('проверяю '+files.length+' config.cpp, '+(all.length/1024/1024).toFixed(1)+' МБ текста');
for(const k of keys){ const n=(all.match(new RegExp(k,'g'))||[]).length; console.log('  '+k.replace(/\\\\b/g,'').padEnd(22)+n); }
// примеры брони и тёплых курток
function sample(re,label,n=4){ const out=[]; let m; const r=new RegExp(re,'g'); while((m=r.exec(all))!==null && out.length<n){ out.push(m[0].replace(/\s+/g,' ').slice(0,90)); } console.log(label+': '+out.join(' | ')); }
sample('class GlobalArmor[\\s\\S]{0,400}?armor\\s*=\\s*[\\d.]+','armor');
sample('heatIsolation\\s*=\\s*[\\d.]+','heatIsolation');
sample('Nutrition[\\s\\S]{0,200}?energy\\s*=\\s*[\\d.]+','nutrition');
sample('weight\\s*=\\s*[\\d]+','weight');
sample('itemSize\\[\\]\\s*=\\s*\\{\\s*\\d+\\s*,\\s*\\d+','itemSize');
