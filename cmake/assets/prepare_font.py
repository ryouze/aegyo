"""
Script: prepare_font.py

Create a C++ header file that contains the raw font data so that the font can be embedded in the executable.

**Note:** You can create a subset of the font file using `pyftsubset` to reduce the file size.

For example: `pyftsubset NanumGothic-Regular.ttf --output-file=NanumGothic-Regular-subset.ttf --unicodes="U+0020-007F,U+AC00-D7A3,U+1100-11FF,U+3130-318F" --layout-features="*"`.
"""

from pathlib import Path


def create_header(
    font_path: Path,
    header_path: Path,
) -> None:
    """
    Create a C++ header file that contains the raw font data.

    Args:
        font_path (Path): Input font file path (e.g., "NanumGothic-Regular-subset.ttf").
        header_path (Path): Output header file path (e.g., "font.hpp").
    """
    with font_path.open("rb") as font_file:
        font_data: bytes = font_file.read()

    with header_path.open("w", encoding="utf-8") as header_file:
        header_file.write("constexpr unsigned char font_data[] = { ")
        header_file.write(", ".join(f"0x{byte:02X}" for byte in font_data))
        header_file.write(" };\n")


if __name__ == "__main__":
    # Get path to this script's directory (regardless of CWD)
    this_directory: Path = Path(__file__).resolve().parent
    create_header(
        Path(this_directory / "NanumGothic-Regular-subset.ttf"),
        Path(this_directory / "font.hpp"),
    )
