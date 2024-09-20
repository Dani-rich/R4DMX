#include "R4DMX.h"

R4DMX::R4DMX(int dirPin, int txPin, int rxPin) 
    : dmxDirPin(dirPin), dmxTxPin(txPin), dmxRxPin(rxPin),
      lastFrameTime(0), lastDiagnosticTime(0), frameCount(0), dmxError(false),
      refreshRateStable(true), isChaseRunning(false), isPulseRunning(false),
      isStrobeRunning(false) {
    memset(dmxData, 0, DMX_UNIVERSE_SIZE);
    initPredefinedColors();
}

void R4DMX::initPredefinedColors() {
    predefinedColors["r"] = {255, 0, 0};    // Rojo
    predefinedColors["g"] = {0, 255, 0};    // Verde
    predefinedColors["b"] = {0, 0, 255};    // Azul
    predefinedColors["y"] = {255, 255, 0};  // Amarillo
    predefinedColors["m"] = {255, 0, 255};  // Magenta
    predefinedColors["c"] = {0, 255, 255};  // Cyan
    predefinedColors["v"] = {147, 0, 255};  // Ultravioleta
    predefinedColors["p"] = {255, 192, 203};// Rosa
    predefinedColors["a"] = {255, 191, 0};  // Ámbar
}

void R4DMX::begin() {
    Serial.begin(115200);
    Serial1.begin(250000, SERIAL_8N2);
    
    pinMode(dmxDirPin, OUTPUT);
    pinMode(dmxTxPin, OUTPUT);
    pinMode(dmxRxPin, INPUT);
    
    digitalWrite(dmxDirPin, HIGH);
    
    Serial.println("Inicialización DMX completada");
    printAvailableCommands();
}

void R4DMX::loop() {
    unsigned long currentTime = micros();
    
    if (currentTime - lastFrameTime >= FRAME_TIME) {
        sendDmxUniverse();
        lastFrameTime = currentTime;
        frameCount++;

        if (isChaseRunning) updateChase();
        if (isPulseRunning) updatePulse();
        if (isStrobeRunning) updateStrobe();
    }
    
    if (currentTime - lastDiagnosticTime >= 1000000) {
        performDiagnostic();
        lastDiagnosticTime = currentTime;
    }
}

void R4DMX::setChannel(int channel, uint8_t value) {
    if (channel >= 1 && channel < DMX_UNIVERSE_SIZE) {
        dmxData[channel] = value;
    }
}

void R4DMX::sendDmxUniverse() {
    noInterrupts();
    Serial1.end();
    pinMode(dmxTxPin, OUTPUT);
    digitalWrite(dmxTxPin, LOW);
    delayMicroseconds(92);
    
    digitalWrite(dmxTxPin, HIGH);
    delayMicroseconds(12);
    
    Serial1.begin(250000, SERIAL_8N2);
    
    Serial1.write(static_cast<uint8_t>(0)); // Start code
    for (int i = 1; i < DMX_UNIVERSE_SIZE; i++) {
        Serial1.write(dmxData[i]);
    }
    interrupts();
}

void R4DMX::performDiagnostic() {
    bool currentStability = (frameCount >= REFRESH_RATE - 3);
    if (currentStability != refreshRateStable) {
        refreshRateStable = currentStability;
        if (!refreshRateStable) {
            Serial.print("ADVERTENCIA: Tasa de refresco inestable. Tasa actual: ");
            Serial.print(frameCount);
            Serial.println(" Hz");
        } else {
            Serial.println("Tasa de refresco estabilizada");
        }
    }
    
    if (dmxError) {
        Serial.println("ERROR: Problema al enviar datos DMX");
        dmxError = false;
    }
    
    frameCount = 0;
}

void R4DMX::setColor(int startChannel, const String& colorName) {
    auto it = predefinedColors.find(colorName);
    if (it != predefinedColors.end()) {
        Color color = it->second;
        setChannel(startChannel, color.r);
        setChannel(startChannel + 1, color.g);
        setChannel(startChannel + 2, color.b);
        Serial.println("Color aplicado: " + colorName + " (R:" + String(color.r) + ", G:" + String(color.g) + ", B:" + String(color.b) + ")");
    } else {
        Serial.println("Color no encontrado: " + colorName);
    }
}

