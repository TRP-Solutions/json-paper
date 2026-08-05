# Device fonts

Drop TrueType (`.ttf`) files into this directory and rebuild.

Use these filename forms:

- `family-Regular.ttf`
- `family-Bold.ttf`
- `family.ttf` (regular only)

The family sent over JSON is the lowercase filename family. For example,
`News-Sans-Bold.ttf` is selected with `"family": "news-sans"` and
`"weight": "bold"`.

The build embeds every TTF in firmware flash. Subset large fonts before adding
them, and include their redistribution license in this directory.
