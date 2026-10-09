
/* Stereo spectrum visualizer for an ESP32, spectrum shield, and 64x32 matrix.
   Adjust DISPLAY_INPUT_MAX to tune bar height for the shield's output level. */
#include <Adafruit_GFX.h>
#include <P3RGB64x32MatrixPanel.h>

P3RGB64x32MatrixPanel ledmatrix(14, 19, 21, 27, 25, 26, 15, 32, 33, 12, 16, 17, 18);

const uint8_t STROBE_PIN = 13;
const uint8_t RESET_PIN = 4;
const uint8_t RIGHT_CHANNEL_PIN = 34;
const uint8_t LEFT_CHANNEL_PIN = 39;
const uint8_t MATRIX_WIDTH = 64;
const uint8_t MATRIX_HEIGHT = 32;
const uint16_t DISPLAY_INPUT_MAX = 3000;
const uint16_t FRAME_INTERVAL_MS = 25;

uint16_t rightLevels[7];
uint16_t leftLevels[7];
unsigned long lastFrameAt = 0;

void readFrequencies() {
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, LOW);
  delayMicroseconds(30);

  for (uint8_t band = 0; band < 7; band++) {
    rightLevels[band] = analogRead(RIGHT_CHANNEL_PIN);
    leftLevels[band] = analogRead(LEFT_CHANNEL_PIN);

    digitalWrite(STROBE_PIN, HIGH);
    delayMicroseconds(30);
    digitalWrite(STROBE_PIN, LOW);
    delayMicroseconds(30);
  }
}

uint8_t barHeight(uint16_t level) {
  const long scaled = map(level, 0, DISPLAY_INPUT_MAX, 0, MATRIX_HEIGHT);
  return static_cast<uint8_t>(constrain(scaled, 0, MATRIX_HEIGHT));
}

void drawSpectrum() {
  ledmatrix.fillScreen(ledmatrix.color444(0, 0, 0));

  for (uint8_t band = 0; band < 7; band++) {
    const uint8_t leftHeight = barHeight(leftLevels[band]);
    const uint8_t rightHeight = barHeight(rightLevels[band]);
    const uint8_t leftX = 4 + band * 8;
    const uint8_t rightX = leftX + 4;

    if (leftHeight > 0) {
      ledmatrix.fillRect(leftX, MATRIX_HEIGHT - leftHeight, 3, leftHeight,
                         ledmatrix.color444(0, 12, 15));
    }
    if (rightHeight > 0) {
      ledmatrix.fillRect(rightX, MATRIX_HEIGHT - rightHeight, 3, rightHeight,
                         ledmatrix.color444(15, 7, 0));
    }
  }
}

void setup() {
  Serial.begin(115200);
  ledmatrix.begin();
  ledmatrix.fillScreen(ledmatrix.color444(0, 0, 0));

  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(RIGHT_CHANNEL_PIN, INPUT);
  pinMode(LEFT_CHANNEL_PIN, INPUT);
  digitalWrite(STROBE_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);
}

void loop() {
  const unsigned long now = millis();
  if (now - lastFrameAt < FRAME_INTERVAL_MS) {
    return;
  }

  lastFrameAt = now;
  readFrequencies();
  drawSpectrum();
}
