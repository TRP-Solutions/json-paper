package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonValue;

public enum LineStyle implements WireValue {
    SOLID("solid"),
    DOTTED("dotted");

    private final String value;

    LineStyle(String value) {
        this.value = value;
    }

    @Override
    @JsonValue
    public String value() {
        return value;
    }
}
