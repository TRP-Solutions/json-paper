package solutions.trp.jsonpaper;
import com.fasterxml.jackson.annotation.JsonValue;

public enum FontWeight implements WireValue {
    REGULAR("regular"), BOLD("bold");
    private final String value;
    FontWeight(String value) { this.value = value; }
    @Override @JsonValue public String value() { return value; }
}
