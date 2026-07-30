package solutions.trp.jsonpaper;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

class PaperImageTest {
    @TempDir
    Path temporaryDirectory;

    @Test
    void packsTwoPixelsPerByteWithTransparentOddPixel() {
        BufferedImage image = new BufferedImage(3, 1, BufferedImage.TYPE_INT_ARGB);
        image.setRGB(0, 0, 0xff000000);
        image.setRGB(1, 0, 0xffff0000);
        image.setRGB(2, 0, 0x7fffffff);

        PaperImage converted = PaperImage.from(image);

        assertEquals(3, converted.width());
        assertEquals(1, converted.height());
        assertArrayEquals(new byte[]{0x03, 0x40}, converted.data());
        assertEquals("A0A=", converted.base64Data());
    }

    @Test
    void choosesTheNearestDisplayPaletteColor() {
        assertEquals(0, PaperImage.closestColorIndex(10, 10, 10));
        assertEquals(1, PaperImage.closestColorIndex(250, 250, 250));
        assertEquals(2, PaperImage.closestColorIndex(250, 240, 10));
        assertEquals(3, PaperImage.closestColorIndex(240, 20, 20));
    }

    @Test
    void imageCommandContainsDimensionsDataAndTransparencyIndex() {
        BufferedImage image = new BufferedImage(2, 1, BufferedImage.TYPE_INT_ARGB);
        image.setRGB(0, 0, 0xff000000);
        image.setRGB(1, 0, 0xffffffff);

        DisplayCommand command = new Display()
            .image(7, 8, image, Color.TRANSPARENT)
            .document()
            .commands()
            .get(0);

        assertEquals("draw_image", command.cmd());
        assertEquals(2, command.args().get("width"));
        assertEquals(1, command.args().get("height"));
        assertEquals("AQ==", command.args().get("data"));
        assertEquals(4, command.args().get("transparent"));
    }

    @Test
    void loadsSvgAtIntrinsicAndRequestedDimensions() throws IOException {
        Path svg = temporaryDirectory.resolve("image.svg");
        Files.writeString(svg, """
            <svg xmlns="http://www.w3.org/2000/svg" width="4" height="2">
              <rect width="2" height="2" fill="black"/>
              <rect x="2" width="2" height="2" fill="red"/>
            </svg>
            """);

        BufferedImage intrinsic = ImageLoader.load(svg);
        BufferedImage scaled = ImageLoader.load(svg, 8, 4);

        assertEquals(4, intrinsic.getWidth());
        assertEquals(2, intrinsic.getHeight());
        assertEquals(8, scaled.getWidth());
        assertEquals(4, scaled.getHeight());
    }
}
