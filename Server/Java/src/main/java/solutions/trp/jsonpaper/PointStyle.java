package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonValue;

public enum PointStyle implements WireValue {
    AROUND("around"),
    RIGHTUP("rightup");

    private final String value;

    PointStyle(String value) {
        this.value = value;
    }

    @Override
    @JsonValue
    public String value() {
        return value;
    }
}
