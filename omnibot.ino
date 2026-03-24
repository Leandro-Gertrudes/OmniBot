/*
 * ============================================================
 *  MECANUM ROBOT - 42Porto Robotics Club
 * ============================================================
 *
 *  MAPA DE PINOS
 *
 *  L298N #1  (motores da FRENTE) - lado de cima do ESP32
 *    FL_ENA -> D23   FR_ENB -> D13
 *    FL_IN1 -> D22   FR_IN3 -> D18
 *    FL_IN2 -> D2    FR_IN4 -> D4
 *
 *  L298N #2  (motores de TRAS) - lado de baixo do ESP32
 *    BL_ENA -> D32   BR_ENB -> D27
 *    BL_IN1 -> D25   BR_IN3 -> D14
 *    BL_IN2 -> D26   BR_IN4 -> D12
 *
 * ============================================================
 *  ESTRUTURA DO CODIGO
 *
 *  [PARTE 1] Configuracao de pinos e constantes  -> DONE
 *  [PARTE 2] Funcoes auxiliares dos motores      -> DONE
 *  [PARTE 3] Funcoes de movimento               -> Clube
 *  [PARTE 4] Wi-Fi, servidor web, Setup e Loop  -> DONE
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>

// ============================================================
//  PARTE 1 - CONFIGURACAO - DONE
// ============================================================

	const char* AP_SSID = "42Porto-robotics";
	const char* AP_PASS = "robotics42";

	int SPEED = 180;
	const int SPEED_MIN = 80;

	// L298N #1 (frente) - lado de cima do ESP32
	const int FL_ENA = 23;
	const int FL_IN1 = 22;
	const int FL_IN2 = 2;

	const int FR_ENB = 13;
	const int FR_IN3 = 18;
	const int FR_IN4 = 4;

	// L298N #2 (tras) - lado de baixo do ESP32
	const int BL_ENA = 32;
	const int BL_IN1 = 25;
	const int BL_IN2 = 26;

	const int BR_ENB = 27;
	const int BR_IN3 = 14;
	const int BR_IN4 = 12;

	WebServer server(80);

	//const float ROTATION_FACTOR = 0.6; 

// ============================================================
//  PARTE 2 - FUNCOES AUXILIARES DOS MOTORES - DONE
// ============================================================

/*
 *  setMotor - controla um motor individual
 *
 *  pino_en : pino ENA/ENB do L298N (PWM de velocidade)
 *  inA, inB: pinos IN do L298N (direcao)
 *  valor    : -255 a 255
 *             positivo = frente, negativo = tras, 0 = parado
 */
void setMotor(int pino_en, int inA, int inB, int valor) {
	valor = constrain(valor, -255, 255);

	//se valor for zero, motores ficam LOW e retorna a funcao.
	if (valor == 0) {
		digitalWrite(inA, LOW);
		digitalWrite(inB, LOW);
		ledcWrite(pino_en, 0);
		return;
	}

	int pwm = abs(valor); // o valor de pwm precisa ser positivo.

  // === KICK START ===
	// Kickstart - se o valor da velocidade for muito baixo o carrinho nao consegue vencer a inercia e se mover.
	// Entao damos um impulso para ele começar a andar e depois voltaos à velocidade escolhida.
															// caso o pwd seja menor do que speedmin
	// if (pwm < SPEED_MIN) {
	// 	digitalWrite(inA, valor > 0 ? HIGH : LOW);
	// 	digitalWrite(inB, valor > 0 ? LOW : HIGH);
	// 	ledcWrite(pino_en, SPEED_MIN);
	// 	delay(80);
	// }

	if(valor > 0){
		digitalWrite(inA, HIGH);
		digitalWrite(inB, LOW);
		ledcWrite(pino_en, pwm);
	}
	else {
		digitalWrite(inA, LOW);
		digitalWrite(inB, HIGH);
		ledcWrite(pino_en, pwm);
	}
}

