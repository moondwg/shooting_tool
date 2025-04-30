#include <M5Cardputer.h>
#include <SD.h>

// Use local strings when possible
String inputString = "";
bool inputComplete = false;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5Cardputer.begin();

  if (!SD.begin()) {
    M5Cardputer.Display.println(F("SD init failed!"));
    while (1);
  }

  mainMenu();
}

void loop() {
  M5Cardputer.update();
  char c = M5Cardputer.Keyboard.read();
  if (c == '
' || c == '
') {
    inputComplete = true;
  } else if (c != 0) {
    inputString += c;
  }
}

void waitForKey() {
  inputString = "";
  inputComplete = false;
  while (!inputComplete) loop();
}

String getInput(const __FlashStringHelper *prompt) {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(prompt);
  inputString = "";
  inputComplete = false;
  while (!inputComplete) loop();
  return inputString;
}

void mainMenu() {
  while (true) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(0, 0);
    M5Cardputer.Display.println(F("== Shooting Tools =="));
    M5Cardputer.Display.println(F("(1) Manage Profiles"));
    M5Cardputer.Display.println(F("(2) MOA/MIL Calc"));
    M5Cardputer.Display.println(F("(3) Exit"));

    waitForKey();

    if (inputString == "1") manageProfiles();
    else if (inputString == "2") moaMilCalculator();
    else if (inputString == "3") {
      M5Cardputer.Display.clear();
      M5Cardputer.Display.println(F("Goodbye!"));
      delay(1000);
      ESP.restart();
    }
  }
}

void manageProfiles() {
  while (true) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setCursor(0, 0);
    M5Cardputer.Display.println(F("== Profiles =="));
    M5Cardputer.Display.println(F("(1) New"));
    M5Cardputer.Display.println(F("(2) View"));
    M5Cardputer.Display.println(F("(3) Back"));

    waitForKey();

    if (inputString == "1") newProfile();
    else if (inputString == "2") viewProfiles();
    else if (inputString == "3") return;
  }
}

void newProfile() {
  String name = getInput(F("Profile Name:"));
  String path = "/" + name;
  if (!SD.exists(path)) SD.mkdir(path);

  String distance = getInput(F("Distance (yds):"));
  String vert = getInput(F("Vertical Adj (MOA):"));
  String horiz = getInput(F("Horizontal Adj (MOA):"));

  File file = SD.open(path + "/settings.txt", FILE_WRITE);
  if (file) {
    file.println("Distance: " + distance);
    file.println("Vertical: " + vert);
    file.println("Horizontal: " + horiz);
    file.close();
  }

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(F("Saved."));
  delay(1000);
}

void viewProfiles() {
  File root = SD.open("/");
  int index = 1;
  String profiles[20];

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(F("== Profiles =="));

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      profiles[index] = file.name();
      M5Cardputer.Display.print("("); M5Cardputer.Display.print(index);
      M5Cardputer.Display.print(") "); M5Cardputer.Display.println(file.name());
      index++;
    }
    file = root.openNextFile();
  }

  if (index == 1) {
    M5Cardputer.Display.println(F("None found."));
    delay(1500);
    return;
  }

  waitForKey();
  int sel = inputString.toInt();
  if (sel > 0 && sel < index) showProfile(profiles[sel]);
}

void showProfile(String name) {
  File file = SD.open("/" + name + "/settings.txt");
  if (!file) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.println(F("Not found."));
    delay(1000);
    return;
  }
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(name);
  while (file.available()) M5Cardputer.Display.println(file.readStringUntil('\n'));
  file.close();
  waitForKey();
}

void moaMilCalculator() {
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(F("MOA/MIL Calc"));

  float d = getInput(F("Distance yds:")).toFloat();
  float o = getInput(F("Offset in:")).toFloat();
  float c = getInput(F("Clicks/1MOA:")).toFloat();

  float moa = (o / d) * 100;
  float clicks = moa * c;
  float mil = (o / (d * 36)) * 1000;

  M5Cardputer.Display.clear();
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println(F("== Result =="));
  M5Cardputer.Display.print(F("MOA: ")); M5Cardputer.Display.println(moa, 2);
  M5Cardputer.Display.print(F("Clicks: ")); M5Cardputer.Display.println(clicks, 1);
  M5Cardputer.Display.print(F("MIL: ")); M5Cardputer.Display.println(mil, 2);

  waitForKey();
}
