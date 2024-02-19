#pragma once

namespace Data
{
    namespace Forms
    {
        constexpr static std::string_view FileName{ "Bounty Quests Redone.esp" };

        constexpr static RE::FormID BQR_QUST_Catalogue{ 0x1 };

        constexpr static RE::FormID BQR_DLBR_ListBandit{ 0xB0 };
        constexpr static RE::FormID BQR_DLBR_ListCreature{ 0xB1 };
        constexpr static RE::FormID BQR_DLBR_ListDragon{ 0xB2 };
        constexpr static RE::FormID BQR_DLBR_ListDraugr{ 0xB3 };
        constexpr static RE::FormID BQR_DLBR_ListDwemer{ 0xB4 };
        constexpr static RE::FormID BQR_DLBR_ListFalmer{ 0xB5 };
        constexpr static RE::FormID BQR_DLBR_ListForsworn{ 0xB6 };
        constexpr static RE::FormID BQR_DLBR_ListGiant{ 0xB7 };
        constexpr static RE::FormID BQR_DLBR_ListMage{ 0xB8 };
        constexpr static RE::FormID BQR_DLBR_ListReaver{ 0xB9 };
        constexpr static RE::FormID BQR_DLBR_ListRiekling{ 0xBA };
        constexpr static RE::FormID BQR_DLBR_ListVampire{ 0xBB };

        constexpr static RE::FormID BQR_GLOB_HoldHasBandit{ 0x7D };
        constexpr static RE::FormID BQR_GLOB_HoldHasCreature{ 0x7E };
        constexpr static RE::FormID BQR_GLOB_HoldHasDragon{ 0x7F };
        constexpr static RE::FormID BQR_GLOB_HoldHasDraugr{ 0x80 };
        constexpr static RE::FormID BQR_GLOB_HoldHasDwemer{ 0x81 };
        constexpr static RE::FormID BQR_GLOB_HoldHasFalmer{ 0x82 };
        constexpr static RE::FormID BQR_GLOB_HoldHasForsworn{ 0x83 };
        constexpr static RE::FormID BQR_GLOB_HoldHasGiant{ 0x84 };
        constexpr static RE::FormID BQR_GLOB_HoldHasMage{ 0x85 };
        constexpr static RE::FormID BQR_GLOB_HoldHasReaver{ 0x86 };
        constexpr static RE::FormID BQR_GLOB_HoldHasRiekling{ 0x87 };
        constexpr static RE::FormID BQR_GLOB_HoldHasVampire{ 0x88 };

        constexpr static RE::FormID BQR_GLOB_RewardSwitch{ 0x903 };
        constexpr static RE::FormID BQR_GLOB_DisableVanillaBounties{ 0x904 };
        constexpr static RE::FormID BQR_GLOB_QuestCount{ 0x905 };
        constexpr static RE::FormID BQR_GLOB_QuestLimit{ 0x906 };

        constexpr static RE::FormID BQBranchNodeSHARES{ 0xCBA9F };
    }

    namespace Offsets
    {
        constexpr static auto GetStageDone{ RELOCATION_ID(24483, 25011) };
        constexpr static auto SetStage{ RELOCATION_ID(24482, 25004) };
        constexpr static auto GetLocation { RELOCATION_ID(24538, 25067) };
    }

    namespace Scripts
    {
        constexpr static std::string_view BQR_SCPT_StartQuest{ "BQR_SCPT_StartQuest" };
        constexpr static std::string_view Target{ "Target" };
    }
}