// Atalhos para cada motor
void motorFL(int v) { setMotor(FL_ENA, FL_IN1, FL_IN2, v); }
void motorFR(int v) { setMotor(FR_ENB, FR_IN3, FR_IN4, v); }
void motorBL(int v) { setMotor(BL_ENA, BL_IN1, BL_IN2, v); }
void motorBR(int v) { setMotor(BR_ENB, BR_IN3, BR_IN4, v); }


// ============================================================
//  PARTE 3 - FUNCOES DE MOVIMENTO  ** TAREFA **
// ============================================================
/*
 *  Referencia o das rodas mecanum:
 *            
 *    FL ( )------( ) FR 
 *        |        |
 *        |  CAR   |
 *        |        |
 *    BL ( )------( ) BR 
 *
 *  Valor positivo  = roda gira para FRENTE
 *  Valor negativo  = roda gira para TRAS
 *  0               = roda parada
 *
 *  Exemplo - FRENTE:
 *    motorFL(SPEED); motorFR(SPEED); motorBL(SPEED); motorBR(SPEED);
 *
 *
 *  A variavel SPEED contem a velocidade atual (0-255)
 *  definida pelo slider na pagina web.
 */

void mover_frente() {
	motorFL(SPEED);
	motorFR(SPEED);
	motorBL(SPEED);
	motorBR(SPEED);
}

void mover_tras() {
  // ** TAREFA: implementar
}

void mover_direita() {
  // ** TAREFA: implementar
}

void mover_esquerda() {
  // ** TAREFA: implementar
}

void diagonal_frente_direita() {
  // ** TAREFA: implementar
  motorFL(SPEED);
  motorFR(0);
  motorBL(0);
  motorBR(SPEED);
}

void diagonal_frente_esquerda() {
	motorFL(0);
	motorFR(SPEED);
	motorBL(SPEED);
	motorBR(0);
}

void diagonal_tras_direita() {
  // ** TAREFA: implementar
}

void diagonal_tras_esquerda() {
 	motorFL(-SPEED);
	motorFR(0);
	motorBL(0);
	motorBR(-SPEED);
}

void girar_direita() {
  // ** TAREFA: implementar 
}

void girar_esquerda() {
	motorFL(-SPEED);
	motorFR(SPEED);
	motorBL(-SPEED);
	motorBR(SPEED);
}

void parar() {
  motorFL(0);
  motorFR(0);
  motorBL(0);
  motorBR(0);
}


// ============================================================
//  PARTE 4 - WI-FI, SERVIDOR WEB, SETUP E LOOP
// ============================================================

