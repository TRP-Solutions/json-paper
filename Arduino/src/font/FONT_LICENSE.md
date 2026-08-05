# Font licenses

No device font is bundled by default.

Place `.ttf` files and their redistribution licenses in `assets/`. Both the
ESP32 firmware and emulator embed those exact files through
`scripts/font_codegen.py`.

Large fonts should be subsetted before being added. For example:

```sh
pyftsubset MyFont-Regular.ttf \
  --unicodes='U+0020-007E,U+00A0-024F,U+2000-206F,U+20A0-20CF,U+FFFD' \
  --layout-features='kern' --output-file=assets/my-family-Regular.ttf
```
