#include "M5Cardputer.h"
#include "M5GFX.h"

M5Canvas canvas(&M5Cardputer.Display);

enum AppState {
  STATE_MENU,
  STATE_SHOOTING_TOOL,
};

AppState appState = STATE_MENU;

int menuIndex = 0;
const int menuItems = 1;
String menuOptions[] = { "🧠 Ballistic Calculator" };

String inputBuffer = "> ";
int currentStep = 0;
float elevation = 0.0;
float windage = 0.0;
float distance = 0.0;
bool useMOA = true;

// === DRAW MAIN MENU ===
void drawMainMenu() {
  M5Cardputer.Display.clear();
  canvas.clear();
  canvas.setCursor(0, 0);
  canvas.setTextSize(1);
  canvas.setTextDatum(top_left);
  canvas.setTextColor(GREEN);
  canvas.println("<< Tactical Tool Suite >>\n");

  for (int i = 0; i < menuItems; ++i) {
    if (i == menuIndex) {
      canvas.setTextColor(WHITE, GREEN);  // Highlighted
    } else {
      canvas.setTextColor(GREEN);
    }
    canvas.printf("  %s\n", menuOptions[i].c_str());
  }

  canvas.setTextColor(GREEN);
  canvas.println("\nUse ↑ ↓ to navigate.\nPress Enter to launch.");

  canvas.pushSprite(4, 4);
}

// === DRAW SHOOTING TOOL PROMPT ===
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

// === RESET SHOOTING TOOL STATE ===
void resetAll() {
  currentStep = 0;
  elevation = 0.0;
  windage = 0.0;
  distance = 0.0;
  useMOA = true;
  inputBuffer = "> ";
  drawInputPrompt("Step 1 of 4:\nEnter bullet impact\n ELEVATION (+/- inches)", "-3.5");
}

// === SHOW SUMMARY ===
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

// === SETUP ===
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

  drawMainMenu();
}

// === LOOP ===
void loop() {
  M5Cardputer.update();

  if (appState == STATE_MENU) {
    if (M5Cardputer.Keyboard.isKeyPressed('w')) {  // UP arrow (w key)
      if (menuIndex > 0) menuIndex--;
      drawMainMenu();
      delay(150);
    }
    if (M5Cardputer.Keyboard.isKeyPressed('s')) {  // DOWN arrow (s key)
      if (menuIndex < menuItems - 1) menuIndex++;
      drawMainMenu();
      delay(150);
    }
    if (M5Cardputer.Keyboard.isKeyPressed('\n')) {  // Enter key
      if (menuIndex == 0) {
        appState = STATE_SHOOTING_TOOL;
        resetAll();
      }
    }
    return;
  }

  // SHOOTING TOOL
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
}
