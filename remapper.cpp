#include "remapper.h"

#include "keymap.h"
#include "keyboard.h"

#define KEYMAP_SIZE 6
const keymap::KeyMap keymaps[KEYMAP_SIZE] =
{
  {{MOD_LEFT_CTRL, 0}, {0, 0x39}},  // left-ctrl to capsLock
  {{0, 0x39}, {MOD_LEFT_CTRL, 0}},  // capsLock to left-ctrl
  {{0, 0x04}, {0, 0x05}},  // 'a' to 'b'
  {{0, 0x05}, {0, 0x04}},  // 'b' to 'a'
  {{MOD_LEFT_SHIFT, 0}, {MOD_LEFT_CTRL, 0}},  // left-shift to left-ctrl
  {{MOD_RIGHT_SHIFT, 0x06}, {0, 0x04}},  // R-Shift + 'c' to 'a'
};
bool keyPressedFlags[KEYMAP_SIZE];  // TODO: Confirm that this occupies KEYMAP_SIZE bits, but not KEYMAP_SIZE * 8 bits for memory efficiency.

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after)
{
  uint8_t mappedMod, mappedKey;
  bool isKeyMapped, mappedKeyPressed;

  int mappingOccurred = keymap::onModChanged(keymaps, KEYMAP_SIZE, keyPressedFlags, before, after, &mappedMod, &isKeyMapped, &mappedKey, &mappedKeyPressed);
  if (mappingOccurred == 0) { return; }

  if (isKeyMapped) {
    if (mappedKeyPressed) {
      keyboard::reportPress(mappedMod, mappedKey);
    } else {
      keyboard::reportRelease(mappedMod, mappedKey);
    }
  }

  keyboard::reportModifier(mappedMod);
}

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  keymap::onKeyPressed(keymaps, KEYMAP_SIZE, keyPressedFlags, &mod, &key);

  if (key == 0) {
    if (mod > 0) {
      keyboard::reportModifier(mod);
    }
  } else {
    keyboard::reportPress(mod, key);
  }
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  keymap::onKeyReleased(keymaps, KEYMAP_SIZE, keyPressedFlags, &mod, &key);

  if (key == 0) {
    if (mod > 0) {
      keyboard::reportModifier(mod);
    }
  } else {
    keyboard::reportRelease(mod, key);
  }
}

void initRemapper(void) {
  keyboard::initKeyboard();

  memset(&keyPressedFlags, 0, sizeof(keyPressedFlags));
}