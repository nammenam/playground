

#include "time.h"


//NOTE Sys_Tasks needs to be called to update the SYS_TIME variables. Without SYS_Tasks running it will not update correctly. All code
//returns time in MS. Divide by 1000 to get seconds if needed and so on.

// Function to get the current system time in milliseconds
uint32_t getCurrentTimeMS() 
{
  return SYS_TIME_CounterGet() / (SYS_TIME_FrequencyGet() / 1000);
}



uint64_t getCurrentTimeUS() 
{
  return (uint64_t) SYS_TIME_CounterGet() * 1000000 / SYS_TIME_FrequencyGet();
}


//Returns uint32_t of elapsed time between the initalization of the argument and when u call the function
uint32_t getElapsedTimeMS(uint32_t startTime) 
{
  // startTime = getCurrentTimeMS(); 
  uint32_t endTime = getCurrentTimeMS();
  return endTime - startTime; 
}







void delayms_goblinmode(uint32_t ms)
{
  SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

  if (SYS_TIME_DelayMS(ms, &timer) != SYS_TIME_SUCCESS)
  {
    // Handle error
        // Not needed ATM as we dont use the function. If we ever actually need this function we need to add error handling here.
  }
  else if(SYS_TIME_DelayIsComplete(timer) != true)
  {
    // Wait till the delay has not expired
    while (SYS_TIME_DelayIsComplete(timer) == false);
  }
}




void delayus(uint32_t us)
{
  SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

  if (SYS_TIME_DelayUS(us, &timer) != SYS_TIME_SUCCESS)
  {
    // Handle error
        // Not needed ATM as we dont use the function. If we ever actually need this function we need to add error handling here.
  }
  else if(SYS_TIME_DelayIsComplete(timer) != true)
  {
    // Wait till the delay has not expired
    while (SYS_TIME_DelayIsComplete(timer) == false);
  }
}
