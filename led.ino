#include <NintendoExtensionCtrl.h>
#include <FastLED.h>

Nunchuk leftNchuk(Wire);
Nunchuk rightNchuk(Wire1);
Nunchuk controllers[] = {leftNchuk, rightNchuk};

#define CONTROLLER_COUNT 2
#define LED_PIN 17
#define CLOCK_PIN 17
#define LED_COUNT 300
// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 300

// int ledPosition = 0;
int ledPositions[] =  {0, 1};
#define RACECAR_LENGTH 1
#define RACECAR_STEP 1
#define LED_TRACK_LENGTH 200
#define NUM_COLORS 12

// Declare our NeoPixel strip object:
//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[LED_COUNT];

int zButtonClickCounts[] = {0, 0};
boolean zButtonDepressedVals[] = {false, false};
boolean cButtonDepressedVals[] = {false, false};

int playersAreReady[] = {0, 0};

// config
int startGameDelaySeconds = 5;
CRGB colors[] = {0x00FF00, 0x0000FF, 0xFF0000, 0xFFFF00, 0x800000, 0x008000, 0x00FFFF, 0x008080, 0x000080, 0xFF00FF, 0x800080, 0xffa500};
int playerColorIndices[] = {0, 1};

// game states
boolean waitingForPlayers = true;
boolean gameIsInPlay = false;
boolean gameIsEnding = false;

/**
  * Waiting for players: players cannot advance
  * All players ready: after 3-2-1 countdown allow advancement
  *
  **/

void initializeControllers() {
  print("Waiting for players");
  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    Nunchuk controller = controllers[controllerIndex];
    controller.begin();
    controller.i2c().setClock(400000);

    boolean controllerIsConnected = false;
    while (!(controllerIsConnected)) {
      controllerIsConnected  = controller.connect();
    }
  }
}

void connectControllers() {
  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    boolean success = controllers[controllerIndex].update();
    if (!success) {
      controllers[controllerIndex].reconnect();
    }
  }
}

void startGame() {
  int timeDelay = 1000;
  if (gameIsInPlay || gameIsEnding) {
    return;
  }

  ledPositions[0] = 0;
  ledPositions[1] = 0;

  print("All players are ready!");
  delay(timeDelay);
  print("Click the Z button as fast as you can!");
  delay(timeDelay);
  for (int i = 0; i < startGameDelaySeconds; i++) {
    print(String(startGameDelaySeconds - i));
    delay(timeDelay);
  }
  clearTrack();
  gameIsInPlay = true;
  print("Go!");
}

void resetGame() {
  ledPositions[0] = 0;
  ledPositions[1] = 1;
  zButtonClickCounts[0] = 0;
  zButtonClickCounts[1] = 0;
  zButtonDepressedVals[0] = false;
  zButtonDepressedVals[1] = false;
  playersAreReady[0] = 0;
  playersAreReady[1] = 0;
  waitingForPlayers = true;
  gameIsInPlay = false;
  gameIsEnding = false;
  // connectControllers();
}

void endGame(int ledCount, int controllerIndex) {
  if (ledCount >= LED_TRACK_LENGTH) {
    gameIsEnding = true;
    print("Player " + String(controllerIndex + 1) + " wins!");
    delay(10);
    // TODO: reset game
    showcaseWinner(controllerIndex);
    //delay(3000);
    clearTrack();
    resetGame();
  }
}

void print(String data) {
  Serial.println("\"" + data + "\"");
}

void waitForPlayers(int controllerIndex) {
  // count of players who are ready
  int totalPlayersNotReady = CONTROLLER_COUNT;

  if (controllerIndex != -1) {
    // controller index of player who is ready
    playersAreReady[controllerIndex] = 1;
  }
  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    if (playersAreReady[controllerIndex] == 1) {
      totalPlayersNotReady--;
    }
  }

  if (totalPlayersNotReady == 0) {
    waitingForPlayers = false;
    startGame();
  } else {
    if (totalPlayersNotReady == 1) {
      print("Waiting for " + String(totalPlayersNotReady) + " player.  Click the Z button to join.  Click the C button to change your color.");
    } else {
      print("Waiting for " + String(totalPlayersNotReady) + " players.  Click the Z button to join.  Click the C button to change your color.");
    }
  }
}

