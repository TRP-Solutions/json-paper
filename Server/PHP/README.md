# JSON Drawing Protocol

A lightweight JSON-based drawing command protocol for rendering graphics, shapes, text, and images on e-paper

## Overview

The protocol consists of a JSON object with:

- `version` - protocol version
- `commands` - ordered list of drawing commands executed sequentially

Later commands overwrite earlier ones unless transparency is used.

---

## Root Structure

```json
{
  "version": "1.0",
  "commands": []
}
```

---

# Color Palette

Internally indexed colors:

| Index | Color |
|---|---|
| 0 | black |
| 1 | white |
| 2 | yellow |
| 3 | red |
| 4 | transparent *(reserved)* |

## Notes

- Commands execute in order
- Later commands overwrite previous pixels
- Transparency only applies to `draw_image`
- Drawing outside display bounds may be clipped

---

# Commands

## clear

Clears the entire display with a solid color.

### Arguments

| Name | Type | Description |
|---|---|---|
| color | string | `black`, `white`, `yellow`, `red` |

### Example

```json
{
  "cmd": "clear",
  "args": {
    "color": "white"
  }
}
```

---

## clear_window

Fills a rectangular region with a color.

### Arguments

| Name | Type | Description |
|---|---|---|
| x_start | int | Left coordinate |
| y_start | int | Top coordinate |
| x_end | int | Right coordinate |
| y_end | int | Bottom coordinate |
| color | string | `white`, `black`, `red`, `yellow` |

### Notes

- Rectangle includes both start and end coordinates
- Coordinates are relative to top left `(0,0)`

### Example

```json
{
  "cmd": "clear_window",
  "args": {
    "x_start": 10,
    "y_start": 10,
    "x_end": 40,
    "y_end": 60,
    "color": "red"
  }
}
```

---

## draw_point

Draws a single pixel or block.

### Arguments

| Name | Type | Description |
|---|---|---|
| x | int | X position |
| y | int | Y position |
| color | string | `white`, `black`, `red`, `yellow` |
| width | string | `1x1`, `2x2`, `3x3`, `4x4` |
| style | string | `around`, `rightup` |

### Styles

| Style | Description |
|---|---|
| around | Centered / symmetric block |
| rightup | Biased toward top-right |

### Example

```json
{
  "cmd": "draw_point",
  "args": {
    "x": 10,
    "y": 80,
    "color": "black",
    "width": "1x1",
    "style": "around"
  }
}
```

---

## draw_line

Draws a line between two points.

### Arguments

| Name | Type | Description |
|---|---|---|
| x_start | int | Start X |
| y_start | int | Start Y |
| x_end | int | End X |
| y_end | int | End Y |
| color | string | `white`, `black`, `red`, `yellow` |
| width | string | Line thickness |
| style | string | `solid`, `dotted` |

### Example

```json
{
  "cmd": "draw_line",
  "args": {
    "x_start": 20,
    "y_start": 70,
    "x_end": 70,
    "y_end": 120,
    "color": "black",
    "width": "1x1",
    "style": "solid"
  }
}
```

---

## draw_rectangle

Draws a rectangle.

### Arguments

| Name | Type | Description |
|---|---|---|
| x_start | int | Top-left X |
| y_start | int | Top-left Y |
| x_end | int | Bottom-right X |
| y_end | int | Bottom-right Y |
| color | string | `white`, `black`, `red`, `yellow` |
| width | string | Border thickness |
| fill | string | `empty`, `full` |

### Example

```json
{
  "cmd": "draw_rectangle",
  "args": {
    "x_start": 20,
    "y_start": 70,
    "x_end": 70,
    "y_end": 120,
    "color": "yellow",
    "width": "1x1",
    "fill": "empty"
  }
}
```

---

## draw_circle

Draws a circle.

### Arguments

| Name | Type | Description |
|---|---|---|
| x | int | Center X |
| y | int | Center Y |
| radius | int | Circle radius |
| color | string | `white`, `black`, `red`, `yellow` |
| width | string | Border thickness |
| fill | string | `empty`, `full` |

### Example

```json
{
  "cmd": "draw_circle",
  "args": {
    "x": 45,
    "y": 95,
    "radius": 20,
    "color": "red",
    "width": "1x1",
    "fill": "empty"
  }
}
```

---

## draw_string

Draws a text string.

### Arguments

| Name | Type | Description |
|---|---|---|
| x | int | X position |
| y | int | Y position |
| text | string | Text content |
| font | string | `font12`, `font16` |
| foreground | string | `white`, `black`, `red`, `yellow` |
| background | string | `white`, `black`, `red`, `yellow`, `transparent` |

### Example

```json
{
  "cmd": "draw_string",
  "args": {
    "x": 10,
    "y": 10,
    "text": "Red, yellow, white and black",
    "font": "font16",
    "foreground": "black",
    "background": "transparent"
  }
}
```

---

## draw_num

Optimized numeric rendering command.

### Arguments

| Name | Type | Description |
|---|---|---|
| x | int | X position |
| y | int | Y position |
| num | int | Numeric value |
| font | string | `font12`, `font16` |
| foreground | string | `white`, `black`, `red`, `yellow` |
| background | string | `white`, `black`, `red`, `yellow` |

### Example

```json
{
  "cmd": "draw_num",
  "args": {
    "x": 10,
    "y": 50,
    "num": 123456,
    "font": "font12",
    "foreground": "black",
    "background": "yellow"
  }
}
```

---

## draw_image

Draws a packed indexed image.

### Palette Values

| Value | Meaning |
|---|---|
| 0–3 | Palette colors |
| 4 | Transparent |

### Arguments

| Name | Type | Description |
|---|---|---|
| x | int | Top-left X |
| y | int | Top-left Y |
| width | int | Image width |
| height | int | Image height |
| data | string | Base64 encoded pixel data |

### Notes

- Transparent pixels are skipped during rendering

### Example

```json
{
  "cmd": "draw_image",
  "args": {
    "x": 100,
    "y": 0,
    "width": 100,
    "height": 100,
    "data": "BASE64_DATA"
  }
}
```

---

# Example Full Payload

```json
{
  "version": "1.0",
  "commands": [
    {
      "cmd": "clear",
      "args": {
        "color": "white"
      }
    },
    {
      "cmd": "draw_string",
      "args": {
        "x": 10,
        "y": 10,
        "text": "Hello world",
        "font": "font16",
        "foreground": "black",
        "background": "transparent"
      }
    }
  ]
}
```