#!/usr/bin/env python3
from __future__ import annotations

import argparse
from datetime import datetime

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
    parser = argparse.ArgumentParser(description="Analyze watch references with Right Code chat completions API.")
    add_common_args(parser)
    parser.add_argument("--model", help="Override chat model")
    parser.add_argument("--image", action="append", default=[], help="Image path, URL or data URL. Repeatable.")
    parser.add_argument("--stream", action="store_true", help="Send stream=true. Response parsing still expects JSON.")
    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    config = resolve_config(args.config)

    base_url = require_config(config, "RIGHT_CODE_BASE_URL").rstrip("/")
    api_key = require_config(config, "RIGHT_CODE_API_KEY")
    model = args.model or config.get("RIGHT_CODE_CHAT_MODEL", "gemini-3.1-pro")
    prompt = resolve_prompt(args)

    if args.image:
        content = [{"type": "text", "text": prompt}]
        for item in args.image:
            content.append({"type": "image_url", "image_url": {"url": normalize_image_ref(item)}})
    else:
        content = prompt

    payload = {
        "model": model,
        "stream": bool(args.stream),
        "messages": [{"role": "user", "content": content}],
    }

    response = http_post_json(f"{base_url}/v1/chat/completions", payload, api_key)
    output_dir = ensure_output_dir(args.topic)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    result_path = output_dir / f"{stamp}_chat_analysis.json"
    write_json(result_path, {"request": payload, "response": response})

    message = ""
    choices = response.get("choices", [])
    if choices and isinstance(choices[0], dict):
        message = str(choices[0].get("message", {}).get("content", ""))

    print(f"Saved response: {result_path}")
    if message:
        print("Response:")
        print(message)


if __name__ == "__main__":
    main()
