
const fs=require('fs');
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
// Убираем ПОВТОРЫ свойств внутри одного виджета, оставляя ПОСЛЕДНЕЕ значение
// (движок применяет свойства по порядку, поэтому последнее и есть рабочее:
// поведение не меняется, но файл становится однозначным).
function dedupe(text){
  const lines=text.split('\n');
  const out=[]; const drop=new Set();
  const stack=[]; // {depth, props:Map, inChildren:bool}
  let depth=0;
  for(let i=0;i<lines.length;i++){
    const raw=lines[i]; const t=raw.trim();
    if(t==='' || t.startsWith('//')){ out.push(raw); continue; }
    const isWidgetOpen=/^[A-Za-z_]\w*Class\s+\w+\s*\{$/.test(t);
    if(isWidgetOpen){ stack.push({depth:depth, props:new Map(), children:false}); depth++; out.push(raw); continue; }
    if(t==='{'){ if(stack.length) stack[stack.length-1].children=true; depth++; out.push(raw); continue; }
    if(t==='}'){ depth--; if(stack.length && stack[stack.length-1].depth===depth) stack.pop(); out.push(raw); continue; }
    // строка свойства: "ключ значение" на верхнем уровне виджета
    const m=t.match(/^(?:"([^"]+)"|([A-Za-z_][\w ]*?))\s+(.*)$/);
    if(m && stack.length && !stack[stack.length-1].children){
      const key=(m[1]!==undefined?m[1]:m[2]);
      const props=stack[stack.length-1].props;
      if(props.has(key)) drop.add(props.get(key));
      props.set(key,i);
    }
    out.push(raw);
  }
  return {text: out.filter((_,i)=>!drop.has(i)).join('\n'), dropped: drop.size};
}
let total=0;
for(const f of fs.readdirSync(L).filter(x=>x.endsWith('.layout'))){
  const src=fs.readFileSync(L+f,'utf8');
  const res=dedupe(src);
  if(res.dropped){ fs.writeFileSync(L+f,res.text); console.log(f+': удалено дублей '+res.dropped); total+=res.dropped; }
  else console.log(f+': дублей нет');
}
console.log('всего удалено: '+total);
