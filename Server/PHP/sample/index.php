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
		10,
		10,
		80,
		60,
		Color::RED
	)

	->point(
		20,
		20,
		Color::BLACK,
		Width::W2,
		PointStyle::AROUND
	)

	->line(
		0,
		0,
		100,
		100,
		Color::BLACK,
		Width::W1,
		LineStyle::SOLID
	)

	->rectangle(
		20,
		70,
		80,
		80,
		Color::YELLOW,
		Width::W2,
		FillMode::EMPTY
	)

	->circle(
		160,
		80,
		30,
		Color::RED,
		Width::W1,
		FillMode::FULL
	)

	->text(
		10,
		100,
		"Hello World",
		Font::FONT_16,
		Color::BLACK,
		Color::TRANSPARENT
	)

	->image(
		10,
		150,
		$img,
		Color::TRANSPARENT
	);

$display->output();