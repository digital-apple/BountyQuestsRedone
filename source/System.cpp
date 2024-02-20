#include "System.h"
#include "Data.h"

auto System::GetSingleton() -> System*
{
    static System singleton;
    return std::addressof(singleton);
}

void System::BindScriptToInfo(const RE::TESTopicInfo* a_info, const RE::TESQuest* a_placeholder)
{
    if (!a_info || !a_placeholder) {
        return;
    }

    const auto VM = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    auto handle = VM->GetObjectHandlePolicy()->GetHandleForObject(a_info->GetFormType(), a_info);

    if (VM && handle) {
        RE::BSTSmartPointer<RE::BSScript::Object> object;

        if (!VM->FindBoundObject(handle, Data::Scripts::BQR_SCPT_StartQuest.data(), object)) {
            VM->CreateObject(Data::Scripts::BQR_SCPT_StartQuest.data(), object);

            const auto policy = VM->GetObjectBindPolicy();

            if (policy) {
                policy->BindObject(object, handle);
            }
        }

        if (object) {
            auto properties = object->GetProperty(Data::Scripts::Target);

            if (properties) {
                RE::BSScript::PackValue(properties, a_placeholder);
            }
        }
    }
}

void System::CompleteQuests(const RE::BGSLocation* a_hold)
{
    if (!a_hold) {
        return;
    }

    for (const auto& placeholder : placeholders) {
        const auto quest = placeholder.first;
        if (quest && !quest->IsStopped() && GetStageDone(quest, 20U)) {
            const auto alias = skyrim_cast<RE::BGSLocAlias*>(GetAliasByName(quest, "Hold"sv));

            if (!alias) {
                return;
            }

            const auto hold = GetLocation(quest, alias->aliasID);

            if (hold && hold == a_hold) {
                SetStage(quest, 30);

                SetQuestCount(-1);
            }
        }
    }
}

auto System::GetAliasByName(const RE::TESQuest* a_quest, const std::string_view& a_name) -> RE::BGSBaseAlias*
{
    if (a_quest) {
        for (const auto& alias : a_quest->aliases) {
            if (alias && alias->aliasName.contains(a_name)) {
                return alias;
            }
        }
    }
    return nullptr;
}

auto System::GetNextAvailableInfo(const RE::TESTopic* a_topic) -> RE::TESTopicInfo*
{
    if (a_topic) {
        for (std::size_t i = 0; i < a_topic->numTopicInfos; i++) {
            const auto info = a_topic->topicInfos[i];

            if (info) {
                return info;
            }
        }
    }
    return nullptr;
}

auto System::GetNextAvailablePlaceholder(bool a_force) -> RE::TESQuest*
{
    for (auto& placeholder : placeholders) {
        if (placeholder.first && (placeholder.first->IsStopping() || placeholder.first->IsStopped())) {
            if (a_force || !placeholder.second) {
                placeholder.second = true;
                return placeholder.first;
            }
        }
    }
    return nullptr;
}

auto System::GetNextAvailableTopic(const RE::BGSDialogueBranch* a_branch) -> RE::TESTopic*
{
    if (a_branch) {
        for (const auto& topic : topics) {
            if (topic && topic->ownerBranch == a_branch && topic->fullName.contains("Placeholder"sv)) {
                return topic;
            }
        }
    }
    return nullptr;
}

auto System::GetQuestCount() -> float
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (data) {
        const auto count = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_QuestCount, Data::Forms::FileName);

        if (count) {
            return count->value;
        }
    }
    return 0;
}

auto System::GetQuestLimit() -> float
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (data) {
        const auto limit = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_QuestLimit, Data::Forms::FileName);

        if (limit) {
            return limit->value;
        }
    }
    return 0;
}

bool System::IsLocationAvailable(const RE::BGSLocation* a_location)
{
    if (a_location) {
        for (const auto& placeholder : placeholders) {
            const auto quest = placeholder.first;
            if (quest && !quest->IsStopped()) {
                const auto alias = skyrim_cast<RE::BGSLocAlias*>(GetAliasByName(quest, "Location"sv));

                if (!alias) {
                    return false;
                }
            
                const auto location = GetLocation(quest, alias->aliasID);

                if (location && location == a_location) {
                    return false;
                }
            }
        }
    }
    return a_location ? true : false;
}

