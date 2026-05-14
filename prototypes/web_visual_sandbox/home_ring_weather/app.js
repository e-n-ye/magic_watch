const presets = {
  "xiaomi-197": {
    watchWidth: 240,
    watchHeight: 296,
    safeInsetX: 10,
    safeInsetTop: 20,
    safeInsetBottom: 16,
    stageRadius: 28,
    metricFontScale: 1,
    layout: {
      heroCard: { x: 0, y: 15.483870967741934, width: 220, height: 110 },
      temperature: { x: 18, y: 20.258064516129032, width: 85.29032258064515, height: 40.903225806451616, fontSize: 35.016129032258064 },
      range: { x: 21.870967741935484, y: 65.22580645161291, width: 73.29032258064515, height: 24, fontSize: 11.661290322580646 },
      weatherIcon: { x: 140.2258064516129, y: 17.032258064516128, width: 73, height: 73 },
      sleepCard: { x: 0.9677419354838709, y: 135.67741935483872, width: 106, height: 106 },
      stepsCard: { x: 114, y: 135.6774193548387, width: 106, height: 106 },
      sleepIcon: { x: 0, y: 0, width: 58.387096774193544, height: 58.387096774193544 },
      stepsIcon: { x: 0, y: 0, width: 58.387096774193544, height: 58.387096774193544 },
      sleepValue: { x: 12.129032258064516, y: 68.38709677419355, width: 71.41935483870968, height: 24, fontSize: 13.774193548387096 },
      stepsValue: { x: 12.451612903225808, y: 70.32258064516128, width: 75.09677419354838, height: 30.06451612903226, fontSize: 16.822580645161292 }
    }
  },
  "taller-alt": {
    watchWidth: 252,
    watchHeight: 312,
    safeInsetX: 11,
    safeInsetTop: 22,
    safeInsetBottom: 18,
    stageRadius: 29,
    metricFontScale: 1.02,
    layout: {
      heroCard: { x: 0, y: 16.32, width: 230, height: 116.12 },
      temperature: { x: 18.98, y: 21.36, width: 89.91, height: 43.67, fontSize: 36.9 },
      range: { x: 23.08, y: 69.14, width: 77.25, height: 24.96, fontSize: 12.29 },
      weatherIcon: { x: 146.49, y: 17.96, width: 76.65, height: 76.65 },
      sleepCard: { x: 1.02, y: 143.03, width: 111.3, height: 111.3 },
      stepsCard: { x: 119.7, y: 143.03, width: 111.3, height: 111.3 },
      sleepIcon: { x: 0, y: 0, width: 61.31, height: 61.31 },
      stepsIcon: { x: 0, y: 0, width: 61.31, height: 61.31 },
      sleepValue: { x: 12.74, y: 72.09, width: 74.99, height: 24.96, fontSize: 14.46 },
      stepsValue: { x: 13.07, y: 74.13, width: 78.85, height: 31.27, fontSize: 17.66 }
    }
  }
};

const editableNodes = {
  heroCard: document.getElementById("hero-card"),
  temperature: document.getElementById("temperature-node"),
  range: document.getElementById("range-node"),
  weatherIcon: document.getElementById("weather-icon-slot"),
  sleepCard: document.getElementById("sleep-card"),
  stepsCard: document.getElementById("steps-card"),
  sleepIcon: document.getElementById("sleep-icon-slot"),
  stepsIcon: document.getElementById("steps-icon-slot"),
  sleepValue: document.getElementById("sleep-value-node"),
  stepsValue: document.getElementById("steps-value-node")
};

const iconBindings = [
  {
    image: document.getElementById("weather-icon-image"),
    fallback: document.getElementById("weather-icon-fallback"),
    status: document.getElementById("weather-icon-status"),
    label: "天气"
  },
  {
    image: document.getElementById("sleep-icon-image"),
    fallback: document.getElementById("sleep-icon-fallback"),
    status: document.getElementById("sleep-icon-status"),
    label: "睡眠"
  },
  {
    image: document.getElementById("steps-icon-image"),
    fallback: document.getElementById("steps-icon-fallback"),
    status: document.getElementById("steps-icon-status"),
    label: "步数"
  }
];

