from __future__ import annotations

from pathlib import Path
from textwrap import dedent

from PIL import Image, ImageDraw, ImageFont


WORKSPACE = Path(__file__).resolve().parents[1]
SVG_DIR = WORKSPACE / "assets" / "watchface_digits" / "diffusion_svg"
PNG_DIR = WORKSPACE / "assets" / "watchface_digits" / "diffusion_png"
FONT_PATH = Path(r"C:\Windows\Fonts\bahnschrift.ttf")
CANVAS = (220, 220)
FONT_SIZE = 182
FILL = "#F5F7FB"


def ensure_dirs() -> None:
    SVG_DIR.mkdir(parents=True, exist_ok=True)
    PNG_DIR.mkdir(parents=True, exist_ok=True)


def svg_markup(digit: str) -> str:
    return dedent(
        f"""\
        <svg xmlns="http://www.w3.org/2000/svg" width="{CANVAS[0]}" height="{CANVAS[1]}" viewBox="0 0 {CANVAS[0]} {CANVAS[1]}">
          <rect width="100%" height="100%" fill="none"/>
          <text
            x="50%"
            y="50%"
            text-anchor="middle"
            dominant-baseline="central"
            fill="{FILL}"
            font-family="Bahnschrift, Segoe UI, Arial, sans-serif"
            font-size="{FONT_SIZE}"
            font-weight="700"
            letter-spacing="0">{digit}</text>
        </svg>
        """
    )


def render_png(digit: str) -> Image.Image:
    image = Image.new("RGBA", CANVAS, (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    font = ImageFont.truetype(str(FONT_PATH), FONT_SIZE)
    bbox = draw.textbbox((0, 0), digit, font=font)
    width = bbox[2] - bbox[0]
    height = bbox[3] - bbox[1]
    x = (CANVAS[0] - width) / 2 - bbox[0]
    y = (CANVAS[1] - height) / 2 - bbox[1] - 6
    draw.text((x, y), digit, font=font, fill=FILL)
    return image


def main() -> None:
    ensure_dirs()
    for digit in range(10):
        value = str(digit)
        (SVG_DIR / f"{digit}.svg").write_text(svg_markup(value), encoding="utf-8")
        render_png(value).save(PNG_DIR / f"{digit}.png")
    print(f"Generated diffusion watchface digits in {SVG_DIR} and {PNG_DIR}")


if __name__ == "__main__":
    main()
