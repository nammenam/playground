
#include "system/time/sys_time.h"

#ifdef __cplusplus
extern "C" {
#endif


uint32_t getCurrentTimeMS();
uint32_t getElapsedTimeMS(uint32_t startTime);
uint64_t getCurrentTimeUS();
void delayms_goblinmode(uint32_t ms);
void delayus(uint32_t us);


#ifdef __cplusplus
}
#endif
