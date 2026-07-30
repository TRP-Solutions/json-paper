#include <cmath>

#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"

namespace {

double NormalizeAngle(double angle) {
    angle = std::fmod(angle, 360.0);
    return angle < 0.0 ? angle + 360.0 : angle;
}

bool AngleIsInside(double angle, double start, double sweep) {
    if (sweep >= 360.0) return true;
    return NormalizeAngle(angle - start) <= sweep;
}

}  // namespace

void DrawPieSlice(const PaperCommand& command) {
    std::string xs, ys, radiusString, startString, sweepString, colorString;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "radius", radiusString) ||
        !GetRequiredArg(command, "start_angle", startString) ||
        !GetRequiredArg(command, "sweep_angle", sweepString) ||
        !GetRequiredArg(command, "color", colorString)) {
        log(WARNING, "Missing required args for 'draw_pie_slice'");
        return;
    }

    int x, y, radius, startAngle, sweepAngle;
    if (!ParseInt(xs, x) ||
        !ParseInt(ys, y) ||
        !ParseInt(radiusString, radius) ||
        !ParseInt(startString, startAngle) ||
        !ParseInt(sweepString, sweepAngle)) {
        return;
    }

    uint8_t color;
    if (!ParseColor(colorString, color)) return;

    if (x < 0 || y < 0 ||
        x >= Paint.Width || y >= Paint.Height ||
        radius <= 0 || radius > 4096 ||
        sweepAngle <= 0 || sweepAngle > 360) {
        log(WARNING, "Invalid range for 'draw_pie_slice'");
        return;
    }

    const double normalizedStart = NormalizeAngle(startAngle);
    const int radiusSquared = radius * radius;

    const int minX = x - radius < 0 ? 0 : x - radius;
    const int maxX = x + radius >= Paint.Width ? Paint.Width - 1 : x + radius;
    const int minY = y - radius < 0 ? 0 : y - radius;
    const int maxY = y + radius >= Paint.Height ? Paint.Height - 1 : y + radius;

    for (int py = minY; py <= maxY; ++py) {
        const int dy = py - y;

        for (int px = minX; px <= maxX; ++px) {
            const int dx = px - x;
            if (dx * dx + dy * dy > radiusSquared) continue;

            // Screen coordinates grow downward, so positive angles are
            // clockwise: 0° right, 90° down, 180° left, 270° up.
            const double angle = NormalizeAngle(
                std::atan2(static_cast<double>(dy),
                           static_cast<double>(dx)) *
                180.0 / 3.14159265358979323846
            );

            if (AngleIsInside(angle, normalizedStart, sweepAngle)) {
                Paint_SetPixel(px, py, color);
            }
        }
    }
}
