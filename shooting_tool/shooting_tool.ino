#include "M5Cardputer.h"
#include "M5GFX.h"

M5Canvas canvas(&M5Cardputer.Display);
String inputBuffer = "> ";
int currentStep = 0;
float elevation = 0.0;
float windage = 0.0;
float distance = 0.0;
bool useMOA = true;

int selectedMenuItem = 0;
const char* menuItems[] = {
  "▶ Ballistic Calculator",
  "▶ About / Version",
  "▶ Shutdown"
};
const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);
bool inMenu = true;

void drawMenu() {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setTextDatum(top_center);
  canvas.setTextColor(GREEN);
  int y = 20;
  for (int i = 0; i < menuItemCount; ++i) {
    if (i == selectedMenuItem) {
      canvas.setTextColor(RED);
      canvas.drawString(menuItems[i], canvas.width() / 2, y);
      canvas.setTextColor(GREEN);
    } else {
      canvas.drawString(menuItems[i], canvas.width() / 2, y);
    }
    y += 20;
  }
  canvas.pushSprite(4, 4);
}

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
  drawInputPrompt("Step 1 of 4:\nEnter bullet impact\n ELEVATION (+/- inches)", "-3.5");
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
  canvas.printf("Elevation: %.2f in → %.2f %s\n", elevation, abs(elevationAdj), elevation < 0 ? "\n Down" : "\n Up");
  canvas.printf("Windage  : %.2f in → %.2f %s\n", windage, abs(windageAdj), windage < 0 ? "\n Left" : "\n Right");
  canvas.printf("Distance : %.2f yd\n", distance);
  canvas.printf("Unit     : %s\n", useMOA ? "MOA" : "MIL");
  canvas.println("\nPress Enter to restart.");
  canvas.pushSprite(4, 4);
}

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setTextFont(&fonts::AsciiFont8x16);

  canvas.setTextFont(&fonts::AsciiFont8x16);
  canvas.setTextSize(1);
  canvas.createSprite(M5Cardputer.Display.width() - 8, M5Cardputer.Display.height() - 40);
  canvas.setTextColor(GREEN);
  canvas.setTextDatum(middle_center);

  drawMenu();
}

void loop() {
  M5Cardputer.update();

  if (inMenu) {
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_UP)) {
      selectedMenuItem = (selectedMenuItem - 1 + menuItemCount) % menuItemCount;
      drawMenu();
      delay(150);
    }

    if (M5Cardputer.Keyboard.isKeyPressed(KEY_DOWN)) {
      selectedMenuItem = (selectedMenuItem + 1) % menuItemCount;
      drawMenu();
      delay(150);
    }

    if (M5Cardputer.Keyboard.isKeyPressed('\n')) {
      if (selectedMenuItem == 0) {
        inMenu = false;
        resetAll();
      } else if (selectedMenuItem == 1) {
        canvas.clear();
        canvas.setTextColor(GREEN);
        canvas.setTextDatum(middle_center);
        canvas.drawString("Shooting Tool v1.0\nBuilt for M5Cardputer", canvas.width() / 2, canvas.height() / 2);
        canvas.pushSprite(4, 4);
        delay(3000);
        drawMenu();
      } else if (selectedMenuItem == 2) {
        M5Cardputer.Power.powerOff();
      }
    }
    return;
  }

  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
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
          drawInputPrompt("Step 2 of 4:\nEnter bullet impact\nWINDAGE (+/- inches)", "+1.2");
          break;
        case 1:
          windage = userInput.toFloat();
          drawInputPrompt("Step 3 of 4:\nEnter DISTANCE to\ntarget (yards)", "100");
          break;
        case 2:
          distance = userInput.toFloat();
          drawInputPrompt("Step 4 of 4:\nEnter unit type:\nMOA or MIL", "MOA");
          break;
        case 3:
          userInput.toLowerCase();
          if (userInput == "moa") useMOA = true;
          else if (userInput == "mil") useMOA = false;
          else {
            drawInputPrompt("Invalid unit.\nPlease type MOA or MIL", "MOA");
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
