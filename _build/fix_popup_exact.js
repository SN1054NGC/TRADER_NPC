
const fs=require('fs');
const P='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/scripts/layouts/TraderSellPopup.layout';
let s=fs.readFileSync(P,'utf8');
const before=s;
const pairs=[['27','0.9'],['29','0.88'],['22','0.9'],['24','0.88']];
let total=0;
for(const [pos,size] of pairs){
  const pat='position '+pos+' 0\n             size '+size+' 1\n             halign left_ref\n             valign top_ref\n             hexactpos 0\n             vexactpos 0';
  const rep='position '+pos+' 0\n             size '+size+' 1\n             halign left_ref\n             valign top_ref\n             hexactpos 1\n             vexactpos 1';
  const re=new RegExp(pat,'g');
  const n=(s.match(re)||[]).length; total+=n;
  s=s.replace(re,rep);
  console.log('pos '+pos+' size '+size+' -> '+n+' block(s) fixed');
}
// status row icons: 21x21 in an 18 px cell -> 16x16 so they never bleed into the next row
const reIcon=/size 21 21\n             image0/g;
const nIcon=(s.match(reIcon)||[]).length;
s=s.replace(reIcon,'size 16 16\n             image0');
console.log('icons resized: '+nIcon);
fs.writeFileSync(P,s);
console.log('changed: '+(s!==before)+'  brace balance '+(s.match(/\{/g)||[]).length+'/'+(s.match(/\}/g)||[]).length);
