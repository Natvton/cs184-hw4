#include "keyboard.h"
#include <algorithm>

void Keyboard::keyPress(int key)
{
   heldKeys.push_back(key); 
}

void Keyboard::keyRelease(int key)
{
    heldKeys.remove(key);
}

bool Keyboard::isHeld(int key)
{
    if (std::find(heldKeys.begin(), heldKeys.end(), key) != heldKeys.end()) return true;
    else return false;
}