void System::LoadForms()
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (data) {
        const auto file = data->LookupModByName(Data::Forms::FileName);

        if (file) {
            for (const auto& topic : data->GetFormArray<RE::TESTopic>()) {
                if (!topic || !file->IsFormInMod(topic->GetFormID()) || !topic->fullName.contains("Placeholder"sv)) {
                    continue;
                }
                topics.push_back(topic);
            }

            for (const auto& quest : data->GetFormArray<RE::TESQuest>()) {
                if (!quest || !file->IsFormInMod(quest->GetFormID()) || quest->GetFormID() == data->LookupFormID(Data::Forms::BQR_QUST_Catalogue, Data::Forms::FileName)) {
                    continue;
                }

                placeholders.try_emplace(quest);
            }

            for (const auto& global : data->GetFormArray<RE::TESGlobal>()) {
                if (!global || !file->IsFormInMod(global->GetFormID())) {
                    continue;
                }

                switch (global->GetFormID() & 0x00000FFF) {
                case Data::Forms::BQR_GLOB_HoldHasBandit:
                    globals.try_emplace(Type::kBandit, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasCreature:
                    globals.try_emplace(Type::kCreature, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasDragon:
                    globals.try_emplace(Type::kDragon, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasDraugr:
                    globals.try_emplace(Type::kDraugr, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasDwemer:
                    globals.try_emplace(Type::kDwemer, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasFalmer:
                    globals.try_emplace(Type::kFalmer, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasForsworn:
                    globals.try_emplace(Type::kForsworn, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasGiant:
                    globals.try_emplace(Type::kGiant, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasMage:
                    globals.try_emplace(Type::kMage, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasReaver:
                    globals.try_emplace(Type::kReaver, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasRiekling:
                    globals.try_emplace(Type::kRiekling, global);
                    break;
                case Data::Forms::BQR_GLOB_HoldHasVampire:
                    globals.try_emplace(Type::kVampire, global);
                    break;
                default:
                    break;    
                }
            }
        }
    }
}

auto System::LocationToHold(const RE::BGSLocation* a_location) -> const RE::BGSLocation*
{
    if (a_location) {
        for (const auto& quest : quests) {
            auto current = a_location;

            while (current) {
                if (quest.hold == current) {
                    return current;
                }
                current = current->parentLoc;
            }
        }
    }
    return nullptr;
}

void System::ParseQuests()
{
    const std::filesystem::path source{"Data/SKSE/Plugins/Bounty Quests Redone/Quests"};

    for (auto const& entry : std::filesystem::directory_iterator{source}) {
        if (entry.path().extension() == ".json") {
            std::ifstream path(std::filesystem::absolute(entry.path()));

            const auto data = RE::TESDataHandler::GetSingleton();

            if (!data) {
                return;
            }

            try {
                json config = json::parse(path);
                const json& elements = config["Quests"];

                logs::info("System :: Parsing: '{}' quests from file: '{}'", elements.size(), entry.path().filename().string());

                for (const auto& element : elements.array_range()) {
                    const auto location = data->LookupForm<RE::BGSLocation>(element["Location"]["FormID"].as<RE::FormID>(), element["Location"]["ModName"].as<std::string_view>());
                    const auto hold = data->LookupForm<RE::BGSLocation>(element["Hold"]["FormID"].as<RE::FormID>(), element["Hold"]["ModName"].as<std::string_view>());
                    const auto type = StringToType(element["Type"].as<std::string_view>());

                    if (!location) {
                        logs::warn("System :: Failed to parse location: '0x{:X}' from file: '{}'", element["Location"]["FormID"].as<RE::FormID>(), element["Location"]["ModName"].as<std::string_view>());
                        continue;
                    }

                    if (!hold) {
                        logs::warn("System :: Failed to parse hold: '0x{:X}' | '{}'!", element["Hold"]["FormID"].as<RE::FormID>(), element["Hold"]["ModName"].as<std::string_view>());
                        continue;
                    }

                    if (type == Type::kNone) {
                        logs::error("System :: Failed to parse type: '{}'! Make sure the type name starts with a capital letter. Example: \"Bandit\"", element["Type"].as<std::string_view>());
                        continue;
                    }

                    const Quest instance{ location, hold, type };

                    logs::info("System :: Parsed location: '{}' | '0x{:X}' from file: '{}'", location->GetName(), element["Location"]["FormID"].as<RE::FormID>(), element["Location"]["ModName"].as<std::string_view>());

                    quests.push_back(instance);
                }
            } catch(const jsoncons::ser_error& error) {
                stl::report_and_fail(fmt::format("ERROR!\n\nFailed to parse file: '{}'\n\n{}", entry.path().filename().string(), error.what()));
            }
        }
    }
}

void System::ParseRewards()
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (!data) {
        return;
    }

    std::ifstream path("Data/SKSE/Plugins/Bounty Quests Redone/Rewards.json");

    try {
        json config = json::parse(path);
        const json& elements1 = config["Rewards"];

        for (const auto& element : elements1.array_range()) {
            const auto form = data->LookupForm(element["Reward"]["FormID"].as<RE::FormID>(), element["Reward"]["ModName"].as<std::string_view>());
            const auto quantity = element["Quantity"].as<float>();
            const auto base = element["Base"].as<float>();
            const auto maximum = element["Maximum"].as<float>();

            if (!form) {
                logs::warn("System :: Failed to parse form: '0x{:X}' from file: '{}'", element["Reward"]["FormID"].as<RE::FormID>(), element["Reward"]["ModName"].as<std::string_view>());
            }

            const Reward instance{ form, quantity, base, maximum };

            rewards.push_back(instance);
        }

        const json& elements2 = config["Multipliers"];

        for (const auto& element : elements2.array_range()) {
            const auto type = StringToType(element["Type"].as<std::string_view>());
            const auto multiplier = element["Multiplier"].as<float>();

            if (type == Type::kNone) {
                logs::error("System :: Failed to parse type: '{}'! Make sure the type name starts with a capital letter. Example: \"Bandit\"", element["Type"].as<std::string_view>());
                continue;
            }

            multipliers.try_emplace(type, multiplier);
        }
    } catch(const jsoncons::ser_error& error) {
        stl::report_and_fail(fmt::format("ERROR!\n\nFailed to parse file: 'Rewards.json'\n\n{}", error.what()));
    }
}

void System::ParseSettings(SKSE::MessagingInterface::Message* a_message)
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (!data) {
        return;
    }

    std::ifstream path("Data/SKSE/Plugins/Bounty Quests Redone/Settings.json");

    try {
        json config = json::parse(path);
        const json& settings = config["Settings"];

        for (const auto& setting : settings.array_range()) {
            const auto StartEveryQuestOnNewGame = setting["StartEveryQuestOnNewGame"].as<std::string_view>();
            const auto concurrentQuestLimit = setting["ConcurrentQuestLimit"].as<float>();
            const auto disableVanillaBountyQuests = setting["DisableVanillaBountyQuests"].as<std::string_view>();

            const auto limit = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_QuestLimit, Data::Forms::FileName);

            if (limit) {
                limit->value = concurrentQuestLimit;
            }

            if (StartEveryQuestOnNewGame.contains("True"sv) && a_message->type == SKSE::MessagingInterface::kNewGame) {
                startEveryQuest = true;
            }

            if (disableVanillaBountyQuests.contains("True")) {
                const auto node = data->LookupForm<RE::BGSStoryManagerBranchNode>(Data::Forms::BQBranchNodeSHARES, "Skyrim.esm"sv);

                if (node) {
                    const auto global = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_DisableVanillaBounties, Data::Forms::FileName);

                    const auto item = new RE::TESConditionItem();

                    RE::CONDITION_ITEM_DATA itemData;

                    itemData.object = RE::CONDITIONITEMOBJECT::kSelf;
                    itemData.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetGlobalValue;
                    itemData.functionData.params[0] = std::bit_cast<void*>(global);
                    itemData.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
                    itemData.comparisonValue.f = 0;
                    itemData.flags.isOR = false;

                    item->data = itemData;

                    if (node->conditions && node->conditions.head) {
                        node->conditions.head->next = item;
                    }
                }
            }

            const json& patches = setting["Patches"];

            for (const auto& patch : patches.array_range()) {
                const auto actor = data->LookupForm<RE::Actor>(patch["Actor"]["FormID"].as<RE::FormID>(), patch["Actor"]["ModName"].as<std::string_view>());
                const auto faction = data->LookupForm<RE::TESFaction>(patch["Faction"]["FormID"].as<RE::FormID>(), patch["Faction"]["ModName"].as<std::string_view>());

                if (actor && faction) {
                    actor->AddToFaction(faction, 0);
                }
            }
        }
    } catch(const jsoncons::ser_error& error) {
        stl::report_and_fail(fmt::format("ERROR!\n\nFailed to parse file: 'Settings.json'\n\n{}", error.what()));
    }
}

void System::ResetData()
{
    for (const auto& topic : topics) {
        if (topic) {
            topic->fullName = "Placeholder"sv;

            const auto info = GetNextAvailableInfo(topic);

            if (info) {
                UpdateInfoConditions(info, nullptr, nullptr, false);
            }
        }
    }

    for (auto& placeholder : placeholders) {
        placeholder.second = false;
    }

    for (auto& global : globals) {
        if (global.second) {
            UpdateGlobalValue(global.first, false);
        }
    }
}

void System::RewardPlayer(const RE::BGSLocation* a_location, std::uint32_t a_level)
{
    if (!a_location) {
        return;
    }

    const auto player = RE::PlayerCharacter::GetSingleton();

    if (!player) {
        return;
    }

    for (const auto& quest : quests) {
        if (quest.location == a_location) {
            const auto multiplier = multipliers[quest.type];

            for (const auto& reward : rewards) {
                auto amount = std::ceilf(reward.base + ((reward.quantity * a_level) * multiplier));

                if (reward.maximum > 0) {
                    amount > reward.maximum ? amount = reward.maximum : amount;
                }
                
                const auto sAddItemtoInventory = RE::GameSettingCollection::GetSingleton()->GetSetting("sAddItemtoInventory");

                player->AddObjectToContainer(reward.form->As<RE::TESBoundObject>(), nullptr, static_cast<std::uint32_t>(amount), nullptr);

                if (sAddItemtoInventory) {
                    std::string result = fmt::format("{} {}, {}", sAddItemtoInventory->GetString(), reward.form->GetName(), amount);
                    RE::PlaySound("ITMGoldUpSD");
                    RE::DebugNotification(result.c_str(), nullptr, true);
                }

                const auto data = RE::TESDataHandler::GetSingleton();

                if (data) {
                    const auto rewardSwitch = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_RewardSwitch, Data::Forms::FileName);

                    if (rewardSwitch) {
                        rewardSwitch->value = 0;
                    }
                } 
            }
        }
    }
}

void System::SetQuestCount(float a_value)
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (!data) {
        return;
    }

    const auto count = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_QuestCount, Data::Forms::FileName);

    if (count) {
        count->value += a_value;
    }
}

void System::StartEveryQuest()
{
    for (const auto& quest : quests) {
        if (GetQuestLimit() > 0 && GetQuestCount() >= GetQuestLimit()) {
            return;
        }

        if (IsLocationAvailable(quest.location)) {
            const auto placeholder = GetNextAvailablePlaceholder(true);

            if (placeholder) {
                UpdateLocationAlias(placeholder, "Location"sv, quest.location, false);
                UpdateLocationAlias(placeholder, "Hold"sv, quest.hold, false);

                placeholder->Start();
            }
        }
    }
}

void System::StartEveryQuest(const RE::BGSLocation* a_hold, Type a_type)
{
    if (a_hold) {
        for (const auto& quest : quests) {
            if (GetQuestLimit() > 0 && GetQuestCount() >= GetQuestLimit()) {
                return;
            }

            if (quest.hold == a_hold) {
                if (quest.type == a_type || a_type == Type::kNone) {
                    if (IsLocationAvailable(quest.location)) {
                        const auto placeholder = GetNextAvailablePlaceholder(true);

                        if (placeholder) {
                            UpdateLocationAlias(placeholder, "Location"sv, quest.location, false);
                            UpdateLocationAlias(placeholder, "Hold"sv, quest.hold, false);

                            placeholder->Start();
                        }
                    }
                }
            }
        }
    }
}

bool System::StartOnNewGame()
{
    return startEveryQuest ? (startEveryQuest = false, true) : false;
}

void System::StartRandomQuest(const RE::BGSLocation* a_hold, Type a_type)
{
    if (a_hold) {
        std::unordered_map<RE::BGSLocation*, RE::BGSLocation*> temporary;

        for (const auto& quest : quests) {
            if (GetQuestLimit() > 0 && GetQuestCount() >= GetQuestLimit()) {
                return;
            }

            if (quest.hold == a_hold) {
                if (quest.type == a_type || a_type == Type::kNone) {
                    if (IsLocationAvailable(quest.location)) {
                        temporary.try_emplace(quest.location, quest.hold);
                    }
                }
            }
        }

        const auto placeholder = GetNextAvailablePlaceholder(true);

        if (placeholder) {
            auto random = temporary.begin();
            std::advance(random, std::rand() % temporary.size());

            UpdateLocationAlias(placeholder, "Location"sv, random->first, false);
            UpdateLocationAlias(placeholder, "Hold"sv, random->second, false);

            placeholder->Start();
        }
    }
}

auto System::StringToType(const std::string_view& a_string) -> Type
{
    if (a_string.contains("Bandit"sv)) {
        return Type::kBandit;
    } else if (a_string.contains("Creature"sv))  {
        return Type::kCreature;
    } else if (a_string.contains("Dragon"sv))  {
        return Type::kDragon;
    } else if (a_string.contains("Draugr"sv))  {
        return Type::kDraugr;
    } else if (a_string.contains("Dwemer"sv))  {
        return Type::kDwemer;
    } else if (a_string.contains("Falmer"sv))  {
        return Type::kFalmer;
    } else if (a_string.contains("Forsworn"sv))  {
        return Type::kForsworn;
    } else if (a_string.contains("Giant"sv))  {
        return Type::kGiant;
    } else if (a_string.contains("Mage"sv))  {
        return Type::kMage;
    } else if (a_string.contains("Reaver"sv))  {
        return Type::kReaver;
    } else if (a_string.contains("Riekling"sv))  {
        return Type::kRiekling;
    } else if (a_string.contains("Vampire"sv))  {
        return Type::kVampire;
    }

    return Type::kNone;
}

auto System::TypeToBranch(const Type a_type) -> RE::BGSDialogueBranch*
{
    const auto data = RE::TESDataHandler::GetSingleton();

    if (data) {
        switch (a_type) {
        case Type::kNone:
        case Type::kBandit:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListBandit, Data::Forms::FileName);
            break;
        case Type::kCreature:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListCreature, Data::Forms::FileName);
            break;
        case Type::kDragon:;
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListDragon, Data::Forms::FileName);
            break;
        case Type::kDraugr:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListDraugr, Data::Forms::FileName);
            break;
        case Type::kDwemer:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListDwemer, Data::Forms::FileName);
            break;
        case Type::kFalmer:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListFalmer, Data::Forms::FileName);
            break;
        case Type::kForsworn:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListForsworn, Data::Forms::FileName);
            break;
        case Type::kGiant:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListGiant, Data::Forms::FileName);
            break;
        case Type::kMage:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListMage, Data::Forms::FileName);
            break;
        case Type::kReaver:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListReaver, Data::Forms::FileName);
            break;
        case Type::kRiekling:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListRiekling, Data::Forms::FileName);
            break;
        case Type::kVampire:
            return data->LookupForm<RE::BGSDialogueBranch>(Data::Forms::BQR_DLBR_ListVampire, Data::Forms::FileName);
            break;
        default:
            break;
        }
    }

    return nullptr;
}

