
const fs=require('fs');
const html=fs.readFileSync('D:/DAYZDISKP/@sps_client_bot/layout_viewer.html','utf8');
const a=html.indexOf('// ==PARSER_START=='), b=html.indexOf('// ==PARSER_END==');
var state={loc:true}; var LOC={}; eval(html.slice(a,b));
const dir='D:/DAYZDISKP/@sps_client_bot/addons/sps_client_bot/scripts/layouts/';
const files=fs.readdirSync(dir).filter(f=>f.endsWith('.layout'));

function audit(label, path){
  const res=parseLayout(fs.readFileSync(path,'utf8'));
  let issues=0;
  for(const n of res.all){
    const p=n.props;
    const V=(k,i)=>{ if(p[k]===undefined)return null; const a=String(p[k]).split(' '); const f=parseFloat(a[i]); return isNaN(f)?null:f; };
    const px=(k)=>p[k]==='1'||p[k]==='true';
    const sx=V('size',0), sy=V('size',1), pxx=V('position',0), pyy=V('position',1);
    const hasFa=!!p.fixaspect;
    const ex=sx!==null&&sx>=1.5&&!px('hexactsize')&&!hasFa;
    const ey=sy!==null&&sy>=1.5&&!px('vexactsize')&&!hasFa;
    const epx=pxx!==null&&pxx>=2&&!px('hexactpos');
    const epy=pyy!==null&&pyy>=2&&!px('vexactpos');
    const fx=sx!==null&&sx<=1.2&&sx>0&&px('hexactsize');
    const fy=sy!==null&&sy<=1.2&&sy>0&&px('vexactsize');
    const fpx=pxx!==null&&pxx>0&&pxx<=1.2&&px('hexactpos');
    const fpy=pyy!==null&&pyy>0&&pyy<=1.2&&px('vexactpos');
    if(ex||ey||epx||epy||fx||fy||fpx||fpy){
      issues++;
      const why=[];
      if(ex)why.push('size.x='+sx+' but hexactsize='+p.hexactsize);
      if(ey)why.push('size.y='+sy+' but vexactsize='+p.vexactsize);
      if(epx)why.push('pos.x='+pxx+' but hexactpos='+p.hexactpos);
      if(epy)why.push('pos.y='+pyy+' but vexactpos='+p.vexactpos);
      if(fx)why.push('size.x='+sx+' pixel-flagged');
      if(fy)why.push('size.y='+sy+' pixel-flagged');
      if(fpx)why.push('pos.x='+pxx+' pixel-flagged');
      if(fpy)why.push('pos.y='+pyy+' pixel-flagged');
      console.log('  '+n.name.padEnd(24)+' :: '+why.join(' | '));
    }
  }
  console.log(label.padEnd(34)+' issues='+issues);
  return issues;