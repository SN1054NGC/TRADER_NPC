
const fs=require('fs');
const TYPES='D:/steam/steamapps/common/DayZServer/mpmissions/dayzOffline.Lux/db/types.xml';
const tx=fs.readFileSync(TYPES,'utf8'); const T={};
for(const m of tx.matchAll(/<type\s+name="([^"]+)"\s*>([\s\S]*?)<\/type>/g)){
  const name=m[1], body=m[2];
  const num=(tag)=>{ const r=new RegExp('<'+tag+'>\\s*(-?\\d+)','i').exec(body); return r?parseInt(r[1],10):null; };
  T[name]={nominal:num('nominal'),
    cat:((new RegExp('<category\\s+name="([^"]+)"','i')).exec(body)||[])[1]||'',
    usage:((new RegExp('<usage\\s+name="([^"]+)"','i')).exec(body)||[])[1]||'',
    tier:((new RegExp('<value\\s+name="([^"]+)"','i')).exec(body)||[])[1]||''};
}
// данные из конфигов игры (частичное дерево, в игре движок отдаёт полностью)
const CAL={},CNT={},SLOTS={},CHB={};
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp'){
    const t=fs.readFileSync(p,'utf8'); const re=/class\s+([A-Za-z0-9_]+)\s*(?::\s*[A-Za-z0-9_]+)?\s*\{/g; const mk=[]; let m;
    while((m=re.exec(t))!==null) mk.push({n:m[1],s:m.index});
    for(let i=0;i<mk.length;i++){ const body=t.slice(mk[i].s,(i+1<mk.length)?mk[i+1].s:t.length);
      let r;
      if((r=/\bcaliber\s*=\s*([\d.]+)/.exec(body))) CAL[mk[i].n]=parseFloat(r[1]);
      if((r=/\bcount\s*=\s*([\d]+)/.exec(body))) CNT[mk[i].n]=parseInt(r[1]);
      if((r=/itemsCargoSize\[\]\s*=\s*\{\s*([\d]+)\s*,\s*([\d]+)/.exec(body))) SLOTS[mk[i].n]=parseInt(r[1])*parseInt(r[2]);
      if((r=/chamberableFrom\[\]\s*=\s*\{([^}]*)\}/.exec(body))) CHB[mk[i].n]=r[1].split(',').map(s=>s.trim().replace(/"/g,'')).filter(s=>s&&s!=='NULL');
    } } } })('D:/DAYZDISKP/DZ');
const BASE={weapons:2500,clothes:300,containers:400,tools:200,food:60,explosives:900,medical:120};
const USE={Military:1.6,Police:1.3,Medic:1.2,Underground:1.2,Hunting:1.1,Town:1,Office:0.95,Industrial:0.95,Village:0.9,Coast:0.9,Farm:0.85};
const TIER={Tier1:1,Tier2:1.2,Tier3:1.4,Tier4:1.7,Tier5:2};
const rar=(n)=>(!n||n<=0)?1:(n<=2?1.5:(n<=5?1.25:(n<=12?1.05:0.95)));
const clamp=(v,a,b)=>v<a?a:(v>b?b:v);
function calMul(cls){ let cal=0; const list=CHB[cls]||[]; for(const a of list.slice(0,8)){ if(CAL[a]>cal) cal=CAL[a]; } if(CAL[cls]>cal) cal=CAL[cls]; if(cal<=0) return {m:1,why:'нет калибра'}; return {m:clamp(0.7+cal*0.6,0.7,3),why:'caliber '+cal}; }
function capMul(cls,cat){ let cap=CNT[cls]||0, why='count '+cap; if(cap<=1){ cap=0; why=''; } if(!cap && SLOTS[cls]){ cap=SLOTS[cls]; why='cargo '+cap; } if(!cap) return {m:1,why:'нет вместимости'}; return {m:clamp(0.8+cap/60,0.8,2.5),why:why}; }
function price(cls,cat,usage,tier,nominal,useCal,useCap){
  let f=(BASE[cat]||150)*(USE[usage]||1)*(TIER[tier]||1)*rar(nominal);
  let why=[];
  if(useCal && cat==='weapons'){ const c=calMul(cls); f*=c.m; why.push(c.why); }
  if(useCap && (cat==='containers'||cat==='clothes')){ const c=capMul(cls,cat); f*=c.m; why.push(c.why); }
  if(useCal && cat!=='weapons'){ const c=calMul(cls); if(c.m!==1){ f*=c.m; why.push(c.why); } }
  if(useCap && cat!=='containers'&&cat!=='clothes'){ const c=capMul(cls,cat); if(c.m!==1){ f*=c.m; why.push(c.why); } }
  let buy=Math.round(f); buy=Math.round(buy/10)*10;
  if(buy<5)buy=5; if(buy>50000)buy=50000;
  return {buy,sell:Math.max(1,Math.round(buy*0.10)),why:why.join(';')};
}
const want=[['weapons','AK101'],['weapons','Mosin9130'],['weapons','SVD'],['magazines','Mag_AK101_30Rnd'],['magazines','Mag_CMAG_40rnd'],['ammo','Ammo_556x45'],['ammo','Ammo_308Win'],['clothes','AliceBag_ColorBase'],['clothes','GorkaEJacket_Flat_ColorBase'],['containers','LargeTent'],['containers','MediumTent'],['tools','Hatchet'],['tools','PipeWrench'],['food','Apple'],['food','BakedBeansCan'],['explosives','Plastic_Explosive'],['medical','BandageDressing']];
console.log('=== пример новой экономики (buy / sell) ===');
console.log('предмет'.padEnd(30)+'кат'.padEnd(12)+'usage'.padEnd(12)+'tier'.padEnd(8)+'nom'.padEnd(5)+'покупка'.padEnd(9)+'продажа'.padEnd(9)+'множители');
for(const [cat,cls] of want){
  const t=T[cls]; if(!t){ console.log('  '+cls.padEnd(28)+'(нет в types.xml)'); continue; }
  const p=price(cls,cat,t.usage,t.tier,t.nominal,true,true);
  console.log('  '+cls.padEnd(28)+cat.padEnd(12)+(t.usage||'-').padEnd(12)+(t.tier||'-').padEnd(8)+String(t.nominal||'-').padEnd(5)+String(p.buy).padEnd(9)+String(p.sell).padEnd(9)+p.why);
}
// сводка по всем кандидатам
const TRADE=new Set(['weapons','magazines','ammo','attachments','clothes','containers','food','medical','tools','explosives','dishes']);
let cnt=0,sum=0,min=0,max=0,hist={};
for(const k in T){ const t=T[k]; if(!TRADE.has(t.cat)) continue; if(k.charAt(0)==='#') continue;
  const p=price(k,t.cat,t.usage,t.tier,t.nominal,true,true); cnt++; sum+=p.buy; if(!min||p.buy<min)min=p.buy; if(p.buy>max)max=p.buy;
  const b=p.buy<100?'<100':(p.buy<500?'100-500':(p.buy<2000?'500-2k':(p.buy<10000?'2k-10k':'10k+'))); hist[b]=(hist[b]||0)+1; }
console.log('');
console.log('всего торгуемых классов в types.xml: '+cnt+'   средняя покупка: '+Math.round(sum/cnt)+'   min '+min+'   max '+max);
console.log('распределение: '+Object.entries(hist).map(([k,v])=>k+'='+v).join(', '));
