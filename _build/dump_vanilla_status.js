
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const res=parseLayout(fs.readFileSync('D:/DAYZDISKP/gui/layouts/inventory_new/day_z_inventory_new_inspect.layout','utf8'));
const want=/^(ItemLiquidTypeWidget|ItemTemperatureWidget|ItemWetnessWidget|ItemFoodStageWidget|ItemWeightWidget|ItemQuantityWidget|ItemCleannessWidget|ItemDamageWidget)(Background)?$/;
for(const n of res.all){ if(!want.test(n.name)) continue;
  const p=n.props||{};
  console.log(String(n.name).padEnd(32)+' pos='+String(p.position||'-').padEnd(14)+' size='+String(p.size||'-').padEnd(12)+' ha='+String(p.halign||'-').padEnd(12)+' va='+String(p.valign||'-').padEnd(12)+' exp='+p.hexactpos+' eyp='+p.vexactpos+' exs='+p.hexactsize+' eys='+p.vexactsize); }
