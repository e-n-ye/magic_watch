#!/usr/bin/env python3
from __future__ import annotations

import argparse
import base64
import json
import mimetypes
import os
from pathlib import Path
from typing import Any
from urllib import request


def workspace_root() -> Path:
    return Path(__file__).resolve().parents[4]


def load_env_file(path: Path) -> dict[str, str]:
    values: dict[str, str] = {}
    if not path.exists():
        return values
    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        values[key.strip()] = value.strip()
    return values


def resolve_config(config_path: str | None = None) -> dict[str, str]:
    default_path = workspace_root() / ".agents" / "config" / "right_code.env"
    env_values = load_env_file(Path(config_path) if config_path else default_path)
    merged = dict(env_values)
    for key in [
        "RIGHT_CODE_BASE_URL",
        "RIGHT_CODE_API_KEY",
        "RIGHT_CODE_CHAT_MODEL",
        "RIGHT_CODE_IMAGE_MODEL",
    ]:
        if os.getenv(key):
            merged[key] = os.getenv(key, "")
    return merged


def require_config(config: dict[str, str], key: str) -> str:
    value = config.get(key, "").strip()
    if not value:
        raise SystemExit(f"Missing required config: {key}")
    return value


def read_text_file(path: str | None) -> str | None:
    if path is None:
        return None
    return Path(path).read_text(encoding="utf-8")


def ensure_output_dir(topic: str) -> Path:
    output_dir = workspace_root() / ".agents" / "generated" / topic
    output_dir.mkdir(parents=True, exist_ok=True)
    return output_dir


def local_image_to_data_url(path: Path) -> str:
    mime_type = mimetypes.guess_type(path.name)[0] or "application/octet-stream"
    payload = base64.b64encode(path.read_bytes()).decode("ascii")
    return f"data:{mime_type};base64,{payload}"


def normalize_image_ref(value: str) -> str:
    if value.startswith("http://") or value.startswith("https://") or value.startswith("data:"):
        return value
    path = Path(value)
    if not path.exists():
        raise SystemExit(f"Image reference not found: {value}")
    return local_image_to_data_url(path)


def http_post_json(url: str, payload: dict[str, Any], api_key: str) -> dict[str, Any]:
    body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
    req = request.Request(
        url,
        data=body,
        headers={
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json",
        },
        method="POST",
    )
    with request.urlopen(req, timeout=120) as response:
        return json.loads(response.read().decode("utf-8"))


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")


def add_common_args(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--config", help="Path to right_code.env. Defaults to .agents/config/right_code.env")
    parser.add_argument("--prompt", help="Inline prompt text")
    parser.add_argument("--prompt-file", help="UTF-8 prompt file path")
    parser.add_argument("--topic", default="manual-run", help="Output topic folder name")


def resolve_prompt(args: argparse.Namespace) -> str:
    prompt_parts = []
    if args.prompt:
        prompt_parts.append(args.prompt.strip())
    if args.prompt_file:
        prompt_parts.append(read_text_file(args.prompt_file) or "")
    prompt = "\n\n".join(part for part in prompt_parts if part).strip()
    if not prompt:
        raise SystemExit("Prompt is required. Use --prompt or --prompt-file.")
    return prompt
