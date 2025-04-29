#include <M5Cardputer.h>
#include <SD.h>
#include <FS.h>

String inputString = "";
bool inputComplete = false;

void setup() {
  M5.begin();
  M5.CardKB.begin();
  Serial.begin(115200);

  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    while (1);
  }

  mainMenu();
}

void loop() {
  if (M5.CardKB.available()) {
    char c = M5.CardKB.read();
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
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(prompt);
  inputString = "";
  inputComplete = false;
  while (!inputComplete) {
    loop();
  }
  return inputString;
}

void mainMenu() {
  while (true) {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("== Shooting Tools ==");
    M5.Lcd.println("(1) Manage Profiles");
    M5.Lcd.println("(2) MOA/MIL Calculator");
    M5.Lcd.println("(3) Exit");

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
      M5.Lcd.clear();
      M5.Lcd.println("Goodbye!");
      delay(2000);
      ESP.restart();
    }
  }
}

void manageProfiles() {
  while (true) {
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("== Manage Profiles ==");
    M5.Lcd.println("(1) New Profile");
    M5.Lcd.println("(2) View Profiles");
    M5.Lcd.println("(3) Back");

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

  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Profile Saved.");
  delay(1500);
}

void viewProfiles() {
  File root = SD.open("/");
  int index = 1;
  String profiles[20];
  
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("== Profiles ==");

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      profiles[index] = file.name();
      M5.Lcd.print("(");
      M5.Lcd.print(index);
      M5.Lcd.print(") ");
      M5.Lcd.println(file.name());
      index++;
    }
    file = root.openNextFile();
  }

  if (index == 1) {
    M5.Lcd.println("No profiles found.");
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
    M5.Lcd.clear();
    M5.Lcd.println("Profile not found.");
    delay(2000);
    return;
  }

  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("== " + profileName + " ==");

  while (file.available()) {
    M5.Lcd.println(file.readStringUntil('\n'));
  }
  file.close();

  waitForKey();
}

void moaMilCalculator() {
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("MOA/MIL Calculator");

  String targetDistance = getInput("Distance to target (yds):");
  String impactOffset = getInput("Impact offset (inches):");
  String clicksPerMOA = getInput("Scope clicks per MOA:");

  float distanceYards = targetDistance.toFloat();
  float offsetInches = impactOffset.toFloat();
  float clicksPerMOAValue = clicksPerMOA.toFloat();

  float moa = (offsetInches / distanceYards) * 100;
  float clicks = moa * clicksPerMOAValue;
  float mil = (offsetInches / (distanceYards * 36)) * 1000;

  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("== Adjustment ==");
  M5.Lcd.print("MOA: ");
  M5.Lcd.println(moa, 2);
  M5.Lcd.print("Clicks: ");
  M5.Lcd.println(clicks, 1);
  M5.Lcd.print("MIL: ");
  M5.Lcd.println(mil, 2);

  waitForKey();
}
