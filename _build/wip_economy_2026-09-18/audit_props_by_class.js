
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
const stat={};
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){
    const c=w.cls; if(!stat[c]) stat[c]={n:0, ex:0, sz:0, wrap:0, bold:0};
    stat[c].n++;
    if(w.props['exact text']!==undefined) stat[c].ex++;
    if(w.props['exact text size']!==undefined) stat[c].sz++;
    if(w.props['wrap']!==undefined) stat[c].wrap++;
    if(w.props['bold text']!==undefined) stat[c].bold++;
  }
}
for(const [c,v] of Object.entries(stat).sort((x,y)=>y[1].n-x[1].n)) if(v.n>=3)
  console.log(c.padEnd(28)+' всего '+String(v.n).padStart(4)+'   exact text: '+String(v.ex).padStart(4)+'   exact size: '+String(v.sz).padStart(4)+'   wrap: '+String(v.wrap).padStart(3)+'   bold: '+String(v.bold).padStart(3));
