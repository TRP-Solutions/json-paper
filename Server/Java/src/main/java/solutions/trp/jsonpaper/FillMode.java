package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonValue;

public enum FillMode implements WireValue {
    EMPTY("empty"),
    FULL("full");

    private final String value;

    FillMode(String value) {
        this.value = value;
    }

    @Override
    @JsonValue
    public String value() {
        return value;
    }
}
