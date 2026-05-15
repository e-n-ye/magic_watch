const basePreset = {
  watchWidth: 240,
  watchHeight: 296,
  safeInsetX: 10,
  safeInsetTop: 20,
  safeInsetBottom: 16,
  layout: {
    batteryRow: { x: 92, y: 10, width: 52.62444682060918, height: 18, fontSize: 17, iconSize: 15, gap: 4 },
    hourStage: { x: 0, y: 0, width: 240, height: 296 },
    digitBox: { x: 0, y: 2, width: 280, height: 280 },
    hourGlyph: { fontSize: 280, singleTextLength: 150, dualTextLength: 227 },
    minuteLabel: { x: 75.68766070015823, y: 234.3122620401503, width: 95.37539866000792, height: 27.5611943112144, fontSize: 33.04641800892504 }
  },
  diffusion: {
    innerWidthRatio: 0.31,
    outerWidthRatio: 0.28,
    sliceInsetRatio: 0.18,
    innerShift: 4,
    outerShift: 16,
    innerOpacity: 0.5,
    outerOpacity: 0.29
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
  if (typeof rect.iconSize === "number") {
    scaled.iconSize = Number((rect.iconSize * ((xRatio + yRatio) / 2)).toFixed(2));
  }
  if (typeof rect.gap === "number") {
    scaled.gap = Number((rect.gap * ((xRatio + yRatio) / 2)).toFixed(2));
  }
  return scaled;
}

function scaleLayout(layout, xRatio, yRatio) {
  return {
    batteryRow: scaleRect(layout.batteryRow, xRatio, yRatio),
    hourStage: scaleRect(layout.hourStage, xRatio, yRatio),
    digitBox: scaleRect(layout.digitBox, xRatio, yRatio),
    hourGlyph: {
      fontSize: Number((layout.hourGlyph.fontSize * yRatio).toFixed(2)),
      singleTextLength: Number((layout.hourGlyph.singleTextLength * xRatio).toFixed(2)),
      dualTextLength: Number((layout.hourGlyph.dualTextLength * xRatio).toFixed(2))
    },
    minuteLabel: scaleRect(layout.minuteLabel, xRatio, yRatio)
  };
}

const tallerPreset = (() => {
  const width = 252;
  const height = 312;
  const xRatio = width / basePreset.watchWidth;
  const yRatio = height / basePreset.watchHeight;
  return {
    watchWidth: width,
    watchHeight: height,
    safeInsetX: 11,
    safeInsetTop: 22,
    safeInsetBottom: 18,
    layout: scaleLayout(basePreset.layout, xRatio, yRatio),
    diffusion: deepClone(basePreset.diffusion)
  };
})();

const presets = {
  "xiaomi-197": basePreset,
  "taller-alt": tallerPreset
};

const state = {
  preset: "xiaomi-197",
  hourText: "12",
  minuteText: "24",
  batteryPercent: 21,
  spreadIndex: 8,
  displaySpread: 8,
  layout: deepClone(basePreset.layout),
  diffusion: deepClone(basePreset.diffusion)
};

const root = document.documentElement;
const watchScreen = document.getElementById("watch-screen");
const hourInput = document.getElementById("hour-input");
const minuteInput = document.getElementById("minute-input");
const batteryInput = document.getElementById("battery-input");
const spreadSlider = document.getElementById("spread-slider");
const spreadValue = document.getElementById("spread-value");
const exportArea = document.getElementById("spec-export");
const editStatus = document.getElementById("edit-status");
const toggleEditButton = document.getElementById("toggle-edit-mode");
const resetLayoutButton = document.getElementById("reset-layout");

const batteryRow = document.getElementById("battery-row");
const batteryIcon = document.getElementById("battery-icon");
const batteryText = document.getElementById("battery-text");
const hourStage = document.getElementById("hour-stage");
const hourEmpty = document.getElementById("hour-empty");
const minuteLabel = document.getElementById("minute-label");
const minuteTextNode = document.getElementById("minute-text");

const editableNodes = {
  batteryRow,
  hourStage,
  minuteLabel
};

const hourGroup = document.getElementById("hour-group");
const hourLayers = {
  node: hourGroup,
  core: hourGroup.querySelector(".hour-core"),
  coreImage: hourGroup.querySelector(".hour-core img"),
  leftOuter: hourGroup.querySelector(".slice-left-outer"),
  leftOuterImage: hourGroup.querySelector(".slice-left-outer img"),
  leftInner: hourGroup.querySelector(".slice-left-inner"),
  leftInnerImage: hourGroup.querySelector(".slice-left-inner img"),
  rightInner: hourGroup.querySelector(".slice-right-inner"),
  rightInnerImage: hourGroup.querySelector(".slice-right-inner img"),
  rightOuter: hourGroup.querySelector(".slice-right-outer"),
  rightOuterImage: hourGroup.querySelector(".slice-right-outer img")
};

const sliderMap = {
  innerWidthRatio: { id: "inner-width-slider", output: "inner-width-value", format: (value) => `${Math.round(value * 100)}%` },
  outerWidthRatio: { id: "outer-width-slider", output: "outer-width-value", format: (value) => `${Math.round(value * 100)}%` },
  sliceInsetRatio: { id: "slice-inset-slider", output: "slice-inset-value", format: (value) => `${Math.round(value * 100)}%` },
  innerShift: { id: "inner-shift-slider", output: "inner-shift-value", format: (value) => `${value}px` },
  outerShift: { id: "outer-shift-slider", output: "outer-shift-value", format: (value) => `${value}px` },
  innerOpacity: { id: "inner-opacity-slider", output: "inner-opacity-value", format: (value) => `${Math.round(value * 100)}%` },
  outerOpacity: { id: "outer-opacity-slider", output: "outer-opacity-value", format: (value) => `${Math.round(value * 100)}%` },
  digitWidth: { id: "digit-width-slider", output: "digit-width-value", format: (value) => `${value}px` },
  digitHeight: { id: "digit-height-slider", output: "digit-height-value", format: (value) => `${value}px` },
  hourFontSize: { id: "hour-font-size-slider", output: "hour-font-size-value", format: (value) => `${value}px` },
  singleTextLength: { id: "single-text-length-slider", output: "single-text-length-value", format: (value) => `${value}px` },
  dualTextLength: { id: "dual-text-length-slider", output: "dual-text-length-value", format: (value) => `${value}px` },
  minuteSize: { id: "minute-size-slider", output: "minute-size-value", format: (value) => `${value}px` },
  batterySize: { id: "battery-size-slider", output: "battery-size-value", format: (value) => `${value}px` },
  batteryIconSize: { id: "battery-icon-slider", output: "battery-icon-value", format: (value) => `${value}px` },
  batteryGap: { id: "battery-gap-slider", output: "battery-gap-value", format: (value) => `${value}px` }
};

let editModeEnabled = false;
let activePointer = null;
let spreadAnimationFrame = null;

const SPREAD_MIN = 0;
const SPREAD_MAX = 8;
const SPREAD_OVERSHOOT = 0.55;

function digitAsset(digit) {
  return `../../../../assets/watchface_digits/diffusion_svg/${digit}.svg`;
}

function hourAsset(hourText) {
  const clean = (hourText || "").replace(/[^\d]/g, "").slice(0, 2) || "--";
  const svgWidth = 420;
  const svgHeight = 260;
  const textLength = clean.length > 1 ? state.layout.hourGlyph.dualTextLength : state.layout.hourGlyph.singleTextLength;
  const fontSize = state.layout.hourGlyph.fontSize;
  const svg = `
    <svg xmlns="http://www.w3.org/2000/svg" width="${svgWidth}" height="${svgHeight}" viewBox="0 0 ${svgWidth} ${svgHeight}">
      <rect width="100%" height="100%" fill="none"/>
      <text
        x="50%"
        y="50%"
        text-anchor="middle"
        dominant-baseline="central"
        fill="#F5F7FB"
        font-family="Bahnschrift, Segoe UI, Arial, sans-serif"
        font-size="${fontSize}"
        font-weight="700"
        letter-spacing="0"
        textLength="${textLength}"
        lengthAdjust="spacingAndGlyphs">${clean}</text>
    </svg>`;
  return `data:image/svg+xml;charset=utf-8,${encodeURIComponent(svg)}`;
}

function clamp(value, min, max) {
  return Math.max(min, Math.min(max, value));
}

function setCssVar(name, value) {
  root.style.setProperty(name, value);
}

function setRect(node, rect) {
  node.style.left = `${rect.x}px`;
  node.style.top = `${rect.y}px`;
  node.style.width = `${rect.width}px`;
  node.style.height = `${rect.height}px`;
}

function currentHourDigits() {
  return state.hourText.replace(/[^\d]/g, "").slice(0, 2);
}

function currentPreset() {
  return presets[state.preset];
}

function renderBattery() {
  const rect = state.layout.batteryRow;
  setRect(batteryRow, rect);
  batteryRow.style.fontSize = `${rect.fontSize}px`;
  batteryRow.style.gap = `${rect.gap}px`;
  batteryIcon.style.width = `${rect.iconSize}px`;
  batteryIcon.style.height = `${rect.iconSize}px`;
  batteryText.textContent = `${state.batteryPercent}%`;
}

function applySlice(slotNode, imageNode, slotX, slotWidth, imageX, opacity, digitRect) {
  slotNode.style.left = `${slotX}px`;
  slotNode.style.width = `${slotWidth}px`;
  slotNode.style.height = `${digitRect.height}px`;
  slotNode.style.opacity = String(opacity);
  imageNode.style.left = `${imageX}px`;
  imageNode.style.width = `${digitRect.width}px`;
  imageNode.style.height = `${digitRect.height}px`;
}

function renderHour() {
  const stageRect = state.layout.hourStage;
  const digitRect = state.layout.digitBox;
  const digits = currentHourDigits();

  setRect(hourStage, stageRect);

  if (digits.length === 0) {
    hourEmpty.style.display = "flex";
    hourLayers.node.style.display = "none";
    return;
  }

  hourEmpty.style.display = "none";
  const src = hourAsset(state.hourText);
  const startX = Math.round((stageRect.width - digitRect.width) / 2) + digitRect.x;
  const startY = Math.round((stageRect.height - digitRect.height) / 2) + digitRect.y;
  const innerWidth = Math.round(digitRect.width * state.diffusion.innerWidthRatio);
  const outerWidth = Math.round(digitRect.width * state.diffusion.outerWidthRatio);
  const cropInset = Math.round(digitRect.width * state.diffusion.sliceInsetRatio);
  const baseOverlap = Math.round(digitRect.width * 0.10);
  const spreadRatio = clamp(Math.abs(state.displaySpread), 0, SPREAD_MAX + SPREAD_OVERSHOOT) / SPREAD_MAX;
  const signedSpreadRatio = clamp(state.displaySpread, -SPREAD_OVERSHOOT, SPREAD_MAX + SPREAD_OVERSHOOT) / SPREAD_MAX;
  const innerShift = Math.round(state.diffusion.innerShift * signedSpreadRatio);
  const outerShift = Math.round(state.diffusion.outerShift * signedSpreadRatio);
  const innerOpacity = clamp(state.diffusion.innerOpacity * spreadRatio, 0, 1);
  const outerOpacity = clamp(state.diffusion.outerOpacity * spreadRatio, 0, 1);

  hourLayers.node.style.display = "block";
  hourLayers.node.style.left = `${startX}px`;
  hourLayers.node.style.top = `${startY}px`;
  hourLayers.node.style.width = `${digitRect.width}px`;
  hourLayers.node.style.height = `${digitRect.height}px`;

  hourLayers.core.style.left = "0px";
  hourLayers.core.style.width = `${digitRect.width}px`;
  hourLayers.core.style.height = `${digitRect.height}px`;
  hourLayers.coreImage.src = src;
  hourLayers.coreImage.style.left = "0px";
  hourLayers.coreImage.style.width = `${digitRect.width}px`;
  hourLayers.coreImage.style.height = `${digitRect.height}px`;

  hourLayers.leftOuterImage.src = src;
  hourLayers.leftInnerImage.src = src;
  hourLayers.rightInnerImage.src = src;
  hourLayers.rightOuterImage.src = src;

  applySlice(
    hourLayers.leftOuter,
    hourLayers.leftOuterImage,
    baseOverlap - outerShift,
    outerWidth,
    -cropInset,
    outerOpacity,
    digitRect
  );
  applySlice(
    hourLayers.leftInner,
    hourLayers.leftInnerImage,
    baseOverlap - innerShift,
    innerWidth,
    -cropInset,
    innerOpacity,
    digitRect
  );
  applySlice(
    hourLayers.rightInner,
    hourLayers.rightInnerImage,
    digitRect.width - innerWidth - baseOverlap + innerShift,
    innerWidth,
    -(digitRect.width - innerWidth - cropInset),
    innerOpacity,
    digitRect
  );
  applySlice(
    hourLayers.rightOuter,
    hourLayers.rightOuterImage,
    digitRect.width - outerWidth - baseOverlap + outerShift,
    outerWidth,
    -(digitRect.width - outerWidth - cropInset),
    outerOpacity,
    digitRect
  );
}

function renderMinute() {
  const rect = state.layout.minuteLabel;
  setRect(minuteLabel, rect);
  minuteTextNode.style.fontSize = `${rect.fontSize}px`;
  minuteTextNode.textContent = state.minuteText;
}

function updateOutputs() {
  document.getElementById(sliderMap.innerWidthRatio.output).textContent = sliderMap.innerWidthRatio.format(state.diffusion.innerWidthRatio);
  document.getElementById(sliderMap.outerWidthRatio.output).textContent = sliderMap.outerWidthRatio.format(state.diffusion.outerWidthRatio);
  document.getElementById(sliderMap.sliceInsetRatio.output).textContent = sliderMap.sliceInsetRatio.format(state.diffusion.sliceInsetRatio);
  document.getElementById(sliderMap.innerShift.output).textContent = sliderMap.innerShift.format(state.diffusion.innerShift);
  document.getElementById(sliderMap.outerShift.output).textContent = sliderMap.outerShift.format(state.diffusion.outerShift);
  document.getElementById(sliderMap.innerOpacity.output).textContent = sliderMap.innerOpacity.format(state.diffusion.innerOpacity);
  document.getElementById(sliderMap.outerOpacity.output).textContent = sliderMap.outerOpacity.format(state.diffusion.outerOpacity);
  document.getElementById(sliderMap.digitWidth.output).textContent = sliderMap.digitWidth.format(state.layout.digitBox.width);
  document.getElementById(sliderMap.digitHeight.output).textContent = sliderMap.digitHeight.format(state.layout.digitBox.height);
  document.getElementById(sliderMap.hourFontSize.output).textContent = sliderMap.hourFontSize.format(state.layout.hourGlyph.fontSize);
  document.getElementById(sliderMap.singleTextLength.output).textContent = sliderMap.singleTextLength.format(state.layout.hourGlyph.singleTextLength);
  document.getElementById(sliderMap.dualTextLength.output).textContent = sliderMap.dualTextLength.format(state.layout.hourGlyph.dualTextLength);
  document.getElementById(sliderMap.minuteSize.output).textContent = sliderMap.minuteSize.format(state.layout.minuteLabel.fontSize);
  document.getElementById(sliderMap.batterySize.output).textContent = sliderMap.batterySize.format(state.layout.batteryRow.fontSize);
  document.getElementById(sliderMap.batteryIconSize.output).textContent = sliderMap.batteryIconSize.format(state.layout.batteryRow.iconSize);
  document.getElementById(sliderMap.batteryGap.output).textContent = sliderMap.batteryGap.format(state.layout.batteryRow.gap);
}

function syncControlsFromState() {
  spreadSlider.value = String(state.spreadIndex);
  spreadValue.textContent = formatSpreadValue(state.displaySpread);
  hourInput.value = state.hourText;
  minuteInput.value = state.minuteText;
  batteryInput.value = String(state.batteryPercent);

  document.getElementById(sliderMap.innerWidthRatio.id).value = String(Math.round(state.diffusion.innerWidthRatio * 100));
  document.getElementById(sliderMap.outerWidthRatio.id).value = String(Math.round(state.diffusion.outerWidthRatio * 100));
  document.getElementById(sliderMap.sliceInsetRatio.id).value = String(Math.round(state.diffusion.sliceInsetRatio * 100));
  document.getElementById(sliderMap.innerShift.id).value = String(state.diffusion.innerShift);
  document.getElementById(sliderMap.outerShift.id).value = String(state.diffusion.outerShift);
  document.getElementById(sliderMap.innerOpacity.id).value = String(Math.round(state.diffusion.innerOpacity * 100));
  document.getElementById(sliderMap.outerOpacity.id).value = String(Math.round(state.diffusion.outerOpacity * 100));
  document.getElementById(sliderMap.digitWidth.id).value = String(state.layout.digitBox.width);
  document.getElementById(sliderMap.digitHeight.id).value = String(state.layout.digitBox.height);
  document.getElementById(sliderMap.hourFontSize.id).value = String(state.layout.hourGlyph.fontSize);
  document.getElementById(sliderMap.singleTextLength.id).value = String(state.layout.hourGlyph.singleTextLength);
  document.getElementById(sliderMap.dualTextLength.id).value = String(state.layout.hourGlyph.dualTextLength);
  document.getElementById(sliderMap.minuteSize.id).value = String(state.layout.minuteLabel.fontSize);
  document.getElementById(sliderMap.batterySize.id).value = String(state.layout.batteryRow.fontSize);
  document.getElementById(sliderMap.batteryIconSize.id).value = String(state.layout.batteryRow.iconSize);
  document.getElementById(sliderMap.batteryGap.id).value = String(state.layout.batteryRow.gap);
}

function formatSpreadValue(value) {
  const rounded = Math.round(value * 10) / 10;
  return Number.isInteger(rounded) ? String(rounded) : rounded.toFixed(1);
}

function cancelSpreadAnimation() {
  if (spreadAnimationFrame !== null) {
    cancelAnimationFrame(spreadAnimationFrame);
    spreadAnimationFrame = null;
  }
}

function renderSpreadOnly() {
  spreadValue.textContent = formatSpreadValue(state.displaySpread);
  renderHour();
}

function animateSpreadValue(from, to, duration, easing, onDone) {
  cancelSpreadAnimation();
  const start = performance.now();

  function step(now) {
    const progress = clamp((now - start) / duration, 0, 1);
    const eased = easing(progress);
    state.displaySpread = from + (to - from) * eased;
    renderSpreadOnly();

    if (progress < 1) {
      spreadAnimationFrame = requestAnimationFrame(step);
      return;
    }

    spreadAnimationFrame = null;
    state.displaySpread = to;
    renderSpreadOnly();
    if (onDone) {
      onDone();
    }
  }

  spreadAnimationFrame = requestAnimationFrame(step);
}

function easeOutCubic(value) {
  return 1 - Math.pow(1 - value, 3);
}

function easeOutBack(value) {
  const c1 = 1.70158;
  const c3 = c1 + 1;
  return 1 + c3 * Math.pow(value - 1, 3) + c1 * Math.pow(value - 1, 2);
}

function bounceSpreadToBoundary(direction) {
  const boundary = direction > 0 ? SPREAD_MAX : SPREAD_MIN;
  const overshoot = direction > 0 ? SPREAD_MAX + SPREAD_OVERSHOOT : SPREAD_MIN - SPREAD_OVERSHOOT;
  state.spreadIndex = boundary;
  spreadSlider.value = String(boundary);
  exportSpec();
  animateSpreadValue(state.displaySpread, overshoot, 120, easeOutCubic, () => {
    animateSpreadValue(overshoot, boundary, 280, easeOutBack);
  });
}

function commitSpreadIndex(nextIndex) {
  cancelSpreadAnimation();
  state.spreadIndex = clamp(nextIndex, SPREAD_MIN, SPREAD_MAX);
  state.displaySpread = state.spreadIndex;
  render();
}

function nudgeSpread(delta) {
  const candidate = state.spreadIndex + delta;
  if (candidate > SPREAD_MAX) {
    bounceSpreadToBoundary(1);
    return;
  }
  if (candidate < SPREAD_MIN) {
    bounceSpreadToBoundary(-1);
    return;
  }

  state.spreadIndex = candidate;
  spreadSlider.value = String(state.spreadIndex);
  exportSpec();
  animateSpreadValue(state.displaySpread, state.spreadIndex, 180, easeOutCubic);
}

function exportSpec() {
  const preset = currentPreset();
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
        hourText: state.hourText,
        minuteText: state.minuteText,
        batteryPercent: state.batteryPercent,
        spreadIndex: state.spreadIndex
      },
      layout: state.layout,
      diffusion: state.diffusion
    },
    null,
    2
  );
}

