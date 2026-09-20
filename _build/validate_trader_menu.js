
const fs=require('fs');
const H='D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html';
const html=fs.readFileSync(H,'utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
const FILE=process.argv[2]||'TraderMenu.layout';
const W=parseInt(process.argv[3]||'1300',10), Hh=parseInt(process.argv[4]||'990',10);
const res=parseLayout(fs.readFileSync(L+FILE,'utf8'));
const nodes=[];
function rec(node,avW,avH,ox,oy,cell,parent){
  const r=rectOf(node,avW,avH);
  const left=ox+r.left+(cell?cell.left:0), top=oy+r.top+(cell?cell.top:0);
  const rec2={name:node.name,cls:node.cls,left:left,top:top,width:r.width,height:r.height,parent:parent?parent.name:null,pw:avW,ph:avH};
  nodes.push(rec2);
  if(kindOf(node.cls)==='spacer'){
    const cells=spacerCells(node,r.width,r.height);
    for(let i=0;i<node.children.length;i++){ const cc=cells[i];
      if(cc) rec(node.children[i],cc.width,cc.height,left,top,cc,rec2); else rec(node.children[i],r.width,r.height,left,top,null,rec2); }
  } else for(const c of node.children) rec(c,r.width,r.height,left,top,null,rec2);
}
for(const r of res.roots) rec(r,W,Hh,0,0,null,null);
const F=n=>n.left.toFixed(1).padStart(7), T=n=>n.top.toFixed(1).padStart(6), S=n=>n.width.toFixed(0).padStart(5), Q=n=>n.height.toFixed(0).padStart(4);
console.log('=== '+FILE+'  root '+W+'x'+Hh+'  nodes '+nodes.length+' ===');
for(const n of nodes) console.log('  '+n.name.padEnd(26)+' ['+n.cls+'] L='+F(n)+' T='+T(n)+' W='+S(n)+' H='+Q(n));
let bad=0;
for(const n of nodes){ if(!n.parent) continue;
  const eps=0.6;
  if(n.left< -eps||n.top< -eps||n.left+n.width>n.pw+eps||n.top+n.height>n.ph+eps){
    bad++; console.log('  OUTSIDE '+n.name+' in '+n.parent+' ('+n.pw.toFixed(0)+'x'+n.ph.toFixed(0)+') rect '+n.left.toFixed(1)+','+n.top.toFixed(1)+' '+n.width.toFixed(0)+'x'+n.height.toFixed(0)); } }
console.log('  containment: '+((nodes.length-1-bad))+'/'+(nodes.length-1)+' inside parent  '+(bad?'FAIL':'OK'));
const BLOCKS=['title_wrapper','xcombobox_categorys','SearchPanelWidget','SearchIcon','MoneyPanel','txtlist_items','DetailsPanel','SellBar','btn_cancel','btn_buy','btn_sell'];
const byName={}; for(const n of nodes) byName[n.name]=n;
let ov=0;
for(let i=0;i<BLOCKS.length;i++) for(let j=i+1;j<BLOCKS.length;j++){
  const A=byName[BLOCKS[i]], B=byName[BLOCKS[j]]; if(!A||!B) continue;
  const w=Math.min(A.left+A.width,B.left+B.width)-Math.max(A.left,B.left);
  const h=Math.min(A.top+A.height,B.top+B.height)-Math.max(A.top,B.top);
  if(w>1&&h>1){ ov++; console.log('  OVERLAP '+BLOCKS[i]+' x '+BLOCKS[j]+' = '+w.toFixed(0)+'x'+h.toFixed(0)+' px'); } }
console.log('  block overlaps: '+ov+'  '+(ov?'CHECK':'OK'));
const kids=BLOCKS.map(k=>byName[k]).filter(Boolean).sort((p,q)=>p.top-q.top);
console.log('  vertical order: '+kids.map(k=>k.name+'(T'+k.top.toFixed(0)+'..'+(k.top+k.height).toFixed(0)+')').join(' -> '));
