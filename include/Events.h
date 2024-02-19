#pragma once

class Events final : public RE::BSTEventSink<RE::MenuOpenCloseEvent>, public RE::BSTEventSink<RE::TESActorLocationChangeEvent>
{
public:
    static auto GetSingleton() -> Events*;
    static void Register();
private:
    auto ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) -> RE::BSEventNotifyControl override;
    auto ProcessEvent(const RE::TESActorLocationChangeEvent* a_event, RE::BSTEventSource<RE::TESActorLocationChangeEvent>*) -> RE::BSEventNotifyControl override;
};