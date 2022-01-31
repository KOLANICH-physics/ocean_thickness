#pragma once

enum class SmoothingMode: int{
	none = 0,
	tdigest = 3,
	ddSketch = 4,
	psq = 5,
	kll = 6,
	req = 7,
};
