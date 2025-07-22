#include "Mountable.h"

void Mountable::unmount() {
    mounted = false;
    return;
}

bool Mountable::isMounted() const {
    return mounted;
}

void Mountable::mountToParent() {
    mounted = true;
    return;
}
