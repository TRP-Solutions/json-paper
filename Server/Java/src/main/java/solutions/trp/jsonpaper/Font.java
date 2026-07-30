package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonValue;

public enum Font implements WireValue {
    FONT_8("font8"),
    FONT_12("font12"),
    FONT_16("font16"),
    FONT_20("font20"),
    FONT_24("font24");

    private final String value;

    Font(String value) {
        this.value = value;
    }

    @Override
    @JsonValue
    public String value() {
        return value;
    }
}
