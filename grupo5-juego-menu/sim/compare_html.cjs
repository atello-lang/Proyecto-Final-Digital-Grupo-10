/* Ejecuta el JS original sin navegador. Solo intercepta fillRect entero sobre
   64x64; no prueba CSS, eventos del navegador ni temporización real. */
const fs=require('node:fs'), vm=require('node:vm'), cp=require('node:child_process');
const assert=require('node:assert/strict');
const palette=['#000000','#0000ff','#00ff00','#00ffff','#ff0000','#ff00ff','#ffff00','#ffffff'];
const pixels=new Uint8Array(4096);
const cx={fillStyle:'#000000',fillRect(x,y,w,h){
  const color=palette.indexOf(this.fillStyle); assert(color>=0);
  for(let yy=y;yy<y+h;yy++)for(let xx=x;xx<x+w;xx++)
    if(xx>=0&&xx<64&&yy>=0&&yy<64)pixels[yy*64+xx]=color;
}};
const element=()=>({getContext:()=>cx,classList:{remove(){},add(){},toggle(){}},
  setAttribute(){},addEventListener(){},style:{}});
const gamepads=[];
const sandbox={document:{getElementById:()=>element(),addEventListener(){}},localStorage:{getItem:()=>0,setItem(){}},
  navigator:{getGamepads:()=>gamepads},addEventListener(){},requestAnimationFrame(){}};
vm.createContext(sandbox);
const html=fs.readFileSync('docs/referencia-html.html','utf8');
vm.runInContext(html.match(/<script>([\s\S]*?)<\/script>/)[1],sandbox);
const raw=cp.execFileSync('./build/render_reference');
let index=0;
function compare(name,code,exclude){
  vm.runInContext(code,sandbox);
  const actual=raw.subarray(index*4096,++index*4096);assert.equal(actual.length,4096);
  let checked=0, differences=0, excluded=0;
  for(let y=0;y<64;y++)for(let x=0;x<64;x++){
    if(exclude(x,y)){excluded++;continue;}
    checked++; if(actual[y*64+x]!==pixels[y*64+x])differences++;
  }
  console.log(`${name}: ${checked} comparados, ${excluded} excluidos, ${differences} diferencias`);
  assert.equal(differences,0,name);
}
// Solo el pie compacto difiere del menú HTML.
compare('menu',"menu()",(_x,y)=>y>=47);
for(const n of [1,2]){
  vm.runInContext(`playerCount=${n};selected='pac';start();state.score=67`,sandbox);
  // Comparación completa: sprites, fantasmas y HUD, sin exclusiones.
  const directions=[[0,-1],[1,0],[0,1],[-1,0]];
  for(let dir=0;dir<4;dir++)for(let frame=0;frame<2;frame++)
    compare(`PAC-MAN ${n}J dirección ${dir} fase ${frame}`,
      `state.dir=${JSON.stringify([directions[dir],directions[(dir+2)%4]])};ticks=${frame};draw()`,()=>false);
  vm.runInContext(`playerCount=${n};selected='snake';start()`,sandbox);
  // HUD extendido C: tres cifras y SOLO; arena se compara completa.
  compare(`snake ${n}J inicial`,'draw()',(_x,y)=>y>=56);
  compare(`snake ${n}J paso 1`,'ticks++;snakeTick();draw()',(_x,y)=>y>=56);
}
assert.equal(index*4096,raw.length);
function checkGamepad(code){
  vm.runInContext(code,sandbox);
}
const pad=(x=0,y=0,pressed=false)=>({axes:[x,y],buttons:[{pressed}]});
gamepads[0]=pad();
gamepads[1]=pad();
checkGamepad('menu(); pollGamepads()');
gamepads[0]=pad(0,1);checkGamepad('pollGamepads()');
checkGamepad("if(selected!=='snake')throw Error('J1 Y did not select Snake')");
gamepads[0]=pad();checkGamepad('pollGamepads()');
gamepads[0]=pad(0,0,true);checkGamepad('pollGamepads()');
checkGamepad("if(menuStage!=='players')throw Error('J1 button did not confirm game')");
gamepads[0]=pad();checkGamepad('pollGamepads()');
gamepads[0]=pad(1,0);checkGamepad('pollGamepads()');
checkGamepad("if(playerCount!==2)throw Error('J1 X did not select two players')");
gamepads[0]=pad();checkGamepad('pollGamepads()');
gamepads[0]=pad(0,0,true);checkGamepad('pollGamepads()');
checkGamepad("if(mode!=='play'||state.snakes.length!==2)throw Error('J1 button did not start two-player Snake')");
gamepads[0]=pad();checkGamepad('pollGamepads()');
checkGamepad('state.snakes[1].next=[0,1]');
gamepads[1]=pad(-1,0);checkGamepad('pollGamepads()');
checkGamepad("if(state.snakes[1].next[0]!==-1)throw Error('J2 axis did not control player 2')");
gamepads[1]=pad();checkGamepad('pollGamepads()');
checkGamepad('state.snakes[0].score=7');
gamepads[1]=pad(0,0,true);checkGamepad('pollGamepads()');
checkGamepad("if(state.snakes[0].score!==0||mode!=='play')throw Error('J2 button did not restart')");
gamepads[0]=pad(0,0,true);checkGamepad('pollGamepads()');
checkGamepad("if(mode!=='menu')throw Error('Both buttons did not return to menu')");
console.log('Gamepad controls: menu, two-player start, J2 movement and return-to-menu passed');
