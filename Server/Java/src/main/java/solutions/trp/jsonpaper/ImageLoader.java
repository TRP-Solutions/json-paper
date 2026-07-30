package solutions.trp.jsonpaper;

import org.apache.batik.transcoder.TranscoderException;
import org.apache.batik.transcoder.TranscoderInput;
import org.apache.batik.transcoder.TranscoderOutput;
import org.apache.batik.transcoder.image.ImageTranscoder;

import javax.imageio.ImageIO;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Locale;
import java.util.Objects;

public final class ImageLoader {
    private ImageLoader() {
    }

    public static BufferedImage load(Path path) throws IOException {
        Objects.requireNonNull(path, "path");
        if (isSvg(path)) {
            return loadSvg(path, null, null);
        }

        BufferedImage image = ImageIO.read(path.toFile());
        if (image == null) {
            throw new IOException("Unsupported or invalid image: " + path);
        }
        return image;
    }

    public static BufferedImage load(Path path, int width, int height) throws IOException {
        requirePositiveDimensions(width, height);
        Objects.requireNonNull(path, "path");

        if (isSvg(path)) {
            return loadSvg(path, width, height);
        }

        BufferedImage source = load(path);
        BufferedImage scaled = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D graphics = scaled.createGraphics();
        try {
            graphics.setRenderingHint(
                RenderingHints.KEY_INTERPOLATION,
                RenderingHints.VALUE_INTERPOLATION_BILINEAR
            );
            graphics.drawImage(source, 0, 0, width, height, null);
        } finally {
            graphics.dispose();
        }
        return scaled;
    }

    private static BufferedImage loadSvg(Path path, Integer width, Integer height)
        throws IOException {

        BufferedImageTranscoder transcoder = new BufferedImageTranscoder();
        if (width != null && height != null) {
            transcoder.addTranscodingHint(ImageTranscoder.KEY_WIDTH, width.floatValue());
            transcoder.addTranscodingHint(ImageTranscoder.KEY_HEIGHT, height.floatValue());
        }

        try {
            transcoder.transcode(
                new TranscoderInput(path.toUri().toString()),
                new TranscoderOutput()
            );
        } catch (TranscoderException exception) {
            throw new IOException("Could not render SVG: " + path, exception);
        }

        BufferedImage image = transcoder.image();
        if (image == null) {
            throw new IOException("SVG produced no image: " + path);
        }
        return image;
    }

    private static boolean isSvg(Path path) {
        String name = path.getFileName().toString().toLowerCase(Locale.ROOT);
        return name.endsWith(".svg");
    }

    private static void requirePositiveDimensions(int width, int height) {
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("Image dimensions must be positive");
        }
    }

    private static final class BufferedImageTranscoder extends ImageTranscoder {
        private BufferedImage image;

        @Override
        public BufferedImage createImage(int width, int height) {
            return new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        }

        @Override
        public void writeImage(BufferedImage image, TranscoderOutput output) {
            this.image = image;
        }

        private BufferedImage image() {
            return image;
        }
    }
}