const watchScreen = document.getElementById("watch-screen");
const surfaceStage = document.getElementById("surface-stage");
const layoutExport = document.getElementById("layout-export");
const editStatus = document.getElementById("edit-status");
const toggleEditButton = document.getElementById("toggle-edit-mode");
const resetLayoutButton = document.getElementById("reset-layout");
const root = document.documentElement;

let currentPresetName = "xiaomi-197";
let editModeEnabled = false;
let activePointer = null;
let layoutState = structuredClone(presets[currentPresetName].layout);

function setCssVar(name, value) {
  root.style.setProperty(name, value);
}

function cloneLayout(layout) {
  return JSON.parse(JSON.stringify(layout));
}

function updateSpecPanel(preset) {
  document.getElementById("spec-watch").textContent = `${preset.watchWidth} x ${preset.watchHeight}`;
  document.getElementById("spec-safe").textContent =
    `${preset.safeInsetX} / ${preset.safeInsetTop} / ${preset.safeInsetBottom}`;
  document.getElementById("spec-stage").textContent =
    `${preset.watchWidth - preset.safeInsetX * 2} x ${preset.watchHeight - preset.safeInsetTop - preset.safeInsetBottom}`;
  document.getElementById("spec-gap").textContent =
    `${(layoutState.stepsCard.x - (layoutState.sleepCard.x + layoutState.sleepCard.width)).toFixed(2)}px`;
  document.getElementById("spec-type").textContent = `${preset.metricFontScale.toFixed(2)}x`;
}

function applyPreset(name) {
  const preset = presets[name];
  if (!preset) {
    return;
  }

  currentPresetName = name;
  layoutState = cloneLayout(preset.layout);

  setCssVar("--watch-width", preset.watchWidth);
  setCssVar("--watch-height", preset.watchHeight);
  setCssVar("--safe-inset-x", preset.safeInsetX);
  setCssVar("--safe-inset-top", preset.safeInsetTop);
  setCssVar("--safe-inset-bottom", preset.safeInsetBottom);
  setCssVar("--stage-radius", preset.stageRadius);
  setCssVar("--metric-font-scale", preset.metricFontScale);

  renderLayout();
  updateSpecPanel(preset);

  document.querySelectorAll(".preset-button").forEach((button) => {
    button.classList.toggle("is-active", button.dataset.preset === name);
  });
}

function exportLayout() {
  layoutExport.value = JSON.stringify(
    {
      preset: currentPresetName,
      watch: {
        width: presets[currentPresetName].watchWidth,
        height: presets[currentPresetName].watchHeight,
        safeInsetX: presets[currentPresetName].safeInsetX,
        safeInsetTop: presets[currentPresetName].safeInsetTop,
        safeInsetBottom: presets[currentPresetName].safeInsetBottom
      },
      layout: layoutState
    },
    null,
    2
  );
}

function applyNodeRect(nodeName, rect) {
  const node = editableNodes[nodeName];
  if (!node) {
    return;
  }

  node.style.left = `${rect.x}px`;
  node.style.top = `${rect.y}px`;
  node.style.right = "auto";
  node.style.bottom = "auto";
  node.style.width = `${rect.width}px`;
  node.style.height = `${rect.height}px`;

  if (typeof rect.fontSize === "number") {
    const text = node.querySelector(":scope > span:last-child");
    if (text) {
      text.style.fontSize = `${rect.fontSize}px`;
      text.style.lineHeight = "1";
    }
  }
}

function renderLayout() {
  for (const [name, rect] of Object.entries(layoutState)) {
    applyNodeRect(name, rect);
  }
  exportLayout();
  updateSpecPanel(presets[currentPresetName]);
}

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function isSquareNode(nodeName) {
  return ["sleepCard", "stepsCard", "sleepIcon", "stepsIcon", "weatherIcon"].includes(nodeName);
}

function isTextNode(nodeName) {
  return ["temperature", "range", "sleepValue", "stepsValue"].includes(nodeName);
}

function beginInteraction(event, nodeName, mode) {
  if (!editModeEnabled) {
    return;
  }

  const rect = layoutState[nodeName];
  activePointer = {
    pointerId: event.pointerId,
    nodeName,
    mode,
    startX: event.clientX,
    startY: event.clientY,
    startRect: { ...rect }
  };
  editableNodes[nodeName].classList.add(mode === "drag" ? "dragging" : "resizing");
  event.preventDefault();
}

