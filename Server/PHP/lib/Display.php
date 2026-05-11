<?php

require_once __DIR__ . "/../lib/image.php";

enum Color: string
{
	case BLACK = "black";
	case WHITE = "white";
	case YELLOW = "yellow";
	case RED = "red";
	case TRANSPARENT = "transparent";
}

enum Font: string
{
	case FONT_8 = "font8";
	case FONT_12 = "font12";
	case FONT_16 = "font16";
	case FONT_20 = "font20";
	case FONT_24 = "font24";
}

enum Width: string
{
	case W1 = "1x1";
	case W2 = "2x2";
	case W3 = "3x3";
	case W4 = "4x4";
}

enum LineStyle: string
{
	case SOLID = "solid";
	case DOTTED = "dotted";
}

enum PointStyle: string
{
	case AROUND = "around";
	case RIGHTUP = "rightup";
}

enum FillMode: string
{
	case EMPTY = "empty";
	case FULL = "full";
}
class Display
{
	private array $commands = [];

	private function colorToIndex(
		Color $color
	): int {

		return match ($color) {
			Color::BLACK => 0,
			Color::WHITE => 1,
			Color::YELLOW => 2,
			Color::RED => 3,
			Color::TRANSPARENT => 4,
		};
	}

	public function clear(Color $color): self
	{
		$this->commands[] = [
			"cmd" => "clear",
			"args" => [
				"color" => $color
			]
		];

		return $this;
	}

	public function clearWindow(
		int $xStart,
		int $yStart,
		int $xEnd,
		int $yEnd,
		Color $color
	): self {

		$this->commands[] = [
			"cmd" => "clear_window",
			"args" => [
				"x_start" => $xStart,
				"y_start" => $yStart,
				"x_end" => $xEnd,
				"y_end" => $yEnd,
				"color" => $color
			]
		];

		return $this;
	}

	public function point(
		int $x,
		int $y,
		Color $color = Color::BLACK,
		Width $width = Width::W1,
		PointStyle $style = PointStyle::AROUND
	): self {

		$this->commands[] = [
			"cmd" => "draw_point",
			"args" => [
				"x" => $x,
				"y" => $y,
				"color" => $color,
				"width" => $width,
				"style" => $style
			]
		];

		return $this;
	}

	public function line(
		int $xStart,
		int $yStart,
		int $xEnd,
		int $yEnd,
		Color $color = Color::BLACK,
		Width $width = Width::W1,
		LineStyle $style = LineStyle::SOLID
	): self {

		$this->commands[] = [
			"cmd" => "draw_line",
			"args" => [
				"x_start" => $xStart,
				"y_start" => $yStart,
				"x_end" => $xEnd,
				"y_end" => $yEnd,
				"color" => $color,
				"width" => $width,
				"style" => $style
			]
		];

		return $this;
	}

	public function rectangle(
		int $xStart,
		int $yStart,
		int $xEnd,
		int $yEnd,
		Color $color = Color::BLACK,
		Width $width = Width::W1,
		FillMode $fill = FillMode::EMPTY
	): self {

		$this->commands[] = [
			"cmd" => "draw_rectangle",
			"args" => [
				"x_start" => $xStart,
				"y_start" => $yStart,
				"x_end" => $xEnd,
				"y_end" => $yEnd,
				"color" => $color,
				"width" => $width,
				"fill" => $fill
			]
		];

		return $this;
	}

	public function circle(
		int $x,
		int $y,
		int $radius,
		Color $color = Color::BLACK,
		Width $width = Width::W1,
		FillMode $fill = FillMode::EMPTY
	): self {

		$this->commands[] = [
			"cmd" => "draw_circle",
			"args" => [
				"x" => $x,
				"y" => $y,
				"radius" => $radius,
				"color" => $color,
				"width" => $width,
				"fill" => $fill
			]
		];

		return $this;
	}

	public function text(
		int $x,
		int $y,
		string $text,
		Font $font = Font::FONT_16,
		Color $foreground = Color::BLACK,
		Color $background = Color::TRANSPARENT
	): self {

		$this->commands[] = [
			"cmd" => "draw_string",
			"args" => [
				"x" => $x,
				"y" => $y,
				"text" => $text,
				"font" => $font,
				"foreground" => $foreground,
				"background" => $background
			]
		];

		return $this;
	}

	public function image(
		int $x,
		int $y,
		\Imagick $img,
		Color $transparent = Color::TRANSPARENT
	): self {

		$converted = new Image($img);

		$this->commands[] = [
			"cmd" => "draw_image",
			"args" => [
				"x" => $x,
				"y" => $y,
				"width" => $converted->width,
				"height" => $converted->height,
				"data" => base64_encode(
					$converted->getRawData()
				),
				"transparent" => $this->colorToIndex(
					$transparent
				)
			]
		];

		return $this;
	}

	public function array(): array
	{
		return [
			"version" => "1.0",
			"commands" => $this->commands
		];
	}

	public function json(
		int $flags = JSON_PRETTY_PRINT
	): string {

		return json_encode(
			$this->array(),
			$flags
		);
	}

	public function output(): void
	{
		header('Content-Type: application/json');

		echo $this->json();
	}
}
