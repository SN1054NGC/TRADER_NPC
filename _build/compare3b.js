
const fs=require('fs'),path=require('path');
const OURS='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot';
const THEIRS='D:/DAYZDISKP/_diag/upstream/scripts';
const VAN=['D:/steam/steamapps/common/DayZServer/dta/scripts','D:/steam/steamapps/common/DayZ/dta/scripts'];
function walk(root,ext){const o=[];(function w(d){let es;try{es=fs.readdirSync(d,{withFileTypes:true});}catch(e){return;}for(const e of es){const p=d+'/'+e.name;if(e.isDirectory()){if(e.name==='.git')continue;w(p);}else if(!ext||ext.test(e.name))o.push(p);}})(root);return o;}
const isC=s=>s.startsWith('//')||s.startsWith('/*')||s.startsWith('*')||s.startsWith('*/');
const isB=s=>/^[{}();,]+$/.test(s);
const lines=f=>fs.readFileSync(f,'utf8').split(/\r?\n/).map(s=>s.trim()).filter(s=>s.length>10&&!isC(s)&&!isB(s));
const base=f=>path.basename(f).toLowerCase();
const rel=(f,m)=>{const i=f.toLowerCase().indexOf(m);return i<0?base(f):f.slice(i+m.length).toLowerCase();};
const vanFiles=[];for(const v of VAN)vanFiles.push(...walk(v,/\.c$/));
const vanByRel=new Map(),vanByBase=new Map();
for(const f of vanFiles){const r=rel(f,'scripts/'),b=base(f);
 if(!vanByRel.has(r))vanByRel.set(r,[]);vanByRel.get(r).push(f);
 if(!vanByBase.has(b))vanByBase.set(b,[]);vanByBase.get(b).push(f);}
function table(files,label,marker){
  const rows=[];
  for(const f of files){
    const txt=fs.readFileSync(f,'utf8');
    const mine=lines(f);
    const r=rel(f,marker),cand=vanByRel.get(r)||vanByBase.get(base(f))||[];
    let hit=0;if(cand.length){const cs=new Set();for(const c of cand)for(const l of lines(c))cs.add(l);for(const l of mine)if(cs.has(l))hit++;}
    const modded=(txt.match(/^\s*modded\s+class\s+\w+/gm)||[]).length;
    const cls=(txt.match(/^\s*(modded\s+)?class\s+\w+/gm)||[]).length;
    rows.push({r,tot:mine.length,hit,modded,cls,van:cand.length>0,relPath:r});
  }
  console.log('=== '+label+' ('+files.length+' файлов, '+rows.reduce((s,x)=>s+x.tot,0)+' значимых строк) ===');
  const overwrite=rows.filter(x=>x.tot>=80&&x.van&&x.hit/x.tot>=0.3);
  const patches=rows.filter(x=>x.modded>0&&x.tot<80);
  const newish=rows.filter(x=>!x.van);
  console.log('  A. КРУПНЫЕ файлы с существенной ванильной основой (>=80 строк, >=30% ванили): '+overwrite.length);
  for(const x of overwrite.sort((a,b)=>b.tot-a.tot)) console.log('     '+String(x.tot).padStart(5)+' строк, ваниль '+(Math.round(x.hit*1000/x.tot)/10)+'%, class='+x.cls+', modded='+x.modded+'  '+x.r);
  console.log('  B. маленькие файлы-патчи modded class (<80 строк): '+patches.length+' ('+patches.reduce((s,x)=>s+x.tot,0)+' строк)');
  console.log('     '+patches.map(x=>x.r).join('\n     '));
  console.log('  C. новых файлов без ванильного тёзки: '+newish.length+' ('+newish.reduce((s,x)=>s+x.tot,0)+' строк)');
  for(const x of newish.sort((a,b)=>b.tot-a.tot).slice(0,40)) console.log('     '+String(x.tot).padStart(5)+'  '+x.r);
  console.log('');
  return rows;
}
const ourC=walk(OURS,/\.c$/),theC=walk(THEIRS,/\.c$/);
const o=table(ourC,'НАШИ ФАЙЛЫ','scripts/');
const t=table(theC,'ФАЙЛЫ АВТОРА (upstream)','scripts/');
// что мы переняли по именам
const oBase=new Set(ourC.map(base));
const dropped=t.filter(x=>!oBase.has(base(x.r)));
console.log('=== авторские файлы, которых у нас нет по имени ('+dropped.length+') ===');
for(const x of dropped.sort((a,b)=>b.tot-a.tot)) console.log('  '+String(x.tot).padStart(5)+'  '+x.r);
const tBase=new Set(theC.map(base));
const added=o.filter(x=>!tBase.has(base(x.r)));
console.log('\n=== наши файлы, которых нет у автора ('+added.length+', '+added.reduce((s,x)=>s+x.tot,0)+' строк) ===');
for(const x of added.sort((a,b)=>b.tot-a.tot)) console.log('  '+String(x.tot).padStart(5)+'  '+x.r);
