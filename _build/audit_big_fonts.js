
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
const hist={}; const examples={};
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){ const fo=String(w.props.font||''), sz=parseFloat(w.props['exact text size']); if(!fo||isNaN(sz)) continue;
    const k=fo.replace('gui/fonts/','')+' @'+sz; hist[k]=(hist[k]||0)+1; if(!examples[k]) examples[k]=f.split('/').pop()+':'+w.name; } }
const big=Object.entries(hist).filter(([k])=>{ const s=parseFloat(k.split('@')[1]); return s>=24 && s<=34; }).sort((x,y)=>y[1]-x[1]);
console.log('ванильные пары с размером 24..34:');
for(const [k,v] of big) console.log('  '+k.padEnd(30)+String(v).padStart(4)+'   пример: '+examples[k]);
