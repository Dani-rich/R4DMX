#include <ArduinoBLE.h>

const char* xboxMacAddress = "F4:6A:D7:34:79:F2"; // Asegúrate de que esta sea la dirección correcta

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Iniciando BLE...");
  if (!BLE.begin()) {
    Serial.println("Error al iniciar BLE!");
    while (1);
  }

  Serial.println("BLE iniciado. Escaneando...");
  BLE.scanForAddress(xboxMacAddress);
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print("Dispositivo encontrado: ");
    Serial.println(peripheral.address());

    if (peripheral.address() == xboxMacAddress) {
      Serial.println("Mando Xbox encontrado. Intentando conectar...");
      BLE.stopScan();
      connectToXbox(peripheral);
    }
  }
}

void connectToXbox(BLEDevice peripheral) {
  if (peripheral.connect()) {
    Serial.println("Conectado al mando Xbox!");
    // Mantén la conexión por un tiempo para pruebas
    delay(5000);
    peripheral.disconnect();
  } else {
    Serial.println("Error al conectar!");
  }

  Serial.println("Reiniciando escaneo...");
  BLE.scanForAddress(xboxMacAddress);
}