<?php
require_once "lib/image.php";

$commands = [
	"commands" => [],
	"version" => "1.0"
];

$commands["commands"][] = [
	"cmd" => "clear",
	"args" => ["color" => "white"]
];

$commands["commands"][] = [
	"cmd" => "draw_string",
	"args" => [
		"x" => 10,
		"y" => 10,
		"text" => "This is a test string",
		"font" => "font16",
		"foreground" => "black",
		"background" => "red"
	]
];


header('Content-Type: application/json');
echo json_encode($commands);