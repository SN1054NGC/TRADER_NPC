
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
function show(file,W,Hh,depth){
  const res=parseLayout(fs.readFileSync(L+file,'utf8'));
  console.log('=== '+file+' @'+W+'x'+Hh+' ===');
  function rec(n,avW,avH,ox,oy,d){
    const r=rectOf(n,avW,avH);
    const L2=ox+r.left, T2=oy+r.top;
    if(d<=depth) console.log('  '.repeat(d)+n.name.padEnd(28)+' L='+L2.toFixed(1).padStart(7)+' T='+T2.toFixed(1).padStart(7)+' W='+r.width.toFixed(0).padStart(5)+' H='+r.height.toFixed(0).padStart(5)+'   R='+(L2+r.width).toFixed(0)+' B='+(T2+r.height).toFixed(0));
    const par={left:L2,top:T2,width:r.width,height:r.height};
    if(kindOf(n.cls)==='spacer'){ const cells=spacerCells(n,r.width,r.height);
      for(let i=0;i<n.children.length;i++){ const cc=cells[i]; if(cc) rec(n.children[i],cc.width,cc.height,L2,T2,d+1); else rec(n.children[i],r.width,r.height,L2,T2,d+1); } }
    else for(const c of n.children) rec(c,r.width,r.height,L2,T2,d+1);
  }
  for(const r of res.roots) rec(r,W,Hh,0,0,0);
}
show('TraderSellPopup.layout',1920,1080,2);
show('TraderRating.layout',1920,1080,2);
