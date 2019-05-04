#include "gtest/gtest.h"

#include "../keymap.h"

using namespace keymap;

void clearKeyPressedFlags(KeyPressedFlag keyPressedFlags[], int size, bool value) {
  for (int i=0; i < size; i++) {
    keyPressedFlags[i] = value;
  }
}

TEST(onKeyPressedTest, SingleKeyToKey) {
  const int keymapSize = 1;
  KeyMap keymaps[keymapSize] = {
    {{0, 0x04}, {0, 0x05}},
  };
  KeyPressedFlag keyPressedFlags[keymapSize] = {false};

  uint8_t mod = 0x00000000;  // No modifier key pressed;
  uint8_t key = 0x04;  // The registered source key pressed;

  onKeyPressed(keymaps, keymapSize, keyPressedFlags, &mod, &key);

  EXPECT_EQ(mod, 0x00000000);
  EXPECT_EQ(keyPressedFlags[0], true);
  EXPECT_EQ(key, 0x05);  // Key remapped
}

TEST(onKeyPressedTest, SwappingTwoKeys) {
  const int keymapSize = 2;
  KeyMap keymaps[keymapSize] = {
    {{0, 0x04}, {0, 0x05}},
    {{0, 0x05}, {0, 0x04}},
  };
  KeyPressedFlag keyPressedFlags[keymapSize];

  uint8_t mod = 0x00000000;
  uint8_t key;

  clearKeyPressedFlags(keyPressedFlags, keymapSize, false);
  key = 0x04;
  onKeyPressed(keymaps, keymapSize, keyPressedFlags, &mod, &key);
  EXPECT_EQ(mod, 0x00000000);
  EXPECT_EQ(keyPressedFlags[0], true);
  EXPECT_EQ(keyPressedFlags[1], false);
  EXPECT_EQ(key, 0x05);

  clearKeyPressedFlags(keyPressedFlags, keymapSize, false);
  key = 0x05;
  onKeyPressed(keymaps, keymapSize, keyPressedFlags, &mod, &key);
  EXPECT_EQ(mod, 0x00000000);
  EXPECT_EQ(keyPressedFlags[0], false);
  EXPECT_EQ(keyPressedFlags[1], true);
  EXPECT_EQ(key, 0x04);
}

TEST(onKeyReleasedTest, SingleKeyToKey) {
  const int keymapSize = 1;
  KeyMap keymaps[keymapSize] = {
    {{0, 0x04}, {0, 0x05}},
  };
  KeyPressedFlag keyPressedFlags[keymapSize] = {true};

  uint8_t mod = 0x00000000;  // No modifier key pressed;
  uint8_t key = 0x04;  // The registered source key pressed;

  onKeyReleased(keymaps, keymapSize, keyPressedFlags, &mod, &key);

  EXPECT_EQ(mod, 0x00000000);
  EXPECT_EQ(keyPressedFlags[0], false);
  EXPECT_EQ(key, 0x05);  // Key remapped
}

TEST(onKeyReleasedTest, SwappingTwoKeys) {
  const int keymapSize = 2;
  KeyMap keymaps[keymapSize] = {
    {{0, 0x04}, {0, 0x05}},
    {{0, 0x05}, {0, 0x04}},
  };
  KeyPressedFlag keyPressedFlags[keymapSize];

  uint8_t mod = 0x00000000;
  uint8_t key;

  clearKeyPressedFlags(keyPressedFlags, keymapSize, true);
  key = 0x04;
  onKeyReleased(keymaps, keymapSize, keyPressedFlags, &mod, &key);
  EXPECT_EQ(mod, 0x00000000);
  EXPECT_EQ(keyPressedFlags[0], false);
  EXPECT_EQ(keyPressedFlags[1], true);
  EXPECT_EQ(key, 0x05);

  clearKeyPressedFlags(keyPressedFlags, keymapSize, true);
  key = 0x05;
  onKeyReleased(keymaps, keymapSize, keyPressedFlags, &mod, &key);
  EXPECT_EQ(mod, 0x00000000);
  EXPECT_EQ(keyPressedFlags[0], true);
  EXPECT_EQ(keyPressedFlags[1], false);
  EXPECT_EQ(key, 0x04);
}