void System::UpdateCatalogueTopics(const RE::BGSLocation* a_hold)
{
    if (!a_hold) {
        return;
    }

    if (GetQuestLimit() > 0 && GetQuestCount() >= GetQuestLimit()) {
        return;
    }

    for (auto& quest : quests) {
        if (!quest.location || !quest.hold || !IsLocationAvailable(quest.location)) {
            continue;
        }

        if (quest.hold == a_hold) {
            UpdateGlobalValue(quest.type, true);
            
            const auto branch = TypeToBranch(quest.type);

            if (branch) {
                const auto topic = GetNextAvailableTopic(branch);
                const auto placeholder = GetNextAvailablePlaceholder();

                if (topic && placeholder) {
                    const auto string = fmt::format<std::string_view>("{}."sv, quest.location->fullName);
                    topic->fullName = string;

                    const auto info = GetNextAvailableInfo(topic);

                    if (info) {
                        UpdateInfoConditions(info, quest.location, quest.hold, true);
                        BindScriptToInfo(info, placeholder);
                        UpdateLocationAlias(placeholder, "Location"sv, quest.location, false);
                        UpdateLocationAlias(placeholder, "Hold"sv, quest.hold, false);
                    }
                }
            }
        }
    }
}

void System::UpdateGlobalValue(const Type a_type, bool a_value)
{
    if (globals[a_type]) {
        globals[a_type]->value = a_value;
    }
}

