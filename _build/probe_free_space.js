
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
function rects(file){
  const res=parseLayout(fs.readFileSync('D:/DAYZDISKP/'+file,'utf8'));
  const out=[];
  function rec(n,pw,ph,ox,oy){
    const r=rectOf(n,pw,ph); const l=ox+r.left, t=oy+r.top;
    if(r.width>30 && r.height>20) out.push({n:n.name,l:l,t:t,w:r.width,h:r.height});
    for(const c of n.children) rec(c,r.width,r.height,l,t);
  }
  for(const r of res.roots) rec(r,1920,1080,0,0);
  return out;
}
console.log('=== ванильный HUD: крупные виджеты (T>900 или T<200) ===');
for(const x of rects('gui/layouts/day_z_hud.layout')) if(x.t>880||x.t<200) console.log('  '+x.n.padEnd(30)+' L='+x.l.toFixed(0).padStart(5)+' T='+x.t.toFixed(0).padStart(5)+' W='+x.w.toFixed(0).padStart(5)+' H='+x.h.toFixed(0));
console.log('=== ванильный инвентарь: низ (T>800) и верх (T<220) ===');
for(const x of rects('gui/layouts/inventory_new/day_z_inventory_new.layout')) if(x.t>800||x.t<220) console.log('  '+x.n.padEnd(30)+' L='+x.l.toFixed(0).padStart(5)+' T='+x.t.toFixed(0).padStart(5)+' W='+x.w.toFixed(0).padStart(5)+' H='+x.h.toFixed(0));
