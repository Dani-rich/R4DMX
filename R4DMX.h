#ifndef R4DMX_h
#define R4DMX_h

#include <Arduino.h>
#include <vector>
#include <map>

struct Color {
    uint8_t r, g, b;
};

class R4DMX {
public:
    R4DMX(int dirPin, int txPin, int rxPin);
    void begin();
    void loop();
    void setChannel(int channel, uint8_t value);
    void setColor(int startChannel, const String& colorName);
    void startFade(int startChannel, const String& colorName, unsigned long durationMs);
    void startStrobeEffect(int startChannel, const String& colorName, int frequency);
    void stopStrobeEffect();
    void startChaseEffect(const std::vector<String>& colorNames, unsigned long intervalMs);
    void stopChaseEffect();
    void startPulseEffect(int startChannel, const String& colorName);
    void stopPulseEffect();
    void clearAllChannels();
    void locateFixture(int startChannel, int numChannels);
    void printAvailableCommands();

private:
    static const int DMX_UNIVERSE_SIZE = 513;
    static const int REFRESH_RATE = 40;
    static const long FRAME_TIME = 1000000 / REFRESH_RATE;

    uint8_t dmxData[DMX_UNIVERSE_SIZE];
    unsigned long lastFrameTime;
    unsigned long lastDiagnosticTime;
    unsigned long frameCount;
    bool dmxError;
    bool refreshRateStable;

    int dmxDirPin;
    int dmxTxPin;
    int dmxRxPin;

    bool isChaseRunning;
    unsigned long chaseInterval;
    unsigned long lastChaseUpdate;
    size_t activeChaseStep;
    std::vector<String> chaseColors;

    bool isPulseRunning;
    int pulseChannel;
    unsigned long pulseStartTime;
    Color pulseColor;

    bool isStrobeRunning;
    int strobeChannel;
    int strobeFrequency;
    Color strobeColor;

    std::map<String, Color> predefinedColors;

    void initPredefinedColors();
    void sendDmxUniverse();
    void performDiagnostic();
    void updateChase();
    void updatePulse();
    void updateStrobe();
    Color parseColorInput(const String& colorInput);
    bool isNumeric(const String& str);
};

#endif
