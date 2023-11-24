#pragma once
#include <3ds.h>

/**
 * @brief Restart a timer
 * 
 * @return svcGetSystemTick()
 */
u64     Timer_Restart(void);

/**
 * @brief Determine, whether an amount of time has passed
 * 
 * @param nbmsecToWait Number of milliseconds
 * @param timer Base timer
 * @return `true` if the time has passed, `false` otherwise
 */
bool    Timer_HasTimePassed(float nbmsecToWait, u64 timer);

/**
 * @brief Get a time of a timer in milliseconds
 * 
 * @param timer Base timer
 * @return Time in milliseconds
 */
u64		getTimeInMsec(u64 timer);

/**
 * @brief Pause the current thread for an amount of time
 * 
 * @param seconds Number of seconds to wait (decimals allowed)
 */
void    Sleep(float seconds);