void recordZButtonClicks(Nunchuk controller, int controllerIndex) {
  if (controller.buttonZ()) {
    if (!zButtonDepressedVals[controllerIndex]) {
      // signal that player is ready
      waitForPlayers(controllerIndex);

      // still waiting for players or game to start, do not advance
      if (!gameIsInPlay || gameIsEnding) {
        return;
      }

      zButtonClickCounts[controllerIndex] += RACECAR_STEP;
      // check if game should end
      endGame(zButtonClickCounts[controllerIndex], controllerIndex);
      zButtonDepressedVals[controllerIndex] = true;
    }
  } else if (!controller.buttonC()) {
    // z button released, reset
    if (zButtonDepressedVals[controllerIndex]) {
      zButtonDepressedVals[controllerIndex] = false;
    }
  }
}

void recordCButtonClicks(Nunchuk controller, int controllerIndex) {
  if (controller.buttonC()) {
    if (!cButtonDepressedVals[controllerIndex]) {
      // still waiting for players or game to start, do not advance
      if (gameIsInPlay) {
        // return;
      }
      int playerColorIndex = playerColorIndices[controllerIndex];
      if (playerColorIndex < NUM_COLORS - 1) {
        playerColorIndices[controllerIndex]++;
      } else {
        playerColorIndices[controllerIndex] = 0;
      }
      cButtonDepressedVals[controllerIndex] = true;
    }
  } else if (!controller.buttonZ()) {
    // c button released, reset
    if (cButtonDepressedVals[controllerIndex]) {
      cButtonDepressedVals[controllerIndex] = false;
    }
  }
}

void advancePlayer() {
  if (waitingForPlayers) {
    return;
  }

  for (int playerIndex = 0; playerIndex < CONTROLLER_COUNT; playerIndex++) {
    // clear led position for end of racecar
    for (int led = ledPositions[playerIndex] - RACECAR_LENGTH; led < ledPositions[playerIndex] - RACECAR_LENGTH + RACECAR_STEP; led++) {
      //strip.setPixelColor(led, strip.Color(0, 0, 0, 0));
      //leds[led] = CRGB::Black;
      leds[led] = 0x000000;
    }

    //strip.show();
    FastLED.show();
    // display new position
    ledPositions[playerIndex] = zButtonClickCounts[playerIndex] % LED_COUNT;
  }
}

void clearTrack() {
  // clear led position for end of racecar
  for (int led = 0; led <= LED_TRACK_LENGTH; led++) {
    leds[led] = 0x000000;
  }
  FastLED.show();
}

void showcaseWinner(int playerIndex) {
  // clear led position for end of racecar
  for (int led = ledPositions[playerIndex] - RACECAR_LENGTH; led <= ledPositions[playerIndex] - RACECAR_LENGTH + RACECAR_STEP; led++) {
    //strip.setPixelColor(led, strip.Color(0, 0, 0, 0));
    //leds[led] = CRGB::Black;
    leds[led] = 0x000000;
  }

  //strip.show();
  FastLED.show();
  // display new position
  for (int i = zButtonClickCounts[playerIndex]; i >=0; i-= RACECAR_STEP) {
    zButtonClickCounts[playerIndex] = i;
    ledPositions[playerIndex] = zButtonClickCounts[playerIndex] % LED_COUNT;
    leds[ledPositions[playerIndex]] = colors[playerColorIndices[playerIndex]];
    FastLED.show();
    delay(10);
  }
}

void setup() {
  Serial.begin(9600);

  initializeControllers();
  Wire.begin();

  //FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.addLeds<WS2813, LED_PIN, RGB>(leds, LED_COUNT);
  /*
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  */
}

void loop() {
  if (waitingForPlayers) {
    // show how many players need to join
    waitForPlayers(-1);
  }

//  else {
//    Serial.println("{\"a\": true }");
//  }

  for (int controllerIndex = 0; controllerIndex < CONTROLLER_COUNT; controllerIndex++) {
    Nunchuk controller = controllers[controllerIndex];

    boolean controllerIsReady = controller.update();

    if (controllerIsReady) {
      recordZButtonClicks(controller, controllerIndex);
      recordCButtonClicks(controller, controllerIndex);
      advancePlayer();
    } else {
      controller.reconnect();
    }

    // display players
    for (int i = ledPositions[controllerIndex]; i > ledPositions[controllerIndex] - RACECAR_LENGTH; i--) {
      leds[i] = colors[playerColorIndices[controllerIndex]];
      FastLED.show();
    }
  }

}