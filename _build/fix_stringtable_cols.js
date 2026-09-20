
const fs=require('fs');
const P='D:/DAYZDISKP/@TRADER_NPC/addons/TRADER_NPC/languagecore/stringtable.csv';
// column order of the vanilla header:
// Language, original, english, czech, german, russian, polish, hungarian, italian, spanish, french, chinese, japanese, portuguese, chinesesimp
const EN=(s)=>s, rows={
 tm_money:            ['Money','Money','Peníze','Geld','Деньги','Pieniądze','Money','Money','Dinero','Argent','金钱','お金','Dinheiro','金錢'],
 tm_quantity:         ['Quantity','Quantity','Množství','Menge','Количество','Ilość','Quantity','Quantity','Cantidad','Quantité','数量','数量','Quantidade','數量'],
 tm_quality:          ['Condition','Condition','Stav','Zustand','Состояние','Stan','Condition','Condition','Estado','État','状态','状態','Condição','狀態'],
 tm_cargo_size:       ['Cargo size','Cargo size','Velikost nákladu','Ladekapazität','Объём груза','Pojemność','Cargo size','Cargo size','Tamano de carga','Taille du chargement','容量','容量','Tamanho da carga','容量'],
 tm_cond_pristine:    ['Pristine','Pristine','Bezchybný','Makellos','Идеальное','Idealny','Pristine','Pristine','Impecable','Impeccable','完好','新品同様','Impecável','完好無損'],
 tm_cond_worn:        ['Worn','Worn','Opotřebovaný','Abgenutzt','Поношенное','Zużyty','Worn','Worn','Desgastado','Usé','磨损','使用感あり','Desgastado','磨損'],
 tm_cond_damaged:     ['Damaged','Damaged','Poškozený','Beschädigt','Повреждённое','Uszkodzony','Damaged','Damaged','Dañado','Endommagé','损坏','損傷','Danificado','損壞'],
 tm_cond_badly:       ['Badly damaged','Badly damaged','Silně poškozený','Stark beschädigt','Сильно повреждённое','Mocno uszkodzony','Badly damaged','Badly damaged','Muy dañado','Très endommagé','严重损坏','重度損傷','Muito danificado','嚴重損壞'],
 tm_cond_ruined:      ['Ruined','Ruined','Zničený','Ruiniert','Разрушенное','Zniszczony','Ruined','Ruined','Arruinado','Ruiné','毁坏','破壊','Arruinado','毀壞'],
 tm_owned:            ['Owned','Owned','Vlastněno','Besitz','В наличии','Posiadane','Owned','Owned','En posesión','Possédé','拥有','所持','Possui','擁有'],
 tm_sell_amount:      ['Sell','Sell','Prodat','Verkaufen','Продать','Sprzedaj','Sell','Sell','Vender','Vendre','出售','売却','Vender','出售'],
 tm_sell_hint:        ['Right mouse button - sell one','Right mouse button - sell one','Pravé tlačítko myši - prodat','Rechte Maustaste - verkaufen','ПКМ - продать','PPM - sprzedaj','Right mouse button - sell one','Right mouse button - sell one','Boton derecho - vender','Clic droit - vendre','右键 - 出售','右クリック - 売却','Botão direito - vender','右鍵 - 出售'],
 tm_sellables_only:   ['Only sellable','Only sellable','Pouze prodejné','Nur Verkaufbares','Только продаваемое','Tylko sprzedawalne','Only sellable','Only sellable','Solo vendible','Vendable uniquement','仅可出售','売却可能のみ','Apenas vendável','僅可出售'],
 tm_bonus_breakdown:  ['with attachments','with attachments','s příslušenstvím','mit Zubehör','с обвесом','z dodatkami','with attachments','with attachments','con accesorios','avec accessoires','含配件','付属品込み','com acessórios','含配件'],
 tm_ragged_not_bought:['The trader does not buy this','The trader does not buy this','Obchodník toto nekupuje','Der Händler kauft das nicht','Торговец это не покупает','Trader tego nie kupuje','The trader does not buy this','The trader does not buy this','El comerciante no compra esto','Le marchand n achete pas ceci','商人不收购','商人は買い取らない','O comerciante não compra isto','商人不收購'],
 tm_trader:           ['Trader','Trader','Obchodník','Händler','Торговец','Trader','Trader','Trader','Comerciante','Marchand','商人','商人','Comerciante','商人']
};
const lines=fs.readFileSync(P,'utf8').split('\n');
let fixed=0;
for(let i=0;i<lines.length;i++){
  const m=lines[i].match(/^"(tm_[a-z_]+)"/);
  if(!m) continue;
  const key=m[1];
  if(!rows[key]) continue;
  const f=rows[key];
  lines[i]='"'+key+'","'+f.slice(0,14).join('","')+'",';
  fixed++;
}
fs.writeFileSync(P,lines.join('\n'));
console.log('rows rewritten: '+fixed);
