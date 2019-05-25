#include "gtest/gtest.h"

#include "../../../keymap.h"

#include "../utils.h"

using namespace keymap;

class CapsCtrlPlusKeyTest : public ::testing::Test {
protected:
  const int keymapSize = 2;
  KeyMap keymaps[2] = {
    {{0, {0x39, 0}}, {MOD_LEFT_CTRL, 0}},  // capsLock to left-ctrl
    {{0, {0x13, 0x39}}, {0, 0x52}},  // capsLock + 'p' to 'up'
  };
  KeyPressedFlag keyPressedFlags[1];
};

TEST_F(CapsCtrlPlusKeyTest, onKeyPressedCaps) {
  clearKeyPressedFlags(keyPressedFlags, keymapSize, false);

  uint8_t mod = 0;

  uint8_t mappedMod;
  bool isMappedModChanged;
  uint8_t pressedKeys[MAPPED_KEYS_NUM];
  uint8_t releasedKeys[MAPPED_KEYS_NUM];

  uint8_t sortedKeysBefore0[] = {0, 0, 0, 0, 0, 0};
  uint8_t sortedKeysAfter0[] = {0x39, 0, 0, 0, 0, 0};
  onKeysChanged(keymaps, keymapSize, keyPressedFlags, mod, sortedKeysAfter0, &isMappedModChanged, &mappedMod, pressedKeys, releasedKeys);
  EXPECT_EQ(mappedMod, MOD_LEFT_CTRL);
  EXPECT_EQ(isMappedModChanged, true);
  for (int i = 0; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(pressedKeys[i], 0);
  }
  for (int i = 0; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(releasedKeys[i], 0);
  }

  uint8_t sortedKeysBefore1[] = {0x39, 0, 0, 0, 0, 0};
  uint8_t sortedKeysAfter1[] = {0x13, 0x39, 0, 0, 0, 0};
  onKeysChanged(keymaps, keymapSize, keyPressedFlags, mod, sortedKeysAfter1, &isMappedModChanged, &mappedMod, pressedKeys, releasedKeys);
  EXPECT_EQ(mappedMod, 0);
  EXPECT_EQ(isMappedModChanged, true);
  EXPECT_EQ(pressedKeys[0], 0x52);
  for (int i = 1; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(pressedKeys[i], 0);
  }
  for (int i = 0; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(releasedKeys[i], 0);
  }

  uint8_t sortedKeysBefore2[] = {0x13, 0x39, 0, 0, 0, 0};
  uint8_t sortedKeysAfter2[] = {0x39, 0, 0, 0, 0, 0};
  onKeysChanged(keymaps, keymapSize, keyPressedFlags, mod, sortedKeysAfter2, &isMappedModChanged, &mappedMod, pressedKeys, releasedKeys);
  EXPECT_EQ(mappedMod, MOD_LEFT_CTRL);
  EXPECT_EQ(isMappedModChanged, true);
  for (int i = 0; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(pressedKeys[i], 0);
  }
  EXPECT_EQ(releasedKeys[0], 0x52);
  for (int i = 1; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(releasedKeys[i], 0);
  }

  uint8_t sortedKeysBefore3[] = {0x39, 0, 0, 0, 0, 0};
  uint8_t sortedKeysAfter3[] = {0, 0, 0, 0, 0, 0};
  onKeysChanged(keymaps, keymapSize, keyPressedFlags, mod, sortedKeysAfter3, &isMappedModChanged, &mappedMod, pressedKeys, releasedKeys);
  EXPECT_EQ(mappedMod, 0);
  EXPECT_EQ(isMappedModChanged, true);
  for (int i = 0; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(pressedKeys[i], 0);
  }
  for (int i = 0; i < MAPPED_KEYS_NUM; i++) {
    EXPECT_EQ(releasedKeys[i], 0);
  }
}
