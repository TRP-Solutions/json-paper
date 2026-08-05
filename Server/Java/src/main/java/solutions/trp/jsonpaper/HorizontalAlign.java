package solutions.trp.jsonpaper;
import com.fasterxml.jackson.annotation.JsonValue;

public enum HorizontalAlign implements WireValue {
    LEFT("left"), CENTER("center"), RIGHT("right");
    private final String value;
    HorizontalAlign(String value) { this.value = value; }
    @Override @JsonValue public String value() { return value; }
}
