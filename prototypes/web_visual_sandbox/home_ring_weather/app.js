const surfaceOrder = ["payments", "nfc", "health", "weather"];

const basePreset = {
  watchWidth: 240,
  watchHeight: 296,
  safeInsetX: 10,
  safeInsetTop: 20,
  safeInsetBottom: 16,
  stageRadius: 28,
  surfaces: {
    weather: {
      focus: "Weather / Steps / Sleep",
      layout: {
        heroCard: { x: 0, y: 15.48, width: 220, height: 110 },
        temperature: { x: 18.0, y: 20.26, width: 85.29, height: 40.9, fontSize: 35.02 },
        range: { x: 21.87, y: 65.23, width: 73.29, height: 24, fontSize: 11.66 },
        weatherIcon: { x: 140.23, y: 17.03, width: 73, height: 73 },
        sleepCard: { x: 0.97, y: 135.68, width: 106, height: 106 },
        stepsCard: { x: 114.0, y: 135.68, width: 106, height: 106 },
        sleepIcon: { x: 0, y: 0, width: 58.39, height: 58.39 },
        stepsIcon: { x: 0, y: 0, width: 58.39, height: 58.39 },
        sleepValue: { x: 12.13, y: 68.39, width: 71.42, height: 24, fontSize: 13.77 },
        stepsValue: { x: 12.45, y: 70.32, width: 75.1, height: 30.06, fontSize: 16.82 }
      }
    },
    payments: {
      focus: "Alipay / WeChat / Music",
      layout: {
        alipayCard: { x: 0, y: 15, width: 106, height: 106 },
        wechatCard: { x: 114, y: 15, width: 106, height: 106 },
        musicCard: { x: 0, y: 136, width: 220, height: 106 },
        alipayIcon: { x: 2.39, y: 1.74, width: 49.29, height: 49.29 },
        wechatIcon: { x: 7.23, y: 4.97, width: 40.26, height: 40.26 },
        alipayLabel: { x: 16, y: 72, width: 72, height: 20, fontSize: 15.5 },
        wechatLabel: { x: 16, y: 58, width: 74, height: 36, fontSize: 15.5 },
        musicStatus: { x: 16, y: 16, width: 182, height: 20, fontSize: 13.5 },
        musicPrev: { x: 21, y: 63, width: 24, height: 24, fontSize: 27 },
        musicPlay: { x: 92, y: 55, width: 36, height: 36, fontSize: 38 },
        musicNext: { x: 176, y: 63, width: 24, height: 24, fontSize: 27 }
      }
    },
    nfc: {
      focus: "Title / Subtitle / Large Card",
      layout: {
        title: { x: 56.97, y: 17.74, width: 92.52, height: 18.32, fontSize: 21.81 },
        subtitle: { x: 29.61, y: 47.23, width: 151.1, height: 18, fontSize: 11.48 },
        nfcCard: { x: 6, y: 74.97, width: 208, height: 119.87 },
        nfcImage: { x: -1.87, y: -4.06, width: 209.48, height: 124.32 }
      }
    },
    health: {
      focus: "Heart / SpO2 / Breathe / Stress",
      layout: {
        heartCard: { x: 0, y: 15, width: 106, height: 106 },
        spo2Card: { x: 114, y: 15, width: 106, height: 106 },
        breatheCard: { x: 0, y: 129, width: 106, height: 106 },
        stressCard: { x: 114, y: 129, width: 106, height: 106 },
        heartIcon: { x: 18, y: 16, width: 36, height: 36 },
        spo2Icon: { x: 18, y: 12, width: 40, height: 40 },
        breatheIcon: { x: 13.74, y: 13.29, width: 38.58, height: 38.58 },
        stressIcon: { x: 18, y: 16, width: 44, height: 44 },
        heartValue: { x: 12, y: 74, width: 52, height: 18, fontSize: 15 },
        spo2Value: { x: 12, y: 74, width: 52, height: 18, fontSize: 15 },
        breatheLabel: { x: 14, y: 76, width: 72, height: 20, fontSize: 14 },
        stressValue: { x: 12, y: 74, width: 52, height: 18, fontSize: 15 }
      }
    }
  }
};

