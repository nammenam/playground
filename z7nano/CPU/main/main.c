#include <stdio.h>
#include "registers.h"

int main()
{
    *GPIO_DIRM_0 |= (1 << LED_PIN);
    *GPIO_OEN_0  |= (1 << LED_PIN);


    while(1) {
        *GPIO_DATA_0 |= (1 << LED_PIN);  // Turn ON (Set bit 0 to 1)
        sleep(1);
        *GPIO_DATA_0 &= ~(1 << LED_PIN); // Turn OFF (Clear bit 0 to 0)
        sleep(1);
    }
    return 0;
}
