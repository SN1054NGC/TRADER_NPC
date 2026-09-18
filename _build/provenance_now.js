
const fs=require('fs'),path=require('path'),crypto=require('crypto');
const OURS='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot';
const THEIRS='D:/DAYZDISKP/_diag/upstream';
const VAN='D:/DAYZDISKP/scripts';
function walk(root){ const o=[]; (function w(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) w(p); else o.push(p); } })(root); return o; }
const md5=f=>crypto.createHash('md5').update(fs.readFileSync(f)).digest('hex');
const ourFiles=walk(OURS);
const theirFiles=fs.existsSync(THEIRS)?walk(THEIRS):[];
console.log('upstream ('+THEIRS+'): '+theirFiles.length+' файлов');
console.log('  примеры: '+theirFiles.slice(0,12).map(f=>f.slice(THEIRS.length+1)).join(', '));
if(!theirFiles.length){ console.log('нет upstream по этому пути'); process.exit(0); }
const byName={}; for(const f of theirFiles){ const n=path.basename(f).toLowerCase(); (byName[n]=byName[n]||[]).push(f); }
let matched=0,identical=0,modified=0,noPair=0,addL=0,delL=0;
const ourLines=[], theLines=new Set();
let ourTotal=0, inTheirs=0, inVan=0, unique=0;
const vanSet=new Set();
for(const f of walk(VAN)){ if(!/\.c$|\.layout$/i.test(f)) continue; for(const l of fs.readFileSync(f,'utf8').split(/\r?\n/)){ const t=l.trim(); if(t.length>10) vanSet.add(t); } }
for(const f of ourFiles){
  const n=path.basename(f).toLowerCase();
  const cand=byName[n];
  if(cand && cand.length===1){
    const t=cand[0]; matched++;
    const A=fs.readFileSync(f,'utf8').split(/\r?\n/), B=fs.readFileSync(t,'utf8').split(/\r?\n/);
    if(md5(f)===md5(t)) identical++; else { modified++;
      const As=new Set(A.map(s=>s.trim()).filter(s=>s.length>2)), Bs=new Set(B.map(s=>s.trim()).filter(s=>s.length>2));
      for(const s of As) if(!Bs.has(s)) addL++;
      for(const s of Bs) if(!As.has(s)) delL++; }
    for(const l of B) { const s=l.trim(); if(s.length>10) theLines.add(s); }
  } else noPair++;
  for(const l of fs.readFileSync(f,'utf8').split(/\r?\n/)){ const t=l.trim(); if(t.length<=10) continue; ourTotal++;
    if((cand&&cand.length===1&&theLines.has(t))) inTheirs++;
    else if(vanSet.has(t)) inVan++;
    else unique++; }
}
console.log('');
console.log('Сопоставлено файлов по имени: '+matched+'   из них ИДЕНТИЧНЫ: '+identical+'   ИЗМЕНЕНО: '+modified+'   без пары: '+noPair);
console.log('Наших строк (значащих): '+ourTotal);
console.log('  есть дословно в upstream: '+inTheirs+' ('+Math.round(inTheirs*1000/ourTotal)/10+'%)');
console.log('  есть в ванили           : '+inVan+' ('+Math.round(inVan*1000/ourTotal)/10+'%)');
console.log('  только наши             : '+unique+' ('+Math.round(unique*1000/ourTotal)/10+'%)');
console.log('Churn в изменённых: наших новых строк +'+addL+', удалённых их строк -'+delL);
