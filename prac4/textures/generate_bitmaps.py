

from math import sqrt
from pathlib import Path
import struct


WIDTH = 256
HEIGHT = 256
ROWS = 14
COLS = 16
DIMPLE_RADIUS = 0.032
EDGE_SOFTNESS = 0.010


ROOT = Path(__file__).resolve().parent
COLOUR_PATH = ROOT / "colour" / "colour.bmp"
DISPLACEMENT_PATH = ROOT / "displacement" / "displacement.bmp"
ALPHA_PATH = ROOT / "alpha" / "alpha.bmp"


def clamp(value, low, high):
    return max(low, min(high, value))


def smoothstep(edge0, edge1, value):
    t = clamp((value - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def build_centres():
    centres = []
    vertical_step = 1.0 / float(ROWS)
    horizontal_step = 1.0 / float(COLS)

    for row in range(ROWS):
        v = (row + 0.5) * vertical_step
        offset = 0.5 * horizontal_step if (row % 2) == 1 else 0.0
        for col in range(COLS):
            u = (col + 0.5) * horizontal_step + offset
            if u >= 1.0:
                u -= 1.0
            centres.append((u, v))

    return centres


CENTRES = build_centres()


def wrapped_distance(u0, v0, u1, v1):
    du = abs(u0 - u1)
    dv = abs(v0 - v1)
    du = min(du, 1.0 - du)
    return sqrt(du * du + dv * dv)


def dimple_strength(u, v):
    nearest = 10.0
    for centre_u, centre_v in CENTRES:
        nearest = min(nearest, wrapped_distance(u, v, centre_u, centre_v))

    edge0 = DIMPLE_RADIUS - EDGE_SOFTNESS
    edge1 = DIMPLE_RADIUS + EDGE_SOFTNESS
    return 1.0 - smoothstep(edge0, edge1, nearest)


def shade_colour(mask):
    bright = 235
    dark = 115
    value = int(round(bright + (dark - bright) * mask))
    return value, value, value


def shade_displacement(mask):
    bright = 255
    dark = 20
    value = int(round(bright + (dark - bright) * mask))
    return value, value, value


def shade_alpha(mask):
    value = 0 if mask > 0.5 else 255
    return value, value, value


def write_bmp(path, width, height, pixels):
    path.parent.mkdir(parents=True, exist_ok=True)

    row_stride = width * 3
    padding = (4 - (row_stride % 4)) % 4
    image_size = (row_stride + padding) * height
    file_size = 54 + image_size

    with path.open("wb") as handle:
        handle.write(b"BM")
        handle.write(struct.pack("<I", file_size))
        handle.write(struct.pack("<HH", 0, 0))
        handle.write(struct.pack("<I", 54))

        handle.write(struct.pack("<I", 40))
        handle.write(struct.pack("<i", width))
        handle.write(struct.pack("<i", height))
        handle.write(struct.pack("<H", 1))
        handle.write(struct.pack("<H", 24))
        handle.write(struct.pack("<I", 0))
        handle.write(struct.pack("<I", image_size))
        handle.write(struct.pack("<i", 2835))
        handle.write(struct.pack("<i", 2835))
        handle.write(struct.pack("<I", 0))
        handle.write(struct.pack("<I", 0))

        pad = b"\x00" * padding
        for row in range(height - 1, -1, -1):
            start = row * width
            end = start + width
            for red, green, blue in pixels[start:end]:
                handle.write(bytes((blue, green, red)))
            handle.write(pad)


def build_pixels(shader):
    pixels = []
    for y in range(HEIGHT):
        v = (y + 0.5) / float(HEIGHT)
        for x in range(WIDTH):
            u = (x + 0.5) / float(WIDTH)
            mask = dimple_strength(u, v)
            pixels.append(shader(mask))
    return pixels


def main():
    colour_pixels = build_pixels(shade_colour)
    displacement_pixels = build_pixels(shade_displacement)
    alpha_pixels = build_pixels(shade_alpha)

    write_bmp(COLOUR_PATH, WIDTH, HEIGHT, colour_pixels)
    write_bmp(DISPLACEMENT_PATH, WIDTH, HEIGHT, displacement_pixels)
    write_bmp(ALPHA_PATH, WIDTH, HEIGHT, alpha_pixels)

    print("Wrote:")
    print(f"  {COLOUR_PATH}")
    print(f"  {DISPLACEMENT_PATH}")
    print(f"  {ALPHA_PATH}")


if __name__ == "__main__":
    main()
