
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
function vanillaPairs(){
  const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
  (function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
  const s=new Set();
  for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
    for(const w of res.all){ const fo=String(w.props.font||''), sz=w.props['exact text size']; if(fo&&sz!==undefined) s.add(fo+' @'+sz); } }
  return s;
}
const V=vanillaPairs();
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
for(const f of fs.readdirSync(L).filter(x=>x.endsWith('.layout'))){
  const res=parseLayout(fs.readFileSync(L+f,'utf8'));
  const mine=new Map();
  for(const w of res.all){ const fo=w.props.font, sz=w.props['exact text size']; if(fo&&sz!==undefined){ const k=fo+' @'+sz; mine.set(k,(mine.get(k)||0)+1); } }
  if(!mine.size) continue;
  const bad=[...mine.entries()].filter(([k])=>!V.has(k));
  console.log(f+': пар '+mine.size+', НЕванильных: '+bad.length+(bad.length?'  -> '+bad.map(([k,n])=>k+' x'+n).join(', '):''));
}
