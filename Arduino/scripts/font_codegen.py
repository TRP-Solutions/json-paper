#!/usr/bin/env python3
"""Generate a C++ registry from TTF files dropped into src/font/assets."""

from pathlib import Path
import re
import sys


def font_name(path: Path) -> tuple[str, str]:
    match = re.fullmatch(r"(.+?)-(regular|bold)", path.stem, re.IGNORECASE)
    family, weight = match.groups() if match else (path.stem, "regular")
    family = re.sub(r"[^a-z0-9]+", "-", family.lower()).strip("-")
    if not family:
        raise ValueError(f"font filename has no usable family name: {path.name}")
    return family, weight.lower()


def identifier(path: Path, index: int) -> str:
    stem = re.sub(r"[^A-Za-z0-9_]", "_", path.stem)
    return f"json_paper_font_{index}_{stem}"


def generate(project: Path) -> None:
    font_dir = project / "src" / "font"
    assets = sorted((font_dir / "assets").glob("*.ttf"), key=lambda p: p.name.lower())
    output = font_dir / "generated_font_registry.h"
    entries: list[tuple[str, str, str, int]] = []
    chunks = [
        "#pragma once\n",
        "#include <cstddef>\n",
        "\n",
        "struct JsonPaperFontAsset {\n",
        "    const char* family;\n    const char* weight;\n",
        "    const unsigned char* data;\n    size_t size;\n};\n\n",
    ]
    for index, path in enumerate(assets):
        family, weight = font_name(path)
        symbol = identifier(path, index)
        data = path.read_bytes()
        chunks.append(f"static const unsigned char {symbol}[] = {{\n")
        for start in range(0, len(data), 16):
            row = ", ".join(f"0x{byte:02x}" for byte in data[start:start + 16])
            chunks.append(f"    {row},\n")
        chunks.append("};\n\n")
        entries.append((family, weight, symbol, len(data)))

    if entries:
        chunks.append("static const JsonPaperFontAsset JSON_PAPER_FONT_STORAGE[] = {\n")
        for family, weight, symbol, size in entries:
            chunks.append(f'    {{"{family}", "{weight}", {symbol}, {size}}},\n')
        chunks.append("};\n")
        chunks.append("static const JsonPaperFontAsset* JSON_PAPER_FONTS = JSON_PAPER_FONT_STORAGE;\n")
        chunks.append("static constexpr size_t JSON_PAPER_FONT_COUNT = "
                      "sizeof(JSON_PAPER_FONT_STORAGE) / sizeof(JSON_PAPER_FONT_STORAGE[0]);\n")
    else:
        chunks.append("static const JsonPaperFontAsset* JSON_PAPER_FONTS = nullptr;\n")
        chunks.append("static constexpr size_t JSON_PAPER_FONT_COUNT = 0;\n")
    content = "".join(chunks)
    if not output.exists() or output.read_text() != content:
        output.write_text(content)
        print(f"Generated {output} with {len(entries)} font mappings")


if __name__ == "__main__":
    root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(__file__).resolve().parents[1]
    generate(root)
