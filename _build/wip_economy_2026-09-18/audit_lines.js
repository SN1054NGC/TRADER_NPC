
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){ if(w.cls!=='TextListboxWidgetClass') continue; const p=w.props;
    if(p.colums===undefined && p['title visible']===undefined) continue;
    console.log((f.split('/').pop()+':'+w.name).padEnd(48)+' lines='+String(p.lines===undefined?'-':p.lines).padEnd(5)+' titleVis='+String(p['title visible']===undefined?'-':p['title visible']).padEnd(4)+' size='+String(p.size||'-').padEnd(12)+' font='+String(p.font||'-').replace('gui/fonts/','').padEnd(18)+' colums='+String(p.colums||'-').slice(0,40));
  }
}
