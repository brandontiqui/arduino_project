#include <NintendoExtensionCtrl.h>
#include <Adafruit_NeoPixel.h>

Nunchuk leftNchuk(Wire);
Nunchuk rightNchuk(Wire1);
Nunchuk controllers[] = {leftNchuk, rightNchuk};

#define CONTROLLER_COUNT 1
#define LED_PIN 17
#define LED_COUNT 300
// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50

int ledPosition = 0;
#define RACECAR_LENGTH 5
#define RACECAR_STEP 1

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

void advancePlayer() {
  // clear led position for end of racecar
  for (int led = ledPosition - RACECAR_LENGTH; led < ledPosition - RACECAR_LENGTH + RACECAR_STEP; led++) {
    strip.setPixelColor(led, strip.Color(0, 0, 0, 0));
  }

  //strip.setPixelColor(ledPosition - RACECAR_LENGTH, strip.Color(0, 0, 0, 0));
  strip.show();
  // display new position
  ledPosition = zButtonClickCounts[0] % LED_COUNT;
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
  }

  for (int i = ledPosition; i > ledPosition - RACECAR_LENGTH; i--) {
    strip.setPixelColor(i, strip.Color(255, 0, 0, 0));
    strip.show();
  }
}
