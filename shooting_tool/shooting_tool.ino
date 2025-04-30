#include "M5Cardputer.h"
#include "M5GFX.h"

M5Canvas canvas(&M5Cardputer.Display);
String inputBuffer = "> ";
int currentStep = 0;
float elevation = 0.0;
float windage = 0.0;
float distance = 0.0;
bool useMOA = true;

void drawInputPrompt(const String& prompt, const String& example = "") {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setCursor(0, 0);
  canvas.println(prompt);
  if (example.length() > 0) {
    canvas.println("Example: " + example);
  }
  canvas.pushSprite(4, 4);
}

void resetAll() {
  currentStep = 0;
  elevation = 0.0;
  windage = 0.0;
  distance = 0.0;
  useMOA = true;
  inputBuffer = "> ";
  drawInputPrompt("Step 1 of 4: Enter bullet impact ELEVATION (+ above, - below)", "-3.5");
}

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);

  // Set terminal green theme
  M5Cardputer.Display.setTextColor(GREEN, BLACK);
  M5Cardputer.Display.setTextFont(&fonts::Font6x10);
  M5Cardputer.Display.setTextSize(1);

  canvas.setTextFont(&fonts::Font6x10);
  canvas.setTextSize(1);
  canvas.setTextColor(GREEN, BLACK);
  canvas.createSprite(M5Cardputer.Display.width() - 8, M5Cardputer.Display.height() - 36);
  canvas.setTextScroll(true);

  resetAll();
}

void showSummary() {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setCursor(0, 0);

  float factor = useMOA ? 1.047 : 3.6;
  float elevationAdj = elevation / (distance * factor);
  float windageAdj = windage / (distance * factor);

  canvas.println("=== Summary ===");
  canvas.printf("Elevation: %.2f in → %.2f %s\n", elevation, abs(elevationAdj), elevation < 0 ? "Down" : "Up");
  canvas.printf("Windage  : %.2f in → %.2f %s\n", windage, abs(windageAdj), windage < 0 ? "Left" : "Right");
  canvas.printf("Distance : %.2f yd\n", distance);
  canvas.printf("Unit     : %s\n", useMOA ? "MOA" : "MIL");
  canvas.println("\nPress Enter to restart.");
  canvas.pushSprite(4, 4);
}

void loop() {
  M5Cardputer.update();

  static unsigned long lastBlink = 0;
  static bool showCursor = true;

  // Blink cursor logic
  if (millis() - lastBlink > 500) {
    lastBlink = millis();
    showCursor = !showCursor;
  }

  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

      for (auto c : status.word) inputBuffer += c;

      if (status.del && inputBuffer.length() > 2)
        inputBuffer.remove(inputBuffer.length() - 1);

      if (status.enter) {
        String userInput = inputBuffer.substring(2);
        inputBuffer = "> ";

        switch (currentStep) {
          case 0:
            elevation = userInput.toFloat();
            drawInputPrompt("Step 2 of 4: Enter bullet impact WINDAGE (+ right, -
