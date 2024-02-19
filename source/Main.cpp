#include "Events.h"
#include "System.h"
#include "Papyrus.h"

void InitLogging()
{
    auto path = logs::log_directory();
    if (!path)
        return;

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    *path /= fmt::format("{}.log", plugin->GetName());

    std::vector<spdlog::sink_ptr> sinks{ 
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true), 
        std::make_shared<spdlog::sinks::msvc_sink_mt>() 
    };

    auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::info);

    spdlog::set_default_logger(std::move(logger));
    spdlog::set_pattern("[%^%L%$] %v");
}

void InitMessaging()
{
    logs::trace("Initializing messaging listener.");
    const auto interface = SKSE::GetMessagingInterface();
    if (!interface->RegisterListener([](SKSE::MessagingInterface::Message* a_message) {
        switch (a_message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            Events::GetSingleton()->Register();
            System::GetSingleton()->LoadForms();
            System::GetSingleton()->ParseQuests();
            System::GetSingleton()->ParseRewards();
            break;
        case SKSE::MessagingInterface::kNewGame:
            System::GetSingleton()->ParseSettings(a_message);
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
            System::GetSingleton()->ParseSettings(a_message);
            break;
        }
        })) {
        stl::report_and_fail("Failed to initialize message listener.");
    }
}

void InitPapyrus()
{
    logs::trace("Initializing papyrus bindings...");

    if (SKSE::GetPapyrusInterface()->Register(Papyrus::RegisterFunctions)) {
        logs::info("Papyrus functions bound.");
    } else {
        stl::report_and_fail("Failure to register Papyrus bindings.");
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    InitLogging();

    const auto plugin = SKSE::PluginDeclaration::GetSingleton();
    logs::info("{} v{} is loading...", plugin->GetName(), plugin->GetVersion());

    SKSE::Init(a_skse);
    InitMessaging();
    InitPapyrus();

    logs::info("{} loaded.", plugin->GetName());

    return true;
}