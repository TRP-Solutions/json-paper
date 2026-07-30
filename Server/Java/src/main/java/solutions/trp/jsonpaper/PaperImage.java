package solutions.trp.jsonpaper;

import java.awt.image.BufferedImage;
import java.util.Arrays;
import java.util.Base64;
import java.util.Objects;

public final class PaperImage {
    private static final int[][] PALETTE = {
        {0, 0, 0},
        {255, 255, 255},
        {255, 255, 0},
        {255, 0, 0}
    };

    private final int width;
    private final int height;
    private final byte[] data;

    private PaperImage(int width, int height, byte[] data) {
        this.width = width;
        this.height = height;
        this.data = data;
    }

    public static PaperImage from(BufferedImage image) {
        Objects.requireNonNull(image, "image");

        int width = image.getWidth();
        int height = image.getHeight();
        byte[] data = new byte[(Math.multiplyExact(width, height) + 1) / 2];

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int argb = image.getRGB(x, y);
                int alpha = (argb >>> 24) & 0xff;
                int value = alpha < 128
                    ? Color.TRANSPARENT.paletteIndex()
                    : closestColorIndex(
                        (argb >>> 16) & 0xff,
                        (argb >>> 8) & 0xff,
                        argb & 0xff
                    );
                setPixel(data, y * width + x, value);
            }
        }

        return new PaperImage(width, height, data);
    }

    static int closestColorIndex(int red, int green, int blue) {
        int bestIndex = 0;
        long bestDistance = Long.MAX_VALUE;

        for (int index = 0; index < PALETTE.length; index++) {
            long redDifference = red - PALETTE[index][0];
            long greenDifference = green - PALETTE[index][1];
            long blueDifference = blue - PALETTE[index][2];
            long distance = redDifference * redDifference
                + greenDifference * greenDifference
                + blueDifference * blueDifference;

            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndex = index;
            }
        }

        return bestIndex;
    }

    private static void setPixel(byte[] data, int pixelIndex, int value) {
        int byteIndex = pixelIndex / 2;
        int current = data[byteIndex] & 0xff;

        if (pixelIndex % 2 == 0) {
            current = (current & 0x0f) | (value << 4);
        } else {
            current = (current & 0xf0) | value;
        }
        data[byteIndex] = (byte) current;
    }

    public int width() {
        return width;
    }

    public int height() {
        return height;
    }

    public byte[] data() {
        return Arrays.copyOf(data, data.length);
    }

    public String base64Data() {
        return Base64.getEncoder().encodeToString(data);
    }
}
