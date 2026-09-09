#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class $modify(SmartRestartPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject * player, GameObject * object) {
        // Сначала даём GD нормально обработать смерть
        PlayLayer::destroyPlayer(player, object);

        if (!player)
            return;

        auto mod = Mod::get();

        // Smart Restart выключен
        if (!mod->getSettingValue<bool>("enabled"))
            return;

        // Получаем максимальный процент
        int restartPercent =
            mod->getSettingValue<int>("restart-percent");

        // Смерть произошла слишком поздно
        if (this->getCurrentPercent() > restartPercent)
            return;

        // GD создал стандартный action рестарта
        auto action = this->getActionByTag(0x10);

        if (!action)
            return;

        // Убираем стандартную задержку
        this->stopActionByTag(0x10);

        // Получаем нашу задержку
        double delay =
            mod->getSettingValue<double>("restart-delay");

        // Создаём тот же рестарт, но с нашей задержкой
        auto restartAction = CCSequence::create(
            CCDelayTime::create(static_cast<float>(delay)),
            CCCallFunc::create(
                this,
                callfunc_selector(PlayLayer::delayedResetLevel)
            ),
            nullptr
        );

        restartAction->setTag(0x10);

        this->runAction(restartAction);
    }
};

$on_game(Loaded) {
    listenForKeybindSettingPresses(
        "toggle-key",
        [](Keybind const&, bool down, bool repeat, double) {
            if (!down || repeat)
                return;

            auto mod = Mod::get();

            bool enabled =
                mod->getSettingValue<bool>("enabled");

            mod->setSettingValue<bool>(
                "enabled",
                !enabled
            );

            geode::log::info(
                "SmartRestart: {}",
                !enabled ? "ON" : "OFF"
            );
        }
    );
}