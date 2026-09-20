
const fs=require('fs');
const root='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/';
const st=fs.readFileSync(root+'languagecore/stringtable.csv','utf8').split('\n');
const val={};
for(const l of st){ const m=l.match(/^"(tm_[a-z_]+)","([^"]*)","([^"]*)"/); if(m) val[m[1]]={en:m[2],ru:m[5]||''}; }
const st2=fs.readFileSync(root+'languagecore/stringtable.csv','utf8').split('\n');
for(const l of st2){ const p=l.split('","'); if(p.length>=5){ const k=(p[0]||'').replace(/^"/,''); val[k]={en:p[2]||p[1]||'',ru:p[5]||''}; } }
const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(/\.c$/.test(e.name)) files.push(p); } })(root+'scripts');
let bad=0, susp=0;
for(const f of files){
  const t=fs.readFileSync(f,'utf8');
  for(const m of t.matchAll(/TranslateString\(\s*"(#?[a-z_]+)"\s*\)\s*\+\s*":\s*"/g)){
    const key=m[1].replace(/^#/,''); const v=val[key];
    if(v && /:\s*$/.test(v.en)){ console.log('ДВОЙНОЕ ДВОЕТОЧИЕ: '+f.replace(root,'')+'  '+key+'  en="'+v.en+'"'); bad++; }
  }
}
// лишние пробелы/двойные двоеточия в самих значениях
for(const l of st2){ const p=l.split('","'); if(p.length<4) continue; const k=(p[0]||'').replace(/^"/,'');
  for(let i=1;i<p.length-1;i++){ const s=p[i];
    if(/::/.test(s)) { console.log(':: в значении: '+k+'  "'+s+'"'); susp++; }
    else if(/^\s|\s$/.test(s) && s.trim()!=='') { console.log('пробел по краю: '+k+'  "'+s+'"'); susp++; }
  } }
console.log('проверено ' + files.length + ' файлов скриптов; проблем с ": " = ' + bad + '; подозрительных значений = ' + susp);
