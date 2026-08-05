package solutions.trp.jsonpaper;
import com.fasterxml.jackson.annotation.JsonValue;

public enum VerticalAlign implements WireValue {
    TOP("top"), MIDDLE("middle"), BOTTOM("bottom");
    private final String value;
    VerticalAlign(String value) { this.value = value; }
    @Override @JsonValue public String value() { return value; }
}
