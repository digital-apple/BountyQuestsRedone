#include "Papyrus.h"
#include "System.h"

constexpr std::string_view PapyrusClass = "BQR"sv;

void CompleteQuests(RE::StaticFunctionTag*, RE::BGSLocation* a_hold)
{
    if (a_hold) {
        System::GetSingleton()->CompleteQuests(a_hold);
    }
}

void RewardPlayer(RE::StaticFunctionTag*, RE::BGSLocation* a_location, std::uint32_t a_level)
{
    if (a_location) {
        System::GetSingleton()->RewardPlayer(a_location, a_level);
    }
}

void StartEveryQuest(RE::StaticFunctionTag*, RE::BGSLocation* a_hold, std::uint32_t a_type)
{
    if (a_hold) {
        System::GetSingleton()->StartEveryQuest(a_hold, static_cast<System::Type>(a_type));
    }
}

void StartRandomQuest(RE::StaticFunctionTag*, RE::BGSLocation* a_hold, std::uint32_t a_type)
{
    if (a_hold) {
        System::GetSingleton()->StartRandomQuest(a_hold, static_cast<System::Type>(a_type));
    }
}

void SetQuestCount(RE::StaticFunctionTag*, std::uint32_t a_value)
{
    System::GetSingleton()->SetQuestCount(static_cast<float>(a_value));
}

void UpdateDialogueMenu(RE::StaticFunctionTag*, RE::BGSLocation* a_hold)
{
    if (a_hold) {
        System::GetSingleton()->ResetData();
        System::GetSingleton()->UpdateCatalogueTopics(a_hold);
    }
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm)
{
    a_vm->RegisterFunction("CompleteQuests"sv, PapyrusClass, CompleteQuests);
    a_vm->RegisterFunction("RewardPlayer"sv, PapyrusClass, RewardPlayer);
    a_vm->RegisterFunction("StartEveryQuest"sv, PapyrusClass, StartEveryQuest);
    a_vm->RegisterFunction("StartRandomQuest"sv, PapyrusClass, StartRandomQuest);
    a_vm->RegisterFunction("SetQuestCount"sv, PapyrusClass, SetQuestCount);
    a_vm->RegisterFunction("UpdateDialogueMenu"sv, PapyrusClass, UpdateDialogueMenu);

    return true;
}