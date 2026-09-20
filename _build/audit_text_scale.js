
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
// 1) что ваниль делает со свойством "exact text" у текстовых классов
const V={};
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){ if(!/Text|Rich/.test(w.cls)) continue;
    const ex=w.props['exact text']; if(ex===undefined) continue;
    V[w.cls]=V[w.cls]||{};
    V[w.cls][ex]=(V[w.cls][ex]||0)+1; } }
console.log('=== ваниль: значения "exact text" ===');
for(const [c,v] of Object.entries(V)) console.log('  '+c.padEnd(28)+' 0:'+String(v['0']||0).padStart(4)+'  1:'+String(v['1']||0).padStart(4));
// 2) мои макеты: где текст может масштабироваться (exact text 0) или нет размера
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
console.log('=== мои макеты: тексты без фиксированного размера ===');
for(const f of fs.readdirSync(L).filter(x=>x.endsWith('.layout'))){
  const res=parseLayout(fs.readFileSync(L+f,'utf8'));
  for(const w of res.all){
    if(!/^(Text|MultilineText|RichText)WidgetClass$/.test(w.cls)) continue;
    const p=w.props;
    const ex=p['exact text'], sz=p['exact text size'];
    if(ex==='0' || sz===undefined)
      console.log('  '+f.padEnd(26)+' '+w.name.padEnd(24)+' '+w.cls.replace('WidgetClass','').padEnd(16)+' exact='+String(ex===undefined?'-':ex)+' size='+String(sz===undefined?'-':sz)+' font='+String(p.font||'-').replace('gui/fonts/',''));
  }
}
// 3) мои макеты: текст длиннее виджета (оценка 0.55*size на символ)
console.log('=== мои макеты: вероятный выход за ширину (оценка) ===');
for(const f of fs.readdirSync(L).filter(x=>x.endsWith('.layout'))){
  const res=parseLayout(fs.readFileSync(L+f,'utf8'));
  for(const w of res.all){
    const p=w.props; if(!p.text||p.text==='""') continue;
    const t=String(p.text);
    const sz=parseFloat(p['exact text size']||0);
    if(!sz) continue;
    const wpx=parseFloat(String(p.size||'0').split(' ')[0]);
    const exw=p.hexactsize==='1';
    const est=t.replace(/"/g,'').length*sz*0.55;
    if(!exw && p.wrap!=='1') console.log('  '+f.padEnd(26)+' '+w.name.padEnd(24)+' тек.длина~'+Math.round(est)+'px  ширина='+wpx+(wpx?' (доля родителя)':''));
  }
}
