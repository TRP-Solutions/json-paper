package solutions.trp.jsonpaper;

import org.junit.jupiter.api.Test;

import java.awt.image.BufferedImage;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;

class DashboardSampleTest {
    @Test
    void createsThePhpDashboardCommandSequence() {
        BufferedImage logo = new BufferedImage(2, 1, BufferedImage.TYPE_INT_ARGB);

        DisplayDocument dashboard = DashboardSample.create(logo);
        List<DisplayCommand> commands = dashboard.commands();

        assertEquals("1.0", dashboard.version());
        assertEquals(45, commands.size());
        assertEquals("clear", commands.get(0).cmd());
        assertEquals("draw_image", commands.get(1).cmd());
        assertEquals("draw_pie_slice", commands.get(2).cmd());
        assertEquals(120, commands.get(2).args().get("sweep_angle"));
        assertEquals(120, commands.get(3).args().get("start_angle"));
        assertEquals(240, commands.get(4).args().get("start_angle"));
        assertEquals("draw_rectangle", commands.get(11).cmd());
        assertEquals("dotted", commands.get(13).args().get("style"));
        assertEquals(
            "Total hours spend: 48",
            commands.get(43).args().get("text")
        );
        assertEquals(
            "Total Projects: 15",
            commands.get(44).args().get("text")
        );
    }
}
