
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
let n=0;
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){
    if(w.cls!=='TextListboxWidgetClass') continue;
    n++;
    const p=w.props;
    console.log((f.split('/').pop()+':'+w.name).padEnd(46)+' font='+String(p.font||'-').padEnd(24)+' exact='+String(p['exact text']||'-').padEnd(4)+' size='+String(p['exact text size']||'-').padEnd(4)+' colums='+String(p.colums||'-').slice(0,44)+' titleVis='+String(p['title visible']||'-'));
  }
}
console.log('всего TextListboxWidget: '+n);
