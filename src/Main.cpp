#include "Main.h"

#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/mod/ModManagerRegistry.h"
#include "ll/api/io/Sink.h"
#include "ll/api/io/PatternFormatter.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include "ll/api/utils/StringUtils.h" 
#include "ll/api/service/Bedrock.h"

#include "mc/world/level/Level.h"
#include "mc/world/actor/player/Player.h"

namespace LacLogger {

namespace {

class LacLoggerSink : public ll::io::Sink {
public:
    explicit LacLoggerSink(ll::Polymorphic<ll::io::Formatter> fmt) 
        : ll::io::Sink(std::move(fmt)) {}
    void append(ll::io::LogMessageView const& view) override {
        std::string logContent;
        this->formatter->format(view, logContent);

        std::string mcMessage = ll::utils::string_utils::replaceAnsiToMcCode(logContent);

        auto& executor = ll::thread::ServerThreadExecutor::getDefault();
        executor.execute([mcMessage]() {
            auto level = ll::service::getLevel(); 
            if (level.has_value()) {
                level->forEachPlayer([&](Player& player) {
                    player.sendMessage(mcMessage);
                    return true;
                });
            }
        });
    }
};

}

LacLogger& LacLogger::getInstance() {
    static LacLogger instance;
    return instance;
}

LacLogger::LacLogger() : mSelf(*ll::mod::NativeMod::current()), mLogger(nullptr) {}

bool LacLogger::load() {
    mLogger = &getSelf().getLogger();
    return true;
}

bool LacLogger::enable() {
    getLogger().info("加载 LacLogger 中...");
    getLogger().info("Author: PuceLi");

    auto& registry = ll::mod::ModManagerRegistry::getInstance();

    auto lacMod = registry.getMod("LeviAntiCheat");

    if (lacMod) {
        auto fmt = ll::makePolymorphic<ll::io::PatternFormatter>("{tit} {msg}");
        auto mySink = std::make_shared<LacLoggerSink>(std::move(fmt));
        lacMod->getLogger().addSink(mySink);
        getLogger().info("加载成功");
    } else {
        getLogger().error("加载失败，未找到 LeviAntiCheat");
    }

    return true;
}

bool LacLogger::disable() {
    getLogger().info("禁用 LacLogger 中...");
    return true;
}

} // namespace LacLogger

LL_REGISTER_MOD(LacLogger::LacLogger, LacLogger::LacLogger::getInstance());