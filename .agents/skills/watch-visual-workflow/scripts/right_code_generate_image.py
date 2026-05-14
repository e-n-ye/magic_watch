#!/usr/bin/env python3
from __future__ import annotations

import argparse
import base64
from datetime import datetime
from pathlib import Path
from urllib import request

from right_code_common import (
    add_common_args,
    ensure_output_dir,
    http_post_json,
    normalize_image_ref,
    require_config,
    resolve_config,
    resolve_prompt,
    write_json,
)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate Magic Watch visual references with Right Code images API.")
    add_common_args(parser)
    parser.add_argument("--model", help="Override image model")
    parser.add_argument("--size", default="1024x1024", help="Image size, for example 1024x1024")
    parser.add_argument("--response-format", default="b64_json", help="Response format, default b64_json")
    parser.add_argument(
        "--reference",
        action="append",
        default=[],
        help="Reference image path, URL or data URL. Repeatable.",
    )
    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    config = resolve_config(args.config)

    base_url = require_config(config, "RIGHT_CODE_BASE_URL").rstrip("/")
    api_key = require_config(config, "RIGHT_CODE_API_KEY")
    model = args.model or config.get("RIGHT_CODE_IMAGE_MODEL", "gpt-image-2")
    prompt = resolve_prompt(args)
    images = [normalize_image_ref(item) for item in args.reference]

    payload = {
        "model": model,
        "prompt": prompt,
        "image": images,
        "size": args.size,
        "response_format": args.response_format,
    }

    response = http_post_json(f"{base_url}/v1/images/generations", payload, api_key)
    output_dir = ensure_output_dir(args.topic)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    result_path = output_dir / f"{stamp}_image_generation.json"
    write_json(result_path, {"request": payload, "response": response})

    image_url = ""
    image_bytes = b""
    data = response.get("data", [])
    if data and isinstance(data[0], dict):
        image_url = str(data[0].get("url", ""))
        b64_image = str(data[0].get("b64_json", ""))
        if b64_image:
            image_bytes = base64.b64decode(b64_image)
        elif image_url:
            with request.urlopen(image_url, timeout=120) as response:
                image_bytes = response.read()

    image_path: Path | None = None
    latest_json_path = output_dir / "latest.json"
    latest_json_path.write_text(result_path.read_text(encoding="utf-8"), encoding="utf-8")

    if image_bytes:
        image_path = output_dir / f"{stamp}_image_generation.png"
        image_path.write_bytes(image_bytes)
        (output_dir / "latest.png").write_bytes(image_bytes)

    print(f"Saved response: {result_path}")
    if image_path is not None:
        print(f"Saved image: {image_path}")
    if image_url:
        print(f"Image URL: {image_url}")


if __name__ == "__main__":
    main()
