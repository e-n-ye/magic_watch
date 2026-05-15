const basePreset = {
  watchWidth: 240,
  watchHeight: 296,
  safeInsetX: 10,
  safeInsetTop: 20,
  safeInsetBottom: 16,
  layout: {
    batteryChip: { x: 70, y: 12, width: 80, height: 30, fontSize: 13 },
    hourStage: { x: 10, y: 58, width: 220, height: 168 },
    minuteLabel: { x: 62, y: 236, width: 116, height: 34, fontSize: 34 },
    digitImage: { x: 29, y: 0, width: 162, height: 162 },
    emptyLabel: { x: 76, y: 42, width: 68, height: 54, fontSize: 52 }
  }
};

function scaleRect(rect, xRatio, yRatio) {
  const scaled = {
    x: Number((rect.x * xRatio).toFixed(2)),
    y: Number((rect.y * yRatio).toFixed(2)),
    width: Number((rect.width * xRatio).toFixed(2)),
    height: Number((rect.height * yRatio).toFixed(2))
  };
  if (typeof rect.fontSize === "number") {
    scaled.fontSize = Number((rect.fontSize * ((xRatio + yRatio) / 2)).toFixed(2));
  }
  return scaled;
}

const tallerPreset = (() => {
  const width = 252;
  const height = 312;
  const xRatio = width / basePreset.watchWidth;
  const yRatio = height / basePreset.watchHeight;
  const layout = {};
  for (const [key, value] of Object.entries(basePreset.layout)) {
    layout[key] = scaleRect(value, xRatio, yRatio);
  }
  return {
    watchWidth: width,
    watchHeight: height,
    safeInsetX: 11,
    safeInsetTop: 22,
    safeInsetBottom: 18,
    layout
  };
})();

const presets = {
  "xiaomi-197": basePreset,
  "taller-alt": tallerPreset
};

const state = {
  preset: "xiaomi-197",
  digit: 8,
  spreadIndex: 4,
  minuteText: "58",
  batteryPercent: 86
};

const root = document.documentElement;
const digitRow = document.getElementById("digit-row");
const spreadSlider = document.getElementById("spread-slider");
const spreadValue = document.getElementById("spread-value");
const minuteInput = document.getElementById("minute-input");
const batteryInput = document.getElementById("battery-input");
const exportArea = document.getElementById("spec-export");
const batteryChip = document.getElementById("battery-chip");
const batteryText = document.getElementById("battery-text");
const hourStage = document.getElementById("hour-stage");
const minuteLabel = document.getElementById("minute-label");
const hourEmpty = document.getElementById("hour-empty");
const layerCore = document.getElementById("layer-core");
const layerLeftOuter = document.getElementById("layer-left-outer");
const layerLeftInner = document.getElementById("layer-left-inner");
const layerRightInner = document.getElementById("layer-right-inner");
const layerRightOuter = document.getElementById("layer-right-outer");

const layerEntries = [
  { key: "leftOuter", node: layerLeftOuter, direction: -1, blur: 6, width: 30 },
  { key: "leftInner", node: layerLeftInner, direction: -1, blur: 2, width: 40 },
  { key: "rightInner", node: layerRightInner, direction: 1, blur: 2, width: 40 },
  { key: "rightOuter", node: layerRightOuter, direction: 1, blur: 6, width: 30 }
];

function digitAsset(digit) {
  return `../../../../assets/watchface_digits/diffusion_svg/${digit}.svg`;
}

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function lerp(spreadIndex, min, max) {
  return min + ((max - min) * spreadIndex) / 8;
}

function setRect(node, rect) {
  node.style.left = `${rect.x}px`;
  node.style.top = `${rect.y}px`;
  node.style.width = `${rect.width}px`;
  node.style.height = `${rect.height}px`;
  if (typeof rect.fontSize === "number") {
    node.style.fontSize = `${rect.fontSize}px`;
  }
}

function ensureDigitButtons() {
  for (let digit = 0; digit <= 9; digit += 1) {
    const button = document.createElement("button");
    button.type = "button";
    button.className = "digit-button";
    button.dataset.digit = String(digit);
    button.textContent = String(digit);
    button.addEventListener("click", () => {
      state.digit = digit;
      render();
    });
    digitRow.appendChild(button);
  }
}

function updatePresetButtons() {
  document.querySelectorAll(".preset-button").forEach((button) => {
    button.classList.toggle("is-active", button.dataset.preset === state.preset);
  });
  document.querySelectorAll(".digit-button").forEach((button) => {
    button.classList.toggle("is-active", Number(button.dataset.digit) === state.digit);
  });
}

function diffusionLayerSpec(spreadIndex) {
  return {
    innerOffset: Number(lerp(spreadIndex, 34, 58).toFixed(2)),
    outerOffset: Number(lerp(spreadIndex, 62, 92).toFixed(2)),
    innerOpacity: Number(lerp(spreadIndex, 0.24, 0.78).toFixed(3)),
    outerOpacity: Number(lerp(spreadIndex, 0.16, 0.44).toFixed(3)),
    innerScale: Number(lerp(spreadIndex, 1.0, 1.16).toFixed(3)),
    outerScale: Number(lerp(spreadIndex, 1.0, 1.26).toFixed(3))
  };
}

