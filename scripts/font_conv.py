#!/usr/bin/env python3

import sys
import freetype
import struct
import os
from typing import List, Tuple


class GlyphInfo:
    __slots__ = ("codepoint", "offset", "width", "height")

    def __init__(self, codepoint, offset, width, height):
        self.codepoint = codepoint
        self.offset = offset
        self.width = width
        self.height = height


def convert_font_to_a8(
    font_path: str,
    first_glyph: int,
    last_glyph: int,
    render_px: int,
) -> Tuple[bytearray, List[GlyphInfo], int, int]:
    face = freetype.Face(font_path)
    face.set_pixel_sizes(0, render_px)

    asc = face.size.ascender >> 6
    desc = -(face.size.descender >> 6)

    h_px = asc + desc
    base_line = asc

    bitmap_data = bytearray()
    glyphs: List[GlyphInfo] = []

    offset = 0

    for cp in range(first_glyph, last_glyph + 1):
        face.load_char(cp, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL)
        g = face.glyph
        bmp = g.bitmap

        width = bmp.width
        advance = g.advance.x >> 6
        if width == 0 and advance > 0:
            width = advance

        bmp_top = g.bitmap_top
        src_h = bmp.rows

        pad_top = base_line - bmp_top
        if pad_top < 0:
            pad_top = 0
        if pad_top > h_px:
            pad_top = h_px

        rows_to_copy = h_px - pad_top
        if rows_to_copy > src_h:
            rows_to_copy = src_h
        if rows_to_copy < 0:
            rows_to_copy = 0

        pad_bottom = h_px - pad_top - rows_to_copy
        if pad_bottom < 0:
            pad_bottom = 0

        if width > 0:
            if pad_top:
                bitmap_data.extend(b"\x00" * (width * pad_top))

            if bmp.width > 0:
                pitch = bmp.pitch
                src = bmp.buffer
                for y in range(rows_to_copy):
                    row = src[y * pitch : y * pitch + bmp.width]
                    bitmap_data.extend(row)
                    if bmp.width < width:
                        bitmap_data.extend(b"\x00" * (width - bmp.width))
            else:
                bitmap_data.extend(b"\x00" * (width * rows_to_copy))

            if pad_bottom:
                bitmap_data.extend(b"\x00" * (width * pad_bottom))

            size = width * h_px
        else:
            size = 0

        glyphs.append(GlyphInfo(cp, offset, width, h_px))
        offset += size

    return bitmap_data, glyphs, h_px, base_line


# -------------------- C OUTPUT --------------------

def print_c_output(
    name: str,
    bitmap: bytearray,
    glyphs: List[GlyphInfo],
    h_px: int,
    base_line: int,
    first_glyph: int,
    last_glyph: int,
):
    print('#include "ls_font.h"\n')

    print(f"static const unsigned char {name}_glyph_bitmap[] = {{")
    for g in glyphs:
        print(f"    /* Unicode: U+{g.codepoint:04X} */")
        idx = g.offset
        if g.width > 0:
            for _ in range(g.height):
                row = bitmap[idx : idx + g.width]
                print("    " + ", ".join(f"0x{b:02X}" for b in row) + ",")
                idx += g.width
        print()
    print("};\n")

    print(f"static const ls_font_glyph_dsc_t {name}_glyph_dsc[] = {{\n")
    for g in glyphs:
        ch = chr(g.codepoint) if 32 <= g.codepoint < 127 else "?"
        print(
            f"    {{.w_px = {g.width},   .offset = {g.offset}}},"
            f"    /*Unicode: U+{g.codepoint:04X} ({ch})*/"
        )
    print("};\n")

    print(f"ls_font_t {name} = {{")
    print(f"    .glyph_id_first = {first_glyph},")
    print(f"    .glyph_id_last  = {last_glyph},")
    print(f"    .h_px           = {h_px},")
    print(f"    .base_line      = {base_line},")
    print(f"    .glyph_bitmap   = {name}_glyph_bitmap,")
    print(f"    .glyph_dsc      = {name}_glyph_dsc,")
    print(f"    .glyph_cnt      = {len(glyphs)},")
    print("};")


# -------------------- FILE OUTPUT --------------------

def write_file_output(
    base_name: str,
    bitmap: bytearray,
    glyphs: List[GlyphInfo],
    h_px: int,
    base_line: int,
    first_glyph: int,
    last_glyph: int,
):
    with open(f"{base_name}.bitmap", "wb") as f:
        f.write(bitmap)

    with open(f"{base_name}.glyphs", "wb") as f:
        for g in glyphs:
            f.write(struct.pack("<II", g.width, g.offset))

    with open(f"{base_name}.meta", "wb") as f:
        f.write(
            struct.pack(
                "<iiIII",
                first_glyph,
                last_glyph,
                h_px,
                base_line,
                len(glyphs),
            )
        )


# -------------------- MAIN --------------------

def main():
    if len(sys.argv) != 7:
        sys.exit(
            "usage: font_conv.py font.ttf first last render_px name c_output|file_output"
        )

    font_path = sys.argv[1]
    first_glyph = int(sys.argv[2])
    last_glyph = int(sys.argv[3])
    render_px = int(sys.argv[4])
    name = sys.argv[5]
    mode = sys.argv[6]
    font_file = os.path.basename(font_path)
    base_name = f"{font_file}.{render_px}"

    bitmap, glyphs, h_px, base_line = convert_font_to_a8(
        font_path, first_glyph, last_glyph, render_px
    )

    if mode == "c_output":
        print_c_output(
            name, bitmap, glyphs, h_px, base_line, first_glyph, last_glyph
        )
    elif mode == "file_output":
        write_file_output(
            base_name, bitmap, glyphs, h_px, base_line, first_glyph, last_glyph
        )
    else:
        sys.exit("mode must be: c_output or file_output")


if __name__ == "__main__":
    main()

