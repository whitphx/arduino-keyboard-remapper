#include <string.h>
#include "keymap.h"

namespace keymap {

void onKeysChanged(KeyMap keymaps[], int keymapSize, KeyPressedFlag keyPressedFlags[], uint8_t mod, uint8_t* sortedKeysBefore, uint8_t* sortedKeysAfter, bool* isMappedModChanged, uint8_t* mappedMod, uint8_t* pressedKeys, uint8_t* releasedKeys) {
  uint8_t pressedKeySetIdx = 0;
  uint8_t releasedKeySetIdx = 0;

  memset(pressedKeys, 0, MAPPED_KEYS_NUM * sizeof(uint8_t));
  memset(releasedKeys, 0, MAPPED_KEYS_NUM * sizeof(uint8_t));

  bool isMappedKeyChanged = false;

  *mappedMod = mod;
  *isMappedModChanged = false;
  uint8_t mappedModBits = 0;

  // Already mapped mods are ignored
  for (int i=0; i<keymapSize; i++) {
    if (keyPressedFlags[i]) {
      *mappedMod &= ~keymaps[i].src.mod;
    }
  }

  for (uint8_t iKeymap = 0; iKeymap < keymapSize; iKeymap++) {
    if ((keymaps[iKeymap].src.mod == 0 || keymaps[iKeymap].src.mod == mod)
        && keymaps[iKeymap].src.keys[0] == sortedKeysAfter[0]
        && keymaps[iKeymap].src.keys[1] == sortedKeysAfter[1]
        && 0 == sortedKeysAfter[2]
        && 0 == sortedKeysAfter[3]
        && 0 == sortedKeysAfter[4]
        && 0 == sortedKeysAfter[5]) {
      if (keyPressedFlags[iKeymap] == false) {
        // Newly pressed
        if (keymaps[iKeymap].dst.key != 0) {
          pressedKeys[pressedKeySetIdx++] = keymaps[iKeymap].dst.key;
          isMappedKeyChanged = true;
        }
        if (keymaps[iKeymap].dst.mod != 0) {
          // Set keymaps[i].dst.mod's bit in mappedMod to 1;
          *mappedMod |= keymaps[iKeymap].dst.mod;
          // Set keymaps[i].src.mod's bit in mappedMod to 0 if the bit is not mapped from any mod bits;
          *mappedMod &= ~(keymaps[iKeymap].src.mod & ~mappedModBits);
          // Save the mapped bit
          mappedModBits |= keymaps[iKeymap].src.mod;

          *isMappedModChanged = true;
        }
      }
      keyPressedFlags[iKeymap] = true;
    } else {
      if (keyPressedFlags[iKeymap] == true) {
        // Newly released
        if (keymaps[iKeymap].dst.key != 0) {
          releasedKeys[releasedKeySetIdx++] = keymaps[iKeymap].dst.key;
          isMappedKeyChanged = true;
        }
        if (keymaps[iKeymap].dst.mod != 0) {
          // Set keymaps[i].dst.mod's bit in mappedMod to 0 since it is regarded as being released;
          *mappedMod &= ~keymaps[iKeymap].dst.mod;
          // Set keymaps[i].src.mod's bit in mappedMod to 0 if the bit is not mapped from any mod bits;
          *mappedMod &= ~(keymaps[iKeymap].src.mod & ~mappedModBits);
          // Save the mapped bit
          mappedModBits |= keymaps[iKeymap].src.mod;

          *isMappedModChanged = true;
        }
      }
      keyPressedFlags[iKeymap] = false;
    }
  }

  if (!isMappedKeyChanged && !*isMappedModChanged) {
    for (uint8_t i = 0; i < 6; i++) {
      bool down = false;
      bool up = false;

      for (uint8_t j = 0; j < 6; j++) {
        if (sortedKeysAfter[i] == sortedKeysBefore[j] && sortedKeysAfter[i] != 1)
          down = true;
        if (sortedKeysAfter[j] == sortedKeysBefore[i] && sortedKeysBefore[i] != 1)
          up = true;
      }

      if (!down) {
        pressedKeys[pressedKeySetIdx++] = sortedKeysAfter[i];
      }
      if (!up) {
        releasedKeys[releasedKeySetIdx++] = sortedKeysBefore[i];
      }
    }
  }
}

/*
void onKeyPressed(KeyMap keymaps[], int keymapSize, KeyPressedFlag keyPressedFlags[], uint8_t* mod, uint8_t* key) {
  uint8_t mappedMod = *mod;
  uint8_t mappedModBits = 0;
  uint8_t mappedKey = *key;

  // Already mapped mods are ignored
  for (int i=0; i<keymapSize; i++) {
    if (keyPressedFlags[i]) {
      mappedMod &= ~keymaps[i].from.mod;
    }
  }

  // Combination keymaps are first priority at detecting pressing.
  bool combinationFound = false;
  for (int i=0; i<keymapSize; i++) {
    // from mod-key combination
    if (keymaps[i].from.key == *key && keymaps[i].from.mod & *mod) {
      // to key
      mappedKey = keymaps[i].to.key;
      // to mod
      // Set pressed flag to set mod later (see below)
      keyPressedFlags[i] = true;
      // Set keymaps[i].to.mod's bit in mappedMod to 1;
      mappedMod |= keymaps[i].to.mod;
      // Set keymaps[i].from.mod's bit in mappedMod to 0 if the bit is not mapped from any mod bits;
      mappedMod &= ~(keymaps[i].from.mod & ~mappedModBits);
      // Save the mapped bit
      mappedModBits |= keymaps[i].from.mod;

      combinationFound = true;
    }
  }

  // If no combination keymap matched to the given `key` is found, then search single keymaps.
  if (!combinationFound) {
    for (int i=0; i<keymapSize; i++) {
      // from key
      if (keymaps[i].from.key == *key && keymaps[i].from.mod == 0) {
        // to key
        mappedKey = keymaps[i].to.key;  // This assumes one 'from' key is mapped to at most 1 'to' key. If There are multiple mappings, only the last one affects.
        // to mod (continue to below)
        keyPressedFlags[i] = true; // On the other hand, 'from' key can be mapped to multiple 'to' mods.
      }

      // from mod
      if (keymaps[i].from.mod & *mod && keymaps[i].from.key == 0) {
        // keymaps[i].from.mod is pressed
        keyPressedFlags[i] = true;

        // TODO: to key

        // to mod
        // Set keymaps[i].to.mod's bit in mappedMod to 1;
        mappedMod |= keymaps[i].to.mod;
        // Set keymaps[i].from.mod's bit in mappedMod to 0 if the bit is not mapped from any mod bits;
        mappedMod &= ~(keymaps[i].from.mod & ~mappedModBits);
        // Save the mapped bit
        mappedModBits |= keymaps[i].from.mod;
      }
    }
  }

  // from key to mod
  for (int i=0; i<keymapSize; i++) {
    if (keyPressedFlags[i]) {
      mappedMod |= keymaps[i].to.mod;
    }
  }

  *key = mappedKey;
  *mod = mappedMod;
}

void onKeyReleased(KeyMap keymaps[], int keymapSize, KeyPressedFlag keyPressedFlags[], uint8_t* mod, uint8_t* key) {
  uint8_t mappedMod = *mod;
  uint8_t mappedModBits = 0;
  uint8_t mappedKey = *key;

  // Search single keymaps
  for (int i=0; i<keymapSize; i++) {
    // from key
    if (keymaps[i].from.key == *key) {
      // to key
      if (keymaps[i].from.mod == 0) {
        // Only single source key (without source mod) is handled here.
        mappedKey = keymaps[i].to.key;
      }
      // to mod (continue to below)
      keyPressedFlags[i] = false;  // Combination keymap is anyway released if at least the key is released.
    }
    // from mod
    if (keymaps[i].from.mod & *mod && keymaps[i].from.key == 0) {
      // keymaps[i].from.mod is pressed
      keyPressedFlags[i] = true;

      // TODO: to key

      // to mod
      // Set keymaps[i].to.mod's bit in mappedMod to 1;
      mappedMod |= keymaps[i].to.mod;
      // Set keymaps[i].from.mod's bit in mappedMod to 0 if the bit is not mapped from any mod bits;
      mappedMod &= ~(keymaps[i].from.mod & ~mappedModBits);
      // Save the mapped bit
      mappedModBits |= keymaps[i].from.mod;
    }
  }

  // Combination keymaps are handled as OR condition at releasing.
  for (int i=0; i<keymapSize; i++) {
    // from mod-key combination
    if (keymaps[i].from.key == *key && keymaps[i].from.mod & *mod) {
      // to key
      mappedKey = keymaps[i].to.key;
      // to mod
      // Set pressed flag to set mod later (see below)
      keyPressedFlags[i] = false;
      // Set keymaps[i].to.mod's bit in mappedMod to 0 since it is regarded as being released;
      mappedMod &= ~keymaps[i].to.mod;
      // Set keymaps[i].from.mod's bit in mappedMod to 0 if the bit is not mapped from any mod bits;
      mappedMod &= ~(keymaps[i].from.mod & ~mappedModBits);
      // Save the mapped bit
      mappedModBits |= keymaps[i].from.mod;
    }
  }

  // from key to mod
  for (int i=0; i<keymapSize; i++) {
    if (keyPressedFlags[i]) {
      mappedMod |= keymaps[i].to.mod;
    }
  }

  *key = mappedKey;
  *mod = mappedMod;
}
*/
int onModChanged(KeyMap keymaps[], int keymapSize, KeyPressedFlag keyPressedFlags[], uint8_t before, uint8_t after, uint8_t* _mappedMod, bool* _isKeyMapped, uint8_t* _mappedKey, bool* _mappedKeyPressed) {
  uint8_t change = before ^ after;
  if (change == 0) { return 0; }

  uint8_t mappedMod = after;
  uint8_t mappedModBits = 0;
  uint8_t mappedKey = 0;
  bool isKeyMapped = false, mappedKeyPressed;

  for (int i=0; i<keymapSize; i++) {
    if (keymaps[i].src.keys[0] != 0 || keymaps[i].src.keys[1] != 0) { continue; }

    if (keymaps[i].src.mod & change) {
      // keymaps[i].from.mod changes

      if (after & change) {
        // keymaps[i].from.mod DOWN (newly set to 1)
        keyPressedFlags[i] = true;

        // 'To' key is added. This assumes one 'from' mod is mapped to at most 1 'to' keys. If there are multiple 'to' keys, the only last one affects.
        if (keymaps[i].dst.key != 0) {
          isKeyMapped = true;
          mappedKey = keymaps[i].dst.key;
          mappedKeyPressed = true;
        }

        // Set mappedMod's target bit to 1
        mappedMod |= keymaps[i].dst.mod;
      } else {
        // keymaps[i].from.mod UP (newly set to 0)
        keyPressedFlags[i] = false;

        // 'To' key is released.
        if (keymaps[i].dst.key != 0) {
          isKeyMapped = true;
          mappedKey = keymaps[i].dst.key;
          mappedKeyPressed = false;
        }

        // Set mappedMod's target bit to 0
        mappedMod &= ~keymaps[i].dst.mod;
      }

      // Set mappedMod's source bit to 0 if the bit is not mapped from any mod bits;
      mappedMod &= ~(keymaps[i].src.mod & ~mappedModBits);

      // Save mapping bit
      mappedModBits |= keymaps[i].src.mod;
    }
  }

  *_mappedMod = mappedMod;
  *_isKeyMapped = isKeyMapped;
  *_mappedKey = mappedKey;
  *_mappedKeyPressed = mappedKeyPressed;

  return 1;
}

}
