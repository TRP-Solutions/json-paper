#include "../core.h"
#include "../../e-paper/paper_command.h"

bool ParseColor(const std::string& color, uint8_t& out) {
    if (colorMap.count(color)) {
        out = colorMap[color];
        return true;
    }
    log(WARNING, ("invalid color: " + color).c_str());

    return false;
}

bool GetRequiredArg(const PaperCommand& cmd, const std::string& key, std::string& out) {
    auto it = cmd.args.find(key);
    if (it == cmd.args.end()) {
        log(WARNING, ("missing '" + key + "'").c_str());

        return false;
    }
    out = it->second;
    return true;
}

bool GetOptionalArg(const PaperCommand& cmd, const std::string& key, std::string& out) {
    auto it = cmd.args.find(key);
    if (it == cmd.args.end()) {
        return false;
    }
    out = it->second;
    return true;
}

bool ParseInt(const std::string& s, int& out) {
    char* end;

    long value = strtol(s.c_str(), &end, 10);

    if (*end != '\0') {
        log(WARNING, ("invalid int: " + s).c_str());
        return false;
    }

    out = static_cast<int>(value);
    return true;
}

bool ParseDotPixel(const std::string& s, DOT_PIXEL& out) {
    if (s == "1x1") out = DOT_PIXEL_1X1;
    else if (s == "2x2") out = DOT_PIXEL_2X2;
    else if (s == "3x3") out = DOT_PIXEL_3X3;
    else if (s == "4x4") out = DOT_PIXEL_4X4;
    else {
        log(WARNING, ("invalid width: " + s).c_str());

        return false;
    }
    return true;
}

bool ParseLineStyle(const std::string& s, LINE_STYLE& out) {
    if (lineStyleMap.count(s)) {
        out = lineStyleMap[s];
        return true;
    }
    log(WARNING, ("invalid line style: " + s).c_str());

    return false;
}
bool ParseDrawFill(const std::string& s, DRAW_FILL& out) {
    if (drawFillMap.count(s)) {
        out = drawFillMap[s];
        return true;
    }
    log(WARNING, ("invalid fill: " + s).c_str());

    return false;
}
bool ParseDotStyle(const std::string& s, DOT_STYLE& out) {
    if (dotStyleMap.count(s)) {
        out = dotStyleMap[s];
        return true;
    }
    log(WARNING, ("invalid dot style: " + s).c_str());

    return false;
}
