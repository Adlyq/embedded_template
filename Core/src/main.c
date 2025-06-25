#include "main.h"

#include "short_circuit_protection.h"
#include "iwdg.h"
#include "output.h"

__NO_RETURN int main(void) {
    iwdgInit();
    shortCircuitProtectionInit();
    outputInit();
    while (true) {
        if (isShortCircuit()) {
            outputFlash();
            goto END;
        }

    END:
        IWDG_ReloadKey();
    }
}
