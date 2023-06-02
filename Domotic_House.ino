#include <WiFi101.h>
#include <Servo.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

Servo servoMotorPuerta;
Servo servoMotorCortina;

DHT dht(DHTPIN, DHTTYPE);

bool encendidaAlarma;
char action;
const int led1 = 6;
const int Trigger = 0;
const int Echo = 1;
const int Buzzer = 5;
const int Ventilador = 7;

char ssid[] = "Wifi";  //nombre de red wifi
char pass[] = "1234567890";         //password

int status = 0;

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(9600);

  //pines
  pinMode(led1, OUTPUT);
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Ventilador, OUTPUT);
  servoMotorPuerta.attach(3);
  servoMotorCortina.attach(4);
  servoMotorPuerta.write(180);
  dht.begin();

  //Activar el wifi
  enable_WiFi();
  //nos conectamos
  connect_WiFi();

  //iniciamos el server
  server.begin();

  //imprimir por serial la info
  printWifiStatus();
}

void loop() {
  // put your main code here, to run repeatedly:
  client = server.available();

  if (client) {
    printWEB(client);
  }
  float t = dht.readTemperature();
  Serial.print("Temperatura: ");
  Serial.println(t);
  if (t >= 25) {
    digitalWrite(Ventilador, HIGH);
  }
  if (encendidaAlarma == true) {
    Alarma(Trigger, Echo, Buzzer);
  } else {
    digitalWrite(Buzzer, LOW);
  }
  delay(1000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

//se queda en el ciclo hasta que conecte
void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // Wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB(WiFiClient client) {
  client.println("<!DOCTYPE html>");
  client.println("<html>");

  client.println("<head>");
  client.println("<title>Control de casa mediante Ardunino MKR1000</title>");
  client.println("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css\">");
  client.println("<style>");
  client.println("body {");
  client.println("  font-family: 'Inter', sans-serif;");
  client.println("  background-color: #f2f2f2;");
  client.println("  margin: 0;");
  client.println("  padding: 20px;");
  client.println("}");
  client.println("h1 {");
  client.println("  color: #333;");
  client.println("  text-align: center;");
  client.println("}");
  client.println(".button-container {");
  client.println("  display: flex;");
  client.println("  justify-content: center;");
  client.println("  margin-bottom: 20px;");
  client.println("}");
  client.println(".button-container button {");
  client.println("  width: 200px;");
  client.println("}");
  client.println(".sensor-container {");
  client.println("  display: flex;");
  client.println("  justify-content: center;");
  client.println("  align-items: center;");
  client.println("  margin-bottom: 20px;");
  client.println("}");
  client.println(".on-button, .off-button {");
  client.println("  color: #000;");
  client.println("  border: 2px solid #000;");
  client.println("  padding: 10px 20px;");
  client.println("  margin: 0 5px;");
  client.println("  border-radius: 5px;");
  client.println("  cursor: pointer;");
  client.println("  font-weight: bold;");
  client.println("  width: 100%;");
  client.println("}");
  client.println(".on-button.active {");
  client.println("  background-color: #00FF00;");
  client.println("}");
  client.println(".off-button.active {");
  client.println("  background-color: #FF0000;");
  client.println("}");
  client.println(".on-button:hover {");
  client.println("  background-color: #00FF00;");
  client.println("}");
  client.println(".off-button:hover {");
  client.println("  background-color: #FF0000;");
  client.println("}");
  client.println(".humidity-label {");
  client.println("  font-size: 24px;");
  client.println("  color: #333;");
  client.println("  margin-right: 10px;");
  client.println("}");
  client.println(".humidity-value {");
  client.println("  font-size: 36px;");
  client.println("  color: #222;");
  client.println("}");
  client.println(".footer {");
  client.println("  text-align: center;");
  client.println("  font-size: 14px;");
  client.println("  color: #666;");
  client.println("  margin-top: 40px;");
  client.println("}");
  client.println("</style>");
  client.println("</head>");

  client.println("<body>");
  client.println("<h1>Control de casa mediante Arduino MRK1000</h1>");

  client.println("<div class=\"button-container\">");
  client.println("<button class=\"on-button\" onclick=\"ledControl(1)\"><i class=\"fas fa-lightbulb\"></i> LUZ ON</button>");
  client.println("<button class=\"off-button\" onclick=\"ledControl(2)\"><i class=\"far fa-lightbulb\"></i> LUZ OFF</button>");
  client.println("</div>");

  client.println("<div class=\"button-container\">");
  client.println("<button class=\"on-button\" onclick=\"ledControl(3)\"><i class=\"fas fa-window-maximize\"></i> ABRIR VENTANAS</button>");
  client.println("<button class=\"off-button\" onclick=\"ledControl(4)\"><i class=\"fas fa-window-maximize\"></i> CERRAR VENTANAS</button>");
  client.println("</div>");

  client.println("<div class=\"button-container\">");
  client.println("<button class=\"on-button\" onclick=\"ledControl(5)\"><i class=\"fas fa-fan\"></i> ENCENDER VENTILADORES</button>");
  client.println("<button class=\"off-button\" onclick=\"ledControl(6)\"><i class=\"fas fa-fan\"></i> APAGAR VENTILADORES</button>");
  client.println("</div>");

  client.println("<div class=\"button-container\">");
  client.println("<button class=\"on-button\" onclick=\"ledControl(7)\"><i class=\"fas fa-door-open\"></i> ABRIR PUERTA</button>");
  client.println("<button class=\"off-button\" onclick=\"ledControl(8)\"><i class=\"fas fa-door-closed\"></i> CERRAR PUERTA</button>");
  client.println("</div>");

  client.println("<div class=\"button-container\">");
  client.println("<button class=\"on-button\" onclick=\"ledControl(9)\"><i class=\"fas fa-bell\"></i> ENCENDER ALARMA</button>");
  client.println("<button class=\"off-button\" onclick=\"ledControl(10)\"><i class=\"far fa-bell\"></i> APAGAR ALARMA</button>");
  client.println("</div>");

  client.println("<div class=\"sensor-container\">");
  client.println("<span class=\"humidity-label\">Humedad:</span>");
  client.println("<span class=\"humidity-value\">50%</span>");
  client.println("</div>");

  client.println("<div class=\"footer\">");
  client.println("Proyecto creado por Kevin Quiroz, Emanuel Patino y Julian Agudelo<br>");
  client.println("Electronica Digital<br>");
  client.println("2023");
  client.println("</div>");

  client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/js/all.min.js\"></script>");
  client.println("<script>");
  client.println("var buttons = document.getElementsByClassName('on-button, off-button');");
  client.println("for (var i = 0; i < buttons.length; i++) {");
  client.println("  buttons[i].addEventListener('click', function() {");
  client.println("    var current = this;");
  client.println("    var siblings = current.parentNode.childNodes;");
  client.println("    for (var j = 0; j < siblings.length; j++) {");
  client.println("      if (siblings[j].nodeType === 1) {");
  client.println("        siblings[j].classList.remove('active');");
  client.println("      }");
  client.println("    }");
  client.println("    current.classList.add('active');");
  client.println("  });");
  client.println("}");
  client.println("function ledControl(ledNum) {");
  client.println("  var xhttp = new XMLHttpRequest();");
  client.println("  xhttp.onreadystatechange = function() {");
  client.println("    if (this.readyState == 4 && this.status == 200) {");
  client.println("      console.log('LED control successful.');");
  client.println("    }");
  client.println("  };");
  client.println("  xhttp.open('GET', '/control?led=' + ledNum, true);");
  client.println("  xhttp.send();");
  client.println("}");
  client.println("</script>");

  client.println("</body>");
  client.println("</html>");


  // handle LED control
  if (client.available()) {
    String currentLine = "";
    while (client.available()) {
      char c = client.read();
      currentLine += c;
      if (c == '\n') {
        if (currentLine.startsWith("GET /control")) {
          int ledNum = currentLine.substring(currentLine.indexOf('=') + 1).toInt();
          controlLED(ledNum);
        }
        currentLine = "";
      }
    }
    client.stop();
  }
}

