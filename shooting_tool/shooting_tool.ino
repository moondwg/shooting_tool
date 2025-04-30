#include "M5Cardputer.h"
#include "M5GFX.h"

M5Canvas canvas(&M5Cardputer.Display);
String inputBuffer = "> ";

enum InputStep {
  STEP_ELEV_SIGN,
  STEP_ELEV_VAL,
  STEP_WIND_SIGN,
  STEP_WIND_VAL,
  STEP_UNIT,
  STEP_DISTANCE,
  STEP_DONE
};

InputStep step = STEP_ELEV_SIGN;
float elevation = 0.0;
int elevationSign = 1;
float windage = 0.0;
int windageSign = 1;
float distance = 100.0;
bool useMOA = true;

void printInstruction(const String &text) {
  canvas.println();
  canvas.println(text);
  canvas.pushSprite(4, 4);
}

void processInput(String userInput) {
  userInput.trim();

  switch (step) {
    case STEP_ELEV_SIGN:
      if (userInput == "+") {
        elevationSign = 1;
        step = STEP_ELEV_VAL;
        printInstruction("Enter elevation offset (inches):");
      } else if (userInput == "-") {
        elevationSign = -1;
        step = STEP_ELEV_VAL;
        printInstruction("Enter elevation offset (inches):");
      } else {
        printInstruction("Enter '+' for UP or '-' for DOWN.");
      }
      break;

    case STEP_ELEV_VAL:
      elevation = userInput.toFloat();
      step = STEP_WIND_SIGN;
      printInstruction("Enter windage direction '+' = Right, '-' = Left:");
      break;

    case STEP_WIND_SIGN:
      if (userInput == "+") {
        windageSign = 1;
        step = STEP_WIND_VAL;
        printInstruction("Enter windage offset (inches):");
      } else if (userInput == "-") {
        windageSign = -1;
        step = STEP_WIND_VAL;
        printInstruction("Enter windage offset (inches):");
      } else {
        printInstruction("Enter '+' for RIGHT or '-' for LEFT.");
      }
      break;

    case STEP_WIND_VAL:
      windage = userInput.toFloat();
      step = STEP_UNIT;
      printInstruction("Use 'M' for MIL or 'A' for MOA:");
      break;

    case STEP_UNIT:
      if (userInput.equalsIgnoreCase("m")) {
        useMOA = false;
        step = STEP_DISTANCE;
        printInstruction("Enter distance to target (yards):");
      } else if (userInput.equalsIgnoreCase("a")) {
        useMOA = true;
        step = STEP_DISTANCE;
        printInstruction("Enter distance to target (yards):");
      } else {
        printInstruction("Type 'M' for MIL or 'A' for MOA.");
      }
      break;
    
case STEP_DISTANCE: {
      distance = userInput.toFloat();
      step = STEP_DONE;

      float factor = useMOA ? 1.047 : 3.6;
      float elevationAdj = (elevationSign * elevation) / (distance * factor);
      float windageAdj = (windageSign * windage) / (distance * factor);

      canvas.println();
      canvas.println("== ADJUSTMENTS ==");
      canvas.println("Elevation: " + String(elevationAdj, 2) + (useMOA ? " MOA" : " MIL"));
      canvas.println("Windage:  " + String(windageAdj, 2) + (useMOA ? " MOA" : " MIL"));
      canvas.println();
      canvas.pushSprite(4, 4);

      step = STEP_ELEV_SIGN;
      printInstruction("Enter elevation direction '+' = Up, '-' = Down:");
      break;
    }

    default:
      break;
  }
}

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(0.5);
  M5Cardputer.Display.setTextFont(&fonts::FreeSerifBoldItalic18pt7b);

  canvas.setTextFont(&fonts::FreeSerifBoldItalic18pt7b);
  canvas.setTextSize(0.5);
  canvas.createSprite(M5Cardputer.Display.width() - 8, M5Cardputer.Display.height() - 36);
  canvas.setTextScroll(true);

  printInstruction("MOA/MIL Adjustment Tool");
  printInstruction("Enter elevation direction '+' = Up, '-' = Down:");
  inputBuffer = "> ";
  M5Cardputer.Display.drawString(inputBuffer, 4, M5Cardputer.Display.height() - 24);
}

void loop() {
  M5Cardputer.update();

  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

      for (auto c : status.word) {
        inputBuffer += c;
      }

      if (status.del && inputBuffer.length() > 2) {
        inputBuffer.remove(inputBuffer.length() - 1);
      }

      if (status.enter) {
        String userInput = inputBuffer.substring(2);
        canvas.println(inputBuffer);  // echo input
        canvas.pushSprite(4, 4);

        processInput(userInput);

        inputBuffer = "> ";
      }

      // Redraw prompt
      M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 28, M5Cardputer.Display.width(), 25, BLACK);
      M5Cardputer.Display.drawString(inputBuffer, 4, M5Cardputer.Display.height() - 24);
    }
  }
}