function render() {
  const preset = currentPreset();
  setCssVar("--watch-width", preset.watchWidth);
  setCssVar("--watch-height", preset.watchHeight);
  setCssVar("--safe-inset-x", preset.safeInsetX);
  setCssVar("--safe-inset-top", preset.safeInsetTop);
  setCssVar("--safe-inset-bottom", preset.safeInsetBottom);

  document.querySelectorAll(".preset-button[data-preset]").forEach((button) => {
    button.classList.toggle("is-active", button.dataset.preset === state.preset);
  });

  watchScreen.classList.toggle("edit-mode", editModeEnabled);
  toggleEditButton.classList.toggle("is-active", editModeEnabled);
  toggleEditButton.textContent = editModeEnabled ? "Disable Layout Edit" : "Enable Layout Edit";
  editStatus.textContent = `Edit mode: ${editModeEnabled ? "on" : "off"}`;

  syncControlsFromState();
  renderBattery();
  renderHour();
  renderMinute();
  updateOutputs();
  exportSpec();
}

function applyPreset(name) {
  const preset = presets[name];
  state.preset = name;
  state.layout = deepClone(preset.layout);
  state.diffusion = deepClone(preset.diffusion);
  state.displaySpread = state.spreadIndex;
  render();
}

function getNodeBounds() {
  const preset = currentPreset();
  return { width: preset.watchWidth, height: preset.watchHeight };
}

