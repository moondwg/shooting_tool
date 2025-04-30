#include "M5Cardputer.h"
#include "M5GFX.h"

M5Canvas canvas(&M5Cardputer.Display);
String inputString = "> ";
bool inputComplete = false;

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
    canvas.println("Press Key and Enter to Input Text");
    canvas.pushSprite(4, 4);
    M5Cardputer.Display.drawString(inputString, 4, M5Cardputer.Display.height() - 24);
}

void loop() {
    M5Cardputer.update();

    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            // Append typed characters
            for (auto c : status.word) {
                inputString += c;
            }

            // Handle backspace/delete
            if (status.del && inputString.length() > 2) { // Ensure we don't delete the prompt
                inputString.remove(inputString.length() - 1);
            }

            // Handle Enter key
            if (status.enter) {
                // Remove prompt before processing
                String userInput = inputString.substring(2);
                canvas.println(userInput);
                canvas.pushSprite(4, 4);
                inputString = "> ";
            }

            // Update display
            M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 28, M5Cardputer.Display.width(), 25, BLACK);
            M5Cardputer.Display.drawString(inputString, 4, M5Cardputer.Display.height() - 24);
        }
    }
}
