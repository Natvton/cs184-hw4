#include <list>
class Keyboard
{
        std::list<int> heldKeys;
    public:
        void keyPress(int);
        void keyRelease(int);
        bool isHeld(int);
};
