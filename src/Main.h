#pragma once

#include "ll/api/mod/NativeMod.h"

namespace LacNoticePlus {

class LacNoticePlus {
public:
    static LacNoticePlus& getInstance();

    LacNoticePlus() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();
    bool enable();
    bool disable();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace LacNoticePlus