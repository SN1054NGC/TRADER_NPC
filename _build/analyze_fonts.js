
const fs=require('fs'), path=require('path');
const H='D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html';
const html=fs.readFileSync(H,'utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const ROOT='D:/DAYZDISKP/gui/layouts';
function files(dir){ let out=[]; for(const e of fs.readdirSync(dir,{withFileTypes:true})){ const p=path.join(dir,e.name); if(e.isDirectory()) out=out.concat(files(p)); else if(e.name.endsWith('.layout')) out.push(p); } return out; }
const all=files(ROOT);
const fonts={}; let hexPosMissing=0, hexSizeMissing=0, total=0; const missingExamples=[];
for(const f of all){
  const res=parseLayout(fs.readFileSync(f,'utf8')); const rel=f.substring(ROOT.length+1);
  const walk=(n)=>{
    total++;
    if(n.props.font){ const k=n.props.font.replace('gui/fonts/','')+' @ '+(n.props['exact text size']||'(no size)'); fonts[k]=(fonts[k]||0)+1; }
    const hasP = n.props.hexactpos!==undefined || n.props.vexactpos!==undefined;
    const hasS = n.props.hexactsize!==undefined || n.props.vexactsize!==undefined;
    if(!hasP) hexPosMissing++;
    if(!hasS) hexSizeMissing++;
    if(!hasP && missingExamples.length<6) missingExamples.push(rel+' :: '+n.name+' pos='+n.props.position+' size='+n.props.size+' halign='+(n.props.halign||'-'));
    for(const c of n.children) walk(c);
  };
  for(const r of res.roots) walk(r);
}
console.log('total widgets '+total);
console.log('widgets WITHOUT hexactpos/vexactpos: '+hexPosMissing+'   WITHOUT hexactsize/vexactsize: '+hexSizeMissing);
console.log('examples without hexactpos: '); missingExamples.forEach(s=>console.log('  '+s));
console.log('\n=== font @ exact text size (top 30) ===');
for(const k of Object.keys(fonts).sort((x,y)=>fonts[y]-fonts[x]).slice(0,30)) console.log('  '+k.padEnd(44)+fonts[k]);
console.log('\n=== sizes used with each common sdf font ===');
const byFont={};
for(const k in fonts){ const [fn,sz]=k.split(' @ '); byFont[fn]=byFont[fn]||{}; byFont[fn][sz]=(byFont[fn][sz]||0)+fonts[k]; }
for(const fn of ['sdf_MetronBook24','sdf_MetronLight24','sdf_MetronBold24','Metron22','Metron28','Metron48']){
  if(byFont[fn]) console.log('  '+fn.padEnd(20)+JSON.stringify(byFont[fn]));
}
