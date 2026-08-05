package solutions.trp.jsonpaper;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

/**
 * Java equivalent of {@code Server/PHP/sample/index.php}.
 */
public final class DashboardSample {
    private static final String DEVICE_FONT = "helvetica";
    private DashboardSample() {
    }

    public static DisplayDocument create(Path logoPath) throws IOException {
        return create(ImageLoader.load(logoPath));
    }

    public static DisplayDocument create(BufferedImage logo) {
        int todoTaskCount = 5;
        int awaitingTaskCount = 5;
        int finishedTaskCount = 5;

        int[] workHours = {
            3, 12, 6, 1, 2,
            7, 3, 4, 5, 5
        };
        String[] workDayLabels = {
            "MON", "TUE", "WED", "THU", "FRI",
            "MON", "TUE", "WED", "THU", "FRI"
        };
        Color[] workColors = {
            Color.RED, Color.RED, Color.RED, Color.RED, Color.RED,
            Color.YELLOW, Color.YELLOW, Color.YELLOW, Color.YELLOW, Color.YELLOW
        };

        int totalTaskCount = todoTaskCount
            + awaitingTaskCount
            + finishedTaskCount;

        int todoSweepAngle = (int) (
            360.0 * todoTaskCount / totalTaskCount
        );
        int awaitingSweepAngle = (int) (
            360.0 * awaitingTaskCount / totalTaskCount
        );
        int finishedSweepAngle = (int) (
            360.0 * finishedTaskCount / totalTaskCount
        );

        Display display = new Display()
            .clear(Color.WHITE)
            .image(5, 5, logo, Color.TRANSPARENT)
            .pieSlice(85, 185, 70, 0, todoSweepAngle, Color.RED)
            .pieSlice(
                85,
                185,
                70,
                todoSweepAngle,
                awaitingSweepAngle,
                Color.YELLOW
            )
            .pieSlice(
                85,
                185,
                70,
                todoSweepAngle + awaitingSweepAngle,
                finishedSweepAngle,
                Color.BLACK
            )
            .circle(175, 157, 7, Color.RED, Width.W1, FillMode.FULL)
            .text(label(195, 150, 100, 22, "Todo", 16, Color.RED))
            .circle(175, 187, 7, Color.YELLOW, Width.W1, FillMode.FULL)
            .text(label(195, 180, 100, 22, "Awaiting", 16, Color.YELLOW))
            .circle(175, 217, 7, Color.BLACK, Width.W1, FillMode.FULL)
            .text(label(195, 210, 100, 22, "Finished", 16, Color.BLACK));

        int chartLeft = 320;
        int chartTop = 10;
        int chartRight = 780;
        int chartBottom = 160;
        int chartInnerHeight = chartBottom - chartTop - 2;
        int barSlotWidth = (chartRight - chartLeft) / workHours.length;
        int barWidth = barSlotWidth - 4;
        int maxWorkHours = Math.max(1, Arrays.stream(workHours).max().orElse(1));

        display.rectangle(
            chartLeft,
            chartTop,
            chartRight,
            chartBottom,
            Color.BLACK,
            Width.W1,
            FillMode.EMPTY
        );

        int tickStep = Math.max(1, (int) Math.ceil(maxWorkHours / 6.0));
        for (int hour = tickStep; hour <= maxWorkHours; hour += tickStep) {
            addTick(
                display,
                hour,
                maxWorkHours,
                chartLeft,
                chartTop,
                chartRight,
                chartBottom,
                chartInnerHeight
            );
        }
        if (maxWorkHours % tickStep != 0) {
            addTick(
                display,
                maxWorkHours,
                maxWorkHours,
                chartLeft,
                chartTop,
                chartRight,
                chartBottom,
                chartInnerHeight
            );
        }

        for (int barIndex = 0; barIndex < workHours.length; barIndex++) {
            int hours = workHours[barIndex];
            int barCenter = chartLeft
                + barIndex * barSlotWidth
                + barSlotWidth / 2;
            int barHeight = (int) Math.round(
                (double) chartInnerHeight * hours / maxWorkHours
            );

            if (barHeight > 0) {
                display.rectangle(
                    barCenter - barWidth / 2,
                    chartBottom - barHeight,
                    barCenter + barWidth / 2 - 1,
                    chartBottom,
                    workColors[barIndex],
                    Width.W1,
                    FillMode.FULL
                );
            }

            display.text(label(barCenter - 20, 165, 40, 16,
                workDayLabels[barIndex], 12, Color.BLACK));
        }

        int totalWorkHours = Arrays.stream(workHours).sum();
        display
            .text(label(320, 210, 300, 22, "Total hours spend: " + totalWorkHours, 16, Color.BLACK))
            .text(label(320, 240, 300, 22, "Total Projects: " + totalTaskCount, 16, Color.BLACK));

        return display.document();
    }

    private static void addTick(
        Display display,
        int hour,
        int maxWorkHours,
        int chartLeft,
        int chartTop,
        int chartRight,
        int chartBottom,
        int chartInnerHeight
    ) {
        int tickY = chartBottom - (int) Math.round(
            (double) chartInnerHeight * hour / maxWorkHours
        );
        String tickText = Integer.toString(hour);

        display.text(TextBox.builder(chartLeft - 40,
                Math.max(chartTop, Math.min(chartBottom - 12, tickY - 6)), 34, 16)
            .horizontalAlign(HorizontalAlign.RIGHT)
            .span(TextSpan.builder(tickText).family(DEVICE_FONT).size(12).build())
            .build());

        if (hour != maxWorkHours) {
            display.line(
                chartLeft,
                tickY,
                chartRight,
                tickY,
                Color.BLACK,
                Width.W1,
                LineStyle.DOTTED
            );
        }
    }

    private static TextBox label(int x, int y, int width, int height,
                                 String text, int size, Color color) {
        return new TextBox(x, y, width, height, List.of(
            TextSpan.builder(text).family(DEVICE_FONT).size(size).color(color).build()));
    }
}
