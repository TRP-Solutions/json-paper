#include "request.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <cstdlib>

#include "../e-paper/paper_command.h"

WiFiClient httpClient;
WiFiClientSecure httpsClient;

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
        // Provisioning currently has no CA-certificate mechanism.
        httpsClient.setInsecure();
    }
    else if (addr.rfind("http://", 0) == 0) {

        addr = addr.substr(7);

        protocol = "HTTP";
        port = 80;

        client = &httpClient;
    }
    else {
        protocol = "HTTP";
        port = 80;
        client = &httpClient;
    }

    size_t slashPos = addr.find('/');

    if (slashPos != std::string::npos) {
        path = addr.substr(slashPos);
        addr = addr.substr(0, slashPos);
    }

    // An explicit port overrides the protocol default. Hostnames and IPv4
    // addresses are supported; bracketed IPv6 literals are not.
    size_t colonPos = addr.rfind(':');
    if (colonPos != std::string::npos) {
        std::string portText = addr.substr(colonPos + 1);
        char* end = nullptr;
        long parsedPort = std::strtol(portText.c_str(), &end, 10);

        if (!portText.empty() && end != nullptr && *end == '\0' &&
            parsedPort >= 1 && parsedPort <= 65535) {
            port = static_cast<int>(parsedPort);
            addr = addr.substr(0, colonPos);
        } else {
            Serial.println("Invalid port in JSON URL");
            addr.clear();
        }
    }

    return addr;
}


// Http
std::string httpGet(std::string host, std::string path) {

    std::string response;

    if (host.empty()) {
        Serial.println("Invalid JSON URL");
        return response;
    }

    Serial.println((protocol + " connecting...").c_str());

    if (client->connect(host.c_str(), port)) {

        Serial.println((protocol + " OK").c_str());

        client->print("GET ");
        client->print(path.c_str());
        client->println(" HTTP/1.1");
        client->print("Host: ");
        client->print(host.c_str());
        if ((protocol == "HTTP" && port != 80) ||
            (protocol == "HTTPS" && port != 443)) {
            client->print(":");
            client->print(port);
        }
        client->println();
        client->println("Connection: close");
        client->println();

        size_t contentLength = 0;
        unsigned long lastDataTime = millis();
        const unsigned long responseIdleTimeoutMs = 5000;
        const size_t maxResponseBytes = 128 * 1024;

        // Headers are short, line-oriented fields. Stop using line reads as
        // soon as the blank separator is reached: a base64 JSON value can be
        // one very large line and would fragment the Arduino String heap.
        while (client->connected() || client->available()) {
            if (!client->available()) {
                if (millis() - lastDataTime >= responseIdleTimeoutMs) {
                    Serial.println("HTTP header timeout");
                    client->stop();
                    return {};
                }
                delay(10);
                continue;
            }

            String line = client->readStringUntil('\n');
            lastDataTime = millis();

            if (line.startsWith("Content-Length:")) {
                contentLength = static_cast<size_t>(line.substring(15).toInt());
                if (contentLength > maxResponseBytes) {
                    Serial.println("HTTP response is too large");
                    client->stop();
                    return {};
                }
            }

            if (line == "\r") {
                break;
            }
        }

        if (contentLength > 0) {
            response.reserve(contentLength);
        }

        // Read the body in bounded chunks so a long base64 line never creates
        // a second, repeatedly-growing Arduino String.
        char buffer[512];
        lastDataTime = millis();
        while (client->connected() || client->available()) {
            int availableBytes = client->available();
            if (availableBytes <= 0) {
                if (millis() - lastDataTime >= responseIdleTimeoutMs) {
                    Serial.println("HTTP response idle timeout; processing received data");
                    break;
                }
                delay(10);
                continue;
            }

            size_t bytesToRead = static_cast<size_t>(availableBytes);
            if (bytesToRead > sizeof(buffer)) {
                bytesToRead = sizeof(buffer);
            }

            int bytesRead = client->read(
                reinterpret_cast<uint8_t*>(buffer),
                bytesToRead
            );
            if (bytesRead <= 0) {
                continue;
            }
            lastDataTime = millis();

            if (response.size() + static_cast<size_t>(bytesRead) >
                maxResponseBytes) {
                Serial.println("HTTP response is too large");
                client->stop();
                return {};
            }
            response.append(buffer, static_cast<size_t>(bytesRead));

            if (contentLength > 0 && response.size() >= contentLength) {
                break;
            }
        }

        client->stop();
    }
    else {

        Serial.println("Connection failed");
    }

    Serial.println("Return response");
    Serial.print("Response body bytes: ");
    Serial.println(response.length());

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

    JsonDocument doc;
    // ArduinoJson can parse a mutable buffer in place, avoiding a second copy
    // of large strings such as base64-encoded image data.
    DeserializationError error = deserializeJson(doc, body.data());

    if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        Serial.print("Response begins with: ");
        Serial.println(body.substr(0, 200).c_str());

        return commands;
    }

    JsonArray jsonCommands = doc["commands"];
    if (jsonCommands.isNull()) {
        Serial.println("JSON response has no 'commands' array");
        return commands;
    }

    for (JsonObject item : jsonCommands) {

        PaperCommand command;

        command.name = item["cmd"].as<const char*>();

        if (item.containsKey("args")) {
            JsonObject args = item["args"];

            for (JsonPair kv : args) {
                std::string key = kv.key().c_str();
                std::string value = kv.value().as<std::string>().c_str();

                command.args[key] = value;
            }
        }

        commands.push_back(command);
    }

    Serial.print("Received drawing commands: ");
    Serial.println(commands.size());

    return commands;
}
