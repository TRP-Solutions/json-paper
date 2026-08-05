package solutions.trp.jsonpaper;
import com.fasterxml.jackson.annotation.JsonValue;

public enum TextOverflow implements WireValue {
    CLIP("clip"), ELLIPSIS("ellipsis");
    private final String value;
    TextOverflow(String value) { this.value = value; }
    @Override @JsonValue public String value() { return value; }
}