function deepClone(value) {
  return JSON.parse(JSON.stringify(value));
}

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

function scaleSurfaceLayout(layout, xRatio, yRatio) {
  const next = {};
  for (const [name, rect] of Object.entries(layout)) {
    next[name] = scaleRect(rect, xRatio, yRatio);
  }
  return next;
}

const tallerPreset = (() => {
  const width = 252;
  const height = 312;
  const stageWidthRatio = (width - 11 * 2) / (basePreset.watchWidth - basePreset.safeInsetX * 2);
  const stageHeightRatio = (height - 22 - 18) / (basePreset.watchHeight - basePreset.safeInsetTop - basePreset.safeInsetBottom);
  const surfaces = {};
  for (const [surfaceName, surfaceSpec] of Object.entries(basePreset.surfaces)) {
    surfaces[surfaceName] = {
      focus: surfaceSpec.focus,
      layout: scaleSurfaceLayout(surfaceSpec.layout, stageWidthRatio, stageHeightRatio)
    };
  }
  return {
    watchWidth: width,
    watchHeight: height,
    safeInsetX: 11,
    safeInsetTop: 22,
    safeInsetBottom: 18,
    stageRadius: 29,
    surfaces
  };
})();

const presets = {
  "xiaomi-197": basePreset,
  "taller-alt": tallerPreset
};

const root = document.documentElement;
const surfaceStage = document.getElementById("surface-stage");
const layoutExport = document.getElementById("layout-export");
const editStatus = document.getElementById("edit-status");
const surfaceStatus = document.getElementById("surface-status");
const toggleEditButton = document.getElementById("toggle-edit-mode");
const resetLayoutButton = document.getElementById("reset-layout");
const pagerDots = Array.from(document.querySelectorAll(".pager-dot"));

const surfaceViews = Object.fromEntries(
  Array.from(document.querySelectorAll(".surface-view")).map((node) => [node.dataset.surface, node])
);

const editableNodesBySurface = {};
for (const [surfaceName, surfaceView] of Object.entries(surfaceViews)) {
  editableNodesBySurface[surfaceName] = Object.fromEntries(
    Array.from(surfaceView.querySelectorAll("[data-node]")).map((node) => [node.dataset.node, node])
  );
}

let currentPresetName = "xiaomi-197";
let currentSurfaceName = "nfc";
let editModeEnabled = false;
let activePointer = null;
let presetState = {};

function setCssVar(name, value) {
  root.style.setProperty(name, value);
}

function getStageDimensions(preset) {
  return {
    width: preset.watchWidth - preset.safeInsetX * 2,
    height: preset.watchHeight - preset.safeInsetTop - preset.safeInsetBottom
  };
}

function clonePresetSurfaces(preset) {
  const next = {};
  for (const [surfaceName, surfaceSpec] of Object.entries(preset.surfaces)) {
    next[surfaceName] = deepClone(surfaceSpec.layout);
  }
  return next;
}

function ensurePresetState(name) {
  if (!presetState[name]) {
    presetState[name] = clonePresetSurfaces(presets[name]);
  }
}

function currentLayout() {
  ensurePresetState(currentPresetName);
  return presetState[currentPresetName][currentSurfaceName];
}

function currentNodes() {
  return editableNodesBySurface[currentSurfaceName];
}

function applyNodeRect(node, rect) {
  node.style.left = `${rect.x}px`;
  node.style.top = `${rect.y}px`;
  node.style.width = `${rect.width}px`;
  node.style.height = `${rect.height}px`;
  if (typeof rect.fontSize === "number") {
    const textNode = node.querySelector(":scope > .node-text");
    if (textNode) {
      textNode.style.fontSize = `${rect.fontSize}px`;
      textNode.style.lineHeight = "1";
    }
  }
}

