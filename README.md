# R4DMX Library

R4DMX es una librería Arduino diseñada para controlar dispositivos DMX de manera sencilla y eficiente. Permite manejar múltiples fixtures y crear efectos de iluminación complejos con facilidad.

## Características

- Control de hasta 512 canales DMX
- Soporte para múltiples fixtures
- Efectos predefinidos: fade, chase, arcoíris, estroboscópico
- Fácil configuración de pines para DIR, TX y RX

## Instalación

1. Descarga la librería R4DMX
2. Descomprime el archivo descargado
3. Renombra la carpeta a "R4DMX"
4. Copia la carpeta en tu directorio de librerías Arduino (generalmente en Documents/Arduino/libraries/)
5. Reinicia el IDE de Arduino

## Uso básico

```cpp
#include <R4DMX.h>

R4DMX dmx(2, 1, 0); // Pines DIR, TX, RX

void setup() {
  dmx.begin();
}

void loop() {
  dmx.setChannel(1, 255); // Establece el canal 1 al valor máximo
  dmx.loop();
}

Ejemplos
La librería incluye varios ejemplos para ayudarte a comenzar:
DMXBasic: Ejemplo básico de control de un fixture DMX
DMXEffects: Demuestra varios efectos de iluminación predefinidos
DMXFixtures: Control de múltiples fixtures DMX

Puedes encontrar estos ejemplos en el menú Archivo > Ejemplos > R4DMX en el IDE de Arduino.

Funciones principales
begin(): Inicializa la librería
loop(): Actualiza el universo DMX (debe llamarse en cada iteración del loop)
setChannel(channel, value): Establece el valor de un canal DMX
getChannel(channel): Obtiene el valor actual de un canal DMX
startFade(startChannel, colorName, duration): Inicia un efecto de fade
startChaseEffect(colorNames, intervalMs): Inicia un efecto de persecución
startStrobeEffect(startChannel, colorName, frequency): Inicia un efecto estroboscópico
startRainbowEffect(startChannel, duration): Inicia un efecto arcoíris

Configuración
La librería R4DMX está configurada por defecto para usar los pines 2 (DIR), 1 (TX) y 0 (RX). Puedes cambiar estos pines en el constructor de la clase R4DMX.

Contribuciones
Las contribuciones son bienvenidas. Por favor, abre un issue para discutir los cambios propuestos antes de hacer un pull request.

Licencia
Esta librería está licenciada bajo la Licencia MIT.
