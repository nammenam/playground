#include <definitions.h>


int main(void) 
{
    SYS_Initialize(NULL);

    PWR_LED_OutputEnable();
    ERR_LED_OutputEnable();
    BATT_LED_OutputEnable();
    MCU_LED_OutputEnable();
    PWR_LED_Set();

    IMU_SLAVE_SELECT_OutputEnable();
    PT_SLAVE_SELECT_OutputEnable();
    MAGN_SLAVE_SELECT_OutputEnable();
    PWR_STATUS_InputEnable();

    IMU_SLAVE_SELECT_Set();
    MAGN_SLAVE_SELECT_Set();
    PT_SLAVE_SELECT_Set();

    while (true) 
    {
        SYS_Tasks();
    }

    return (EXIT_FAILURE);
}
