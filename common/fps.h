#pragma once
#include <chrono>

namespace ct
{
    class FPS
    {
    public:
        void fire()
        {
            count_++;
            auto now = std::chrono::high_resolution_clock::now();
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_);
            if (dur > std::chrono::seconds{ 1 })
            {
                fps_ = count_ * 1000.f / dur.count();
                count_ = 0;
                last_ = std::chrono::high_resolution_clock::now();
            }
        }
        float get() const { return fps_; }

    private:
        int fps_ = 0;
        int count_ = 0;
        std::chrono::high_resolution_clock::time_point last_;
    };
}