package solutions.trp.jsonpaper;

import java.nio.file.Path;
import java.util.Objects;

public record RasterFontFamily(Path regular, Path bold) {
    public RasterFontFamily {
        Objects.requireNonNull(regular, "regular");
        Objects.requireNonNull(bold, "bold");
    }
}
