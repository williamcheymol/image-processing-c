# Image Processing in C

![C](https://img.shields.io/badge/C-99-blue)
![Status](https://img.shields.io/badge/status-complete-success)

A pixel-level image processing library written in C — no external dependencies. Handles PPM (color) and PGM (grayscale) binary formats end-to-end: file I/O, photometric operations, LUT-based filters, and two resampling methods.

Built as a first-year project at **ENSIIE** (2024–2025). Full report (in French): [`Rapport.pdf`](./Rapport.pdf).

---

## Project Structure

```
image-processing-c/
├── src/
│   ├── main.c          # Entry point — runs all 16 operations
│   ├── pixel.[c|h]     # Pixel-level getters/setters (PPM & PGM)
│   ├── pictures.[c|h]  # Core library — I/O, filters, resampling
│   ├── lut.[c|h]       # Look-Up Table engine
│   └── filename.[c|h]  # Path utilities (extension, name, dir)
└── images/
    ├── Lenna_color.ppm  # Test image — PPM color (512×512)
    ├── Lenna_gray.pgm   # Test image — PGM grayscale
    └── Lenna_BW.pgm     # Test image — PGM grayscale (mask)
```

---

## Data Structures

```c
// Single color pixel — R, G, B components
typedef struct { byte components[3]; } pixel_ppm;

// Single grayscale pixel — intensity byte
typedef struct { byte intensity; } pixel_pgm;

// Image container — works for both formats
typedef struct {
    unsigned int width, height, channels;  // channels = 1 (PGM) or 3 (PPM)
    void* data;                            // cast to pixel_ppm* or pixel_pgm*
} picture;

// Look-Up Table — 256-entry transfer function
struct lut { unsigned int taille; byte* table; };
```

---

## Build

```bash
cd src
make
```

Requires GCC. No external libraries.

---

## Usage

The executable takes **3 image paths** as arguments — all files must be in the same directory:

```bash
./pixel <color.ppm> <gray.pgm> <mask.pgm>
```

Example with the provided test images:
```bash
./pixel images/Lenna_color.ppm images/Lenna_gray.pgm images/Lenna_BW.pgm
```

This runs all 16 operations and writes the results alongside the input files.

---

## Operations

| # | Output file suffix | Description |
|---|---|---|
| 1 | `_convert_color.ppm` | PGM → PPM conversion |
| 2 | `_convert_gray.pgm` | PPM → PGM (BT.601 luminosity) |
| 3 | `_red / _green / _blue.pgm` | RGB channel split |
| 4 | `_brighten` | Multiply each channel by ×1.5, clamp 255 |
| 5 | `_melted` | Melt effect — bright pixels drip downward |
| 6 | `_inverse` | LUT inversion: `table[i] = 255 - i` |
| 7–8 | `_dynamic` | Normalize histogram to [0, 255] |
| 9 | `_levels` | Staircase quantization (8 levels color, 5 gray) |
| 10–11 | `_smaller_nearest / _bilinear` | Downscale ÷1.36 |
| 12–13 | `_larger_nearest / _bilinear` | Upscale ×1.36 |
| 14 | `_difference` | Diff between nearest and bilinear upscale |
| 15 | `_product` | Per-pixel multiplication with mask |
| 16 | `_mixture` | Alpha-blend with mask: `(1-α)·inverted + α·original` |

---

## Gallery

| Original | Inverted | Brightened ×1.6 | Levels n=4 |
|:---:|:---:|:---:|:---:|
| ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_color.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_color_inverted.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_color_bright.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_color_levels.png) |

| Original (gray) | Normalized | Melt (n=80k) | Diff (×3) |
|:---:|:---:|:---:|:---:|
| ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_gray.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_gray_normalized.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_melt.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_diff_visible.png) |

**Resampling — 4× upscale from 75px:**

| Nearest neighbour | Bilinear interpolation |
|:---:|:---:|
| ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_nearest.png) | ![](https://williamcheymol.github.io/assets/imageprocessing/lenna_bilinear.png) |
