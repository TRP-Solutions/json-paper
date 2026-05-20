#include "request.h"

#include <WiFiS3.h>
#include <ArduinoJson.h>

#include "../e-paper/paper_command.h"

WiFiClient httpClient;
WiFiSSLClient httpsClient;

WiFiClient* client;

int port = 0;
std::string protocol;


// Url parser
std::string NormalizeHost(std::string addr, std::string& path) {

    path = "/";

    if (addr.rfind("https://", 0) == 0) {

        addr = addr.substr(8);

        protocol = "HTTPS";
        port = 443;

        client = &httpsClient;
    }
    else if (addr.rfind("http://", 0) == 0) {

        addr = addr.substr(7);

        protocol = "HTTP";
        port = 80;

        client = &httpClient;
    }

    size_t slashPos = addr.find('/');

    if (slashPos != std::string::npos) {
        path = addr.substr(slashPos);
        addr = addr.substr(0, slashPos);
    }

    return addr;
}


// Http
std::string httpGet(std::string host, std::string path) {

    std::string response;

    Serial.println((protocol + " connecting...").c_str());

    if (client->connect(host.c_str(), port)) {

        Serial.println((protocol + " OK").c_str());

        client->print("GET ");
        client->print(path.c_str());
        client->println(" HTTP/1.1");

        client->print("Host: ");
        client->println(host.c_str());

        client->println("Connection: close");
        client->println();

        bool headerEnded = false;

        while (client->connected() || client->available()) {
            String line = client->readStringUntil('\n');

            if (line == "\r") {
                headerEnded = true;
                continue;
            }

            if (headerEnded) {
                response += std::string(line.c_str());
            }
        }

        client->stop();
    }
    else {

        Serial.println("Connection failed");
    }

    return response;
}

// Request
std::vector<PaperCommand> Request::RequestConfig(std::string addr) {

    std::string path;

    std::string host = NormalizeHost(addr, path);

    std::string body = httpGet(host, path);

    std::vector<PaperCommand> commands;

    if (body.empty()) {
        Serial.println("Empty response");
        return commands;
    }

    StaticJsonDocument<2048> doc;

    DeserializationError error = deserializeJson(doc, body.c_str());

    if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());

        return commands;
    }

    JsonArray jsonCommands =
        doc["commands"];

    int countCmd = 1;

    for (JsonObject item : jsonCommands) {

        PaperCommand command;

        command.name = item["cmd"].as<const char*>();

        Serial.println(("#" + String(countCmd) + " CMD:").c_str());

        Serial.print("   • ");
        Serial.println(command.name.c_str());

        countCmd++;

        if (item.containsKey("args")) {
            JsonObject args = item["args"];

            for (JsonPair kv : args) {
                std::string key = kv.key().c_str();

                std::string value = kv.value().as<String>().c_str();

                command.args[key] = value;

                Serial.print("      ");
                Serial.print(key.c_str());
                Serial.print(" = ");
                Serial.println(value.c_str());
            }
        }

        commands.push_back(command);
    }

    return commands;
}