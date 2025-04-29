void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange()) {
    auto keyEvent = M5Cardputer.Keyboard.read();
    if (keyEvent.wasPressed()) {
      char c = keyEvent.getChar();
      if (c == '\n') {
        inputComplete = true;
      } else if (c != 0) {
        inputString += c;
      }
    }
  }
}