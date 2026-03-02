#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Motor Pins
#define ENA 5    // D1
#define IN1 14   // D5
#define IN2 12   // D6
#define IN3 13   // D7
#define IN4 15   // D8
#define ENB 4    // D2

// ✅ Buzzer Pin (NEW)
#define BUZZER 0   // D3 (GPIO0)

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // ✅ Buzzer setup (NEW)
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  analogWriteFreq(1000);
  analogWriteRange(1023);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void move(int left, int right) {

  if (right >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  if (left >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  analogWrite(ENA, abs(right));
  analogWrite(ENB, abs(left));
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_TEXT) {

    String data = String((char*)payload);

    // ✅ Buzzer Control (NEW)
    if (data == "B1") {
      digitalWrite(BUZZER, HIGH);
      return;
    }
    if (data == "B0") {
      digitalWrite(BUZZER, LOW);
      return;
    }

    int commaIndex = data.indexOf(',');
    int v = data.substring(0, commaIndex).toInt();
    int t = data.substring(commaIndex + 1).toInt();

    int left = constrain(v + t, -1023, 1023);
    int right = constrain(v - t, -1023, 1023);

    move(left, right);
  }
}

void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
body {
  background:#111;
  display:flex;
  flex-direction:column;
  justify-content:center;
  align-items:center;
  height:100vh;
  margin:0;
  font-family:sans-serif;
}
h2 { color:cyan; }
#base {
  width:250px; height:250px;
  background:#222;
  border-radius:50%;
  position:relative;
  border:3px solid cyan;
}
#stick {
  width:80px; height:80px;
  background:cyan;
  border-radius:50%;
  position:absolute;
  left:85px; top:85px;
  transition:0.05s;
}
button {
  margin-top:20px;
  padding:15px 40px;
  font-size:18px;
  background:cyan;
  border:none;
  border-radius:10px;
}
</style>
</head>
<body>

<h2>GUHAN Race Car</h2>
<div id="base">
  <div id="stick"></div>
</div>

<!-- ✅ Buzzer Button (NEW) -->
<button ontouchstart="buzzOn()" ontouchend="buzzOff()">BUZZER</button>

<script>
let ws = new WebSocket("ws://" + location.hostname + ":81/");
let stick = document.getElementById("stick");
let base = document.getElementById("base");

let max = 80;

base.addEventListener("touchmove", function(e){
  e.preventDefault();

  let rect = base.getBoundingClientRect();
  let x = e.touches[0].clientX - rect.left - 125;
  let y = e.touches[0].clientY - rect.top - 125;

  let dist = Math.sqrt(x*x + y*y);
  if(dist > max){
    x *= max/dist;
    y *= max/dist;
  }

  stick.style.transform = `translate(${x}px, ${y}px)`;

  let v = Math.round(-y * 12);
  let t = Math.round(x * 12);

  ws.send(v + "," + t);
});

base.addEventListener("touchend", function(){
  stick.style.transform = "translate(0px,0px)";
  ws.send("0,0");
});

// ✅ Buzzer JS (NEW)
function buzzOn(){
  ws.send("B1");
}
function buzzOff(){
  ws.send("B0");
}
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}
