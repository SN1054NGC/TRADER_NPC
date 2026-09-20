
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@TRADER_NPC/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const base='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/';
const files=['TraderSellPopup.layout','TraderMenu.layout','TraderNotification.layout','TraderNotificationsContainer.layout','TraderItemInspectSell.layout'];
for(const f of files){
  const raw=fs.readFileSync(base+f,'utf8');
  const res=parseLayout(raw);
  // expected property lines = lines that are not blank, not braces, not widget declarations
  let expected=0;
  for(const line of res.lines){
    const t=line.replace(/\/\/.*$/,'').trim();
    if(t===''||t==='{'||t==='}') continue;
    if(t.charAt(t.length-1)==='{') continue;
    expected++;
  }
  let got=0; for(const n of res.all) got+=n.propOrder.length;
  console.log(f.padEnd(34)+' props expected='+String(expected).padStart(4)+' parsed='+String(got).padStart(4)+'  '+(expected===got?'OK':'MISMATCH'));
  // assets referenced
  const imgs=new Set(), fonts=new Set(), styles=new Set();
  for(const n of res.all){
    if(n.props.image0) imgs.add(n.props.image0);
    if(n.props.imageTexture) imgs.add(n.props.imageTexture);
    if(n.props.font) fonts.add(n.props.font);
    if(n.props.style) styles.add(n.props.style);
  }
  if(imgs.size) console.log('    images: '+[...imgs].join(' | '));
  if(fonts.size) console.log('    fonts:  '+[...fonts].join(' | '));
  if(styles.size) console.log('    styles: '+[...styles].join(' | '));
}
