PImage iconoReloj; // Variable que almacena imagen icono del Reloj 
PImage iconoSemaforo; // Variable que almacena imagen icono semaforo

void setup() {
  size(500, 500);
  iconoReloj = loadImage("ClockIcon/212px-ClockIcon.svg.png");
  iconoSemaforo = loadImage("Traffic_light/Traffic_light.gif");
}

void draw() {
  image(iconoReloj, 150, 30, 40, 40);
  image(iconoSemaforo, 100, 200);
  image(iconoSemaforo, 300, 200);
}
