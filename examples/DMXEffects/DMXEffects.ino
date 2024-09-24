#include <R4DMX.h>

// Inicializa la librería R4DMX con los pines DIR, TX, RX
R4DMX dmx(2, 1, 0);

// Define los canales DMX para cada función
const int DIMMER_CHANNEL = 1;
const int RED_CHANNEL = 2;
const int GREEN_CHANNEL = 3;
const int BLUE_CHANNEL = 4;

// Variables para controlar el cambio de efectos
unsigned long lastEffectTime = 0;
int currentEffect = 0;
const char* effectNames[] = {"Fade", "Strobe", "Chase", "Pulse"};

void setup() {
  dmx.begin();
  Serial.begin(115200);
  Serial.println("R4DMX Effects Example Started");
}

void loop() {
  // Actualiza el universo DMX
  dmx.loop();

  // Cambia de efecto cada 15 segundos
  unsigned long currentTime = millis();
  if (currentTime - lastEffectTime > 15000) {
    currentEffect = (currentEffect + 1) % 4;
    lastEffectTime = currentTime;
    Serial.print("Changing to effect: ");
    Serial.println(effectNames[currentEffect]);
  }

  // Ejecuta el efecto actual
  switch (currentEffect) {
    case 0:
      fadeEffect();
      break;
    case 1:
      strobeEffect();
      break;
    case 2:
      chaseEffect();
      break;
    case 3:
      pulseEffect();
      break;
  }
}

// Efecto de fade: transición suave entre colores
void fadeEffect() {
  static unsigned long lastFadeTime = 0;
  static int fadeStep = 0;
  
  if (millis() - lastFadeTime > 20) {
    lastFadeTime = millis();
    
    // Calcula los valores RGB para el fade
    int r = (fadeStep < 255) ? fadeStep : 510 - fadeStep;
    int g = ((fadeStep + 85) % 510 < 255) ? (fadeStep + 85) % 510 : 510 - ((fadeStep + 85) % 510);
    int b = ((fadeStep + 170) % 510 < 255) ? (fadeStep + 170) % 510 : 510 - ((fadeStep + 170) % 510);
    
    // Establece los valores en los canales DMX
    dmx.setChannel(DIMMER_CHANNEL, 255);
    dmx.setChannel(RED_CHANNEL, r);
    dmx.setChannel(GREEN_CHANNEL, g);
    dmx.setChannel(BLUE_CHANNEL, b);
    
    fadeStep = (fadeStep + 1) % 510;

    // Imprime el progreso cada 10%
    if (fadeStep % 51 == 0) {
      Serial.print("Fade effect: ");
      Serial.print(fadeStep / 5.1);
      Serial.println("%");
    }
  }
}

// Efecto de strobe: parpadeo rápido
void strobeEffect() {
  static unsigned long lastStrobeTime = 0;
  static bool strobeState = false;
  static int strobeCount = 0;
  
  if (millis() - lastStrobeTime > 50) {
    lastStrobeTime = millis();
    strobeState = !strobeState;
    
    // Alterna entre encendido y apagado
    dmx.setChannel(DIMMER_CHANNEL, strobeState ? 255 : 0);
    dmx.setChannel(RED_CHANNEL, 255);
    dmx.setChannel(GREEN_CHANNEL, 255);
    dmx.setChannel(BLUE_CHANNEL, 255);

    // Imprime un mensaje cada 20 ciclos
    if (++strobeCount % 20 == 0) {
      Serial.println("Strobe effect: Flash!");
    }
  }
}

// Efecto de chase: secuencia de colores
void chaseEffect() {
  static unsigned long lastChaseTime = 0;
  static int chaseStep = 0;
  
  if (millis() - lastChaseTime > 500) {
    lastChaseTime = millis();
    
    // Alterna entre rojo, verde y azul
    dmx.setChannel(DIMMER_CHANNEL, 255);
    dmx.setChannel(RED_CHANNEL, chaseStep == 0 ? 255 : 0);
    dmx.setChannel(GREEN_CHANNEL, chaseStep == 1 ? 255 : 0);
    dmx.setChannel(BLUE_CHANNEL, chaseStep == 2 ? 255 : 0);
    
    chaseStep = (chaseStep + 1) % 3;

    Serial.print("Chase effect: ");
    Serial.println(chaseStep == 0 ? "Red" : (chaseStep == 1 ? "Green" : "Blue"));
  }
}

// Efecto de pulse: efecto de respiración
void pulseEffect() {
  static unsigned long lastPulseTime = 0;
  static int pulseValue = 0;
  static bool pulseDirection = true;
  
  if (millis() - lastPulseTime > 2) { 
    lastPulseTime = millis();
    
    // Ajusta el brillo para crear el efecto de respiración
    dmx.setChannel(DIMMER_CHANNEL, pulseValue);
    dmx.setChannel(RED_CHANNEL, 255);
    dmx.setChannel(GREEN_CHANNEL, 255);
    dmx.setChannel(BLUE_CHANNEL, 255);
    
    // Incrementa o decrementa el valor de pulse
    if (pulseDirection) {
      pulseValue += 2;
      if (pulseValue >= 255) {
        pulseValue = 255;
        pulseDirection = false;
      }
    } else {
      pulseValue -= 2;
      if (pulseValue <= 0) {
        pulseValue = 0;
        pulseDirection = true;
      }
    }

    // Imprime el progreso del efecto
    if (pulseValue % 25 == 0 || pulseValue == 255 || pulseValue == 0) {
      Serial.print("Pulse effect: ");
      Serial.print(pulseValue / 2.55);
      Serial.println("%");
    }
  }
}