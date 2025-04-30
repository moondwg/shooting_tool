#include <M5Cardputer.h>
#include <LGFX_AUTODETECT.hpp>

// State variables
int currentStep = 0;
float elevation = 0.0;
float windage = 0.0;
float distance = 0.0;
bool useMOA = true;
String inputBuffer = "> ";

LGFX_Sprite canvas(&M5Cardputer.Display);

// Function declarations
void drawInputPrompt(const String& prompt, const String& example);
void resetAll();
void showSummary();

void setup() {
  M5.begin();
  canvas.setColorDepth(8);
  canvas.createSprite(320, 240);
  resetAll();
}

void loop() {
  M5.update();
  auto status = M5Cardputer.Keyboard.keysState();

  if (status.wasPressed) {
    char c = status.charPressed;

    if (c == '\n') {
      // Handle Enter key
      if (currentStep == 0) {
        elevation = inputBuffer.substring(2).toFloat();
        currentStep++;
        inputBuffer = "> ";
        drawInputPrompt("Windage offset (inches):", "e.g. -3.5");
      } else if (currentStep == 1) {
        windage = inputBuffer.substring(2).toFloat();
        currentStep++;
        inputBuffer = "> ";
        drawInputPrompt("Distance to target (yards):", "e.g. 100");
      } else if (currentStep == 2) {
        distance = inputBuffer.substring(2).toFloat();
        currentStep++;
        inputBuffer = "> ";
        drawInputPrompt("Use MOA or MIL? (M/m for MOA, else MIL)", "e.g. M");
      } else if (currentStep == 3) {
        String mode = inputBuffer.substring(2);
        mode.toUpperCase();
        useMOA = (mode == "M");
        showSummary();
      }
    } else if (c == '\b') {
      // Handle backspace
      if (inputBuffer.length() > 2) {
        inputBuffer.remove(inputBuffer.length() - 1);
        drawInputPrompt((currentStep == 0) ? "Elevation offset (inches):" :
                         (currentStep == 1) ? "Windage offset (inches):" :
                         (currentStep == 2) ? "Distance to target (yards):" :
                         "Use MOA or MIL? (M/m for MOA, else MIL)",
                         (currentStep == 0) ? "e.g. +2.5" :
                         (currentStep == 1) ? "e.g. -3.5" :
                         (currentStep == 2) ? "e.g. 100" : "e.g. M");
      }
    } else {
      inputBuffer += c;
      drawInputPrompt((currentStep == 0) ? "Elevation offset (inches):" :
                       (currentStep == 1) ? "Windage offset (inches):" :
                       (currentStep == 2) ? "Distance to target (yards):" :
                       "Use MOA or MIL? (M/m for MOA, else MIL)",
                       (currentStep == 0) ? "e.g. +2.5" :
                       (currentStep == 1) ? "e.g. -3.5" :
                       (currentStep == 2) ? "e.g. 100" : "e.g. M");
    }
  }
}

void drawInputPrompt(const String& prompt, const String& example) {
  canvas.fillSprite(BLACK);
  canvas.setTextColor(WHITE);
  canvas.setTextSize(1);
  canvas.setCursor(10, 10);
  canvas.print(prompt);

  canvas.setCursor(10, 40);
  canvas.setTextColor(DARKGREY);
  canvas.print(example);

  canvas.setCursor(10, 80);
  canvas.setTextColor(GREEN);
  canvas.print(inputBuffer);

  canvas.pushSprite(0, 0);
}

void resetAll() {
  currentStep = 0;
  elevation = 0.0;
  windage = 0.0;
  distance = 0.0;
  useMOA = true;
  inputBuffer = "> ";
  drawInputPrompt("Elevation offset (inches):", "e.g. +2.5");
}

void showSummary() {
  canvas.fillSprite(BLACK);
  canvas.setTextColor(WHITE);
  canvas.setTextSize(1);
  canvas.setCursor(10, 10);

  String unit = useMOA ? "MOA" : "MIL";
  float factor = useMOA ? 1.047 : 3.6;

  float elevationAdj = (elevation * factor) / distance;
  float windageAdj = (windage * factor) / distance;

  canvas.printf("Correction Summary (%s):\n", unit.c_str());
  canvas.printf("Distance: %.1f yds\n", distance);
  canvas.printf("Elevation Offset: %.2f in\n", elevation);
  canvas.printf("Windage Offset: %.2f in\n", windage);
  canvas.printf("-> Elevation Adj: %.2f %s\n", elevationAdj, unit.c_str());
  canvas.printf("-> Windage Adj: %.2f %s\n", windageAdj, unit.c_str());

  canvas.setCursor(10, 160);
  canvas.setTextColor(YELLOW);
  canvas.print("Press any key to restart.");

  canvas.pushSprite(0, 0);

  while (true) {
    M5.update();
    auto status = M5Cardputer.Keyboard.keysState();
    if (status.wasPressed) {
      resetAll();
      break;
    }
  }
}