void R4DMX::startFade(int startChannel, const String& colorName, unsigned long durationMs) {
    auto it = predefinedColors.find(colorName);
    if (it != predefinedColors.end()) {
        Color targetColor = it->second;
        uint8_t startR = dmxData[startChannel];
        uint8_t startG = dmxData[startChannel + 1];
        uint8_t startB = dmxData[startChannel + 2];
        
        unsigned long startTime = millis();
        while (millis() - startTime < durationMs) {
            float progress = (millis() - startTime) / (float)durationMs;
            setChannel(startChannel, startR + (targetColor.r - startR) * progress);
            setChannel(startChannel + 1, startG + (targetColor.g - startG) * progress);
            setChannel(startChannel + 2, startB + (targetColor.b - startB) * progress);
            loop();
        }
        setChannel(startChannel, targetColor.r);
        setChannel(startChannel + 1, targetColor.g);
        setChannel(startChannel + 2, targetColor.b);
        Serial.println("Fade completado");
    } else {
        Serial.println("Color no encontrado: " + colorName);
    }
}

void R4DMX::startStrobeEffect(int startChannel, const String& colorName, int frequency) {
    auto it = predefinedColors.find(colorName);
    if (it != predefinedColors.end()) {
        strobeColor = it->second;
        strobeChannel = startChannel;
        strobeFrequency = frequency;
        isStrobeRunning = true;
        Serial.println("Strobe iniciado con color: " + colorName);
    } else {
        Serial.println("Color no encontrado: " + colorName);
    }
}

void R4DMX::updateStrobe() {
    static unsigned long lastToggle = 0;
    static bool isOn = false;
    unsigned long interval = 1000 / (strobeFrequency * 2);
    unsigned long currentTime = millis();

    if (currentTime - lastToggle >= interval) {
        if (isOn) {
            setChannel(strobeChannel, strobeColor.r);
            setChannel(strobeChannel + 1, strobeColor.g);
            setChannel(strobeChannel + 2, strobeColor.b);
        } else {
            setChannel(strobeChannel, 0);
            setChannel(strobeChannel + 1, 0);
            setChannel(strobeChannel + 2, 0);
        }
        isOn = !isOn;
        lastToggle = currentTime;
    }
}

void R4DMX::stopStrobeEffect() {
    isStrobeRunning = false;
    setChannel(strobeChannel, 0);
    setChannel(strobeChannel + 1, 0);
    setChannel(strobeChannel + 2, 0);
    Serial.println("Strobe detenido");
}

void R4DMX::startChaseEffect(const std::vector<String>& colorNames, unsigned long intervalMs) {
    chaseColors.clear();
    for (const auto& colorName : colorNames) {
        auto it = predefinedColors.find(colorName);
        if (it != predefinedColors.end()) {
            chaseColors.push_back(colorName);
        } else {
            Serial.println("Color no encontrado para chase: " + colorName);
        }
    }
    if (!chaseColors.empty()) {
        chaseInterval = intervalMs;
        activeChaseStep = 0;
        lastChaseUpdate = millis();
        isChaseRunning = true;
        Serial.println("Chase iniciado con " + String(chaseColors.size()) + " colores");
    } else {
        Serial.println("No se encontraron colores válidos para el chase");
    }
}

void R4DMX::updateChase() {
    unsigned long currentTime = millis();
    if (currentTime - lastChaseUpdate >= chaseInterval) {
        if (!chaseColors.empty()) {
            auto it = predefinedColors.find(chaseColors[activeChaseStep]);
            if (it != predefinedColors.end()) {
                Color color = it->second;
                setChannel(2, color.r);
                setChannel(3, color.g);
                setChannel(4, color.b);
            }
            activeChaseStep = (activeChaseStep + 1) % chaseColors.size();
        }
        lastChaseUpdate = currentTime;
    }
}

