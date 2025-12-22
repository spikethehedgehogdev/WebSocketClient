#include "reconnect_policy.h"
#include <algorithm>

int reconnect_policy::next_delay_ms()
{
    attempt_++;
    return std::min(10000, 1000 + attempt_ * 1000);
}