function beginInteraction(event, nodeName, mode) {
  if (!editModeEnabled) {
    return;
  }

  const rect = state.layout[nodeName];
  if (!rect || typeof rect !== "object") {
    return;
  }

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

  const node = editableNodes[activePointer.nodeName];
  const bounds = getNodeBounds();
  const scale = parseFloat(getComputedStyle(root).getPropertyValue("--preview-scale"));
  const dx = (event.clientX - activePointer.startX) / scale;
  const dy = (event.clientY - activePointer.startY) / scale;
  const rect = { ...activePointer.startRect };

  if (activePointer.mode === "drag") {
    rect.x = clamp(activePointer.startRect.x + dx, 0, bounds.width - rect.width);
    rect.y = clamp(activePointer.startRect.y + dy, 0, bounds.height - rect.height);
  } else {
    rect.width = clamp(activePointer.startRect.width + dx, 24, bounds.width - rect.x);
    rect.height = clamp(activePointer.startRect.height + dy, 18, bounds.height - rect.y);
    if (node.dataset.role === "text" && typeof activePointer.startRect.fontSize === "number") {
      rect.fontSize = clamp(activePointer.startRect.fontSize + dy * 0.35, 10, 64);
    }
  }

  state.layout[activePointer.nodeName] = rect;
  render();
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
  const handle = node.querySelector(":scope > .edit-handle");

  node.addEventListener("pointerdown", (event) => {
    if (event.target === handle) {
      event.stopPropagation();
      beginInteraction(event, nodeName, "resize");
      return;
    }

    beginInteraction(event, nodeName, "drag");
  });
}

