package solutions.trp.jsonpaper;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DisplayTest {
    private final ObjectMapper objectMapper = new ObjectMapper();

    @Test
    void writesEveryPhpCommandWithFirmwareFieldNames() throws Exception {
        Display display = new Display()
            .clear(Color.WHITE)
            .clearWindow(1, 2, 3, 4, Color.RED)
            .point(5, 6, Color.YELLOW, Width.W2, PointStyle.RIGHTUP)
            .line(7, 8, 9, 10, Color.BLACK, Width.W3, LineStyle.DOTTED)
            .rectangle(11, 12, 13, 14, Color.RED, Width.W4, FillMode.FULL)
            .circle(15, 16, 17, Color.YELLOW, Width.W2, FillMode.EMPTY)
            .pieSlice(18, 19, 20, 270, 90, Color.RED)
            .text(
                21, 22, "A \"quoted\" value",
                Font.FONT_24, Color.BLACK, Color.TRANSPARENT
            );

        JsonNode root = objectMapper.readTree(display.toJson());

        assertEquals("1.0", root.get("version").textValue());
        assertEquals(8, root.get("commands").size());
        assertEquals("clear_window", root.at("/commands/1/cmd").textValue());
        assertEquals(1, root.at("/commands/1/args/x_start").intValue());
        assertEquals("rightup", root.at("/commands/2/args/style").textValue());
        assertEquals("dotted", root.at("/commands/3/args/style").textValue());
        assertEquals("full", root.at("/commands/4/args/fill").textValue());
        assertEquals("draw_pie_slice", root.at("/commands/6/cmd").textValue());
        assertEquals(270, root.at("/commands/6/args/start_angle").intValue());
        assertEquals(90, root.at("/commands/6/args/sweep_angle").intValue());
        assertEquals(
            "A \"quoted\" value",
            root.at("/commands/7/args/text").textValue()
        );
        assertEquals("font24", root.at("/commands/7/args/font").textValue());
        assertEquals(
            "transparent",
            root.at("/commands/7/args/background").textValue()
        );
    }

    @Test
    void appliesPhpDefaults() {
        DisplayDocument document = new Display()
            .point(1, 2)
            .line(1, 2, 3, 4)
            .rectangle(1, 2, 3, 4)
            .circle(1, 2, 1)
            .text(1, 2, "text")
            .document();

        List<DisplayCommand> commands = document.commands();
        assertEquals("black", commands.get(0).args().get("color"));
        assertEquals("1x1", commands.get(0).args().get("width"));
        assertEquals("around", commands.get(0).args().get("style"));
        assertEquals("solid", commands.get(1).args().get("style"));
        assertEquals("empty", commands.get(2).args().get("fill"));
        assertEquals("empty", commands.get(3).args().get("fill"));
        assertEquals("font16", commands.get(4).args().get("font"));
        assertEquals("transparent", commands.get(4).args().get("background"));
    }

    @Test
    void documentIsAnImmutableSnapshot() throws Exception {
        Display display = new Display().clear(Color.WHITE);
        DisplayDocument snapshot = display.document();

        display.point(1, 1);

        assertEquals(1, snapshot.commands().size());
        assertEquals(2, display.document().commands().size());
        assertFalse(snapshot.commands().isEmpty());
        assertTrue(snapshot.commands().get(0).args().containsKey("color"));
        assertEquals(
            objectMapper.readTree(objectMapper.writeValueAsString(snapshot)),
            objectMapper.readTree(new ObjectMapper().writeValueAsString(snapshot))
        );
    }

    @Test
    void prettyJsonRepresentsTheSameDocument() throws Exception {
        Display display = new Display().text(1, 2, "hello");

        assertEquals(
            objectMapper.readTree(display.toJson()),
            objectMapper.readTree(display.toPrettyJson())
        );
        assertTrue(display.toPrettyJson().contains(System.lineSeparator()));
    }
}