void System::UpdateInfoConditions(const RE::TESTopicInfo* a_info, const RE::BGSLocation* a_location, const RE::BGSLocation* a_hold, bool a_switch)
{
    if (!a_info) {
        return;
    }

    auto conditions = a_info->objConditions.head;

    while (conditions) {
        const auto type = conditions->data.functionData.function.get();

        switch(type) {
        case RE::FUNCTION_DATA::FunctionID::kGetGlobalValue:
            conditions->data.comparisonValue.f = static_cast<float>(a_switch);
            break;
        case RE::FUNCTION_DATA::FunctionID::kGetIsEditorLocation:
            conditions->data.functionData.params[0] = a_hold ? std::bit_cast<void*>(a_hold) : nullptr;
            break;
        case RE::FUNCTION_DATA::FunctionID::kGetRefTypeAliveCount:
            conditions->data.functionData.params[0] = a_location ? std::bit_cast<void*>(a_location) : nullptr;
            break;
        default:
            break;
        }

        conditions = conditions->next;
    }
}

void System::UpdateLocationAlias(RE::TESQuest* a_quest, const std::string_view& a_name, const RE::BGSLocation* a_location, bool a_restart)
{
    if (!a_quest || !a_location) {
        return;
    }

    const auto alias = GetAliasByName(a_quest, a_name);

    if (alias) {
        if (a_restart) {
            a_quest->Stop();
        }

        const auto location = skyrim_cast<RE::BGSLocAlias*>(alias);
        location->unk28 = reinterpret_cast<std::uintptr_t>(a_location);

        if (a_restart) {
            bool result;
            a_quest->EnsureQuestStarted(result, false);
        }
    }
}

