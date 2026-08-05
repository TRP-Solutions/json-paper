//
// Created by mikke on 01/04/2026.
//

#include "../../Arduino/src/network/request.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

#include "e-paper/paper_command.h"

using json = nlohmann::json;

bool ParseEndpoint(
    const std::string& address,
    std::string& origin,
    std::string& target
) {
    std::string url = address;
    if (url.find("://") == std::string::npos) {
        url = "http://" + url;
    }

    const size_t schemeEnd = url.find("://");
    const std::string scheme = url.substr(0, schemeEnd);
    if (scheme != "http" && scheme != "https") {
        return false;
    }

    const size_t authorityStart = schemeEnd + 3;
    const size_t targetStart = url.find_first_of("/?#", authorityStart);
    const size_t authorityEnd =
        targetStart == std::string::npos ? url.size() : targetStart;
    if (authorityEnd == authorityStart) {
        return false;
    }

    origin = url.substr(0, authorityEnd);
    target = "/";

    if (targetStart != std::string::npos && url[targetStart] != '#') {
        const size_t fragmentStart = url.find('#', targetStart);
        std::string requestTarget = url.substr(
            targetStart,
            fragmentStart == std::string::npos
                ? std::string::npos
                : fragmentStart - targetStart
        );
        target = requestTarget.front() == '?'
            ? "/" + requestTarget
            : requestTarget;
    }

    return true;
}

std::vector<PaperCommand> Request::RequestConfig(std::string addr) {
    std::string origin;
    std::string target;
    if (!ParseEndpoint(addr, origin, target)) {
        std::cerr << "Invalid endpoint URL: " << addr << "\n";
        return {};
    }

    httplib::Client cli(origin);
    cli.set_connection_timeout(10);
    cli.set_read_timeout(10);
    cli.set_write_timeout(10);

    auto res = cli.Get(target);

    if (!res) {
        std::cerr << "Request failed: "
                  << httplib::to_string(res.error()) << "\n";
        return {};
    }
    if (res->status != 200) {
        std::cerr << "Request failed with HTTP status "
                  << res->status << "\n";
        return {};
    }

    try {
        std::vector<PaperCommand> commands;

        json parsed = json::parse(res->body);
        std::cout << parsed.dump(4) << std::endl;

        if (parsed.value("version", "") != "2.0") {
            std::cerr << "Unsupported document version (expected 2.0)\n";
            return {};
        }

        for (const auto& item : parsed["commands"])
        {
            PaperCommand command{};

            // --- cmd ---
            std::string cmdStr = item["cmd"];

            if (!cmdMap.count(cmdStr)) {
                std::cerr << "Unknown command from server: " << cmdStr << "\n";
                continue;
            }

            command.name = cmdStr;

            if (cmdStr == "draw_text") {
                const auto& args = item.at("args");
                auto& text = command.text;
                text.x = args.at("x").get<int>(); text.y = args.at("y").get<int>();
                text.width = args.at("width").get<int>(); text.height = args.at("height").get<int>();
                text.background = args.value("background", "transparent");
                text.horizontalAlign = args.value("horizontal_align", "left");
                text.verticalAlign = args.value("vertical_align", "top");
                text.wrap = args.value("wrap", "word");
                text.overflow = args.value("overflow", "ellipsis");
                text.lineSpacing = args.value("line_spacing", 0);
                for (const auto& source : args.at("spans")) {
                    PaperTextSpan span;
                    span.text = source.at("text").get<std::string>();
                    span.family = source.value("family", "sans");
                    span.weight = source.value("weight", "regular");
                    span.size = source.at("size").get<int>();
                    span.color = source.value("color", "black");
                    span.letterSpacing = source.value("letter_spacing", 0);
                    span.underline = source.value("underline", false);
                    span.strikeout = source.value("strikeout", false);
                    text.spans.push_back(std::move(span));
                }
            }

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
        // std::string err = std::string("JSON error:\n") + e.what();
        // ConsoleManager::get().log(FATAL, err.c_str());
        //
        // std::string response = std::string("Response:\n") + res->body;
        // ConsoleManager::get().log(FATAL, response.c_str());

        std::cerr << "JSON error: " << e.what() << std::endl;
        std::cerr << "Response:\n" << res->body << std::endl;
        return {};
    }
}
