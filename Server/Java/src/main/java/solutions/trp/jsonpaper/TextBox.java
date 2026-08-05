package solutions.trp.jsonpaper;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import java.util.List;
import java.util.Objects;

@JsonPropertyOrder({"x", "y", "width", "height", "background", "horizontal_align", "vertical_align", "wrap", "overflow", "line_spacing", "spans"})
public record TextBox(
    int x, int y, int width, int height,
    Color background,
    @JsonProperty("horizontal_align") HorizontalAlign horizontalAlign,
    @JsonProperty("vertical_align") VerticalAlign verticalAlign,
    String wrap,
    TextOverflow overflow,
    @JsonProperty("line_spacing") int lineSpacing,
    List<TextSpan> spans
) {
    public TextBox {
        if (width <= 0 || height <= 0) throw new IllegalArgumentException("text box width and height must be positive");
        Objects.requireNonNull(background, "background");
        Objects.requireNonNull(horizontalAlign, "horizontalAlign");
        Objects.requireNonNull(verticalAlign, "verticalAlign");
        if (!"word".equals(wrap)) throw new IllegalArgumentException("only word wrapping is supported");
        Objects.requireNonNull(overflow, "overflow");
        if (lineSpacing < 0) throw new IllegalArgumentException("lineSpacing must not be negative");
        spans = List.copyOf(Objects.requireNonNull(spans, "spans"));
        if (spans.isEmpty()) throw new IllegalArgumentException("at least one text span is required");
        if (spans.stream().anyMatch(Objects::isNull)) throw new NullPointerException("spans contains null");
    }

    public TextBox(int x, int y, int width, int height, List<TextSpan> spans) {
        this(x, y, width, height, Color.TRANSPARENT, HorizontalAlign.LEFT,
            VerticalAlign.TOP, "word", TextOverflow.ELLIPSIS, 0, spans);
    }

    public static Builder builder(int x, int y, int width, int height) { return new Builder(x, y, width, height); }
    public static final class Builder {
        private final int x, y, width, height;
        private Color background = Color.TRANSPARENT;
        private HorizontalAlign horizontalAlign = HorizontalAlign.LEFT;
        private VerticalAlign verticalAlign = VerticalAlign.TOP;
        private TextOverflow overflow = TextOverflow.ELLIPSIS;
        private int lineSpacing;
        private List<TextSpan> spans = List.of();
        private Builder(int x, int y, int width, int height) { this.x=x; this.y=y; this.width=width; this.height=height; }
        public Builder background(Color v) { background=v; return this; }
        public Builder horizontalAlign(HorizontalAlign v) { horizontalAlign=v; return this; }
        public Builder verticalAlign(VerticalAlign v) { verticalAlign=v; return this; }
        public Builder overflow(TextOverflow v) { overflow=v; return this; }
        public Builder lineSpacing(int v) { lineSpacing=v; return this; }
        public Builder spans(List<TextSpan> v) { spans=v; return this; }
        public Builder span(TextSpan v) { spans=List.of(v); return this; }
        public TextBox build() { return new TextBox(x,y,width,height,background,horizontalAlign,verticalAlign,"word",overflow,lineSpacing,spans); }
    }
}
