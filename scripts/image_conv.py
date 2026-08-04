#!/usr/bin/env python3

import sys
import os
import struct
from PIL import Image

try:
    import cairosvg
    HAVE_SVG = True
except ImportError:
    HAVE_SVG = False


def round_up(v: int, align: int) -> int:
    if align <= 0:
        return v
    return (v + align - 1) // align * align


def load_image_rgba(path: str) -> Image.Image:
    lp = path.lower()
    if lp.endswith(".svg"):
        if not HAVE_SVG:
            sys.exit("SVG support requires: pip install cairosvg")
        png_bytes = cairosvg.svg2png(url=path)
        from io import BytesIO
        return Image.open(BytesIO(png_bytes)).convert("RGBA")

    return Image.open(path).convert("RGBA")


def convert_pixels_rgba_to_format(img: Image.Image, fmt_name: str) -> bytearray:
    w, h = img.size
    px = img.load()

    if fmt_name == "argb8888":
        out = bytearray(w * h * 4)
        i = 0
        for y in range(h):
            for x in range(w):
                r, g, b, a = px[x, y]
                out[i + 0] = b
                out[i + 1] = g
                out[i + 2] = r
                out[i + 3] = a
                i += 4
        return out

    if fmt_name == "a8":
        out = bytearray(w * h)
        i = 0
        for y in range(h):
            for x in range(w):
                out[i] = px[x, y][3]
                i += 1
        return out

    if fmt_name == "rgb565":
        out = bytearray(w * h * 2)
        i = 0
        for y in range(h):
            for x in range(w):
                r, g, b, _a = px[x, y]
                v = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
                out[i + 0] = v & 0xFF
                out[i + 1] = (v >> 8) & 0xFF
                i += 2
        return out

    raise ValueError("unknown format")


def pad_rows(raw: bytearray, width: int, height: int, bpp: int, stride_align: int):
    row_bytes = width * bpp
    stride = round_up(row_bytes, stride_align)
    out = bytearray(stride * height)

    src_i = 0
    dst_i = 0
    for _y in range(height):
        out[dst_i:dst_i + row_bytes] = raw[src_i:src_i + row_bytes]
        src_i += row_bytes
        dst_i += stride

    return out, stride


def output_c(name: str, fmt_sym: str, width: int, height: int, stride: int, data: bytearray):
    print('#include <stdint.h>')
    print('#include "ls_image.h"\n')

    print(f"static const uint8_t {name}_map[] = {{")

    idx = 0
    for _y in range(height):
        row = data[idx:idx + stride]
        print("    " + ", ".join(f"0x{b:02X}" for b in row) + ",")
        idx += stride

    print("};\n")

    print(f"const ls_image_dsc_t {name} = {{")
    print(f"    .color_format  = {fmt_sym},")
    print(f"    .width = {width},")
    print(f"    .height = {height},")
    print(f"    .stride = {stride},")
    print(f"    .data = {name}_map,")
    print("};")


def output_file(base: str, fmt_val: int, width: int, height: int, stride: int, data: bytearray):
    with open(f"{base}.pixels", "wb") as f:
        f.write(data)

    with open(f"{base}.meta", "wb") as f:
        f.write(struct.pack("<IIII", fmt_val, width, height, stride))


def main():
    if len(sys.argv) != 6:
        sys.exit(
            "usage: image_conv.py image.(png|jpg|jpeg|bmp|svg) "
            "argb8888|a8|rgb565 stride_align name c_output|file_output"
        )

    image_path = sys.argv[1]
    fmt_name = sys.argv[2].lower()
    stride_align = int(sys.argv[3])
    name = sys.argv[4]
    mode = sys.argv[5]

    FORMAT_MAP = {
        "a8":       ("LS_IMAGE_COLOR_FORMAT_A8",       0, 1),
        "argb8888": ("LS_IMAGE_COLOR_FORMAT_ARGB8888", 1, 4),
        "rgb565":   ("LS_IMAGE_COLOR_FORMAT_RGB565",   2, 2),
    }

    if fmt_name not in FORMAT_MAP:
        sys.exit("invalid color format")

    fmt_sym, fmt_val, bpp = FORMAT_MAP[fmt_name]

    img = load_image_rgba(image_path)
    width, height = img.size

    raw = convert_pixels_rgba_to_format(img, fmt_name)
    padded, stride = pad_rows(raw, width, height, bpp, stride_align)

    if mode == "c_output":
        output_c(name, fmt_sym, width, height, stride, padded)
    elif mode == "file_output":
        base = f"{os.path.basename(image_path)}"
        output_file(base, fmt_val, width, height, stride, padded)
    else:
        sys.exit("mode must be c_output or file_output")


if __name__ == "__main__":
    main()