function applyImage(node, src, rect, scale) {
  const image = node.querySelector("img");
  image.src = src;
  image.style.left = `${rect.x}px`;
  image.style.top = `${rect.y}px`;
  image.style.width = `${rect.width}px`;
  image.style.height = `${rect.height}px`;
  image.style.transform = `scale(${scale})`;
}

function renderHourLayers() {
  const preset = presets[state.preset];
  const layout = preset.layout;
  const stageRect = layout.hourStage;
  const digitRect = layout.digitImage;
  const emptyRect = layout.emptyLabel;
  const src = digitAsset(state.digit);
  const spreadSpec = diffusionLayerSpec(state.spreadIndex);

  setRect(hourStage, stageRect);
  setRect(hourEmpty, emptyRect);
  hourEmpty.style.display = "none";

  setRect(layerCore, {
    x: digitRect.x,
    y: digitRect.y,
    width: digitRect.width,
    height: digitRect.height
  });
  applyImage(layerCore, src, { x: 0, y: 0, width: digitRect.width, height: digitRect.height }, 1);

  layerEntries.forEach((entry) => {
    const isInner = entry.key.includes("Inner");
    const offset = isInner ? spreadSpec.innerOffset : spreadSpec.outerOffset;
    const opacity = isInner ? spreadSpec.innerOpacity : spreadSpec.outerOpacity;
    const scale = isInner ? spreadSpec.innerScale : spreadSpec.outerScale;
    const width = entry.width;
    const centerX = stageRect.width / 2 + entry.direction * offset;

    setRect(entry.node, {
      x: centerX - width / 2,
      y: 0,
      width,
      height: stageRect.height
    });
    entry.node.style.opacity = String(opacity);
    applyImage(entry.node, src, { x: digitRect.x - (centerX - width / 2), y: digitRect.y, width: digitRect.width, height: digitRect.height }, scale);
  });

  return spreadSpec;
}

function render() {
  const preset = presets[state.preset];
  const layout = preset.layout;

  root.style.setProperty("--watch-width", preset.watchWidth);
  root.style.setProperty("--watch-height", preset.watchHeight);
  root.style.setProperty("--safe-inset-x", preset.safeInsetX);
  root.style.setProperty("--safe-inset-top", preset.safeInsetTop);
  root.style.setProperty("--safe-inset-bottom", preset.safeInsetBottom);

  setRect(batteryChip, layout.batteryChip);
  batteryChip.style.fontSize = `${layout.batteryChip.fontSize}px`;
  batteryText.textContent = `${state.batteryPercent}%`;

  setRect(minuteLabel, layout.minuteLabel);
  minuteLabel.textContent = state.minuteText;

  const spreadSpec = renderHourLayers();
  spreadSlider.value = String(state.spreadIndex);
  spreadValue.textContent = String(state.spreadIndex);
  minuteInput.value = state.minuteText;
  batteryInput.value = String(state.batteryPercent);

  updatePresetButtons();
  exportArea.value = JSON.stringify(
    {
      preset: state.preset,
      watch: {
        width: preset.watchWidth,
        height: preset.watchHeight,
        safeInsetX: preset.safeInsetX,
        safeInsetTop: preset.safeInsetTop,
        safeInsetBottom: preset.safeInsetBottom
      },
      preview: {
        digit: state.digit,
        minuteText: state.minuteText,
        batteryPercent: state.batteryPercent,
        spreadIndex: state.spreadIndex
      },
      layout,
      diffusion: {
        leftOuter: {
          windowWidth: 30,
          offsetX: spreadSpec.outerOffset,
          opacity: spreadSpec.outerOpacity,
          scale: spreadSpec.outerScale,
          blurPx: 6
        },
        leftInner: {
          windowWidth: 40,
          offsetX: spreadSpec.innerOffset,
          opacity: spreadSpec.innerOpacity,
          scale: spreadSpec.innerScale,
          blurPx: 2
        },
        rightInner: {
          windowWidth: 40,
          offsetX: spreadSpec.innerOffset,
          opacity: spreadSpec.innerOpacity,
          scale: spreadSpec.innerScale,
          blurPx: 2
        },
        rightOuter: {
          windowWidth: 30,
          offsetX: spreadSpec.outerOffset,
          opacity: spreadSpec.outerOpacity,
          scale: spreadSpec.outerScale,
          blurPx: 6
        }
      }
    },
    null,
    2
  );
}

document.querySelectorAll(".preset-button").forEach((button) => {
  button.addEventListener("click", () => {
    state.preset = button.dataset.preset;
    render();
  });
});

spreadSlider.addEventListener("input", () => {
  state.spreadIndex = Number(spreadSlider.value);
  render();
});

minuteInput.addEventListener("input", () => {
  const next = minuteInput.value.replace(/[^\d-]/g, "").slice(0, 2);
  state.minuteText = next.length === 0 ? "--" : next;
  render();
});

batteryInput.addEventListener("input", () => {
  state.batteryPercent = clamp(Number(batteryInput.value || 0), 0, 100);
  render();
});

ensureDigitButtons();
render();
