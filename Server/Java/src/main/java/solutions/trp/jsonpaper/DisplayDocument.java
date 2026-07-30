package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.util.List;
import java.util.Objects;

@JsonPropertyOrder({"version", "commands"})
public record DisplayDocument(String version, List<DisplayCommand> commands) {
    public DisplayDocument {
        Objects.requireNonNull(version, "version");
        commands = List.copyOf(Objects.requireNonNull(commands, "commands"));
    }
}
