
const fs=require('fs');
const H='D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html';
const html=fs.readFileSync(H,'utf8');
const s=html.indexOf('<script>')+8, e=html.lastIndexOf('</script>');
try{ new Function(html.slice(s,e)); console.log('JS SYNTAX: OK'); }catch(err){ console.log('JS SYNTAX ERROR: '+err.message); process.exit(1); }
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
// absolute rects: accumulate the parent origin, exactly like the DOM nesting does
function absRects(file,w,h){
  const res=parseLayout(fs.readFileSync(L+file,'utf8'));
  const out={};
  function rec(node,avW,avH,ox,oy,cell){
    const r=rectOf(node,avW,avH);
    const L2=ox+r.left+(cell?cell.left:0), T2=oy+r.top+(cell?cell.top:0);
    out[node.name]={left:L2,top:T2,width:r.width,height:r.height};
    if(kindOf(node.cls)==='spacer'){
      const cells=spacerCells(node,r.width,r.height);
      for(let i=0;i<node.children.length;i++){ const cc=cells[i];
        if(cc) rec(node.children[i],cc.width,cc.height,L2,T2,cc); else rec(node.children[i],r.width,r.height,L2,T2,null); }
    } else for(const c of node.children) rec(c,r.width,r.height,L2,T2,null);
  }
  for(const r of res.roots) rec(r,w,h,0,0,null);
  return out;
}
function row(name,R,tag){ const r=R[name]; if(!r){ console.log('  MISSING '+name); return null; }
  console.log('  '+(tag||name).padEnd(26)+' L='+r.left.toFixed(0).padStart(4)+' T='+r.top.toFixed(0).padStart(4)+' W='+r.width.toFixed(0).padStart(4)+' H='+r.height.toFixed(0).padStart(3)); return r; }
console.log('=== TraderSellPopup.layout @1920x1080 ===');
const R=absRects('TraderSellPopup.layout',1920,1080);
const w1=row('WindowWidget',R); console.log('  centred: cx='+(w1.left+w1.width/2).toFixed(0)+' cy='+(w1.top+w1.height/2).toFixed(0)+' (960/540)');
row('HeaderBar',R); row('ItemNameWidget',R); row('ItemDamageWidget',R); row('BackBtn1',R);
row('PreviewCard',R); row('ItemFrameWidget',R);
row('InventoryInfoPanelWidget',R); row('ItemDescWidget',R); row('StatusSpacer',R);
const st=['ItemLiquidTypeWidget','ItemTemperatureWidget','ItemWetnessWidget','ItemFoodStageWidget','ItemWeightWidget','ItemQuantityWidget','ItemCleannessWidget'];
let prev=-1e9, ok=true;
for(const n of st){ const r=row(n,R); if(!r){ok=false;continue;} if(r.top<prev-0.5||r.height<8) ok=false; prev=r.top; }
console.log('  7 status rows stacked: '+(ok?'OK':'BROKEN'));
row('SellRow',R); row('SellPriceWidget',R); row('SellAmountWidget',R); row('SellSlider',R); row('SellButton',R); row('NoPriceWidget',R);
console.log('=== TraderRating.layout @1920x1080 ===');
const B=absRects('TraderRating.layout',1920,1080);
const b1=row('RatingRoot',B); console.log('  anchored bottom-left: left='+b1.left.toFixed(0)+' bottom='+(b1.top+b1.height).toFixed(0)+' of 1080');
row('RatingTitle',B); row('RatingValue',B); row('RatingInfo',B); row('RatingBar',B);
