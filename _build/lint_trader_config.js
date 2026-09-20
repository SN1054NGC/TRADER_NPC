
const fs=require('fs');
const CFG=process.argv[2]||'D:/steam/steamapps/common/DayZServer/Profiles/Trader_NPC_Prof/TraderConfig.txt';
const TYPES=process.argv[3]||'D:/steam/steamapps/common/DayZServer/mpmissions/dayzOffline.Lux/db/types.xml';
const REPORT='D:/DAYZDISKP/@TRADER_NPC/_build/log/trader_config_lint.txt';
const out=[]; const say=(s)=>{ out.push(s); console.log(s); };
const tx=fs.readFileSync(TYPES,'utf8'); const T={};
for(const m of tx.matchAll(/<type\s+name="([^"]+)"\s*>([\s\S]*?)<\/type>/g)){
  const name=m[1], body=m[2];
  const num=(tag)=>{ const r=new RegExp('<'+tag+'>\\s*(-?\\d+)','i').exec(body); return r?parseInt(r[1],10):null; };
  T[name]={nominal:num('nominal'),lifetime:num('lifetime'),qmax:num('quantmax'),
    cat:((new RegExp('<category\\s+name="([^"]+)"','i')).exec(body)||[])[1]||'',
    usage:((new RegExp('<usage\\s+name="([^"]+)"','i')).exec(body)||[])[1]||'',
    tier:((new RegExp('<value\\s+name="([^"]+)"','i')).exec(body)||[])[1]||''};
}
const NOT_W=new Set(['clothes','food','containers','medical','explosives']);
const NOT_S=new Set(['clothes','tools','containers','weapons']);
const raw=fs.readFileSync(CFG,'utf8').split(/\r?\n/);
let trader='',category='',lineNo=0,coef='default';
const rows=[],err=[],warn=[],info=[];
for(const line of raw){
  lineNo++; let s=line; const ci=s.indexOf('//'); if(ci>=0) s=s.slice(0,ci); s=s.trim();
  if(!s) continue;
  if(/^<FileEnd>/i.test(s)) break;
  let m;
  if((m=/^<Trader>\s*(.+)$/i.exec(s))){ trader=m[1].trim(); category=''; coef='default'; continue; }
  if((m=/^<Category>\s*(.+)$/i.exec(s))){ category=m[1].trim(); coef='default'; continue; }
  if((m=/^<SellCoef>\s*([\d.]+)/i.exec(s))){ coef=m[1]; info.push({line:lineNo,msg:'SellCoef '+coef+' для '+(category?('категории '+category):('трейдера '+trader))}); continue; }
  if(/^<Currency(Name)?>/i.test(s)) continue;
  if(/^<OpenFile>/i.test(s)){ info.push({line:lineNo,msg:'включение файла: '+s}); continue; }
  if(s.startsWith('<')) continue;
  const f=s.split(',').map(x=>x.trim());
  if(f.length<3){ err.push({line:lineNo,msg:'полей меньше 3: "'+s.slice(0,60)+'"'}); continue; }
  const cls=f[0], qty=(f[1]||'').toUpperCase();
  let buy=parseInt(f[2],10), sell=(f.length>=4?parseInt(f[3],10):null);
  if(f[2]===''||isNaN(buy)){ err.push({line:lineNo,msg:'цена покупки не число ("'+f[2]+'", '+cls+')'}); buy=null; }
  if(f.length>=4 && f[3]!=='*' && isNaN(sell)){ err.push({line:lineNo,msg:'цена продажи не число ("'+f[3]+'", '+cls+')'}); sell=null; }
  const t=T[cls];
  if(!t && cls.charAt(0)!=='#') info.push({line:lineNo,msg:'нет в types.xml: '+cls});
  else if(t){
    if(qty==='W' && NOT_W.has(t.cat)) warn.push({line:lineNo,msg:'токен W, а категория '+t.cat+': '+cls});
    if(qty==='S' && NOT_S.has(t.cat)) warn.push({line:lineNo,msg:'токен S, а категория '+t.cat+': '+cls});
  }
  if(buy!==null && buy!==-1 && buy<=0) warn.push({line:lineNo,msg:'цена покупки <= 0: '+cls});
  if(sell!==null && sell!==-1 && sell<0) warn.push({line:lineNo,msg:'цена продажи < 0: '+cls});
  if(buy!==null && sell!==null && buy>0 && sell>buy) warn.push({line:lineNo,msg:'продажа '+sell+' > покупки '+buy+': '+cls});
  rows.push({line:lineNo,trader,category,cls,qty,buy,sell,coef});
}
const seen={},dups=[];
for(const r of rows){ const k=r.trader+'|'+r.category+'|'+r.cls; if(seen[k]) dups.push({line:r.line,msg:'дубль: '+r.trader+' / '+r.category+' / '+r.cls}); seen[k]=1; }
// ---- ПРАВИЛО: дубли = денежный насос (купить по дешёвой строке, продать по дорогой) ----
if (dups.length){ say('--- ДУБЛИ как ОШИБКА: '+dups.length+' ---'); for(const i of dups) err.push(i); }
// ---- ПРАВИЛО: межтрейдеровый АРБИТРАЖ (sell одного > buy другого) ----
const byCls={};
for(const r of rows){ if(!byCls[r.cls]) byCls[r.cls]={minBuy:0,maxSell:0,buyAt:'',sellAt:''};
  const e=byCls[r.cls];
  if(r.buy>0 && (e.minBuy===0 || r.buy<e.minBuy)){ e.minBuy=r.buy; e.buyAt=r.trader; }
  if(r.sell>0 && r.sell>e.maxSell){ e.maxSell=r.sell; e.sellAt=r.trader; } }
