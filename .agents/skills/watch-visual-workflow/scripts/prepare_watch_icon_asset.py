#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Prepare small transparent watch icon assets from generated images.")
    parser.add_argument("--input", required=True, help="Input image path")
    parser.add_argument("--output", required=True, help="Output PNG path")
    parser.add_argument("--size", type=int, default=256, help="Output square canvas size")
    parser.add_argument("--trim", type=int, default=10, help="Alpha threshold when trimming content bounds")
    parser.add_argument("--padding", type=float, default=0.22, help="Relative padding inside output canvas")
    parser.add_argument("--key-color", default="#00ff00", help="Chroma-key background color")
    parser.add_argument("--force", action="store_true", help="Overwrite existing output")
    return parser.parse_args()


def remove_background(source: Path, target: Path, key_color: str) -> None:
    helper = Path.home() / ".codex" / "skills" / ".system" / "imagegen" / "scripts" / "remove_chroma_key.py"
    command = [
        sys.executable,
        str(helper),
        "--input",
        str(source),
        "--out",
        str(target),
        "--auto-key",
        "border",
        "--soft-matte",
        "--transparent-threshold",
        "12",
        "--opaque-threshold",
        "220",
        "--despill",
        "--edge-contract",
        "1",
        "--key-color",
        key_color,
        "--force",
    ]
    subprocess.run(command, check=True)


def normalize_canvas(source: Path, target: Path, size: int, trim: int, padding: float, force: bool) -> None:
    try:
        from PIL import Image
    except ImportError as exc:
        raise SystemExit("Pillow is required. Install it with `uv pip install pillow`.") from exc

    if target.exists() and not force:
        raise SystemExit(f"Output already exists: {target}")

    image = Image.open(source).convert("RGBA")
    alpha = image.getchannel("A")
    bbox = alpha.point(lambda value: 255 if value > trim else 0).getbbox()
    if bbox is None:
        raise SystemExit("No visible icon content found after background removal.")

    icon = image.crop(bbox)
    inner_size = max(1, int(round(size * (1.0 - padding * 2.0))))
    scale = min(inner_size / icon.width, inner_size / icon.height)
    resized = icon.resize(
        (max(1, int(round(icon.width * scale))), max(1, int(round(icon.height * scale)))),
        Image.LANCZOS,
    )

    canvas = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    offset = ((size - resized.width) // 2, (size - resized.height) // 2)
    canvas.alpha_composite(resized, offset)
    target.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(target)


def main() -> None:
    args = parse_args()
    source = Path(args.input)
    target = Path(args.output)
    temp_dir = Path("tmp") / "watch_icon_prepare"
    temp_dir.mkdir(parents=True, exist_ok=True)
    temp = temp_dir / f"{target.stem}.tmp_alpha.png"

    remove_background(source, temp, args.key_color)
    normalize_canvas(temp, target, args.size, args.trim, args.padding, args.force)
    temp.unlink(missing_ok=True)
    print(f"Prepared icon asset: {target}")


if __name__ == "__main__":
    main()
