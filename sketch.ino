#include <NintendoExtensionCtrl.h>

Nunchuk leftNchuk(Wire);
Nunchuk rightNchuk(Wire1);

void setup() {
  Serial.println("test");
  // I2C Init
  leftNchuk.begin();
  rightNchuk.begin();
  leftNchuk.i2c().setClock(400000);
  rightNchuk.i2c().setClock(400000);

  boolean leftConnected = false;
  boolean rightConnected = false;

  // connect left controller
  while (!(leftConnected)) {
    Serial.println("left is not connected");
    leftConnected  = leftNchuk.connect();
  }
  Serial.println("left is connected");
  // connect right controller
  while (!(rightConnected)) {
    Serial.println("right is not connected");
    rightConnected = rightNchuk.connect();
  }
  Serial.println("right is connected");
}

void loop() {
  boolean leftReady = leftNchuk.update();
  boolean rightReady = rightNchuk.update();

  if (leftReady) {
    if (leftNchuk.buttonC()) {
      Serial.println("left c pressed");
    }
    if (leftNchuk.buttonZ()) {
      Serial.println("left z pressed");
    }
  }

  if (rightReady) {
    if (rightNchuk.buttonC()) {
      Serial.println("right c pressed");
    }
    if (rightNchuk.buttonZ()) {
      Serial.println("right z pressed");
    }
  }
}
