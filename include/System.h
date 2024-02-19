#pragma once

class System
{
public:
    enum class Type : std::uint32_t
    {
        kNone = 0,
        kBandit = 1,
        kCreature = 2,
        kDragon = 3,
        kDraugr = 4,
        kDwemer = 5,
        kFalmer = 6,
        kForsworn = 7,
        kGiant = 8,
        kMage = 9,
        kReaver = 10,
        kRiekling = 11,
        kVampire = 12
    };

    struct Reward
    {
        RE::TESForm* form;
        float quantity;
        float base;
        float maximum;
    };

    struct Quest
    {
        RE::BGSLocation* location;
        RE::BGSLocation* hold;
        Type type;
    };

    struct Counter
    {
        float level;
        float quantity;
    };

    static auto GetSingleton() -> System*;

    void BindScriptToInfo(const RE::TESTopicInfo* a_info, const RE::TESQuest* a_placeholder);
    void CompleteQuests(const RE::BGSLocation* a_hold);
    auto GetAliasByName(const RE::TESQuest* a_quest, const std::string_view& a_name) -> RE::BGSBaseAlias*;
    auto GetNextAvailableInfo(const RE::TESTopic* a) -> RE::TESTopicInfo*;
    auto GetNextAvailablePlaceholder(bool a_force = false) -> RE::TESQuest*;
    auto GetNextAvailableTopic(const RE::BGSDialogueBranch* a_branch) -> RE::TESTopic*;
    auto GetQuestCount() -> float;
    auto GetQuestLimit() -> float;
    bool IsLocationAvailable(const RE::BGSLocation* a_location);
    void LoadForms();
    auto LocationToHold(const RE::BGSLocation* a_location) -> const RE::BGSLocation*;
    void ParseQuests();
    void ParseRewards();
    void ParseSettings(SKSE::MessagingInterface::Message* a_message);
    void ResetData();
    void RewardPlayer(const RE::BGSLocation* a_location, std::uint32_t a_level);
    void SetQuestCount(float a_value);
    void StartEveryQuest();
    void StartEveryQuest(const RE::BGSLocation* a_hold, Type a_type);
    bool StartOnNewGame();
    void StartRandomQuest(const RE::BGSLocation* a_hold, Type a_type);
    auto StringToType(const std::string_view& a_string) -> Type;
    auto TypeToBranch(const Type a_type) -> RE::BGSDialogueBranch*;
    void UpdateCatalogueTopics(const RE::BGSLocation* a_hold);
    void UpdateGlobalValue(const Type a_type, bool a_value);
    void UpdateInfoConditions(const RE::TESTopicInfo* a_info, const RE::BGSLocation* a_location, const RE::BGSLocation* a_hold, bool a_switch);
    void UpdateLocationAlias(RE::TESQuest* a_quest, const std::string_view& a_name, const RE::BGSLocation* a_location, bool a_restart);
    void UpdatePendingRewards(const RE::BGSLocation* a_hold);

    static auto GetLocation(RE::TESQuest* a_quest, std::uint32_t a_alias) -> RE::BGSLocation*;
    static bool GetStageDone(RE::TESQuest* a_quest, std::uint16_t a_stage);
    static bool SetStage(RE::TESQuest* a_quest, std::uint16_t a_stage);
private:
    System() = default;
    System(const System&) = delete;
    System(System&&) = delete;

    ~System() = default;

    System& operator=(const System&) = delete;
    System& operator=(System&&) = delete;

    bool startEveryQuest;

    std::vector<Quest> quests;
    std::vector<Reward> rewards;
    std::vector<RE::TESTopic*> topics;

    std::unordered_map<Type, RE::TESGlobal*> globals;
    std::unordered_map<Type, float> multipliers;
    std::unordered_map<RE::TESQuest*, bool> placeholders;
};