#pragma once

#ifndef ReturnIfFalse
#define ReturnIfFalse(x)                       \
{                                                               \
    if(!x)                                                      \
    {                                               \
        assert(false);                          \
        return false;                          \
    }                                                       \
}
#endif

