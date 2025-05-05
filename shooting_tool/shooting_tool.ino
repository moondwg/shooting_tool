#include <M5Cardputer.h>

#define KEY_UP    0x52  // Arrow up
#define KEY_DOWN  0x51  // Arrow down
#define KEY_OK    '\n'  // Enter key

// Forward declaration
void drawInputPrompt(const String& prompt, const String& example = "");
void resetAll(); // Your first tool

typedef void (*ToolHandler)();  // Function pointer type
ToolHandler menuHandlers[] = { resetAll };
const char* menuItems[] = { "Ballistic Calculator" };
const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

int selectedMenuIndex = 0;
bool inTool = false;

void setup() {
  M5.begin();
  M5Cardputer.begin();
  M5Cardputer.update();
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Initializing...");
  delay(500);
}

void drawMenu() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("== M5 Shooting Tools ==\n");

  for (int i = 0; i < menuLength; ++i) {
    if (i == selectedMenuIndex) {
      M5Cardputer.Display.print("> ");
    } else {
      M5Cardputer.Display.print("  ");
    }
    M5Cardputer.Display.println(menuItems[i]);
  }

  M5Cardputer.Display.println("\nUse UP/DOWN to navigate.\nPress ENTER to select.");
}

void drawInputPrompt(const String& prompt, const String& example) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(prompt);
  if (example != "") {
    M5Cardputer.Display.print("Example: ");
    M5Cardputer.Display.println(example);
  }
}

void resetAll() {
  drawInputPrompt("Step 1 of 4:\nEnter bullet impact\n ELEVATION (+/- inches)", "-3.5");

  while (true) {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_OK)) {
      break;
    }
  }

  drawInputPrompt("Step 2 of 4:\nEnter bullet impact\n WINDAGE (+/- inches)", "+1.2");
  while (true) {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_OK)) {
      break;
    }
  }

  drawInputPrompt("Step 3 of 4:\nEnter distance to target (yards)", "100");
  while (true) {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_OK)) {
      break;
    }
  }

  drawInputPrompt("Step 4 of 4:\nEnter scope click value (inches/click)", "0.25");
  while (true) {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_OK)) {
      break;
    }
  }

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Calculating adjustments...");
  delay(2000);
  M5Cardputer.Display.println("Adjust: +14 UP, -3 RIGHT");

  delay(3000);
  inTool = false;
  drawMenu();
}

void loop() {
  M5Cardputer.update();

  if (!inTool) {
    if (M5Cardputer.Keyboard.isKeyPressed(KEY_UP)) {
      selectedMenuIndex = (selectedMenuIndex - 1 + menuLength) % menuLength;
      drawMenu();
      delay(200);
    }

    if (M5Cardputer.Keyboard.isKeyPressed(KEY_DOWN)) {
      selectedMenuIndex = (selectedMenuIndex + 1) % menuLength;
      drawMenu();
      delay(200);
    }

    if (M5Cardputer.Keyboard.isKeyPressed(KEY_OK)) {
      inTool = true;
      menuHandlers[selectedMenuIndex]();
    }
  }
}
