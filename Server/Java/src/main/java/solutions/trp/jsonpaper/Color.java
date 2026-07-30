package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonValue;

public enum Color implements WireValue {
    BLACK("black", 0),
    WHITE("white", 1),
    YELLOW("yellow", 2),
    RED("red", 3),
    TRANSPARENT("transparent", 4);

    private final String value;
    private final int paletteIndex;

    Color(String value, int paletteIndex) {
        this.value = value;
        this.paletteIndex = paletteIndex;
    }

    @Override
    @JsonValue
    public String value() {
        return value;
    }

    public int paletteIndex() {
        return paletteIndex;
    }
}