document.querySelectorAll(".preset-button[data-preset]").forEach((button) => {
  button.addEventListener("click", () => applyPreset(button.dataset.preset));
});

toggleEditButton.addEventListener("click", () => {
  editModeEnabled = !editModeEnabled;
  render();
});

resetLayoutButton.addEventListener("click", () => {
  const preset = currentPreset();
  state.layout = deepClone(preset.layout);
  state.diffusion = deepClone(preset.diffusion);
  state.displaySpread = state.spreadIndex;
  render();
});

spreadSlider.addEventListener("input", () => {
  commitSpreadIndex(Number(spreadSlider.value));
});

hourInput.addEventListener("input", () => {
  state.hourText = hourInput.value.replace(/[^\d]/g, "").slice(0, 2);
  render();
});

minuteInput.addEventListener("input", () => {
  const next = minuteInput.value.replace(/[^\d]/g, "").slice(0, 2);
  state.minuteText = next.length === 0 ? "--" : next;
  render();
});

batteryInput.addEventListener("input", () => {
  state.batteryPercent = clamp(Number(batteryInput.value || 0), 0, 100);
  render();
});

document.getElementById(sliderMap.innerWidthRatio.id).addEventListener("input", (event) => {
  state.diffusion.innerWidthRatio = Number(event.target.value) / 100;
  render();
});

