# Right Code API Notes

Base URL:

- `https://www.right.codes/draw`

## Chat Completions

Endpoint:

- `POST /v1/chat/completions`

Headers:

- `Authorization: Bearer sk-xxxxx`
- `Content-Type: application/json`

Use for:

- pure text reasoning
- image understanding
- turning screenshots into structured design guidance

Important response fields:

- `choices[0].message.content`
- `usage.prompt_tokens`
- `usage.completion_tokens`
- `usage.total_tokens`

Image understanding message shape:

```json
[
  {
    "role": "user",
    "content": [
      {"type": "text", "text": "这张图的布局骨架是什么"},
      {"type": "image_url", "image_url": {"url": "https://example.com/demo.png"}}
    ]
  }
]
```

## Image Generations

Endpoint:

- `POST /v1/images/generations`

Headers:

- `Authorization: Bearer sk-xxxxx`
- `Content-Type: application/json`

Typical body:

```json
{
  "model": "gpt-image-2",
  "prompt": "生成一张适合圆形智能手表表盘的天气快捷页视觉参考图",
  "image": [],
  "size": "1024x1024",
  "response_format": "url"
}
```

Important response fields:

- `data[0].url`
- `usage.total_tokens`

## Project Guidance

- prefer one surface per generation round
- keep round-display readability explicit in the prompt
- use chat analysis first when the inspiration image is strong but hard to verbalize
