#include <NintendoExtensionCtrl.h>

Nunchuk leftNchuk(Wire);

void setup() {
  Serial.println("test");
  // I2C Init
  leftNchuk.begin();
  leftNchuk.i2c().setClock(400000);

  boolean leftConnected = false;

  // connect left controller
  while (!(leftConnected)) {
    Serial.println("left is not connected");
    if (!leftConnected)  { leftConnected  = leftNchuk.connect(); }
  }
  Serial.println("left is connected");
}

void loop() {
  if (leftNchuk.buttonC()) {
    Serial.println("left c pressed");
  }
  if (leftNchuk.buttonZ()) {
    Serial.println("left z pressed");
  }
}
