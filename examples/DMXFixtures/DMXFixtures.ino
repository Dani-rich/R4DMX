#include <R4DMX.h>

// Inicializa la librería R4DMX con los pines DIR, TX, RX
R4DMX dmx(2, 1, 0);

// Define las direcciones DMX para cada fixture
const int FIXTURE1_START = 1;  // Fixture 1 comienza en el canal 1
const int FIXTURE2_START = 5;  // Fixture 2 comienza en el canal 5
const int FIXTURE3_START = 9;  // Fixture 3 comienza en el canal 9

// Número de canales por fixture
const int CHANNELS_PER_FIXTURE = 4;

// Variables para el control de tiempo
unsigned long lastDMXUpdate = 0;
const unsigned long DMX_UPDATE_INTERVAL = 30; // 33Hz tasa de actualización

void setup() {
  dmx.begin();
  Serial.begin(115200);
  Serial.println("Ejemplo R4DMX Fixtures Iniciado");
  
  // Enciende todos los dimmers al inicio
  dmx.setChannel(FIXTURE1_START, 255);
  dmx.setChannel(FIXTURE2_START, 255);
  dmx.setChannel(FIXTURE3_START, 255);
}

void loop() {
  // Actualiza el universo DMX a una tasa constante
  updateDMX();

  // Secuencia de demostración
  allOn();
  delay(2000);
  
  chaseEffect();
  delay(1000);
  
  rainbowEffect();
  delay(1000);
  
  strobeEffect();
  delay(1000);
  
  allOff();
  delay(2000);
}

// Función para actualizar DMX de manera controlada
void updateDMX() {
  unsigned long currentTime = millis();
  if (currentTime - lastDMXUpdate >= DMX_UPDATE_INTERVAL) {
    dmx.loop();
    lastDMXUpdate = currentTime;
  }
}

// Enciende todos los fixtures en blanco
void allOn() {
  Serial.println("Todos los fixtures encendidos");
  setFixtureColor(FIXTURE1_START, 255, 255, 255);
  setFixtureColor(FIXTURE2_START, 255, 255, 255);
  setFixtureColor(FIXTURE3_START, 255, 255, 255);
  updateDMX();
}

// Apaga todos los fixtures
void allOff() {
  Serial.println("Todos los fixtures apagados");
  setFixtureColor(FIXTURE1_START, 0, 0, 0);
  setFixtureColor(FIXTURE2_START, 0, 0, 0);
  setFixtureColor(FIXTURE3_START, 0, 0, 0);
  updateDMX();
}

// Efecto de persecución
void chaseEffect() {
  Serial.println("Efecto de persecución");
  for (int i = 0; i < 5; i++) {
    setFixtureColor(FIXTURE1_START, 255, 0, 0);
    setFixtureColor(FIXTURE2_START, 0, 0, 0);
    setFixtureColor(FIXTURE3_START, 0, 0, 0);
    updateDMX();
    delay(200);
    setFixtureColor(FIXTURE1_START, 0, 0, 0);
    setFixtureColor(FIXTURE2_START, 0, 255, 0);
    setFixtureColor(FIXTURE3_START, 0, 0, 0);
    updateDMX();
    delay(200);
    setFixtureColor(FIXTURE1_START, 0, 0, 0);
    setFixtureColor(FIXTURE2_START, 0, 0, 0);
    setFixtureColor(FIXTURE3_START, 0, 0, 255);
    updateDMX();
    delay(200);
  }
}

// Efecto arcoíris
void rainbowEffect() {
  Serial.println("Efecto arcoíris");
  for (int hue = 0; hue < 360; hue += 5) {
    setFixtureHue(FIXTURE1_START, hue);
    setFixtureHue(FIXTURE2_START, (hue + 120) % 360);
    setFixtureHue(FIXTURE3_START, (hue + 240) % 360);
    updateDMX();
    delay(50);
  }
}

// Efecto estroboscópico
void strobeEffect() {
  Serial.println("Efecto estroboscópico");
  for (int i = 0; i < 20; i++) {
    setFixtureColor(FIXTURE1_START, 255, 255, 255);
    setFixtureColor(FIXTURE2_START, 255, 255, 255);
    setFixtureColor(FIXTURE3_START, 255, 255, 255);
    updateDMX();
    delay(50);
    setFixtureColor(FIXTURE1_START, 0, 0, 0);
    setFixtureColor(FIXTURE2_START, 0, 0, 0);
    setFixtureColor(FIXTURE3_START, 0, 0, 0);
    updateDMX();
    delay(50);
  }
}

// Función auxiliar para establecer el color de un fixture
void setFixtureColor(int startChannel, int r, int g, int b) {
  dmx.setChannel(startChannel, 255);  // Mantiene el dimmer siempre encendido
  dmx.setChannel(startChannel + 1, r);
  dmx.setChannel(startChannel + 2, g);
  dmx.setChannel(startChannel + 3, b);
}

// Función auxiliar para establecer el color de un fixture usando HSV
void setFixtureHue(int startChannel, int hue) {
  int r, g, b;
  hsvToRgb(hue, 255, 255, r, g, b);
  setFixtureColor(startChannel, r, g, b);
}

// Convierte HSV a RGB (versión optimizada)
void hsvToRgb(int hue, int sat, int val, int& r, int& g, int& b) {
  hue %= 360;
  uint16_t h = hue / 60;
  uint16_t f = (hue % 60) * 4;
  uint16_t p = (uint16_t)((uint32_t)val * (255 - sat)) / 255;
  uint16_t q = (uint16_t)((uint32_t)val * (255 - ((uint32_t)sat * f) / 240)) / 255;
  uint16_t t = (uint16_t)((uint32_t)val * (255 - ((uint32_t)sat * (240 - f)) / 240)) / 255;

  switch(h) {
    case 0: r = val; g = t; b = p; break;
    case 1: r = q; g = val; b = p; break;
    case 2: r = p; g = val; b = t; break;
    case 3: r = p; g = q; b = val; break;
    case 4: r = t; g = p; b = val; break;
    default: r = val; g = p; b = q; break;
  }
}