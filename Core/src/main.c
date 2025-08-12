#include "main.h"

#include "short_circuit_protection.h"
#include "iwdg.h"
#include "output.h"
#include "util.h"

__attribute__((constructor)) void init(void) {
    shortCircuitProtectionInit();
    iwdgInit();
    outputInit();

#ifdef Output_NORMAL_CLOSE
    outputLDSet(true);
#elif defined(Output_NORMAL_OPEN)
    outputLDSet(false);
#endif
}

__NO_RETURN int main(void) {
    while (true) {
        IWDG_ReloadKey();

        if (checkFlag) {
            checkShort();
        }

        if (isShortCircuit()) {
            outputFlash();
        }
    }
}
