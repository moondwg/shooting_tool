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

  int W = canvas.width();
  int H = canvas.height();

  canvas.setTextColor(GREEN);
  canvas.setTextDatum(middle_center);
  canvas.drawString(prompt, W / 2, H / 2 - 10);
  if (example.length() > 0) {
    canvas.drawString("Example: " + example, W / 2, H / 2 + 10);
  }

  canvas.pushSprite(4, 4);

  M5Cardputer.Display.setTextDatum(middle_center);
  M5Cardputer.Display.setTextColor(GREEN);
  M5Cardputer.Display.drawString(inputBuffer, M5Cardputer.Display.width() / 2, M5Cardputer.Display.height() - 12);
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
  M5Cardputer.Display.setTextSize(2);  // Bigger text
  M5Cardputer.Display.setTextFont(&fonts::Font2x3);  // Change font to Font2x3

  canvas.setTextFont(&fonts::Font2x3);
  canvas.setTextSize(1);
  canvas.createSprite(M5Cardputer.Display.width() - 8, M5Cardputer.Display.height() - 40);
  canvas.setTextColor(GREEN);
  canvas.setTextDatum(middle_center);

  resetAll();
}

void showSummary() {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setCursor(0, 0);
  canvas.setTextDatum(top_left);

  float factor = useMOA ? 1.047 : 3.6;
  float elevationAdj = elevation / (distance * factor);
  float windageAdj = windage / (distance * factor);

  canvas.setTextColor(GREEN);
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
            drawInputPrompt("Step 2 of 4: Enter bullet impact WINDAGE (+ right, - left)", "+1.2");
            break;
          case 1:
            windage = userInput.toFloat();
            drawInputPrompt("Step 3 of 4: Enter DISTANCE to target (yards)", "100");
            break;
          case 2:
            distance = userInput.toFloat();
            drawInputPrompt("Step 4 of 4: Enter unit type: MOA or MIL", "MOA");
            break;
          case 3:
            userInput.toLowerCase();
            if (userInput == "moa") useMOA = true;
            else if (userInput == "mil") useMOA = false;
            else {
              drawInputPrompt("Invalid unit. Please type MOA or MIL", "MOA");
              return;
            }
            showSummary();
            break;
          default:
            resetAll();
            return;
        }
        currentStep++;
      }

      M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 28, M5Cardputer.Display.width(), 25, BLACK);
      M5Cardputer.Display.drawString(inputBuffer, M5Cardputer.Display.width() / 2, M5Cardputer.Display.height() - 12);
    }
  }
}