function updateInteraction(event) {
  if (!activePointer || activePointer.pointerId !== event.pointerId) {
    return;
  }

  const preset = presets[currentPresetName];
  const stageWidth = preset.watchWidth - preset.safeInsetX * 2;
  const stageHeight = preset.watchHeight - preset.safeInsetTop - preset.safeInsetBottom;
  const scale = parseFloat(getComputedStyle(root).getPropertyValue("--preview-scale"));
  const dx = (event.clientX - activePointer.startX) / scale;
  const dy = (event.clientY - activePointer.startY) / scale;
  const rect = { ...activePointer.startRect };

  if (activePointer.mode === "drag") {
    rect.x = clamp(activePointer.startRect.x + dx, 0, stageWidth - rect.width);
    rect.y = clamp(activePointer.startRect.y + dy, 0, stageHeight - rect.height);
  } else {
    const delta = isSquareNode(activePointer.nodeName) ? Math.max(dx, dy) : dx;
    rect.width = clamp(activePointer.startRect.width + delta, 24, stageWidth - rect.x);
    rect.height = isSquareNode(activePointer.nodeName)
      ? rect.width
      : clamp(activePointer.startRect.height + dy, 24, stageHeight - rect.y);

    if (isTextNode(activePointer.nodeName)) {
      rect.fontSize = clamp(activePointer.startRect.fontSize + dy * 0.35, 8, 44);
    }
  }

  layoutState[activePointer.nodeName] = rect;
  renderLayout();
}

function endInteraction(event) {
  if (!activePointer || activePointer.pointerId !== event.pointerId) {
    return;
  }

  editableNodes[activePointer.nodeName].classList.remove("dragging", "resizing");
  activePointer = null;
}

function wireEditableNode(nodeName) {
  const node = editableNodes[nodeName];
  if (!node) {
    return;
  }

  const handle = node.querySelector(":scope > .edit-handle");

  node.addEventListener("pointerdown", (event) => {
    const nestedEditable = event.target.closest(".editable-node");
    if (nestedEditable && nestedEditable !== node) {
      return;
    }

    if (event.target === handle) {
      event.stopPropagation();
      beginInteraction(event, nodeName, "resize");
      return;
    }

    event.stopPropagation();
    beginInteraction(event, nodeName, "drag");
  });
}

function toggleEditMode() {
  editModeEnabled = !editModeEnabled;
  surfaceStage.classList.toggle("edit-mode", editModeEnabled);
  toggleEditButton.classList.toggle("is-active", editModeEnabled);
  toggleEditButton.textContent = editModeEnabled ? "关闭布局编辑" : "开启布局编辑";
  editStatus.textContent = `编辑模式：${editModeEnabled ? "开启" : "关闭"}`;
}

function resetLayout() {
  layoutState = cloneLayout(presets[currentPresetName].layout);
  renderLayout();
}

for (const binding of iconBindings) {
  const showFallback = () => {
    binding.image.classList.add("is-hidden");
    binding.fallback.classList.remove("is-hidden");
    binding.status.textContent = `${binding.label}图标：占位图形`;
  };

  const showGenerated = () => {
    binding.image.classList.remove("is-hidden");
    binding.fallback.classList.add("is-hidden");
    binding.status.textContent = `${binding.label}图标：已加载 PNG`;
  };

  binding.image.addEventListener("load", showGenerated);
  binding.image.addEventListener("error", showFallback);

  if (binding.image.complete && binding.image.naturalWidth > 0) {
    showGenerated();
  } else if (binding.image.complete) {
    showFallback();
  } else {
    showFallback();
  }
}

document.querySelectorAll(".preset-button").forEach((button) => {
  button.addEventListener("click", () => applyPreset(button.dataset.preset));
});

toggleEditButton.addEventListener("click", toggleEditMode);
resetLayoutButton.addEventListener("click", resetLayout);

for (const nodeName of Object.keys(editableNodes)) {
  wireEditableNode(nodeName);
}

window.addEventListener("pointermove", updateInteraction);
window.addEventListener("pointerup", endInteraction);
window.addEventListener("pointercancel", endInteraction);

applyPreset(currentPresetName);
