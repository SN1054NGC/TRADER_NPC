
const fs=require('fs');
const TYPES='D:/steam/steamapps/common/DayZServer/mpmissions/dayzOffline.Lux/db/types.xml';
const OUT='D:/DAYZDISKP/@TRADER_NPC/_build/log/TraderConfig_auto_preview.txt';
const tx=fs.readFileSync(TYPES,'utf8'); const T={};
for(const m of tx.matchAll(/<type\s+name="([^"]+)"\s*>([\s\S]*?)<\/type>/g)){
  const name=m[1], body=m[2];
  const num=(tag)=>{ const r=new RegExp('<'+tag+'>\\s*(-?\\d+)','i').exec(body); return r?parseInt(r[1],10):null; };
  T[name]={nominal:num('nominal'),
    cat:((new RegExp('<category\\s+name="([^"]+)"','i')).exec(body)||[])[1]||'',
    usage:((new RegExp('<usage\\s+name="([^"]+)"','i')).exec(body)||[])[1]||'',
    tier:((new RegExp('<value\\s+name="([^"]+)"','i')).exec(body)||[])[1]||''};
}
const CAL={},CNT={},SLOTS={},CHB={},WARM={},CAMO={},ISIZE={},ARM={};
(function walk(d){ let es; try{ es=fs.readdirSync(d,{withFileTypes:true}); }catch(e){ return; }
  for(const e of es){ const p=d+'/'+e.name; if(e.isDirectory()) walk(p); else if(e.name==='config.cpp'){
    const t=fs.readFileSync(p,'utf8'); const re=/class\s+([A-Za-z0-9_]+)\s*(?::\s*[A-Za-z0-9_]+)?\s*\{/g; const mk=[]; let m;
    while((m=re.exec(t))!==null) mk.push({n:m[1],s:m.index});
    for(let i=0;i<mk.length;i++){ const body=t.slice(mk[i].s,(i+1<mk.length)?mk[i+1].s:t.length); let r;
      if((r=/\bcaliber\s*=\s*([\d.]+)/.exec(body))) CAL[mk[i].n]=parseFloat(r[1]);
      if((r=/\bcount\s*=\s*([\d]+)/.exec(body))) CNT[mk[i].n]=parseInt(r[1]);
      if((r=/itemsCargoSize\[\]\s*=\s*\{\s*([\d]+)\s*,\s*([\d]+)/.exec(body))) SLOTS[mk[i].n]=parseInt(r[1])*parseInt(r[2]);
      if((r=/chamberableFrom\[\]\s*=\s*\{([^}]*)\}/.exec(body))) CHB[mk[i].n]=r[1].split(',').map(s=>s.trim().replace(/"/g,'')).filter(s=>s&&s!=='NULL');
      if((r=/heatIsolation\s*=\s*([\d.]+)/.exec(body))) WARM[mk[i].n]=parseFloat(r[1]);
      if((r=/visibilityModifier\s*=\s*([\d.]+)/.exec(body))) CAMO[mk[i].n]=parseFloat(r[1]);
      if((r=/itemSize\[\]\s*=\s*\{\s*([\d]+)\s*,\s*([\d]+)/.exec(body))) ISIZE[mk[i].n]=parseInt(r[1])*parseInt(r[2]);
      if((r=/class GlobalArmor[\s\S]{0,600}?damage\s*=\s*([\d.]+)/.exec(body))) ARM[mk[i].n]=1-parseFloat(r[1]);
    } } } })('D:/DAYZDISKP/DZ');
const BASE={weapons:2500,clothes:300,containers:400,tools:200,food:60,explosives:900,medical:120};
const USE={Military:1.6,Police:1.3,Medic:1.2,Underground:1.2,Hunting:1.1,Town:1,Office:0.95,Industrial:0.95,Village:0.9,Coast:0.9,Farm:0.85};
const TIER={Tier1:1,Tier2:1.2,Tier3:1.4,Tier4:1.7,Tier5:2};
const clamp=(v,a,b)=>v<a?a:(v>b?b:v);
const rar=(n)=>(!n||n<=0)?1:(n<=2?1.5:(n<=5?1.25:(n<=12?1.05:0.95)));
function calMul(cls){ let cal=0; for(const a of (CHB[cls]||[]).slice(0,8)) if(CAL[a]>cal) cal=CAL[a]; if(CAL[cls]>cal) cal=CAL[cls];
  if(cal<=0){ const c=CNT[cls]||0; return c>0?clamp(0.8+c/40,0.8,3):1; } return clamp(0.5+cal*1.2,0.4,4); }
function capMul(cls){ let cap=CNT[cls]||0; if(cap<=1&&SLOTS[cls]) cap=SLOTS[cls]; if(cap<=1) return 1; return clamp(0.5+cap*0.04,0.5,4); }
function unitToken(cls){ if(/^Ammo_/i.test(cls)) return '1'; if(CNT[cls]>0) return 'M'; return '1'; }
const TRADE=new Set(['weapons','magazines','ammo','attachments','clothes','containers','food','medical','tools','explosives','dishes']);
const lines=['// ПРЕВЬЮ того, что сгенерирует TraderAutoPrices.c (формула + новые жёсткие кривые)','// подключение: <OpenFile>TraderConfig_auto.txt внутри нужного <Trader>/<Category>',''];
let n=0, min=0,max=0,sum=0;
for(const k in T){ const t=T[k]; if(!TRADE.has(t.cat)) continue; if(k.charAt(0)==='#') continue;
  let f=(BASE[t.cat]||150)*(USE[t.usage]||1)*(TIER[t.tier]||1)*rar(t.nominal);
  if(t.cat==='weapons') f*=calMul(k); else f*=Math.max(1,capMul(k)>1?capMul(k):1);
  if(t.cat==='clothes'){ if(ARM[k]>0) f*=clamp(1+ARM[k]*2.5,1,4); if(WARM[k]>0) f*=clamp(1+WARM[k]*0.6,1,2.5); if(CAMO[k]>0) f*=clamp(1+Math.max(0,1.5-CAMO[k])*0.4,1,2); if(ISIZE[k]>0) f*=clamp(1+ISIZE[k]*0.03,1,2); }
  let buy=Math.round(f); buy=Math.round(buy/10)*10; if(buy<5)buy=5; if(buy>50000)buy=50000;
  const sell=Math.max(1,Math.round(buy*0.10));
  lines.push('        '+k+', '+unitToken(k)+', '+buy+', '+sell+',   // '+t.cat+' / '+(t.usage||'-')+' / '+(t.tier||'-'));
  n++; sum+=buy; if(!min||buy<min)min=buy; if(buy>max)max=buy;
}
fs.writeFileSync(OUT, lines.join('\r\n'), 'utf8');
console.log('превью: '+n+' строк, средняя покупка '+Math.round(sum/n)+', диапазон '+min+'..'+max);
console.log('файл: '+OUT);
console.log('');
console.log('примеры (10 случайных строк из файла):');
for(let i=0;i<10;i++){ const idx=Math.floor((i+1)*lines.length/11); console.log('  '+lines[idx]); }
