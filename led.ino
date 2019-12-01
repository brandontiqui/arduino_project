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
#define BRIGHTNESS 100

int ledPositions[] =  {0, 1};
#define RACECAR_LENGTH 1
#define RACECAR_STEP 1
#define LED_TRACK_LENGTH 10

// Declare our NeoPixel strip object:
CRGB leds[LED_COUNT];

int zButtonClickCounts[] = {0, 0};
boolean zButtonDepressedVals[] = {false, false};

int playersAreReady[] = {0, 0};

// config
int startGameDelaySeconds = 5;

// game states
boolean waitingForPlayers = true;
boolean gameIsInPlay = false;
boolean gameIsEnding = false;

void initializeControllers() {
  Serial.println("Waiting for players");
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
  Serial.println("All controllers are ready");
}

void startGame() {
  int timeDelay = 1000;
  if (gameIsInPlay || gameIsEnding) {
    return;
  }

  print("All players are ready!");
  delay(timeDelay);

  for (int i = 0; i < startGameDelaySeconds; i++) {
    print(String(startGameDelaySeconds - i));
    delay(timeDelay);
  }
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
}

void endGame(int ledCount, int controllerIndex) {
  if (ledCount >= LED_TRACK_LENGTH) {
    gameIsEnding = true;
    print("Game over!");
    print("Player " + String(controllerIndex + 1) + " wins!");
    showcaseWinner(controllerIndex);
    delay(3000);
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
    print("Waiting for " + String(totalPlayersNotReady) + " players");
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
      leds[led] = 0x000000;
    }

    FastLED.show();
    // display new position
    ledPositions[playerIndex] = zButtonClickCounts[playerIndex] % LED_COUNT;
  }
}

void clearTrack() {
  for (int led = 0; led <= LED_TRACK_LENGTH; led++) {
    leds[led] = 0x000000;
  }
  FastLED.show();
}

void showcaseWinner(int playerIndex) {
  for (int led = ledPositions[playerIndex] - RACECAR_LENGTH; led <= ledPositions[playerIndex] - RACECAR_LENGTH + RACECAR_STEP; led++) {
    leds[led] = 0x000000;
  }

  FastLED.show();
  // display new position
  CRGB winnerColor = 0x00FF00;
  if (playerIndex == 1) {
    winnerColor = 0x0000FF;
  }
  for (int i = zButtonClickCounts[playerIndex]; i >=0; i-= RACECAR_STEP) {
    zButtonClickCounts[playerIndex] = i;
    ledPositions[playerIndex] = zButtonClickCounts[playerIndex] % LED_COUNT;
    leds[ledPositions[playerIndex]] = winnerColor;
    FastLED.show();
    delay(10);
  }
}

void setup() {
  Serial.begin(9600);

  initializeControllers();
  Wire.begin();

  FastLED.addLeds<WS2813, LED_PIN, RGB>(leds, LED_COUNT);
}

void loop() {
  if (waitingForPlayers) {
    // show how many players need to join
    waitForPlayers(-1);
  }

  // Serial.println("{\"a\": true }");

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
        leds[i] = 0x00FF00;
      } else if (controllerIndex == 1) {
        leds[i] = 0x0000FF;
      }
      FastLED.show();
    }
  }
}