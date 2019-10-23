#include <NintendoExtensionCtrl.h>

Nunchuk leftNchuk(Wire);
Nunchuk rightNchuk(Wire1);
Nunchuk controllers[] = {leftNchuk, rightNchuk};
#define CONTROLLER_COUNT 2

int zButtonClickCounts[] = {0, 0};
boolean zButtonDepressedVals[] = {false, false};

void initializeControllers() {
  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    Nunchuk controller = controllers[controllerIndex];
    controller.begin();
    controller.i2c().setClock(400000);

    boolean controllerIsConnected = false;
    while (!(controllerIsConnected)) {
      controllerIsConnected  = controller.connect();
    }
    Serial.println("Controller connected");
  }
}

void recordZButtonClicks(Nunchuk controller, int controllerIndex) {
  if (controller.buttonZ()) {
    if (!zButtonDepressedVals[controllerIndex]) {
      zButtonClickCounts[controllerIndex]++;
      Serial.println(zButtonClickCounts[controllerIndex]);
      zButtonDepressedVals[controllerIndex] = true;
    }
  } else {
    // z button released, reset
    if (zButtonDepressedVals[controllerIndex]) {
      zButtonDepressedVals[controllerIndex] = false;
    }
  }
}

void setup() {
  initializeControllers();
}

void loop() {
  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    Nunchuk controller = controllers[controllerIndex];

    boolean controllerIsReady = controller.update();

    if (controllerIsReady) {
      recordZButtonClicks(controller, controllerIndex);
    }
  }
}
