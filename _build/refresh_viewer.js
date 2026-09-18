
const fs=require('fs');
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
const H='D:/DAYZDISKP/@sps_client_bot/layout_viewer.html';
const names=fs.readdirSync(L).filter(f=>f.endsWith('.layout')).sort();
const builtin={};
for(const n of names) builtin[n]=fs.readFileSync(L+n,'utf8').replace(/\r/g,'');
const html=fs.readFileSync(H,'utf8');
const bi=html.indexOf('var BUILTIN = ')+14;
if(bi<14){ console.log('MARKER NOT FOUND'); process.exit(1); }
// string-aware brace scan to find the end of the embedded JSON object
let depth=0,inStr=false,esc=false,be=-1;
for(let i=bi;i<html.length;i++){
  const c=html[i];
  if(inStr){ if(esc) esc=false; else if(c==='\\') esc=true; else if(c==='"') inStr=false; continue; }
  if(c==='"'){ inStr=true; continue; }
  if(c==='{') depth++;
  if(c==='}'){ depth--; if(depth===0){ be=i+1; break; } }
}
if(be<0){ console.log('JSON END NOT FOUND'); process.exit(1); }
const oldJson=html.slice(bi,be);
let oldKeys=[]; try{ oldKeys=Object.keys(JSON.parse(oldJson)); }catch(e){ oldKeys=['<unparsable>']; }
const newJson=JSON.stringify(builtin,null,1);
fs.writeFileSync(H, html.slice(0,bi)+newJson+html.slice(be), 'utf8');
console.log('embedded layouts BEFORE: '+oldKeys.join(', '));
console.log('embedded layouts AFTER : '+Object.keys(builtin).join(', '));
console.log('html size: '+html.length+' -> '+(html.length-oldJson.length+newJson.length));
