#include "M5Cardputer.h"

enum InputState {
    ElevationDirection,
    ElevationValue,
    WindageDirection,
    WindageValue,
    UnitSelection,
    DistanceToTarget,
    ShowResult
};

InputState currentState = ElevationDirection;

int elevationSign = 0;  // +1 or -1
float elevationOffset = 0.0;

int windageSign = 0;  // +1 or -1
float windageOffset = 0.0;

bool useMOA = true;
float distanceToTarget = 100.0;

String inputBuffer = "";

void drawCenteredText(const String &text) {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.drawString(text,
        M5Cardputer.Display.width() / 2,
        M5Cardputer.Display.height() / 2);
}

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextColor(GREEN);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setTextFont(&fonts::FreeSerifBoldItalic18pt7b);
    M5Cardputer.Display.setTextSize(1);

    drawCenteredText("Use + / - for Elevation (Up/Down)");
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isKeyPressed()) {
            char key = M5Cardputer.Keyboard.read();

            switch (currentState) {
                case ElevationDirection:
                    if (key == '+') {
                        elevationSign = 1;
                        currentState = ElevationValue;
                        drawCenteredText("Enter elevation offset (inches):");
                        inputBuffer = "";
                    } else if (key == '-') {
                        elevationSign = -1;
                        currentState = ElevationValue;
                        drawCenteredText("Enter elevation offset (inches):");
                        inputBuffer = "";
                    }
                    break;

                case ElevationValue:
                    if (isdigit(key) || key == '.') {
                        inputBuffer += key;
                        drawCenteredText("Elevation: " + inputBuffer + " in");
                    } else if (key == '\n') {
                        elevationOffset = inputBuffer.toFloat();
                        currentState = WindageDirection;
                        drawCenteredText("Use + / - for Windage (Right/Left)");
                    }
                    break;

                case WindageDirection:
                    if (key == '+') {
                        windageSign = 1;
                        currentState = WindageValue;
                        drawCenteredText("Enter windage offset (inches):");
                        inputBuffer = "";
                    } else if (key == '-') {
                        windageSign = -1;
                        currentState = WindageValue;
                        drawCenteredText("Enter windage offset (inches):");
                        inputBuffer = "";
                    }
                    break;

                case WindageValue:
                    if (isdigit(key) || key == '.') {
                        inputBuffer += key;
                        drawCenteredText("Windage: " + inputBuffer + " in");
                    } else if (key == '\n') {
                        windageOffset = inputBuffer.toFloat();
                        currentState = UnitSelection;
                        drawCenteredText("Press M for MIL or A for MOA");
                    }
                    break;

                case UnitSelection:
                    if (key == 'M' || key == 'm') {
                        useMOA = false;
                        currentState = DistanceToTarget;
                        drawCenteredText("Enter target distance (yards):");
                        inputBuffer = "";
                    } else if (key == 'A' || key == 'a') {
                        useMOA = true;
                        currentState = DistanceToTarget;
                        drawCenteredText("Enter target distance (yards):");
                        inputBuffer = "";
                    }
                    break;

                case DistanceToTarget:
                    if (isdigit(key) || key == '.') {
                        inputBuffer += key;
                        drawCenteredText("Target: " + inputBuffer + " yd");
                    } else if (key == '\n') {
                        distanceToTarget = inputBuffer.toFloat();
                        currentState = ShowResult;
                    }
                    break;

                case ShowResult: {
                    float moaConversion = 1.047;
                    float factor = useMOA ? moaConversion : 3.6;

                    float elevationAdj = (elevationSign * elevationOffset) / (distanceToTarget * factor);
                    float windageAdj = (windageSign * windageOffset) / (distanceToTarget * factor);

                    String result = "Elevation: ";
                    result += String(elevationAdj, 2);
                    result += useMOA ? " MOA\\n" : " MIL\\n";
                    result += "Windage: ";
                    result += String(windageAdj, 2);
                    result += useMOA ? " MOA" : " MIL";

                    drawCenteredText(result);
                    currentState = ElevationDirection;
                }
                break;
            }
        }
    }
}
