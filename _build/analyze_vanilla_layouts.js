
const fs=require('fs'), path=require('path');
const H='D:/DAYZDISKP/@sps_client_bot/layout_viewer.html';
const html=fs.readFileSync(H,'utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const ROOT='D:/DAYZDISKP/gui/layouts';
function files(dir){ let out=[]; for(const e of fs.readdirSync(dir,{withFileTypes:true})){ const p=path.join(dir,e.name); if(e.isDirectory()) out=out.concat(files(p)); else if(e.name.endsWith('.layout')) out.push(p); } return out; }
const all=files(ROOT);
const clsCount={}, clsExample={}, alignPairs={}, fonts={}, sliderDefs=[], fixaspect={}, centering=[], corners=[], propsByCls={};
let totalWidgets=0;
for(const f of all){
  const res=parseLayout(fs.readFileSync(f,'utf8'));
  const rel=f.substring(ROOT.length+1);
  const walk=(n)=>{
    totalWidgets++;
    const c=n.cls.replace('WidgetClass','');
    clsCount[c]=(clsCount[c]||0)+1;
    if(!clsExample[c]) clsExample[c]=rel;
    const ha=n.props.halign||'(none)', va=n.props.valign||'(none)';
    const key=ha+'/'+va; alignPairs[key]=(alignPairs[key]||0)+1;
    if(n.props.font){ const k=n.props.font.replace('gui/fonts/','')+' @ '+(n.props['exact text size']||'-'); fonts[k]=(fonts[k]||0)+1; }
    if(n.props.fixaspect) fixaspect[n.props.fixaspect]=(fixaspect[n.props.fixaspect]||0)+1;
    if(c==='Slider') sliderDefs.push(rel+' :: '+JSON.stringify({position:n.props.position,size:n.props.size,halign:n.props.halign,valign:n.props.valign,hexactpos:n.props.hexactpos,hexactsize:n.props.hexactsize,style:n.props.style,maximum:n.props.maximum,step:n.props.step,current:n.props.current,color:n.props.color}));
    // centering candidates: window-ish widgets with center_ref and fraction size
    if((c==='Frame'||c==='Panel'||c==='Window') && ha==='center_ref' && va==='center_ref')
      centering.push(rel+' :: '+n.name+' size='+n.props.size+' pos='+n.props.position+' hexsize='+n.props.hexactsize+' fixaspect='+(n.props.fixaspect||'-'));
    // corner usage: right_ref / bottom_ref with negative position
    if((ha==='right_ref'||va==='bottom_ref') && n.props.position && n.props.position.indexOf('-')>=0)
      corners.push(rel+' :: '+n.name+' pos='+n.props.position+' size='+n.props.size+' '+ha+'/'+va);
    for(const k in n.props){ propsByCls[c]=propsByCls[c]||{}; propsByCls[c][k]=(propsByCls[c][k]||0)+1; }
    for(const ch of n.children) walk(ch);
  };
  for(const r of res.roots) walk(r);
}
console.log('vanilla layouts:'+all.length+'  widgets:'+totalWidgets);
console.log('\n=== WIDGET CLASSES (count | example file) ===');
for(const k of Object.keys(clsCount).sort((x,y)=>clsCount[y]-clsCount[x])) console.log('  '+k.padEnd(26)+String(clsCount[k]).padStart(5)+'   '+clsExample[k]);
console.log('\n=== halign/valign pairs (top 12) ===');
for(const k of Object.keys(alignPairs).sort((x,y)=>alignPairs[y]-alignPairs[x]).slice(0,12)) console.log('  '+k.padEnd(34)+alignPairs[k]);
console.log('\n=== fixaspect values ==='); console.log('  '+JSON.stringify(fixaspect));
console.log('\n=== centre_ref windows (fraction size + centered) top 12 ===');
centering.slice(0,12).forEach(s=>console.log('  '+s));
console.log('\n=== corner usage (right_ref/bottom_ref + negative pos) top 14 ===');
corners.slice(0,14).forEach(s=>console.log('  '+s));
console.log('\n=== SLIDER definitions (all) ===');
sliderDefs.forEach(s=>console.log('  '+s));
console.log('\n=== font @ exact text size (top 22) ===');
for(const k of Object.keys(fonts).sort((x,y)=>fonts[y]-fonts[x]).slice(0,22)) console.log('  '+k.padEnd(40)+fonts[k]);
