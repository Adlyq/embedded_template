#include "main.h"

#include "short_circuit_protection.h"
#include "iwdg.h"
#include "output.h"
#include "util.h"

__NO_RETURN int main(void) {
    shortCircuitProtectionInit();
    iwdgInit();
    outputInit();

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
