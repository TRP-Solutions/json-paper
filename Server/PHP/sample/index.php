<?php
require_once __DIR__."/../lib/Display.php";

$display = new Display();

$img = new \Imagick();

$img->setResolution(72, 72);

$img->setOption('svg:antialias', 'false');
$img->setOption('svg:shape-rendering', 'crispEdges');

$img->setBackgroundColor(new \ImagickPixel('transparent'));

$img->readImage(__DIR__ . '/../../../Media/logo.svg');

$display
	->clear(Color::WHITE)

	->clearWindow(
		x_start: 10,
		y_start: 10,
		x_end: 80,
		y_end: 60,
		color: Color::RED
	)

	->point(
		x: 20,
		y: 20,
		color: Color::BLACK,
		width: Width::W2,
		style: PointStyle::AROUND
	)

	->line(
		x_start: 0,
		y_start: 0,
		x_end: 100,
		y_end: 100,
		color: Color::BLACK,
		width: Width::W1,
		style: LineStyle::SOLID
	)

	->rectangle(
		x_start: 20,
		y_start: 70,
		x_end: 80,
		y_end: 80,
		color: Color::YELLOW,
		width: Width::W2,
		fill: FillMode::EMPTY
	)

	->circle(
		x: 160,
		y: 80,
		radius: 30,
		color: Color::RED,
		width: Width::W1,
		fill: FillMode::FULL
	)

	->text(
		x: 10,
		y: 100,
		text: "Hello World",
		font: Font::FONT_16,
		foreground: Color::BLACK,
		background: Color::TRANSPARENT
	)

	->image(
		x: 10,
		Y: 150,
		img: $img,
		transparent: Color::TRANSPARENT
	);

$display->output();