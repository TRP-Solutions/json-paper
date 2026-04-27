#include "emulation/epd_test.h"
#include "manager/console_manager.h"
#include "util/dev/console/console.h"
#include "util/dev/console/command/auto_completion.h"
#include "util/dev/console/command/registry.h"

void RegisterCoreCommands(CommandRegistry &registry) {

    registry.registerCommand({
        "load_url",
        "Load a url to fetch refresh data from",

        {
            {"url", ArgType::STRING, false}
        },

        [](const ParsedArgs &args) {
            std::string url = std::get<std::string>(args.values.at("url"));

            simulate_epd_5in79g_test(url);
        }
    });

    registry.registerCommand({
        "help",
        "Show available commands",

        {
            {
                "command",
                ArgType::STRING,
                true,
                CompleteCommandNames
            }
        },

        [&registry](const ParsedArgs &args) {
            if (!args.values.contains("command")) {
                ConsoleManager::get().log(INFO, "Available commands:");

                for (const auto &[name, cmd]: registry.all()) {
                    ConsoleManager::get().log(INFO, "  %-12s - %s",
                                              name.c_str(),
                                              cmd.description.c_str()
                    );
                }

                return;
            }

            const std::string &target =
                    std::get<std::string>(args.values.at("command"));

            Command *cmd = registry.find(target);

            if (!cmd) {
                ConsoleManager::get().log(FATAL, "Unknown command: %s", target.c_str());
                return;
            }

            std::string usage = target;

            for (const auto &arg: cmd->args) {
                if (arg.optional)
                    usage += " [" + arg.name + "]";
                else
                    usage += " <" + arg.name + ">";
            }

            ConsoleManager::get().log(INFO, "Usage: %s", usage.c_str());
            ConsoleManager::get().log(INFO, "Description: %s", cmd->description.c_str());

            if (!cmd->args.empty()) {
                ConsoleManager::get().log(INFO, "Arguments:");

                for (const auto &arg: cmd->args) {
                    ConsoleManager::get().log(INFO,
                                              "  %s (%s)%s",
                                              arg.name.c_str(),
                                              ArgTypeToString(arg.type),
                                              arg.optional ? " optional" : ""
                    );
                }
            }
        }
    });

    registry.registerCommand({
        "clear",
        "Clears the console",

        {},

        [](const ParsedArgs &args) {
            ConsoleManager::get().clearLogs();
        }
    });
}
