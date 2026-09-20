
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
function walk(file,W,Hh){
  const res=parseLayout(fs.readFileSync(L+file,'utf8'));
  const nodes=[];
  function rec(node,pw,ph,ox,oy,pAbs){
    const r=rectOf(node,pw,ph);
    const absL=ox+r.left, absT=oy+r.top;
    const me={name:node.name,cls:node.cls,left:absL,top:absT,width:r.width,height:r.height,
              pl:pAbs?pAbs.left:0,pt:pAbs?pAbs.top:0,pr:pAbs?pAbs.left+pAbs.width:W,pb:pAbs?pAbs.top+pAbs.height:Hh,
              pName:pAbs?pAbs.name:'(root)'};
    nodes.push(me);
    if(kindOf(node.cls)==='spacer'){
      const cells=spacerCells(node,r.width,r.height);
      for(let i=0;i<node.children.length;i++){ const cc=cells[i];
        if(cc) rec(node.children[i],cc.width,cc.height,absL+cc.left,absT+cc.top,me);
        else rec(node.children[i],r.width,r.height,absL,absT,me); }
    } else for(const c of node.children) rec(c,r.width,r.height,absL,absT,me);
  }
  for(const r of res.roots) rec(r,W,Hh,0,0,null);
  return nodes;
}
function report(file,W,Hh,blocks){
  const nodes=walk(file,W,Hh);
  console.log('=== '+file+'  root '+W+'x'+Hh+'  nodes '+nodes.length+' ===');
  for(const n of nodes) console.log('  '+n.name.padEnd(30)+' L='+n.left.toFixed(1).padStart(7)+' T='+n.top.toFixed(1).padStart(7)+' W='+n.width.toFixed(0).padStart(5)+' H='+n.height.toFixed(0).padStart(5)+'  in '+n.pName);
  let bad=0;
  for(const n of nodes){ if(n.pName==='(root)') continue;
    const e=0.6;
    if(n.left<n.pl-e||n.top<n.pt-e||n.left+n.width>n.pr+e||n.top+n.height>n.pb+e){ bad++;
      console.log('  OUTSIDE  '+n.name+'  rect '+n.left.toFixed(1)+','+n.top.toFixed(1)+' '+n.width.toFixed(0)+'x'+n.height.toFixed(0)+'  parent '+n.pName+' ['+n.pl.toFixed(1)+'..'+n.pr.toFixed(1)+'] ['+n.pt.toFixed(1)+'..'+n.pb.toFixed(1)+']'); } }
  console.log('  containment: '+(nodes.length-1-bad)+'/'+(nodes.length-1)+(bad?'  FAIL':'  OK'));
  if(blocks){ const by={}; for(const n of nodes) if(!by[n.name]) by[n.name]=n;
    let ov=0;
    for(let i=0;i<blocks.length;i++) for(let j=i+1;j<blocks.length;j++){
      const A=by[blocks[i]],B=by[blocks[j]]; if(!A||!B) continue;
      const w=Math.min(A.left+A.width,B.left+B.width)-Math.max(A.left,B.left);
      const h=Math.min(A.top+A.height,B.top+B.height)-Math.max(A.top,B.top);
      if(w>1&&h>1){ ov++; console.log('  OVERLAP  '+blocks[i]+' x '+blocks[j]+' = '+w.toFixed(0)+'x'+h.toFixed(0)); } }
    console.log('  block overlaps: '+ov+(ov?'  CHECK':'  OK'));
  }
}
report('TraderSellPopup.layout',1920,1080,['HeaderBar','ItemFrameWidgetPanel','InventoryInfoPanelWidget','SellRow','NoPriceWidget','BackBtn1']);
report('TraderRating.layout',1920,1080,['RatingPanel','RatingRows']);
report('TraderMenu.layout',1300,990,['Background','title_wrapper','xcombobox_categorys','SearchPanelWidget','SearchIcon','MoneyPanel','txtlist_items','DetailsPanel','SellBar','btn_cancel','btn_buy','btn_sell']);