void Alarma(int Trigger, int Echo, int Buzzer) {
  long t;
  long d;

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH);
  d = t / 59;

  Serial.print("Distancia: ");
  Serial.print(d);
  Serial.print("cm");
  Serial.println();

  if (d < 20) {
    digitalWrite(Buzzer, HIGH);
  } else {
    digitalWrite(Buzzer, LOW);
  }
}


void abrirPuerta(Servo servomotorPuerta) {
  servoMotorPuerta.write(90);
}

void cerrarPuerta(Servo servoMotorPuerta) {
  servoMotorPuerta.write(180);
}

void abrirCortina(Servo servoMotorCortina) {
  int vel = 0;
  servoMotorCortina.write(vel);
  delay(1000);
  vel = 90;
  servoMotorCortina.write(vel);
}

void cerrarCortina(Servo servoMotorCortina) {
  int vel = 180;
  servoMotorCortina.write(vel);
  delay(1000);
  vel = 90;
  servoMotorCortina.write(vel);
}

void controlLED(int ledNum) {
  switch (ledNum) {
    case 1:
      digitalWrite(led1, HIGH);
      break;
    case 2:
      digitalWrite(led1, LOW);
      break;
    case 3:
      abrirCortina(servoMotorCortina);
      break;
    case 4:
      cerrarCortina(servoMotorCortina);
      break;
    case 5:
      digitalWrite(Ventilador, HIGH);
      break;
    case 6:
      digitalWrite(Ventilador, LOW);
      break;
    case 7:
      abrirPuerta(servoMotorPuerta);
      break;
    case 8:
      cerrarPuerta(servoMotorPuerta);
      break;
    case 9:
      encendidaAlarma=true;
      break;
    case 10:
      encendidaAlarma=false;
      break;
    

      // Add cases for other LEDs if needed
  }
}