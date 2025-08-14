#include "main.h"

#include "short_circuit_protection.h"
#include "iwdg.h"
#include "output.h"
#include "util.h"

__STATIC_FORCEINLINE void init(void) {
    shortCircuitProtectionInit();
    iwdgInit();
    outputInit();

#ifdef Output_NORMAL_CLOSE
    outputLDSet(true);
#elif defined(Output_NORMAL_OPEN)
    outputLDSet(false);
#endif
}

__STATIC_FORCEINLINE void loop(void) {
    IWDG_ReloadKey();

    if (checkFlag) {
        checkShort();
    }

    if (isShortCircuit()) {
        outputFlash();
    }
}

__NO_RETURN int main(void) {
    init();

    while (true) {
        loop();
    }
}
