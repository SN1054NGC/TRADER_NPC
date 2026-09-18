
const fs=require('fs'),path=require('path');
const OURS='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot';
const THEIRS='D:/DAYZDISKP/_diag/upstream';
const VAN_C='D:/DAYZDISKP/scripts';
function walk(root,ext){ const o=[]; (function w(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()){ if(e.name==='.git') continue; w(p); } else if(!ext||ext.test(e.name)) o.push(p); } })(root); return o; }
function lines(f){ return fs.readFileSync(f,'utf8').split(/\r?\n/).map(s=>s.trim()).filter(s=>s.length>10); }
const van=new Set(); for(const f of walk(VAN_C,/\.c$/)) for(const l of lines(f)) van.add(l);
const ourC=walk(OURS,/\.c$/), theirC=walk(THEIRS,/\.c$/);
const theSet=new Set(); for(const f of theirC) for(const l of lines(f)) theSet.add(l);
console.log('ваниль: '+van.size+' уникальных строк   upstream .c файлов: '+theirC.length+'   наших .c: '+ourC.length);
let bothVan=0, ourVanOnly=0, authorKept=0, oursOnly=0, total=0;
for(const f of ourC) for(const l of lines(f)){ total++;
  const inThe=theSet.has(l), inVan=van.has(l);
  if(inThe&&inVan) bothVan++;
  else if(inThe&&!inVan) authorKept++;
  else if(!inThe&&inVan) ourVanOnly++;
  else oursOnly++; }
const p=n=>Math.round(n*1000/total)/10;
console.log('');
console.log('=== НАШИ .c-строки ('+total+') ===');
console.log('  ваниль + upstream (оба скопировали игру): '+bothVan+'  '+p(bothVan)+'%');
console.log('  ваниль, только у нас (upstream это переписал/выкинул): '+ourVanOnly+'  '+p(ourVanOnly)+'%');
console.log('  ТОЛЬКО АВТОР upstream (нет в ванили, мы сохранили): '+authorKept+'  '+p(authorKept)+'%');
console.log('  только наши (ни ваниль, ни upstream): '+oursOnly+'  '+p(oursOnly)+'%');
let aVan=0,aOwn=0,aTotal=0;
for(const f of theirC) for(const l of lines(f)){ aTotal++; if(van.has(l)) aVan++; else aOwn++; }
console.log('');
console.log('=== КОД АВТОРА upstream ('+aTotal+' строк .c) ===');
console.log('  ванильные строки: '+aVan+'  ('+Math.round(aVan*1000/aTotal)/10+'%)');
console.log('  собственная работа автора (не ваниль): '+aOwn+'  ('+Math.round(aOwn*1000/aTotal)/10+'%)');
let kept=0; for(const f of theirC) for(const l of lines(f)) if(!van.has(l) && theSet.has(l) && fs.existsSync(OURS)) { }
// сколько авторских (неванильных) строк upstream мы у себя сохранили
const ourSet=new Set(); for(const f of ourC) for(const l of lines(f)) ourSet.add(l);
let authorKept2=0; for(const f of theirC) for(const l of lines(f)) if(!van.has(l) && ourSet.has(l)) authorKept2++;
console.log('  из них мы сохранили у себя: '+authorKept2+' ('+Math.round(authorKept2*1000/(aOwn||1))/10+'% авторского кода)');
