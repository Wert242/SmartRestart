#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class $modify(SmartRestartPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject * player, GameObject * object) {
       
        PlayLayer::destroyPlayer(player, object);

        if (!player)
            return;

        auto mod = Mod::get();

      
        if (!mod->getSettingValue<bool>("enabled"))
            return;

       
        int restartPercent =
            mod->getSettingValue<int>("restart-percent");

        
        if (this->getCurrentPercent() > restartPercent)
            return;

       
        auto action = this->getActionByTag(0x10);

        if (!action)
            return;

       
        this->stopActionByTag(0x10);

       
        double delay =
            mod->getSettingValue<double>("restart-delay");

        
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

           
            auto playLayer = PlayLayer::get();
            if (!playLayer)
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