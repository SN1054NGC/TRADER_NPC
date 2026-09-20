
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
const want={ButtonWidgetClass:1,CheckBoxWidgetClass:1,XComboBoxWidgetClass:1,EditBoxWidgetClass:1,TextListboxWidgetClass:1,SliderWidgetClass:1};
const stat={};
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){ if(!want[w.cls]) continue;
    stat[w.cls]=stat[w.cls]||{n:0,props:{},fonts:{}};
    stat[w.cls].n++;
    for(const k of Object.keys(w.props)) stat[w.cls].props[k]=(stat[w.cls].props[k]||0)+1;
    if(w.props.font) stat[w.cls].fonts[w.props.font]=(stat[w.cls].fonts[w.props.font]||0)+1;
  }
}
for(const [c,v] of Object.entries(stat)){
  console.log('=== '+c+'  ('+v.n+' шт)');
  console.log('  свойства: '+Object.entries(v.props).sort((x,y)=>y[1]-x[1]).slice(0,14).map(([k,n])=>k+'×'+n).join(', '));
  console.log('  шрифты:   '+Object.entries(v.fonts).sort((x,y)=>y[1]-x[1]).slice(0,6).map(([k,n])=>k.replace('gui/fonts/','')+'×'+n).join(', '));
}
