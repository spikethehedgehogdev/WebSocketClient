#pragma once

class reconnect_policy
{
public:
    void reset() { attempt_ = 0; }
    int attempt() const { return attempt_; }
    int next_delay_ms();

private:
    int attempt_ = 0;
};