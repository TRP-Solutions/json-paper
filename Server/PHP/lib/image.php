<?php
const PALETTE = [
	[0, 0, 0],       // black
	[255, 255, 255], // white
	[255, 255, 0],   // yellow
	[255, 0, 0],     // red
];

class Image {
	public int $width;
	public int $height;

	private string $data;

	public function __construct(int $width, int $height) {
		$this->width = $width;
		$this->height = $height;

		$totalPixels = $width * $height;
		$totalBytes = (int) ceil($totalPixels / 2); // 2 pixels per byte

		$this->data = str_repeat("\0", $totalBytes);
	}

	private function getIndex(int $x, int $y): int {
		return $y * $this->width + $x;
	}

	public function setPixel(int $x, int $y, int $value): void {
		if ($value < 0 || $value > 15) {
			throw new \InvalidArgumentException("Pixel must be 0–15");
		}

		$index = $this->getIndex($x, $y);
		$byteIndex = intdiv($index, 2);

		$byte = ord($this->data[$byteIndex]);

		if ($index % 2 === 0) {
			// high nibble
			$byte = ($byte & 0x0F) | ($value << 4);
		} else {
			// low nibble
			$byte = ($byte & 0xF0) | $value;
		}

		$this->data[$byteIndex] = chr($byte);
	}

	public function getPixel(int $x, int $y): int {
		$index = $this->getIndex($x, $y);
		$byteIndex = intdiv($index, 2);

		$byte = ord($this->data[$byteIndex]);

		if ($index % 2 === 0) {
			return ($byte >> 4) & 0x0F;
		} else {
			return $byte & 0x0F;
		}
	}

	public function getRawData(): string {
		return $this->data;
	}

	public function toJSON(): array {
		return [
			'width' => $this->width,
			'height' => $this->height,
			'data' => base64_encode($this->data),
			'format' => '4bit'
		];
	}
}
function closestColorIndex(int $r, int $g, int $b): int {
	$bestIndex = 0;
	$bestDist = PHP_INT_MAX;

	foreach (PALETTE as $i => [$pr, $pg, $pb]) {
		$dr = $r - $pr;
		$dg = $g - $pg;
		$db = $b - $pb;

		$dist = $dr*$dr + $dg*$dg + $db*$db;

		if ($dist < $bestDist) {
			$bestDist = $dist;
			$bestIndex = $i;
		}
	}

	return $bestIndex;
}

/**
 * @throws ImagickException
 */
function loadImage(string $path, int $w, int $h): GdImage {
	$info = pathinfo($path);
	$ext = strtolower($info['extension'] ?? '');

	if ($ext === 'svg') {
		$imagick = new Imagick();

		$imagick->setResolution(72, 72);

		// Disable anti-aliasing
		$imagick->setOption('svg:antialias', 'false');
		$imagick->setOption('svg:shape-rendering', 'crispEdges');

		$imagick->readImage($path);

		$imagick->setBackgroundColor('transparent');
		$imagick->setImageAlphaChannel(Imagick::ALPHACHANNEL_ACTIVATE);

		$imagick->resizeImage($w, $h, Imagick::FILTER_POINT, 1);

		$imagick->setImageFormat('png');

		$blob = $imagick->getImageBlob();
		return imagecreatefromstring($blob);
	}

	$info = getimagesize($path);

	return match ($info[2]) {
		IMAGETYPE_JPEG => imagecreatefromjpeg($path),
		IMAGETYPE_PNG  => imagecreatefrompng($path),
		IMAGETYPE_WEBP => imagecreatefromwebp($path),
		default => throw new Exception("Unsupported format")
	};
}

function resizeImage(GdImage $src, int $w, int $h): GdImage {
	$dst = imagecreatetruecolor($w, $h);

	imagealphablending($dst, false);
	imagesavealpha($dst, true);

	$transparent = imagecolorallocatealpha($dst, 0, 0, 0, 127);
	imagefill($dst, 0, 0, $transparent);

	imagecopyresampled(
		$dst, $src,
		0, 0, 0, 0,
		$w, $h,
		imagesx($src), imagesy($src)
	);

	return $dst;
}

/**
 * @throws Exception
 */
function convertToIndexedImage(string $path, int $w, int $h): Image {
	$src = loadImage($path, $w, $h);
	$img = resizeImage($src, $w, $h);

	imagealphablending($img, false);
	imagesavealpha($img, true);

	$out = new Image($w, $h);

	for ($y = 0; $y < $h; $y++) {
		for ($x = 0; $x < $w; $x++) {

			$rgba = imagecolorat($img, $x, $y);

			$a = ($rgba & 0x7F000000) >> 24;

			if ($a > 100) {
				$out->setPixel($x, $y, 4);
				continue;
			}

			$r = ($rgba >> 16) & 0xFF;
			$g = ($rgba >> 8) & 0xFF;
			$b = $rgba & 0xFF;

			$index = closestColorIndex($r, $g, $b);

			$out->setPixel($x, $y, $index);
		}
	}

	imagedestroy($src);
	imagedestroy($img);

	return $out;
}
