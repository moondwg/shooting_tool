#include <M5Cardputer.h>
#include <SD.h>
#include <FS.h>

String inputString = "";
bool inputComplete = false;

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextColor(GREEN);
  M5Cardputer.Display.setTextSize(1);
  Serial.begin(115200);

  if (!SD.begin()) {
    M5Cardputer.Display.println("SD card initialization failed!");
    while (1);
  }

  mainMenu();
}

void loop() {
  M5Cardputer.update();

  if (M5Cardputer.Keyboard.isChange()) {
    auto keyEvent = M5Cardputer.Keyboard.readKeyEvent();
    if (keyEvent.pressed) {
      char key = keyEvent.char_code;

      if (key == '\n' || key == '\r') {
        inputComplete = true;
      } else if (key == 8) {  // Backspace
        if (inputString.length() > 0) {
          inputString.remove(inputString.length() - 1);
        }
      } else {
        inputString += key;
      }

      // Display input
      M5Cardputer.Display.clear();
      M5Cardputer.Display.setCursor(0, 0);
      M5Cardputer.Display.println(inputString);
    }
  }
}




void waitForKey() {
  inputString = "";
  inputComplete = false;
  while (!inputComplete) {
    loop();
  }
}

String getInput(String prompt) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(prompt);
  inputString = "";
  inputComplete = false;
  while (!inputComplete) {
    loop();
  }
  return inputString;
}

void mainMenu() {
  while (true) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(0, 0);
    M5Cardputer.Display.println("== Shooting Tools ==");
    M5Cardputer.Display.println("(1) Manage Profiles");
    M5Cardputer.Display.println("(2) MOA/MIL Calculator");
    M5Cardputer.Display.println("(3) Exit");

    inputString = "";
    inputComplete = false;
    while (!inputComplete) {
      loop();
    }

    if (inputString == "1") {
      manageProfiles();
    } else if (inputString == "2") {
      moaMilCalculator();
    } else if (inputString == "3") {
      M5Cardputer.Display.clear();
      M5Cardputer.Display.println("Goodbye!");
      delay(2000);
      ESP.restart();
    }
  }
}

void manageProfiles() {
  while (true) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(0, 0);
    M5Cardputer.Display.println("== Manage Profiles ==");
    M5Cardputer.Display.println("(1) New Profile");
    M5Cardputer.Display.println("(2) View Profiles");
    M5Cardputer.Display.println("(3) Back");

    inputString = "";
    inputComplete = false;
    while (!inputComplete) {
      loop();
    }

    if (inputString == "1") {
      newProfile();
    } else if (inputString == "2") {
      viewProfiles();
    } else if (inputString == "3") {
      return;
    }
  }
}

void newProfile() {
  String name = getInput("Profile Name:");
  String path = "/" + name;
  if (!SD.exists(path)) {
    SD.mkdir(path);
  }

  String distance = getInput("Distance (yds):");
  String verticalAdj = getInput("Vertical Adj (e.g., 1.5 MOA):");
  String horizontalAdj = getInput("Horizontal Adj (e.g., 0.5 MOA):");

  File file = SD.open(path + "/settings.txt", FILE_WRITE);
  if (file) {
    file.println("Distance: " + distance);
    file.println("Vertical: " + verticalAdj);
    file.println("Horizontal: " + horizontalAdj);
    file.close();
  }

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Profile Saved.");
  delay(1500);
}

void viewProfiles() {
  File root = SD.open("/");
  int index = 1;
  String profiles[20];
  
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("== Profiles ==");

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      profiles[index] = file.name();
      M5Cardputer.Display.print("(");
      M5Cardputer.Display.print(index);
      M5Cardputer.Display.print(") ");
      M5Cardputer.Display.println(file.name());
      index++;
    }
    file = root.openNextFile();
  }

  if (index == 1) {
    M5Cardputer.Display.println("No profiles found.");
    delay(2000);
    return;
  }

  inputString = "";
  inputComplete = false;
  while (!inputComplete) {
    loop();
  }

  int selection = inputString.toInt();
  if (selection > 0 && selection < index) {
    showProfile(profiles[selection]);
  }
}

void showProfile(String profileName) {
  String path = "/" + profileName + "/settings.txt";
  File file = SD.open(path);
  if (!file) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.println("Profile not found.");
    delay(2000);
    return;
  }

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("== " + profileName + " ==");

  while (file.available()) {
    M5Cardputer.Display.println(file.readStringUntil('\n'));
  }
  file.close();

  waitForKey();
}

void moaMilCalculator() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("MOA/MIL Calculator");

  String targetDistance = getInput("Distance to target (yds):");
  String impactOffset = getInput("Impact offset (inches):");
  String clicksPerMOA = getInput("Scope clicks per MOA:");

  float distanceYards = targetDistance.toFloat();
  float offsetInches = impactOffset.toFloat();
  float clicksPerMOAValue = clicksPerMOA.toFloat();

  float moa = (offsetInches / distanceYards) * 100;
  float clicks = moa * clicksPerMOAValue;
  float mil = (offsetInches / (distanceYards * 36)) * 1000;

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("== Adjustment ==");
  M5Cardputer.Display.print("MOA: ");
  M5Cardputer.Display.println(moa, 2);
  M5Cardputer.Display.print("Clicks: ");
  M5Cardputer.Display.println(clicks, 1);
  M5Cardputer.Display.print("MIL: ");
  M5Cardputer.Display.println(mil, 2);

  waitForKey();
}
