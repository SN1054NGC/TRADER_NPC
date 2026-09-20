
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
function dump(file){
  const res=parseLayout(fs.readFileSync(L+file,'utf8'));
  console.log('--- '+file+' tree ---');
  function rec(n,d){ console.log('  '.repeat(d)+n.cls+' '+n.name+'  cols='+(n.props.Columns||'-')+' rows='+(n.props.Rows||'-')+' pad='+(n.props.Padding||'-')+' size='+(n.props.size||'-')); for(const c of n.children) rec(c,d+1); }
  for(const r of res.roots) rec(r,0);
}
dump('TraderRating.layout');
const res=parseLayout(fs.readFileSync(L+'TraderRating.layout','utf8'));
function find(n,name){ if(n.name===name) return n; for(const c of n.children){ const f=find(c,name); if(f) return f; } return null; }
const spacer=find(res.roots[0],'RatingRows');
console.log('spacer children='+spacer.children.length+' ->'+spacer.children.map(c=>c.name).join(','));
console.log('cells: '+JSON.stringify(spacerCells(spacer, 365, 84)));
