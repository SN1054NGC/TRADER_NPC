
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
const want={ButtonWidgetClass:1,CheckBoxWidgetClass:1,XComboBoxWidgetClass:1,EditBoxWidgetClass:1,TextListboxWidgetClass:1};
const keys=['text halign','text valign','bold text','italic text','text_proportion','wrap','exact text','exact text size','font','text','style','"no focus"','color'];
const stat={};
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){ if(!want[w.cls]) continue;
    stat[w.cls]=stat[w.cls]||{n:0,k:{}};
    stat[w.cls].n++;
    for(const k of keys) if(w.props[k]!==undefined) stat[w.cls].k[k]=(stat[w.cls].k[k]||0)+1;
  }
}
for(const [c,v] of Object.entries(stat)){
  console.log('=== '+c+' ('+v.n+')');
  console.log('   '+keys.map(k=>k+'='+(v.k[k]||0)).join('  '));
}