const char PAGE_HTML_1[] PROGMEM = R"HTMLEND(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>Mecanum - 42Porto</title>
<link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Rajdhani:wght@500;700;900&display=swap" rel="stylesheet">
<style>
  :root{
    --bg:#080c10;--panel:#0d1520;--border:#1a2d45;
    --accent:#00d4ff;--accent2:#ff6b35;--green:#39ff14;--dim:#3a5570;
    --red:#ff2244;--glow:0 0 12px rgba(0,212,255,0.6);
  }
  *{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent;}
  body{
    background:var(--bg);color:#a8c8e8;font-family:'Share Tech Mono',monospace;
    min-height:100vh;display:flex;flex-direction:column;align-items:center;
    padding:16px;gap:14px;
    background-image:
      repeating-linear-gradient(0deg,transparent,transparent 39px,rgba(0,212,255,0.03) 39px,rgba(0,212,255,0.03) 40px),
      repeating-linear-gradient(90deg,transparent,transparent 39px,rgba(0,212,255,0.03) 39px,rgba(0,212,255,0.03) 40px);
  }
  #sigOverlay{position:fixed;inset:0;z-index:999;display:none;align-items:center;justify-content:center;background:rgba(4,6,10,0.93);backdrop-filter:blur(8px);}
  #sigOverlay.on{display:flex;animation:fadeIn .3s ease;}
  @keyframes fadeIn{from{opacity:0}to{opacity:1}}
  .sig-card{border:2px solid var(--red);border-radius:6px;padding:44px 60px;text-align:center;background:rgba(8,3,6,.97);box-shadow:0 0 80px rgba(255,34,68,.35);animation:cardpulse 1.6s ease-in-out infinite;}
  @keyframes cardpulse{0%,100%{box-shadow:0 0 40px rgba(255,34,68,.25)}50%{box-shadow:0 0 100px rgba(255,34,68,.6)}}
  .sig-icon{font-size:4rem;display:block;margin-bottom:16px;animation:flash .9s step-start infinite;}
  @keyframes flash{0%,100%{opacity:1}50%{opacity:.1}}
  .sig-title{font-family:'Rajdhani',sans-serif;font-size:2.6rem;font-weight:900;color:var(--red);letter-spacing:8px;text-shadow:0 0 30px rgba(255,34,68,.9);margin-bottom:6px;}
  .sig-sub{font-size:.7rem;color:#774455;letter-spacing:3px;margin-bottom:20px;}
  .sig-bar-w{width:260px;height:3px;background:rgba(255,34,68,.12);border-radius:2px;margin:0 auto 20px;overflow:hidden;}
  .sig-bar{height:100%;background:linear-gradient(90deg,var(--red),#ff7788);animation:scanbar 1.8s linear infinite;}
  @keyframes scanbar{0%{width:0%;margin-left:0%}50%{width:60%;margin-left:40%}100%{width:0%;margin-left:100%}}
  .sig-note{font-size:.62rem;color:#442233;letter-spacing:2px;line-height:2;}
  .sig-badge{margin-top:20px;font-size:.55rem;letter-spacing:3px;color:#332233;border-top:1px solid rgba(255,34,68,.15);padding-top:14px;}
  header{width:100%;max-width:500px;display:flex;align-items:center;justify-content:space-between;border-bottom:1px solid var(--border);padding-bottom:10px;}
  .logo{font-family:'Rajdhani',sans-serif;font-weight:900;font-size:1.6rem;letter-spacing:4px;color:var(--accent);text-shadow:var(--glow);line-height:1;}
  .logo span{color:var(--accent2);}
  .logo-sub{font-size:.55rem;letter-spacing:3px;color:var(--dim);margin-top:3px;}
  .conn{display:flex;align-items:center;gap:8px;font-size:.65rem;}
  .dot{width:8px;height:8px;border-radius:50%;background:var(--dim);transition:background .3s,box-shadow .3s;}
  .dot.on{background:var(--green);box-shadow:0 0 8px var(--green);}
  .dot.err{background:var(--red);box-shadow:0 0 8px var(--red);animation:blink .5s infinite;}
  @keyframes blink{0%,100%{opacity:1}50%{opacity:.2}}
  .panel{background:var(--panel);border:1px solid var(--border);border-radius:4px;padding:16px;width:100%;max-width:500px;position:relative;overflow:hidden;}
  .panel::before{content:'';position:absolute;top:0;left:0;right:0;height:2px;background:linear-gradient(90deg,transparent,var(--accent),transparent);opacity:.5;}
  .panel.orange::before{background:linear-gradient(90deg,transparent,var(--accent2),transparent);}
  .panel-title{font-family:'Rajdhani',sans-serif;font-size:.65rem;letter-spacing:4px;color:var(--dim);margin-bottom:14px;}
  .wheels{display:grid;grid-template-columns:1fr 1fr;gap:8px;height:160px;}
  .wheel{background:rgba(0,0,0,.3);border:1px solid var(--border);border-radius:3px;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:3px;position:relative;overflow:hidden;transition:border-color .15s;}
  .wheel-fill{position:absolute;bottom:0;left:0;right:0;height:0%;transition:height .12s;background:linear-gradient(to top,rgba(0,212,255,.2),transparent);}
  .wheel.fwd .wheel-fill{background:linear-gradient(to top,rgba(57,255,20,.25),transparent);}
  .wheel.bwd .wheel-fill{background:linear-gradient(to top,rgba(255,107,53,.25),transparent);}
  .wlabel{font-size:.6rem;color:var(--dim);letter-spacing:2px;position:relative;z-index:1;}
  .wval{font-family:'Rajdhani',sans-serif;font-size:1.3rem;font-weight:700;color:var(--accent);position:relative;z-index:1;transition:color .12s;}
  .wicon{font-size:.9rem;position:relative;z-index:1;}
  .dpad{display:grid;grid-template-columns:repeat(3,64px);grid-template-rows:repeat(3,64px);gap:5px;margin:0 auto;width:fit-content;}
  .btn{background:var(--panel);border:1px solid var(--border);border-radius:4px;display:flex;align-items:center;justify-content:center;font-size:1.3rem;cursor:pointer;user-select:none;-webkit-user-select:none;transition:all .1s;position:relative;overflow:hidden;}
  .btn::after{content:'';position:absolute;inset:0;background:rgba(0,212,255,.15);opacity:0;transition:opacity .1s;}
  .btn:active::after,.btn.pressed::after{opacity:1;}
  .btn:not(.stop):hover{border-color:var(--accent);}
  .btn.stop{border-color:var(--accent2);color:var(--accent2);font-size:.8rem;font-family:'Rajdhani',sans-serif;letter-spacing:1px;}
  .rot-row{display:flex;gap:8px;justify-content:center;margin-top:8px;}
  .rot-btn{width:64px;height:64px;}
  .slider-row{display:flex;align-items:center;gap:12px;}
  .slider-label{font-size:.7rem;color:var(--dim);letter-spacing:2px;width:60px;}
  input[type=range]{flex:1;-webkit-appearance:none;height:4px;background:var(--border);border-radius:2px;outline:none;cursor:pointer;}
  input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:18px;height:18px;border-radius:50%;background:var(--accent);box-shadow:var(--glow);cursor:pointer;}
  .slider-val{font-family:'Rajdhani',sans-serif;font-size:1.1rem;font-weight:700;color:var(--accent);width:44px;text-align:right;}
</style>
</head>
<body>

<div id="sigOverlay">
  <div class="sig-card">
    <span class="sig-icon">&#128225;</span>
    <div class="sig-title">SIGNAL LOST</div>
    <div class="sig-sub">CONNECTION TO ROBOT INTERRUPTED</div>
    <div class="sig-bar-w"><div class="sig-bar"></div></div>
    <div class="sig-note">SCANNING FOR NETWORK...<br>ROBOT IS NOW UNCONTROLLED</div>
    <div class="sig-badge">42PORTO - ROBOTICS CLUB - CYBERSECURITY SHOWCASE</div>
  </div>
</div>

<header>
  <div>
    <div class="logo">MECA<span>NUM</span></div>
    <div class="logo-sub">42PORTO - ROBOTICS CLUB</div>
  </div>
  <div class="conn">
    <div class="dot" id="dot"></div>
    <span id="connTxt">OFFLINE</span>
  </div>
</header>

<div class="panel orange">
  <div class="panel-title">// WHEEL POWER</div>
  <div class="wheels">
    <div class="wheel" id="wFL"><div class="wheel-fill" id="fFL"></div><div class="wlabel">FL</div><div class="wval" id="vFL">0</div><div class="wicon">&#11041;</div></div>
    <div class="wheel" id="wFR"><div class="wheel-fill" id="fFR"></div><div class="wlabel">FR</div><div class="wval" id="vFR">0</div><div class="wicon">&#11041;</div></div>
    <div class="wheel" id="wBL"><div class="wheel-fill" id="fBL"></div><div class="wlabel">BL</div><div class="wval" id="vBL">0</div><div class="wicon">&#11041;</div></div>
    <div class="wheel" id="wBR"><div class="wheel-fill" id="fBR"></div><div class="wlabel">BR</div><div class="wval" id="vBR">0</div><div class="wicon">&#11041;</div></div>
  </div>
</div>

<div class="panel">
  <div class="panel-title">// DIRECTIONAL CONTROL</div>
  <div class="dpad">
    <div class="btn" data-cmd="ul">&#8598;</div>
    <div class="btn" data-cmd="fwd">&#9650;</div>
    <div class="btn" data-cmd="ur">&#8599;</div>
    <div class="btn" data-cmd="left">&#9664;</div>
    <div class="btn stop" data-cmd="stop">STOP</div>
    <div class="btn" data-cmd="right">&#9654;</div>
    <div class="btn" data-cmd="dl">&#8601;</div>
    <div class="btn" data-cmd="bwd">&#9660;</div>
    <div class="btn" data-cmd="dr">&#8600;</div>
  </div>
  <div class="rot-row">
    <div class="btn rot-btn" data-cmd="rot_l">&#8634;</div>
    <div class="btn rot-btn" data-cmd="rot_r">&#8635;</div>
  </div>
</div>

<div class="panel orange">
  <div class="panel-title">// SPEED</div>
  <div class="slider-row">
    <span class="slider-label">DRIVE</span>
    <input type="range" id="spSlider" min="0" max="255" value="180" oninput="setSpeed(this.value)">
    <span class="slider-val" id="spVal">180</span>
  </div>
</div>

)HTMLEND";

const char PAGE_HTML_2[] PROGMEM = R"HTMLEND(
<script>
var FAIL_MAX = 4;
var fails = 0;
var sigLost = false;
var connected = false;
var interval = null;

function onOk() {
  fails = 0;
  if (sigLost) {
    sigLost = false;
    document.getElementById('sigOverlay').classList.remove('on');
    setDot('on');
    document.getElementById('connTxt').textContent = 'ONLINE';
  }
  if (!connected) {
    connected = true;
    setDot('on');
    document.getElementById('connTxt').textContent = 'ONLINE';
  }
}

function onFail() {
  fails++;
  if (fails >= FAIL_MAX && !sigLost) {
    sigLost = true;
    document.getElementById('sigOverlay').classList.add('on');
    setDot('err');
    document.getElementById('connTxt').textContent = 'SIGNAL LOST';
  }
}

function setDot(cls) {
  document.getElementById('dot').className = 'dot ' + cls;
}

var cmdWheels = {
  fwd:   [ 1, 1, 1, 1],
  bwd:   [-1,-1,-1,-1],
  left:  [-1, 1, 1,-1],
  right: [ 1,-1,-1, 1],
  ul:    [ 0, 1, 1, 0],
  ur:    [ 1, 0, 0, 1],
  dl:    [-1, 0, 0,-1],
  dr:    [ 0,-1,-1, 0],
  rot_l: [-1, 1,-1, 1],
  rot_r: [ 1,-1, 1,-1],
  stop:  [ 0, 0, 0, 0]
};

var wIds = ['FL','FR','BL','BR'];

function updateWheels(cmd) {
  var vals = cmdWheels[cmd] || [0,0,0,0];
  for (var i = 0; i < wIds.length; i++) {
    var id = wIds[i];
    var pct = Math.abs(vals[i]) * 100;
    var dir = vals[i] > 0 ? 'fwd' : vals[i] < 0 ? 'bwd' : '';
    document.getElementById('w'+id).className = 'wheel ' + dir;
    document.getElementById('f'+id).style.height = pct + '%';
    document.getElementById('v'+id).textContent = pct;
    document.getElementById('v'+id).style.color = dir === 'fwd' ? 'var(--green)' : dir === 'bwd' ? 'var(--accent2)' : 'var(--accent)';
  }
}

function send(cmd) {
  updateWheels(cmd);
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/cmd?action=' + cmd, true);
  xhr.timeout = 400;
  xhr.onload = function() { if (xhr.status === 200) onOk(); else onFail(); };
  xhr.onerror = function() { onFail(); };
  xhr.ontimeout = function() { onFail(); };
  xhr.send();
}

function setSpeed(v) {
  document.getElementById('spVal').textContent = v;
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/speed?v=' + v, true);
  xhr.send();
}

var btns = document.querySelectorAll('.btn[data-cmd]');
for (var i = 0; i < btns.length; i++) {
  (function(btn) {
    btn.addEventListener('mousedown', function(e) {
      e.preventDefault();
      btn.classList.add('pressed');
      send(btn.dataset.cmd);
      clearInterval(interval);
      interval = setInterval(function() { send(btn.dataset.cmd); }, 100);
    });
    btn.addEventListener('touchstart', function(e) {
      e.preventDefault();
      btn.classList.add('pressed');
      send(btn.dataset.cmd);
      clearInterval(interval);
      interval = setInterval(function() { send(btn.dataset.cmd); }, 100);
    }, {passive: false});
    function release(e) {
      e.preventDefault();
      btn.classList.remove('pressed');
      clearInterval(interval);
      if (btn.dataset.cmd !== 'stop') send('stop');
    }
    btn.addEventListener('mouseup', release);
    btn.addEventListener('touchend', release, {passive: false});
    btn.addEventListener('mouseleave', release);
  })(btns[i]);
}

var keys = {
  ArrowUp:'fwd', ArrowDown:'bwd', ArrowLeft:'left', ArrowRight:'right',
  w:'fwd', s:'bwd', a:'left', d:'right',
  q:'ul', e:'ur', z:'dl', c:'dr',
  j:'rot_l', l:'rot_r'
};
var held = {};

document.addEventListener('keydown', function(e) {
  var cmd = keys[e.key];
  if (!cmd || held[e.key]) return;
  e.preventDefault();
  held[e.key] = true;
  var btn = document.querySelector('[data-cmd="' + cmd + '"]');
  if (btn) btn.classList.add('pressed');
  send(cmd);
  clearInterval(interval);
  interval = setInterval(function() { send(cmd); }, 100);
});

document.addEventListener('keyup', function(e) {
  var cmd = keys[e.key];
  if (!cmd) return;
  held[e.key] = false;
  var btn = document.querySelector('[data-cmd="' + cmd + '"]');
  if (btn) btn.classList.remove('pressed');
  clearInterval(interval);
  if (cmd !== 'stop') send('stop');
});
</script>
</body>
</html>
)HTMLEND";

void handleRoot() {
  String html = FPSTR(PAGE_HTML_1);
  html += FPSTR(PAGE_HTML_2);
  server.send(200, "text/html", html);
}

void handleCmd() {
	String action = server.arg("action");

	if      (action == "fwd")   mover_frente();
	else if (action == "bwd")   mover_tras();
	else if (action == "right") mover_direita();
	else if (action == "left")  mover_esquerda();
	else if (action == "ur")    diagonal_frente_direita();
	else if (action == "ul")    diagonal_frente_esquerda();
	else if (action == "dr")    diagonal_tras_direita();
	else if (action == "dl")    diagonal_tras_esquerda();
	else if (action == "rot_r") girar_direita();
	else if (action == "rot_l") girar_esquerda();
	else if (action == "stop")  parar();

	server.send(200, "text/plain", "ok");
}

void handleSpeed() {
	if (server.hasArg("v")) {
		SPEED = constrain(server.arg("v").toInt(), 0, 255);
	}
	server.send(200, "text/plain", "ok");
}

void setup() {
	Serial.begin(115200);

	// Pinos de direcao como saida
	pinMode(FL_IN1, OUTPUT); pinMode(FL_IN2, OUTPUT);
	pinMode(FR_IN3, OUTPUT); pinMode(FR_IN4, OUTPUT);
	pinMode(BL_IN1, OUTPUT); pinMode(BL_IN2, OUTPUT);
	pinMode(BR_IN3, OUTPUT); pinMode(BR_IN4, OUTPUT);

	// PWM nos pinos ENA/ENB
	ledcAttach(FL_ENA, 1000, 8);
	ledcAttach(FR_ENB, 1000, 8);
	ledcAttach(BL_ENA, 1000, 8);
	ledcAttach(BR_ENB, 1000, 8);

	parar();

	WiFi.softAP(AP_SSID, AP_PASS);
	Serial.print("Rede: "); Serial.println(AP_SSID);
	Serial.print("Acesse: http://"); Serial.println(WiFi.softAPIP());

	server.on("/",      handleRoot);
	server.on("/cmd",   handleCmd);
	server.on("/speed", handleSpeed);

	server.begin();
	Serial.println("Servidor iniciado.");
}

void loop() {
	server.handleClient();
}
