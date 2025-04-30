#include <M5Cardputer.h>
#include <SD.h>

String inputString = "";
bool inputComplete = false;

void setup() {
  M5Cardputer.begin();
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(0, 0);
  mainMenu();
}

void loop() {
  M5Cardputer.update();

  if (M5Cardputer.Keyboard.isChange()) {
    for (int row = 0; row < 5; row++) {
      for (int col = 0; col < 14; col++) {
        if (M5Cardputer.Keyboard.isPressed({col, row})) {
          char keyChar = M5Cardputer.Keyboard.getChar({col, row});
          if (keyChar) {
            inputString += keyChar;

            // Display the character
            M5Cardputer.Display.clear();
            M5Cardputer.Display.setCursor(0, 0);
            M5Cardputer.Display.print("Input: ");
            M5Cardputer.Display.println(inputString);

            // Check for enter key to submit
            if (keyChar == '\n' || keyChar == '\r') {
              inputComplete = true;
            }
          }
        }
      }
    }
  }

  if (inputComplete) {
    processInput(inputString);
    inputString = "";
    inputComplete = false;
    delay(1000);
    mainMenu();
  }
}

void mainMenu() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Toy Shooting Tool");
  M5Cardputer.Display.println("1. Start Shooting");
  M5Cardputer.Display.println("2. Calibrate");
  M5Cardputer.Display.println("3. Settings");
  M5Cardputer.Display.println("Enter option:");
}

void processInput(String input) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);

  if (input.startsWith("1")) {
    M5Cardputer.Display.println("Starting shooting...");
    // Your shooting logic here
  } else if (input.startsWith("2")) {
    M5Cardputer.Display.println("Calibrating...");
    // Your calibration logic here
  } else if (input.startsWith("3")) {
    M5Cardputer.Display.println("Opening settings...");
    // Your settings logic here
  } else {
    M5Cardputer.Display.println("Invalid option");
  }
}
