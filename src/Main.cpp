#include "Main.h"

#include "LeviAntiCheat/PlayerCheatEvent.h"
#include "LeviAntiCheat/SusClientEvent.h"
#include "LeviAntiCheat/CheckType.h"

#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/thread/ServerThreadExecutor.h"

#include "mc/world/level/Level.h"
#include "mc/world/actor/player/Player.h"

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <vector>
#include <algorithm>

namespace LacNoticePlus {

LacNoticePlus& LacNoticePlus::getInstance() {
    static LacNoticePlus instance;
    return instance;
}

void broadcastLacLog(std::string const& message) {
    ll::thread::ServerThreadExecutor::getDefault().execute([message]() {
        auto level = ll::service::getLevel();
        if (level.has_value()) {
            level->forEachPlayer([&](Player& player) {
                player.sendMessage(message);
                return true;
            });
        }
    });
}

bool LacNoticePlus::enable() {
    auto& eventBus = ll::event::EventBus::getInstance();
    auto& logger = getSelf().getLogger();

    eventBus.emplaceListener<lac::punish::PlayerCheatEvent>(
        [&](lac::punish::PlayerCheatEvent& ev) {
            
            std::string playerName = ev.self().getRealName();
            std::string checkName = std::string(magic_enum::enum_name(ev.mCheatType));

            std::vector<std::string> details;

            for (const auto& [key, value] : ev.mExtraData) {
                std::string valStr = std::visit([](auto&& arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, float>) {
                        return fmt::format("{:.2f}", arg);
                    } else {
                        return fmt::format("{}", arg);
                    }
                }, value);

                details.push_back(fmt::format("§7{}:§c{}", key, valStr));
            }

            std::sort(details.begin(), details.end());

            std::string detailsStr;
            if (!details.empty()) {
                for (size_t i = 0; i < details.size(); ++i) {
                    detailsStr += details[i];
                    if (i < details.size() - 1) {
                        detailsStr += "§8, ";
                    }
                }
            } else {
                detailsStr = "§7No Data";
            }

            std::string lacMsg = fmt::format(
                "§8[§4LAC§8] §f{} §7failed §c{} §8({})",
                playerName,
                checkName,
                detailsStr
            );

            broadcastLacLog(lacMsg);
            // logger.info("Broadcast: {}", lacMsg);
        }
    );

    eventBus.emplaceListener<lac::punish::SusClientEvent>(
        [&](lac::punish::SusClientEvent& ev) {
            std::string msg = fmt::format(
                "§8[§4LAC§8] §cSuspicious client: §f{} §7(IP: §8*hidden*§7)",
                ev.mName
            );
            broadcastLacLog(msg);
        }
    );

    logger.info("LACNoticePlus Enabled.");
    return true;
}

bool LacNoticePlus::load() { return true; }

bool LacNoticePlus::disable() { return true; }

} // namespace LacNoticePlus

LL_REGISTER_MOD(LacNoticePlus::LacNoticePlus, LacNoticePlus::LacNoticePlus::getInstance());