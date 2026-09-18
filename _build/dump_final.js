
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
function top(file,W,Hh){
  const res=parseLayout(fs.readFileSync(L+file,'utf8'));
  const rows=[];
  function rec(n,pw,ph,ox,oy,cell){
    const r=rectOf(n,pw,ph);
    const l=ox+r.left+(cell?cell.left:0), t=oy+r.top+(cell?cell.top:0);
    rows.push({n:n.name,l:l,t:t,w:r.width,h:r.height});
    if(kindOf(n.cls)==='spacer'){ const cs=spacerCells(n,r.width,r.height);
      for(let i=0;i<n.children.length;i++){ const c=cs[i]; if(c) rec(n.children[i],c.width,c.height,l,t,c); else rec(n.children[i],r.width,r.height,l,t,null);} }
    else for(const c of n.children) rec(c,r.width,r.height,l,t,null);
  }
  for(const r of res.roots) rec(r,W,Hh,0,0,null);
  console.log('=== '+file+' @'+W+'x'+Hh+' ===');
  for(const x of rows) console.log('  '+x.n.padEnd(28)+' x='+x.l.toFixed(0).padStart(5)+' y='+x.t.toFixed(0).padStart(5)+'  '+x.w.toFixed(0).padStart(5)+' x '+x.h.toFixed(0).padStart(4)+'   правый='+(x.l+x.w).toFixed(0)+' низ='+(x.t+x.h).toFixed(0));
}
top('TraderMenu.layout',1300,990);
