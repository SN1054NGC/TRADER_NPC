
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
for(const f of ['TraderMenu.layout','TraderSellPopup.layout','TraderRating.layout']){
  const res=parseLayout(fs.readFileSync(L+f,'utf8'));
  console.log('=== '+f);
  for(const w of res.all){
    if(w.cls!=='ButtonWidgetClass' && w.cls!=='CheckBoxWidgetClass' && w.cls!=='XComboBoxWidgetClass' && w.cls!=='EditBoxWidgetClass' && w.cls!=='TextListboxWidgetClass') continue;
    const p=w.props;
    console.log('  '+(w.cls.replace('WidgetClass','')+' '+w.name).padEnd(34)+' text='+String(p.text||'-').padEnd(22)+' font='+String(p.font||'-').replace('gui/fonts/','').padEnd(20)+' style='+String(p.style||'-').padEnd(14)+' colums='+String(p.colums||'-').slice(0,30));
  }
}