void System::UpdatePendingRewards(const RE::BGSLocation* a_hold)
{
    if (!a_hold) {
        return;
    }

    const auto data = RE::TESDataHandler::GetSingleton();

    if (!data) {
        return;
    }

    const auto rewardSwitch = data->LookupForm<RE::TESGlobal>(Data::Forms::BQR_GLOB_RewardSwitch, Data::Forms::FileName);

    if (!rewardSwitch) {
        return;
    }

    for (const auto& placeholder : placeholders) {
        const auto quest = placeholder.first;
        if (quest && !quest->IsStopped()) {
            const auto alias = skyrim_cast<RE::BGSLocAlias*>(GetAliasByName(quest, "Hold"sv));

            if (!alias) {
                return;
            }

            const auto hold = GetLocation(quest, alias->aliasID);

            if (hold && hold == a_hold) {
                if (GetStageDone(quest, 20)) {
                    rewardSwitch->value = 1;
                    return;
                }
            }
        }
    }
}

auto System::GetLocation(RE::TESQuest* a_quest, std::uint32_t a_alias) -> RE::BGSLocation*
{
    using func_t = decltype(&GetLocation);
    REL::Relocation<func_t> func{ Data::Offsets::GetLocation };
    return func(a_quest, a_alias);
}

bool System::GetStageDone(RE::TESQuest* a_quest, std::uint16_t a_stage)
{
    using func_t = decltype(&GetStageDone);
    REL::Relocation<func_t> func{ Data::Offsets::GetStageDone };
    return func(a_quest, a_stage);
}

bool System::SetStage(RE::TESQuest* a_quest, std::uint16_t a_stage)
{
    using func_t = decltype(&SetStage);
    REL::Relocation<func_t> func{ Data::Offsets::SetStage };
    return func(a_quest, a_stage);
}