void R4DMX::stopChaseEffect() {
    isChaseRunning = false;
    setChannel(2, 0);
    setChannel(3, 0);
    setChannel(4, 0);
    Serial.println("Chase detenido");
}

void R4DMX::startPulseEffect(int startChannel, const String& colorName) {
    auto it = predefinedColors.find(colorName);
    if (it != predefinedColors.end()) {
        pulseColor = it->second;
        pulseChannel = startChannel;
        pulseStartTime = millis();
        isPulseRunning = true;
        Serial.println("Pulse iniciado con color: " + colorName);
    } else {
        Serial.println("Color no encontrado: " + colorName);
    }
}

void R4DMX::updatePulse() {
    unsigned long currentTime = millis();
    float progress = (currentTime - pulseStartTime) % 1000 / 1000.0f;
    uint8_t intensity = static_cast<uint8_t>(sin(progress * PI) * 255);
    setChannel(pulseChannel, (pulseColor.r * intensity) / 255);
    setChannel(pulseChannel + 1, (pulseColor.g * intensity) / 255);
    setChannel(pulseChannel + 2, (pulseColor.b * intensity) / 255);
}

void R4DMX::stopPulseEffect() {
    isPulseRunning = false;
    setChannel(pulseChannel, 0);
    setChannel(pulseChannel + 1, 0);
    setChannel(pulseChannel + 2, 0);
    Serial.println("Pulse detenido");
}

void R4DMX::clearAllChannels() {
    for (int i = 1; i < DMX_UNIVERSE_SIZE; i++) {
        setChannel(i, 0);
    }
    stopStrobeEffect();
    stopChaseEffect();
    stopPulseEffect();
    Serial.println("Todos los canales han sido restaurados a cero y todos los efectos detenidos");
}

void R4DMX::locateFixture(int startChannel, int numChannels) {
    for (int i = 0; i < numChannels; i++) {
        setChannel(startChannel + i, 255);
    }
    Serial.println("Fixture localizado: canales " + String(startChannel) + " a " + String(startChannel + numChannels - 1) + " establecidos a 255");
}

void R4DMX::printAvailableCommands() {
    Serial.println("Comandos disponibles:");
    Serial.println("set <canal> <valor> - Establece un canal a un valor específico");
    Serial.println("setcolor <canal_inicio> <color> - Aplica un color predefinido");
    Serial.println("fade <canal_inicio> <color> <duración_en_segundos> - Realiza un fade a un color");
    Serial.println("strobe <canal_inicio> <color> <frecuencia> - Inicia un efecto estrobo");
    Serial.println("stop strobe - Detiene el efecto estrobo");
    Serial.println("chase <color1> <color2> ... <intervalo_en_ms> - Inicia un efecto chase");
    Serial.println("stop chase - Detiene el efecto chase");
    Serial.println("pulse <canal_inicio> <color> - Inicia un efecto pulse");
    Serial.println("stop pulse - Detiene el efecto pulse");
    Serial.println("clear - Restaura todos los canales a cero y detiene todos los efectos");
    Serial.println("locate <canal_inicio> <num_canales> - Establece un rango de canales a 255");
    Serial.println("Colores predefinidos: r, g, b, y, m, c, v, p, a");
}

Color R4DMX::parseColorInput(const String& colorInput) {
    if (isNumeric(colorInput)) {
        int r, g, b;
        sscanf(colorInput.c_str(), "%d,%d,%d", &r, &g, &b);
        return {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)};
    } else {
        auto it = predefinedColors.find(colorInput);
        if (it != predefinedColors.end()) {
            return it->second;
        } else {
            Serial.println("Color no reconocido: " + colorInput + ". Usando blanco por defecto.");
            return {255, 255, 255};
        }
    }
}

bool R4DMX::isNumeric(const String& str) {
    for (char c : str) {
        if (!isdigit(c) && c != ',' && c != '.') {
            return false;
        }
    }
    return true;
}
