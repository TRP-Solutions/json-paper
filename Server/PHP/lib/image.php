<?php

const PALETTE = [
	[0, 0, 0],         // black
	[255, 255, 255],   // white
	[255, 255, 0],     // yellow
	[255, 0, 0],       // red
];

class Image {
	public int $width;
	public int $height;

	private string $data;

	private function getIndex(int $x, int $y): int {
		return $y * $this->width + $x;
	}

	public function setPixel(int $x, int $y, int $value): void {
		if ($value < 0 || $value > 15) {
			throw new InvalidArgumentException("Pixel must be 0–15");
		}

		$index = $this->getIndex($x, $y);
		$byteIndex = intdiv($index, 2);

		$byte = ord($this->data[$byteIndex]);

		if ($index % 2 === 0) {
			$byte = ($byte & 0x0F) | ($value << 4);
		} else {
			$byte = ($byte & 0xF0) | $value;
		}

		$this->data[$byteIndex] = chr($byte);
	}

	public function getRawData(): string {
		return $this->data;
	}

	public static function closestColorIndex(int $r, int $g, int $b): int {
		$bestIndex = 0;
		$bestDist = PHP_INT_MAX;

		foreach (PALETTE as $i => [$pr, $pg, $pb]) {
			$dr = $r - $pr;
			$dg = $g - $pg;
			$db = $b - $pb;

			$dist = $dr * $dr + $dg * $dg + $db * $db;

			if ($dist < $bestDist) {
				$bestDist = $dist;
				$bestIndex = $i;
			}
		}

		return $bestIndex;
	}

	public function __construct(Imagick $img) {

		$this->width = $img->getImageWidth();
		$this->height = $img->getImageHeight();

		$totalPixels = $this->width * $this->height;
		$totalBytes = (int) ceil($totalPixels / 2);

		$this->data = str_repeat("\0", $totalBytes);

		$img = clone $img;

		$img->setImageAlphaChannel(Imagick::ALPHACHANNEL_ACTIVATE);

		for ($y = 0; $y < $this->height; $y++) {
			for ($x = 0; $x < $this->width; $x++) {

				$pixel = $img->getImagePixelColor($x, $y);

				$color = $pixel->getColor(true);

				$r = (int) round($color['r'] * 255);
				$g = (int) round($color['g'] * 255);
				$b = (int) round($color['b'] * 255);

				$a = $color['a'];

				if ($a < 0.5) {
					$this->setPixel($x, $y, 4);
					continue;
				}

				$index = self::closestColorIndex($r, $g, $b);

				$this->setPixel($x, $y, $index);
			}
		}
	}
}

