
const fs=require('fs'), path=require('path');
const ROOTS=['D:/DAYZDISKP/DZ','D:/DAYZDISKP/bin'];
const files=[];
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(ROOTS[0]);
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(ROOTS[1]);
console.log('config.cpp найдено: '+files.length);
const info={};
for(const f of files){
  const t=fs.readFileSync(f,'utf8');
  const re=/class\s+([A-Za-z0-9_]+)\s*(?::\s*[A-Za-z0-9_]+)?\s*\{/g; const marks=[]; let m;
  while((m=re.exec(t))!==null) marks.push({name:m[1],start:m.index});
  for(let i=0;i<marks.length;i++){
    const end=(i+1<marks.length)?marks[i+1].start:t.length;
    const body=t.slice(marks[i].start,end);
    const cal=/\bcaliber\s*=\s*([\d.]+)/.exec(body);
    const cnt=/\bcount\s*=\s*([\d]+)/.exec(body);
    const vqm=/\bvarQuantityMax\s*=\s*([\d]+)/.exec(body);
    const ics=/itemsCargoSize\[\]\s*=\s*\{\s*([\d]+)\s*,\s*([\d]+)/.exec(body);
    const chb=/chamberableFrom\[\]\s*=\s*\{([^}]*)\}/.exec(body);
    if(!(cal||cnt||vqm||ics||chb)) continue;
    const o=info[marks[i].name]||{};
    if(cal)o.cal=parseFloat(cal[1]); if(cnt)o.cnt=parseInt(cnt[1]); if(vqm)o.vqm=parseInt(vqm[1]);
    if(ics)o.ics=[parseInt(ics[1]),parseInt(ics[2])];
    if(chb)o.chb=chb[1].split(',').map(s=>s.trim().replace(/"/g,'')).filter(s=>s&&s!=='NULL');
    info[marks[i].name]=o;
  }
}
const cal={}; for(const k in info) if(info[k].cal!==null&&info[k].cal!==undefined) cal[k]=info[k].cal;
const cv=Object.values(cal).sort((a,b)=>a-b);
console.log('классов с caliber: '+cv.length+'   min '+cv[0]+'  медиана '+cv[Math.floor(cv.length/2)]+'  max '+cv[cv.length-1]);
console.log('крупнейшие калибры: '+Object.entries(cal).sort((a,b)=>b[1]-a[1]).slice(0,10).map(([k,v])=>k+'='+v).join(', '));
const cnts={}; for(const k in info) if(info[k].cnt!==null&&info[k].cnt!==undefined) cnts[k]=info[k].cnt;
const kv=Object.values(cnts).sort((a,b)=>a-b);
console.log('магазинов (count): '+kv.length+'   min '+kv[0]+'  медиана '+kv[Math.floor(kv.length/2)]+'  max '+kv[kv.length-1]);
const slots={}; for(const k in info) if(info[k].ics) slots[k]=info[k].ics[0]*info[k].ics[1];
const sv=Object.values(slots).sort((a,b)=>a-b);
console.log('контейнеров (itemsCargoSize): '+sv.length+'   min '+sv[0]+'  медиана '+sv[Math.floor(sv.length/2)]+'  max '+sv[sv.length-1]);
console.log('крупнейшие: '+Object.entries(slots).sort((a,b)=>b[1]-a[1]).slice(0,8).map(([k,v])=>k+'='+v).join(', '));
console.log('');
console.log('=== оружие -> патрон -> калибр ===');
let n=0;
for(const k in info){ const w=info[k]; if(!w.chb||!w.chb.length) continue;
  const ammo=w.chb[0]; const a=info[ammo]; if(!a||a.cal===null||a.cal===undefined) continue;
  console.log('  '+k.padEnd(26)+' -> '+ammo.padEnd(26)+' caliber='+a.cal);
  if(++n>=14) break; }
