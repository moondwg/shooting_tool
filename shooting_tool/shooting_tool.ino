#include "M5Cardputer.h"
#include "M5GFX.h"

M5Canvas canvas(&M5Cardputer.Display);
String inputBuffer = "> ";
int currentStep = 0;
float elevation = 0.0;
float windage = 0.0;
float distance = 0.0;
bool useMOA = true;
float moaPerClick = 0.25; // Default to 1 MOA per click

void drawInputPrompt(const String& prompt, const String& example = "") {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setCursor(0, 0);
  canvas.println(prompt);
  if (example.length() > 0) {
    canvas.println("Example: " + example);
  }
  canvas.pushSprite(4, 4);
  M5Cardputer.Display.drawString(inputBuffer, 4, M5Cardputer.Display.height() - 24);
}

void resetAll() {
  currentStep = 0;
  elevation = 0.0;
  windage = 0.0;
  distance = 0.0;
  useMOA = true;
  moaPerClick = 1.0;  // Default to 1 MOA per click
  inputBuffer = "> ";
  drawInputPrompt("Step 1 of 5: Enter bullet impact ELEVATION (+ above, - below)", "-3.5");
}

void showSummary() {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setCursor(0, 0);

  // Calculate adjustments in MOA or MIL
  float factor = useMOA ? 1.047 : 3.6; // MOA or MIL factor
  float elevationAdj = elevation / (distance * factor); // MOA/MIL adjustment for elevation
  float windageAdj = windage / (distance * factor); // MOA/MIL adjustment for windage

  // Calculate how much the user should adjust on the scope
  float scopeAdjFactor = useMOA ? 1.047 : 3.6;

  // Calculate the number of clicks for both elevation and windage
  float elevationClicks = elevationAdj / moaPerClick;
  float windageClicks = windageAdj / moaPerClick;

  // Display the summary with the required adjustment
  canvas.println("=== Adjustment Summary ===");
  canvas.printf("Elevation: %.2f in → %.2f %s (Adjust your scope by %.2f MOA)\n", elevation, abs(elevationAdj), elevation < 0 ? "Down" : "Up", elevationAdj);
  canvas.printf("Windage  : %.2f in → %.2f %s (Adjust your scope by %.2f MOA)\n", windage, abs(windageAdj), windage < 0 ? "Left" : "Right", windageAdj);
  canvas.printf("Distance : %.2f yd\n", distance);
  canvas.printf("Unit     : %s\n", useMOA ? "MOA" : "MIL");
  canvas.printf("MOA per Click: %.2f\n", moaPerClick);
  canvas.printf("Elevation Clicks: %.2f\n", elevationClicks);
  canvas.printf("Windage Clicks: %.2f\n", windageClicks);

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
            drawInputPrompt("Step 2 of 5: Enter bullet impact WINDAGE (+ right, - left)", "+1.2");
            break;
          case 1:
            windage = userInput.toFloat();
            drawInputPrompt("Step 3 of 5: Enter DISTANCE to target (yards)", "100");
            break;
          case 2:
            distance = userInput.toFloat();
            drawInputPrompt("Step 4 of 5: Enter unit type: MOA or MIL", "MOA");
            break;
          case 3:
            userInput.toLowerCase();
            if (userInput == "moa") useMOA = true;
            else if (userInput == "mil") useMOA = false;
            else {
              drawInputPrompt("Invalid unit. Please type MOA or MIL", "MOA");
              return;
            }
            drawInputPrompt("Step 5 of 5: Enter MOA per click (e.g., 1, 1/2, 1/4)", "1/4");
            break;
          case 4:
            if (userInput == "1") moaPerClick = 1.0;
            else if (userInput == "1/2") moaPerClick = 0.5;
            else if (userInput == "1/4") moaPerClick = 0.25;
            else {
              drawInputPrompt("Invalid MOA per click. Use 1, 1/2, or 1/4.", "1/4");
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
      M5Cardputer.Display.drawString(inputBuffer, 4, M5Cardputer.Display.height() - 24);
    }
  }
}
