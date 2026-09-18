
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
const res=parseLayout(fs.readFileSync(L+'TraderMenu.layout','utf8'));
for(const n of res.all){ if(['TraderMenu','Background','title_wrapper','title_text','SellablesCheckBox','SellQuantitySlider','SellHintIcon'].indexOf(n.name)<0) continue;
  console.log(n.name+' cls='+n.cls); console.log('   '+JSON.stringify(n.props)); }