const arb=[];
for(const c in byCls){ const e=byCls[c];
  if(e.minBuy>0 && e.maxSell>e.minBuy) arb.push({line:0,msg:'АРБИТРАЖ: '+c+' - купить у "'+e.buyAt+'" за '+e.minBuy+', продать "'+e.sellAt+'" за '+e.maxSell+' (+'+(e.maxSell-e.minBuy)+')'}); }
for(const i of arb) err.push(i);
const listed=new Set(rows.map(r=>r.cls));
const TRADE=new Set(['weapons','magazines','ammo','attachments','clothes','containers','food','medical','tools','explosives','dishes']);
const cand={};
for(const k in T){ const t=T[k]; if(!TRADE.has(t.cat)) continue; if(k.charAt(0)==='#') continue; if(listed.has(k)) continue; const key=t.cat+'/'+(t.usage||'-'); cand[key]=(cand[key]||0)+1; }
const noTypes=info.filter(i=>/нет в types.xml/.test(i.msg));
say('=== ЛИНТЕР TraderConfig  (v2: учитывает -1, 3 поля и <SellCoef>) ===');
say('файл: '+CFG);
say('types.xml: '+Object.keys(T).length+' типов');
say('строк-итемов: '+rows.length+'   трейдеров: '+new Set(rows.map(r=>r.trader)).size+'   категорий: '+new Set(rows.map(r=>r.category)).size);
say('ОШИБКИ: '+err.length+'   ПРЕДУПРЕЖДЕНИЯ: '+warn.length+'   ДУБЛИ: '+dups.length);
const tok={}; for(const r of rows) tok[r.qty]=(tok[r.qty]||0)+1;
say('токены: '+Object.entries(tok).sort((a,b)=>b[1]-a[1]).map(([k,v])=>k+'x'+v).join(', '));
say('строк из 3 полей (цена продажи по коэф.): '+rows.filter(r=>r.sell===null||r.sell===undefined).length);
say('строк с ценой -1 (не торгуется в эту сторону): '+rows.filter(r=>r.buy===-1||r.sell===-1).length);
say('');
if(err.length){ say('--- ОШИБКИ ---'); for(const i of err) say('  стр.'+i.line+'  '+i.msg); }
if(dups.length){ say('--- ДУБЛИ: '+dups.length+' (первые 12) ---'); for(const i of dups.slice(0,12)) say('  стр.'+i.line+'  '+i.msg); }
if(warn.length){ say('--- ПРЕДУПРЕЖДЕНИЯ: '+warn.length+' (первые 20) ---'); for(const i of warn.slice(0,20)) say('  стр.'+i.line+'  '+i.msg); }
say('');
say('--- ИНФО ---');
say('  нет в types.xml: '+noTypes.length+'   <SellCoef>: '+info.filter(i=>/SellCoef/.test(i.msg)).length+'   <OpenFile>: '+info.filter(i=>/OpenFile/.test(i.msg)).length);
say('  кандидатов на авто-цены: '+Object.values(cand).reduce((a,b)=>a+b,0)+'  ('+Object.entries(cand).sort((a,b)=>b[1]-a[1]).slice(0,8).map(([k,v])=>k+'='+v).join(', ')+')');
fs.writeFileSync(REPORT,out.join('\r\n'),'utf8');
say('');
say('отчёт: '+REPORT);