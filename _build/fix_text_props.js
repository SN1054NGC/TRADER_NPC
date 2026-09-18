
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const L='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
const OK_TEXTPROPS={TextWidgetClass:1,MultilineTextWidgetClass:1,RichTextWidgetClass:1,MultilineEditBoxWidgetClass:1};
const EDITBOX={EditBoxWidgetClass:1};

function absHeights(file){
  const res=parseLayout(fs.readFileSync(L+file,'utf8'));
  const out={};
  function rec(n,pw,ph,ox,oy,cell){
    const r=rectOf(n,pw,ph);
    const l=ox+r.left+(cell?cell.left:0), t=oy+r.top+(cell?cell.top:0);
    out[n.name]={h:r.height,w:r.width,l:l,t:t};
    if(kindOf(n.cls)==='spacer'){ const cs=spacerCells(n,r.width,r.height);
      for(let i=0;i<n.children.length;i++){ const c=cs[i]; if(c) rec(n.children[i],c.width,c.height,l,t,c); else rec(n.children[i],r.width,r.height,l,t,null);} }
    else for(const c of n.children) rec(c,r.width,r.height,l,t,null);
  }
  const sz=res.roots.length? (file==='TraderMenu.layout'? [1300,990]:[1920,1080]) : [1920,1080];
  for(const r of res.roots) rec(r,sz[0],sz[1],0,0,null);
  return out;
}

for(const f of fs.readdirSync(L).filter(x=>x.endsWith('.layout'))){
  const H=absHeights(f);
  const lines=fs.readFileSync(L+f,'utf8').split('\n');
  const stack=[]; let depth=0; let removed=0, added=0;
  const out=[];
  for(let i=0;i<lines.length;i++){
    const raw=lines[i], t=raw.trim();
    if(t===''||t.startsWith('//')){ out.push(raw); continue; }
    const isOpen=/^[A-Za-z_]\w*Class\s+(\w+)\s*\{$/.exec(t);
    if(isOpen){ stack.push({cls:isOpen[1], name:isOpen[2], children:false, depth:depth}); depth++; out.push(raw); continue; }
    if(t==='{'){ if(stack.length) stack[stack.length-1].children=true; depth++; out.push(raw); continue; }
    if(t==='}'){ depth--; if(stack.length && stack[stack.length-1].depth===depth) stack.pop(); out.push(raw); continue; }
    const cur=stack.length?stack[stack.length-1]:null;
    if(cur && !cur.children){
      const m=t.match(/^"([^"]+)"\s+(.*)$/);
      const key=m?m[1]:t.split(/\s+/)[0];
      const allow = OK_TEXTPROPS[cur.cls] ? true : (EDITBOX[cur.cls] ? (key==='exact text') : false);
      if((key==='exact text'||key==='exact text size') && !allow){
        removed++;
        if(key==='exact text size' && cur.cls==='ButtonWidgetClass'){
          const hh=H[cur.name]?H[cur.name].h:44;
          const want=parseFloat(m?m[2]:'22');
          let prop=Math.round((want/Math.max(1,hh))*100)/100;
          if(prop<0.2)prop=0.2; if(prop>0.9)prop=0.9;
          out.push(raw.replace(t, 'text_proportion '+prop));
          added++;
        }
        continue; // строку не переносим
      }
    }
    out.push(raw);
  }
  const txt=out.join('\n');
  if(removed||added){ fs.writeFileSync(L+f,txt); console.log(f+': убрано несовместимых свойств '+removed+', добавлено text_proportion '+added); }
  else console.log(f+': нечего чистить');
}
