
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/gui/layouts/';
const files=[];
(function walk(d){ for(const e of fs.readdirSync(d,{withFileTypes:true})){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name.endsWith('.layout')) files.push(p); } })(dir);
const hist={};
let n=0;
for(const f of files){
  let res; try{ res=parseLayout(fs.readFileSync(f,'utf8')); }catch(e){ continue; }
  for(const w of res.all){
    const font=String(w.props.font||''); const sz=w.props['exact text size'];
    if(!font||sz===undefined) continue;
    n++;
    const key=font.replace('gui/fonts/','')+' @'+sz;
    hist[key]=(hist[key]||0)+1;
  }
}
const top=Object.entries(hist).sort((x,y)=>y[1]-x[1]).slice(0,18);
console.log('всего пар font+exact size: '+n+'  (файлов '+files.length+')');
console.log('самые частые:');
for(const [k,v] of top) console.log('  '+k.padEnd(34)+v);
const fonts=new Set(Object.keys(hist).map(k=>k.split(' @')[0]));
console.log('\nшрифты в связке с exact text size: '+[...fonts].join(', '));
