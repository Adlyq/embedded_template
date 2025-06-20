#include "main.h"

#include "short_circuit_protection.h"
#include "iwdg.h"
#include "output.h"

__NO_RETURN int main(void) {
    iwdgInit();
    shortCircuitProtectionInit();
    outputInit();
    while (true) {
        IWDG_ReloadKey();
    }
}
