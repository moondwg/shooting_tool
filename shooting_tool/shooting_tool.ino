#include <M5Cardputer.h>
#include <Wire.h>
#include <CardKB.h>
#include <SD.h>
#include <FS.h>

String inputString = "";
bool inputComplete = false;

void setup() {
  M5.begin();
  Wire.begin();
  CardKB.begin();
  Serial.begin(115200);

  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    while (1);
  }

  mainMenu();
}

void loop() {
  if (CardKB.available()) {
    char c = CardKB.read();
    if (c == '\n') {
      inputComplete = true;
    } else {
      inputString += c;
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
  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println(prompt);
  inputString = "";
  inputComplete = false;
  while (!inputComplete) {
    loop();
  }
  return inputString;
}

void mainMenu() {
  while (true) {
    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.println("== Shooting Tools ==");
    M5.Display.println("(1) Manage Profiles");
    M5.Display.println("(2) MOA/MIL Calculator");
    M5.Display.println("(3) Exit");

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
      M5.Display.clear();
      M5.Display.println("Goodbye!");
      delay(2000);
      ESP.restart();
    }
  }
}

void manageProfiles() {
  while (true) {
    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.println("== Manage Profiles ==");
    M5.Display.println("(1) New Profile");
    M5.Display.println("(2) View Profiles");
    M5.Display.println("(3) Back");

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

  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println("Profile Saved.");
  delay(1500);
}

void viewProfiles() {
  File root = SD.open("/");
  int index = 1;
  String profiles[20];
  
  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println("== Profiles ==");

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      profiles[index] = file.name();
      M5.Display.print("(");
      M5.Display.print(index);
      M5.Display.print(") ");
      M5.Display.println(file.name());
      index++;
    }
    file = root.openNextFile();
  }

  if (index == 1) {
    M5.Display.println("No profiles found.");
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
    M5.Display.clear();
    M5.Display.println("Profile not found.");
    delay(2000);
    return;
  }

  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println("== " + profileName + " ==");

  while (file.available()) {
    M5.Display.println(file.readStringUntil('\n'));
  }
  file.close();

  waitForKey();
}

void moaMilCalculator() {
  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println("MOA/MIL Calculator");

  String targetDistance = getInput("Distance to target (yds):");
  String impactOffset = getInput("Impact offset (inches):");
  String clicksPerMOA = getInput("Scope clicks per MOA:");

  float distanceYards = targetDistance.toFloat();
  float offsetInches = impactOffset.toFloat();
  float clicksPerMOAValue = clicksPerMOA.toFloat();

  float moa = (offsetInches / distanceYards) * 100;
  float clicks = moa * clicksPerMOAValue;
  float mil = (offsetInches / (distanceYards * 36)) * 1000;

  M5.Display.clear();
  M5.Display.setCursor(0, 0);
  M5.Display.println("== Adjustment ==");
  M5.Display.print("MOA: ");
  M5.Display.println(moa, 2);
  M5.Display.print("Clicks: ");
  M5.Display.println(clicks, 1);
  M5.Display.print("MIL: ");
  M5.Display.println(mil, 2);

  waitForKey();
}