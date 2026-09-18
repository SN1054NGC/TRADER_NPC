
const fs=require('fs'); const roots=['D:/DAYZDISKP/DZ','D:/DAYZDISKP/bin']; const files=[];
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(roots[0]);
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp') files.push(p); } })(roots[1]);
const withQ=[], withVqm=[];
for(const f of files){ const t=fs.readFileSync(f,'utf8');
  const re=/class\s+([A-Za-z0-9_]+)\s*(?::\s*[A-Za-z0-9_]+)?\s*\{/g; const mk=[]; let m;
  while((m=re.exec(t))!==null) mk.push({n:m[1],s:m.index});
  for(let i=0;i<mk.length;i++){ const body=t.slice(mk[i].s,(i+1<mk.length)?mk[i+1].s:t.length);
    if(/quantityBar\s*=\s*1/.test(body)) withQ.push(mk[i].n);
    if(/varQuantityMax\s*=\s*[\d]+/.test(body)) withVqm.push(mk[i].n); } }
console.log('классов с quantityBar=1: '+withQ.length+'  примеры: '+withQ.slice(0,14).join(', '));
console.log('классов с varQuantityMax: '+withVqm.length+'  примеры: '+withVqm.slice(0,10).join(', '));
