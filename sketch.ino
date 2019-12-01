#include <NintendoExtensionCtrl.h>
#include <Adafruit_NeoPixel.h>

Nunchuk leftNchuk(Wire);
Nunchuk rightNchuk(Wire1);
Nunchuk controllers[] = {leftNchuk, rightNchuk};

#define CONTROLLER_COUNT 2
#define LED_PIN 17
#define LED_COUNT 300
// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 100

// int ledPosition = 0;
int ledPositions[] =  {0, 0};
#define RACECAR_LENGTH 5
#define RACECAR_STEP 5

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

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
      zButtonClickCounts[controllerIndex] += RACECAR_STEP;
      //Serial.println(zButtonClickCounts[controllerIndex]);
      zButtonDepressedVals[controllerIndex] = true;
    }
  } else {
    // z button released, reset
    if (zButtonDepressedVals[controllerIndex]) {
      zButtonDepressedVals[controllerIndex] = false;
    }
  }
}

void advancePlayer() {
  for (int playerIndex = 0; playerIndex < CONTROLLER_COUNT; playerIndex++) {
    // clear led position for end of racecar
    for (int led = ledPositions[playerIndex] - RACECAR_LENGTH; led < ledPositions[playerIndex] - RACECAR_LENGTH + RACECAR_STEP; led++) {
      strip.setPixelColor(led, strip.Color(0, 0, 0, 0));
    }

    strip.show();
    // display new position
    ledPositions[playerIndex] = zButtonClickCounts[playerIndex] % LED_COUNT;
  }
}

void setup() {
  initializeControllers();
  Wire.begin();

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    Nunchuk controller = controllers[controllerIndex];

    boolean controllerIsReady = controller.update();

    if (controllerIsReady) {
      recordZButtonClicks(controller, controllerIndex);
      advancePlayer();
    }

    // display players
    for (int i = ledPositions[controllerIndex]; i > ledPositions[controllerIndex] - RACECAR_LENGTH; i--) {
      if (controllerIndex == 0) {
        strip.setPixelColor(i, strip.Color(255, 0, 0, 0));
      } else if (controllerIndex == 1) {
        strip.setPixelColor(i, strip.Color(0, 0, 255, 0));
      }
      strip.show();
    }
  }
}
