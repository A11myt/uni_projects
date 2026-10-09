#include <Adafruit_GFX.h>
#include <P3RGB64x32MatrixPanel.h>

P3RGB64x32MatrixPanel ledmatrix(14, 19, 21, 27, 25, 26, 15, 32, 33, 12, 16, 17, 18);

const uint8_t PLAYER1_IN = A4;
const uint8_t PLAYER2_IN = A5;
const int CANVAS_WIDTH = 64;
const int CANVAS_HEIGHT = 32;
const int GUTTER_WIDTH = 2;
const int PAD_WIDTH = 2;
const int PAD_HEIGHT = 8;
const int BALL_SIZE = 2;
const int ADC_MAX = 4095;
const unsigned long FRAME_INTERVAL_MS = 25;

int player1Pad = 0;
int player2Pad = 0;
float ballX = CANVAS_WIDTH / 2.0f - BALL_SIZE / 2.0f;
float ballY = CANVAS_HEIGHT / 2.0f - BALL_SIZE / 2.0f;
float ballSpeedX = 1.0f;
float ballSpeedY = 0.45f;
int player1Score = 0;
int player2Score = 0;
unsigned long lastFrameAt = 0;

void resetBall(int direction) {
  ballX = CANVAS_WIDTH / 2.0f - BALL_SIZE / 2.0f;
  ballY = CANVAS_HEIGHT / 2.0f - BALL_SIZE / 2.0f;
  ballSpeedX = direction * 1.0f;
  ballSpeedY = (millis() & 1) ? 0.45f : -0.45f;
}

void updatePaddles() {
  player1Pad = map(analogRead(PLAYER1_IN), 0, ADC_MAX, 0, CANVAS_HEIGHT - PAD_HEIGHT);
  player2Pad = map(analogRead(PLAYER2_IN), 0, ADC_MAX, 0, CANVAS_HEIGHT - PAD_HEIGHT);
}

void updateBall() {
  ballX += ballSpeedX;
  ballY += ballSpeedY;

  if (ballY <= 0) {
    ballY = 0;
    ballSpeedY = -ballSpeedY;
  } else if (ballY >= CANVAS_HEIGHT - BALL_SIZE) {
    ballY = CANVAS_HEIGHT - BALL_SIZE;
    ballSpeedY = -ballSpeedY;
  }

  const int leftPadX = GUTTER_WIDTH;
  const int rightPadX = CANVAS_WIDTH - GUTTER_WIDTH - PAD_WIDTH;
  if (ballSpeedX < 0 && ballX <= leftPadX + PAD_WIDTH
      && ballX + BALL_SIZE >= leftPadX
      && ballY + BALL_SIZE >= player1Pad && ballY <= player1Pad + PAD_HEIGHT) {
    const float hitOffset = (ballY + BALL_SIZE / 2.0f - (player1Pad + PAD_HEIGHT / 2.0f)) / 4.0f;
    ballX = leftPadX + PAD_WIDTH;
    ballSpeedX = -ballSpeedX;
    ballSpeedY = hitOffset * 1.2f;
  } else if (ballSpeedX > 0 && ballX + BALL_SIZE >= rightPadX
             && ballX <= rightPadX + PAD_WIDTH
             && ballY + BALL_SIZE >= player2Pad && ballY <= player2Pad + PAD_HEIGHT) {
    const float hitOffset = (ballY + BALL_SIZE / 2.0f - (player2Pad + PAD_HEIGHT / 2.0f)) / 4.0f;
    ballX = rightPadX - BALL_SIZE;
    ballSpeedX = -ballSpeedX;
    ballSpeedY = hitOffset * 1.2f;
  }

  if (ballX + BALL_SIZE < 0) {
    player2Score++;
    Serial.print("PLAYER2SCORE:");
    Serial.println(player2Score);
    resetBall(1);
  } else if (ballX > CANVAS_WIDTH) {
    player1Score++;
    Serial.print("PLAYER1SCORE:");
    Serial.println(player1Score);
    resetBall(-1);
  }
}

void drawGame() {
  ledmatrix.fillScreen(ledmatrix.color444(0, 0, 0));
  for (int y = 0; y < CANVAS_HEIGHT; y += 4) {
    ledmatrix.fillRect(CANVAS_WIDTH / 2, y, 1, 2, ledmatrix.color444(3, 3, 3));
  }
  ledmatrix.fillRect(GUTTER_WIDTH, player1Pad, PAD_WIDTH, PAD_HEIGHT,
                     ledmatrix.color444(0, 15, 10));
  ledmatrix.fillRect(CANVAS_WIDTH - GUTTER_WIDTH - PAD_WIDTH, player2Pad, PAD_WIDTH, PAD_HEIGHT,
                     ledmatrix.color444(15, 8, 0));
  ledmatrix.fillRect(static_cast<int>(ballX), static_cast<int>(ballY), BALL_SIZE, BALL_SIZE,
                     ledmatrix.color444(15, 0, 15));
}

void setup() {
  Serial.begin(115200);
  ledmatrix.begin();
  ledmatrix.setTextSize(1);
  ledmatrix.fillScreen(ledmatrix.color444(0, 0, 0));
  lastFrameAt = millis();
}

void loop() {
  updatePaddles();
  const unsigned long now = millis();
  if (now - lastFrameAt < FRAME_INTERVAL_MS) return;

  lastFrameAt = now;
  updateBall();
  drawGame();
}
