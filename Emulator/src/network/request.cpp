//
// Created by mikke on 01/04/2026.
//

#include "request.h"

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

#include "../e-Paper/Command.h"

using json = nlohmann::json;

std::string NormalizeHost(std::string& addr, std::string& path) {
    path = "/";

    // Remove protocol
    if (addr.find("http://") == 0)
        addr = addr.substr(7);
    else if (addr.find("https://") == 0)
        addr = addr.substr(8);

    // Split host + path
    size_t slashPos = addr.find('/');
    if (slashPos != std::string::npos) {
        path = addr.substr(slashPos);
        addr = addr.substr(0, slashPos);
    }

    return addr;
}

std::vector<Command> Request::RequestConfig(std::string addr) {
    std::string host = addr;
    std::string path;

    host = NormalizeHost(host, path);

    httplib::Client cli(host.c_str());

    auto res = cli.Get(path.c_str());

    if (!res || res->status != 200) {
        std::cerr << "Request failed\n";
        return {};
    }

    try {
        std::vector<Command> commands;

        json parsed = json::parse(res->body);
        std::cout << parsed.dump(4) << std::endl;

        for (const auto& item : parsed["commands"])
        {
            Command command{};

            // --- cmd ---
            std::string cmdStr = item["cmd"];

            if (!cmdMap.count(cmdStr)) {
                std::cerr << "Unknown command from server: " << cmdStr << "\n";
                continue;
            }

            command.name = cmdStr;

            // --- args ---
            if (item.contains("args"))
            {
                for (auto& [key, value] : item["args"].items())
                {
                    if (value.is_string()) {
                        command.args[key] = value.get<std::string>();
                    } else {
                        command.args[key] = value.dump();
                    }
                }
            }

            commands.push_back(command);
        }

        return commands;
    } catch (const std::exception& e) {
        std::cerr << "JSON error: " << e.what() << std::endl;
        std::cerr << "Response:\n" << res->body << std::endl;
        return {};
    }
}