function renderSurface() {
  const layout = currentLayout();
  const nodes = currentNodes();
  for (const [nodeName, rect] of Object.entries(layout)) {
    if (nodes[nodeName]) {
      applyNodeRect(nodes[nodeName], rect);
    }
  }
}

function setActiveSurface(surfaceName) {
  currentSurfaceName = surfaceName;
  for (const [name, node] of Object.entries(surfaceViews)) {
    node.classList.toggle("is-active", name === surfaceName);
  }
  document.querySelectorAll(".surface-button").forEach((button) => {
    button.classList.toggle("is-active", button.dataset.surface === surfaceName);
  });
  pagerDots.forEach((dot) => dot.classList.remove("pager-dot-active"));
  const activeDot = pagerDots[surfaceOrder.indexOf(surfaceName)];
  if (activeDot) {
    activeDot.classList.add("pager-dot-active");
  }
  surfaceStatus.textContent = `Surface: ${surfaceName.toUpperCase()}`;
}

function updateSpecPanel() {
  const preset = presets[currentPresetName];
  const dims = getStageDimensions(preset);
  document.getElementById("spec-watch").textContent = `${preset.watchWidth} x ${preset.watchHeight}`;
  document.getElementById("spec-surface").textContent = currentSurfaceName.toUpperCase();
  document.getElementById("spec-stage").textContent = `${dims.width} x ${dims.height}`;
  document.getElementById("spec-safe").textContent =
    `${preset.safeInsetX} / ${preset.safeInsetTop} / ${preset.safeInsetBottom}`;
  document.getElementById("spec-focus").textContent = preset.surfaces[currentSurfaceName].focus;
}

function exportLayout() {
  layoutExport.value = JSON.stringify(
    {
      preset: currentPresetName,
      surface: currentSurfaceName,
      watch: {
        width: presets[currentPresetName].watchWidth,
        height: presets[currentPresetName].watchHeight,
        safeInsetX: presets[currentPresetName].safeInsetX,
        safeInsetTop: presets[currentPresetName].safeInsetTop,
        safeInsetBottom: presets[currentPresetName].safeInsetBottom
      },
      layout: currentLayout()
    },
    null,
    2
  );
}

function renderAll() {
  const preset = presets[currentPresetName];
  setCssVar("--watch-width", preset.watchWidth);
  setCssVar("--watch-height", preset.watchHeight);
  setCssVar("--safe-inset-x", preset.safeInsetX);
  setCssVar("--safe-inset-top", preset.safeInsetTop);
  setCssVar("--safe-inset-bottom", preset.safeInsetBottom);
  setCssVar("--stage-radius", preset.stageRadius);
  setActiveSurface(currentSurfaceName);
  renderSurface();
  updateSpecPanel();
  exportLayout();
}

function applyPreset(name) {
  currentPresetName = name;
  ensurePresetState(name);
  document.querySelectorAll(".preset-button").forEach((button) => {
    button.classList.toggle("is-active", button.dataset.preset === name);
  });
  renderAll();
}

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function nodeAllowsOverflow(node) {
  return node.dataset.canOverflow === "true";
}

function nodeIsSquare(node) {
  return node.dataset.lock === "square";
}

function nodeIsText(node) {
  return node.dataset.role === "text";
}

function getNodeBounds(nodeName) {
  const node = currentNodes()[nodeName];
  const preset = presets[currentPresetName];
  const stage = getStageDimensions(preset);
  const parentNode = node.parentElement.closest("[data-node]");

  if (!parentNode) {
    return { width: stage.width, height: stage.height, offsetX: 0, offsetY: 0 };
  }

  const parentRect = currentLayout()[parentNode.dataset.node];
  if (!parentRect) {
    return { width: stage.width, height: stage.height, offsetX: 0, offsetY: 0 };
  }

  return { width: parentRect.width, height: parentRect.height, offsetX: parentRect.x, offsetY: parentRect.y };
}

