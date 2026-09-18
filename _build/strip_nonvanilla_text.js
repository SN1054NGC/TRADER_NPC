
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const ONLY=['exact text','exact text size','text halign','text valign','text color','bold text','italic text','wrap','size to text h','size to text v','text offset','text_proportion','lines','colums','title visible','text','font','highlight row','highlight on focus','no focus'];
const V={};
const dir='D:/DAYZDISKP/gui/layouts/'; const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
for(const f of files){ let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){ V[w.cls]=V[w.cls]||{}; for(const k of ONLY) if(w.props[k]!==undefined) V[w.cls][k]=(V[w.cls][k]||0)+1; }
}
const show=(c)=>ONLY.filter(k=>V[c]&&V[c][k]).map(k=>k+'x'+V[c][k]).join(', ');
console.log('ваниль TextListbox: '+show('TextListboxWidgetClass'));
console.log('ваниль Button:     '+show('ButtonWidgetClass'));
console.log('ваниль CheckBox:   '+show('CheckBoxWidgetClass'));
console.log('ваниль XComboBox:  '+show('XComboBoxWidgetClass'));
console.log('');
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
for(const f of ['TraderMenu.layout','TraderSellPopup.layout','TraderRating.layout']){
  const lines=fs.readFileSync(L+f,'utf8').split('\n');
  const stack=[]; let depth=0; const out=[]; const rep={};
  for(let i=0;i<lines.length;i++){
    const raw=lines[i], t=raw.trim();
    if(t===''||t.startsWith('//')){ out.push(raw); continue; }
    const isOpen=/^([A-Za-z_]\w*)\s+\w+\s*\{$/.exec(t);
    if(isOpen){ stack.push({cls:isOpen[1], children:false, depth:depth}); depth++; out.push(raw); continue; }
    if(t==='{'){ if(stack.length) stack[stack.length-1].children=true; depth++; out.push(raw); continue; }
    if(t==='}'){ depth--; if(stack.length && stack[stack.length-1].depth===depth) stack.pop(); out.push(raw); continue; }
    const cur=stack.length?stack[stack.length-1]:null;
    if(cur && !cur.children){
      const m=t.match(/^"([^"]+)"\s+(.*)$/);
      const key=m?m[1]:t.split(/\s+/)[0];
      if(ONLY.indexOf(key)>=0 && !(V[cur.cls]&&V[cur.cls][key])){
        rep[cur.cls+'.'+key]=(rep[cur.cls+'.'+key]||0)+1; continue;
      }
    }
    out.push(raw);
  }
  const keys=Object.keys(rep);
  if(keys.length){ fs.writeFileSync(L+f,out.join('\n')); console.log(f+'  убрано: '+keys.map(k=>k+'x'+rep[k]).join(', ')); }
  else console.log(f+'  чисто');
}
