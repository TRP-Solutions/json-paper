package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonValue;

public enum Width implements WireValue {
    W1("1x1"),
    W2("2x2"),
    W3("3x3"),
    W4("4x4");

    private final String value;

    Width(String value) {
        this.value = value;
    }

    @Override
    @JsonValue
    public String value() {
        return value;
    }
}
