
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const res=parseLayout(fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/TraderSellPopup.layout','utf8'));
for(const n of res.all){
  const p=n.props||{};
  if(/^Item.*(Widget|Icon|Background)$/.test(n.name) || n.name==='vignette' || n.name==='StatusSpacer' || n.name==='InventoryInfoPanelWidget')
    console.log(String(n.name).padEnd(34)+' pos='+String(p.position||'-').padEnd(12)+' size='+String(p.size||'-').padEnd(12)+' ha='+String(p.halign||'-').padEnd(12)+' va='+String(p.valign||'-').padEnd(12)+' exp='+p.hexactpos+' eyp='+p.vexactpos+' exs='+p.hexactsize+' eys='+p.vexactsize);
}
