
const fs = require('fs');
const CFG = 'D:/steam/steamapps/common/DayZServer/Profiles/Trader_NPC_Prof/TraderConfig.txt';
const lines = fs.readFileSync(CFG, 'utf8').split(/\r?\n/);
const rows = [];
for (const ln of lines) {
  const t = ln.trim();
  if (!t || t.startsWith('//')) continue;
  const f = t.split(',').map(s => s.trim());
  const ai = f.findIndex(x => /^Ammo_/i.test(x.replace(/^#/, '')));
  if (ai < 0) continue;
  const nums = f.slice(ai + 1).filter(x => /^-?\d+$/.test(x)).map(Number);
  if (nums.length >= 3) rows.push({ cls: f[ai], qty: nums[0], buy: nums[1], sell: nums[2] });
}
console.log('строк с Ammo_* в конфиге: ' + rows.length);
const unit = rows.filter(r => r.qty === 1 && r.sell > 0).sort((a, b) => a.sell - b.sell);
console.log('строк "за 1 патрон" с ценой продажи: ' + unit.length);
console.log('--- 6 самых слабых патронов (что торговец платит за 1 шт.) ---');
for (const r of unit.slice(0, 6)) console.log('  ' + r.cls + '  продажа ' + r.sell + '  покупка ' + r.buy);
console.log('\nИТОГ: награда за одного заражённого = ' + (unit.length ? unit[0].sell : 'нет данных') + ' (цена ' + (unit.length ? unit[0].cls : '?') + ')');