document.getElementById(sliderMap.outerWidthRatio.id).addEventListener("input", (event) => {
  state.diffusion.outerWidthRatio = Number(event.target.value) / 100;
  render();
});

document.getElementById(sliderMap.sliceInsetRatio.id).addEventListener("input", (event) => {
  state.diffusion.sliceInsetRatio = Number(event.target.value) / 100;
  render();
});

document.getElementById(sliderMap.innerShift.id).addEventListener("input", (event) => {
  state.diffusion.innerShift = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.outerShift.id).addEventListener("input", (event) => {
  state.diffusion.outerShift = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.innerOpacity.id).addEventListener("input", (event) => {
  state.diffusion.innerOpacity = Number(event.target.value) / 100;
  render();
});

document.getElementById(sliderMap.outerOpacity.id).addEventListener("input", (event) => {
  state.diffusion.outerOpacity = Number(event.target.value) / 100;
  render();
});

document.getElementById(sliderMap.digitWidth.id).addEventListener("input", (event) => {
  state.layout.digitBox.width = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.digitHeight.id).addEventListener("input", (event) => {
  state.layout.digitBox.height = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.hourFontSize.id).addEventListener("input", (event) => {
  state.layout.hourGlyph.fontSize = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.singleTextLength.id).addEventListener("input", (event) => {
  state.layout.hourGlyph.singleTextLength = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.dualTextLength.id).addEventListener("input", (event) => {
  state.layout.hourGlyph.dualTextLength = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.minuteSize.id).addEventListener("input", (event) => {
  state.layout.minuteLabel.fontSize = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.batterySize.id).addEventListener("input", (event) => {
  state.layout.batteryRow.fontSize = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.batteryIconSize.id).addEventListener("input", (event) => {
  state.layout.batteryRow.iconSize = Number(event.target.value);
  render();
});

document.getElementById(sliderMap.batteryGap.id).addEventListener("input", (event) => {
  state.layout.batteryRow.gap = Number(event.target.value);
  render();
});

watchScreen.addEventListener("wheel", (event) => {
  if (editModeEnabled) {
    return;
  }
  event.preventDefault();
  nudgeSpread(event.deltaY < 0 ? 1 : -1);
}, { passive: false });

window.addEventListener("keydown", (event) => {
  if (editModeEnabled) {
    return;
  }
  if (event.key === "q" || event.key === "Q" || event.key === "ArrowLeft") {
    event.preventDefault();
    nudgeSpread(-1);
  } else if (event.key === "e" || event.key === "E" || event.key === "ArrowRight") {
    event.preventDefault();
    nudgeSpread(1);
  }
});

window.addEventListener("pointermove", updateInteraction);
window.addEventListener("pointerup", endInteraction);
window.addEventListener("pointercancel", endInteraction);

Object.keys(editableNodes).forEach(wireEditableNode);
applyPreset(state.preset);
