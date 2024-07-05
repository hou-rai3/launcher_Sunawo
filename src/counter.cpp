#include "counter.hpp"

int launcher_counter(bool A_pre, bool B_pre, bool A_now, bool B_now, int counter)
{
    // ここに関数の実装を移動
    if (A_pre == 0 && B_pre == 0)
    {
        if (A_now == 1 && B_now == 0)
        {
            counter++;
        }
        else if (A_now == 0 && B_now == 1)
        {
            counter--;
        }
    }
    else if (A_pre == 1 && B_pre == 0)
    {
        if (A_now == 1 && B_now == 1)
        {
            counter++;
        }
        else if (A_now == 0 && B_now == 0)
        {
            counter--;
        }
    }
    else if (A_pre == 1 && B_pre == 1)
    {
        if (A_now == 0 && B_now == 1)
        {
            counter++;
        }
        else if (A_now == 1 && B_now == 0)
        {
            counter--;
        }
    }
    else if (A_pre == 0 && B_pre == 1)
    {
        if (A_now == 0 && B_now == 0)
        {
            counter++;
        }
        else if (A_now == 1 && B_now == 1)
        {
            counter--;
        }
    }
    return counter;
}