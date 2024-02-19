#include "Events.h"
#include "System.h"
#include "Data.h"

auto Events::GetSingleton() -> Events*
{
    static Events singleton;
    return std::addressof(singleton);
}

void Events::Register()
{
    if (const auto UI = RE::UI::GetSingleton()) {
        UI->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());
        logs::info("Events :: Registered for MenuOpenCloseEvent.");
    }

    if (const auto events = RE::ScriptEventSourceHolder::GetSingleton()) {
        events->GetEventSource<RE::TESActorLocationChangeEvent>()->AddEventSink(GetSingleton());
        logs::info("Events :: Registered for TESActorLocationChangeEvent.");
    }
}

auto Events::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) -> RE::BSEventNotifyControl
{
    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (a_event->menuName == RE::RaceSexMenu::MENU_NAME && !a_event->opening) {
        const auto system = System::GetSingleton();

        if (system->StartOnNewGame()) {
            system->StartEveryQuest();
        }
    }

    if (a_event->menuName == RE::DialogueMenu::MENU_NAME) {
        const auto system = System::GetSingleton();

        const auto player = RE::PlayerCharacter::GetSingleton();

        if (!player) {
            return RE::BSEventNotifyControl::kContinue;
        }

        if (a_event->opening) {
            const auto location = player->GetCurrentLocation();

            if (location) {
                const auto hold = system->LocationToHold(location);

                if (hold) {
                    system->UpdateCatalogueTopics(hold);
                }
            }
        } else {
            system->ResetData();
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}

auto Events::ProcessEvent(const RE::TESActorLocationChangeEvent* a_event, RE::BSTEventSource<RE::TESActorLocationChangeEvent>*) -> RE::BSEventNotifyControl
{
    if (!a_event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    const auto location = a_event->newLoc;
    const auto actor = a_event->actor;
    const auto player = RE::PlayerCharacter::GetSingleton();

    if (!location || !actor || !actor.get() || !player || actor.get() != player) {
        return RE::BSEventNotifyControl::kContinue;
    }

    const auto hold = System::GetSingleton()->LocationToHold(location);
    const auto data = RE::TESDataHandler::GetSingleton();

    if (hold && data) {
        const auto catalogue = data->LookupForm<RE::TESQuest>(Data::Forms::BQR_QUST_Catalogue, Data::Forms::FileName);

        if (catalogue) {
            System::GetSingleton()->UpdateLocationAlias(catalogue, "Hold"sv, hold, true);
            System::GetSingleton()->UpdatePendingRewards(hold);
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}