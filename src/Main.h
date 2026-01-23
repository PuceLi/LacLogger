#pragma once

#include "ll/api/mod/NativeMod.h"
#include "ll/api/io/Logger.h"

namespace LacLogger {

class LacLogger {
public:
    static LacLogger& getInstance();

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }
    [[nodiscard]] ll::io::Logger& getLogger() const { return *mLogger; }

    bool load();
    bool enable();
    bool disable();

private:
    LacLogger();

    ll::mod::NativeMod& mSelf;
    ll::io::Logger* mLogger;
};

} // namespace LacLogger