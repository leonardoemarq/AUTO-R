import processing.serial.*;

// Variables de imagenes
PImage iconoReloj; // Variable que almacena imagen icono del Reloj 
PImage iconoSemaforo; // Variable que almacena imagen icono semaforo

// Variable puerto Serial
Serial puerto;

// Parametros de configuracion del controlador de Semaforos
long t; // elemento time_t, recibe el tiempo en formato UNIX TimeStamp
int ciclo; // Ciclo (en milisegundos)
int fase1; // % del ciclo que durara la fase 1
int fase2; // % del ciclo de durara la fase 2

int dial1;
int dial2;
int dial3;

String trama;
int i = 0;

void setup() {
  size(500, 500);
  iconoReloj = loadImage("ClockIcon/212px-ClockIcon.svg.png");
  iconoSemaforo = loadImage("Traffic_light/Traffic_light.gif"); 
  puerto = new Serial(this, "COM3", 9600); // objeto que abre el puerto serial y conecta al Arduino
}

void draw() {
  background(200);
  //image(iconoReloj, 150, 30, 40, 40);
  //image(iconoSemaforo, 100, 200);
  //image(iconoSemaforo, 300, 200);
  fill(0);
  
  // Titulos de los sliders
  textSize(32);
  textAlign(LEFT);
  text("F1", 240, 100);
  text("F2", 240, 220);
  text("Ciclo", 240, 340);
  
  // Sliders de Fase
  textSize(20); // Slider F1
  text("0%", 170, 140); 
  line(200, 140, 300, 140);
  text("100%", 310, 140);
  text("0%", 170, 260); // Slider F2
  line(200, 260, 300, 260);
  text("100%", 310, 260);
  text("0 segs", 130, 380); // Slider F2
  line(200, 380, 300, 380);
  text("255 segs", 310, 380);
  
  // Control de los sliders de Fases
  if (mouseX > 200 && mouseX < 300 && mouseY > 120 && mouseY < 160) {
    ellipse(mouseX, 140, 10, 10);
    ellipse(500 - mouseX, 260, 10, 10);
    dial1 = mouseX - 200;
    dial2 = 100 - dial1;
    textSize(10);
    textAlign(CENTER);
    fill(255, 0, 0);
    text(dial1, mouseX, 125);
    fill(0, 0, 255);
    text(dial2, 500 - mouseX, 245);
  }
  
  // Control del slider de Ciclo
  if (mouseX > 200 && mouseX < 300 && mouseY > 360 && mouseY < 400) {
    ellipse(mouseX, 380, 10, 10);
    dial3 = (int)map(mouseX, 200, 300, 0, 255);
    textSize(10);
    textAlign(CENTER);
    fill(125, 125, 0);
    text(dial3, mouseX, 365);
  }
}

void mouseClicked() {
  fase1= dial1;
  fase2 = dial2;
  ciclo = dial3;
  delay(2000);
  t = System.currentTimeMillis()/1000 - 16200;
  trama = "H" + t + "T" + ciclo + fase1 + fase2 + "3131";
  puerto.write(trama);
}