function beginInteraction(event, nodeName, mode) {
  if (!editModeEnabled) {
    return;
  }

  const node = currentNodes()[nodeName];
  const rect = currentLayout()[nodeName];
  activePointer = {
    pointerId: event.pointerId,
    nodeName,
    mode,
    startX: event.clientX,
    startY: event.clientY,
    startRect: { ...rect }
  };
  node.classList.add(mode === "drag" ? "dragging" : "resizing");
  event.preventDefault();
}

function updateInteraction(event) {
  if (!activePointer || activePointer.pointerId !== event.pointerId) {
    return;
  }

  const node = currentNodes()[activePointer.nodeName];
  const bounds = getNodeBounds(activePointer.nodeName);
  const scale = parseFloat(getComputedStyle(root).getPropertyValue("--preview-scale"));
  const dx = (event.clientX - activePointer.startX) / scale;
  const dy = (event.clientY - activePointer.startY) / scale;
  const rect = { ...activePointer.startRect };
  const allowOverflow = nodeAllowsOverflow(node);

  if (activePointer.mode === "drag") {
    const minX = allowOverflow ? -rect.width * 0.5 : 0;
    const minY = allowOverflow ? -rect.height * 0.5 : 0;
    const maxX = allowOverflow ? bounds.width - rect.width * 0.2 : bounds.width - rect.width;
    const maxY = allowOverflow ? bounds.height - rect.height * 0.2 : bounds.height - rect.height;
    rect.x = clamp(activePointer.startRect.x + dx, minX, maxX);
    rect.y = clamp(activePointer.startRect.y + dy, minY, maxY);
  } else {
    if (nodeIsSquare(node)) {
      const delta = Math.max(dx, dy);
      rect.width = clamp(activePointer.startRect.width + delta, 20, allowOverflow ? bounds.width * 1.8 : bounds.width - rect.x);
      rect.height = rect.width;
    } else {
      rect.width = clamp(activePointer.startRect.width + dx, 20, allowOverflow ? bounds.width * 1.8 : bounds.width - rect.x);
      rect.height = clamp(activePointer.startRect.height + dy, 18, allowOverflow ? bounds.height * 1.8 : bounds.height - rect.y);
    }

    if (nodeIsText(node) && typeof activePointer.startRect.fontSize === "number") {
      rect.fontSize = clamp(activePointer.startRect.fontSize + dy * 0.35, 8, 48);
    }
  }

  currentLayout()[activePointer.nodeName] = rect;
  renderSurface();
  updateSpecPanel();
  exportLayout();
}

function endInteraction(event) {
  if (!activePointer || activePointer.pointerId !== event.pointerId) {
    return;
  }

  const node = currentNodes()[activePointer.nodeName];
  node.classList.remove("dragging", "resizing");
  activePointer = null;
}

function wireEditableNode(node) {
  const nodeName = node.dataset.node;
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
  toggleEditButton.textContent = editModeEnabled ? "Disable Layout Edit" : "Enable Layout Edit";
  editStatus.textContent = `Edit mode: ${editModeEnabled ? "on" : "off"}`;
}

function resetLayout() {
  presetState[currentPresetName][currentSurfaceName] = deepClone(presets[currentPresetName].surfaces[currentSurfaceName].layout);
  renderAll();
}

document.querySelectorAll(".preset-button").forEach((button) => {
  button.addEventListener("click", () => applyPreset(button.dataset.preset));
});

document.querySelectorAll(".surface-button").forEach((button) => {
  button.addEventListener("click", () => {
    currentSurfaceName = button.dataset.surface;
    renderAll();
  });
});

toggleEditButton.addEventListener("click", toggleEditMode);
resetLayoutButton.addEventListener("click", resetLayout);

for (const surfaceNodes of Object.values(editableNodesBySurface)) {
  for (const node of Object.values(surfaceNodes)) {
    wireEditableNode(node);
  }
}

window.addEventListener("pointermove", updateInteraction);
window.addEventListener("pointerup", endInteraction);
window.addEventListener("pointercancel", endInteraction);

applyPreset(currentPresetName);
