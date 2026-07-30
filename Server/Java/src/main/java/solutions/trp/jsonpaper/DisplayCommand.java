package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;

@JsonPropertyOrder({"cmd", "args"})
public record DisplayCommand(String cmd, Map<String, Object> args) {
    public DisplayCommand {
        Objects.requireNonNull(cmd, "cmd");
        Objects.requireNonNull(args, "args");
        args = Collections.unmodifiableMap(new LinkedHashMap<>(args));
    }
}
