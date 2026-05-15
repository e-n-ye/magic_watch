const basePreset = {
  watchWidth: 240,
  watchHeight: 296,
  safeInsetX: 10,
  safeInsetTop: 20,
  safeInsetBottom: 16,
  layout: {
    batteryRow: { x: 92, y: 10, width: 52.62444682060918, height: 18, fontSize: 15, iconSize: 15, gap: 4 },
    hourStage: { x: 0, y: 0, width: 240, height: 296 },
    digitBox: { x: 0, y: 2, width: 242, height: 280 },
    hourGlyph: { fontSize: 273, singleTextLength: 110, dualTextLength: 160 },
    minuteLabel: { x: 75.68766070015823, y: 234.3122620401503, width: 95.37539866000792, height: 27.5611943112144, fontSize: 42 }
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
  hourText: "1",
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
const hourAuditGrid = document.getElementById("hour-audit-grid");

const editableNodes = {
  batteryRow,
  hourStage,
  minuteLabel
};

const hourGroup = document.getElementById("hour-group");
const hourLayers = {
  node: hourGroup,
  core: hourGroup.querySelector(".hour-core"),
  coreSurface: hourGroup.querySelector(".hour-core .layer-surface"),
  coreLeftImage: hourGroup.querySelector(".hour-core .digit-left"),
  coreRightImage: hourGroup.querySelector(".hour-core .digit-right"),
  leftOuter: hourGroup.querySelector(".slice-left-outer"),
  leftOuterSurface: hourGroup.querySelector(".slice-left-outer .layer-surface"),
  leftOuterLeftImage: hourGroup.querySelector(".slice-left-outer .digit-left"),
  leftOuterRightImage: hourGroup.querySelector(".slice-left-outer .digit-right"),
  leftInner: hourGroup.querySelector(".slice-left-inner"),
  leftInnerSurface: hourGroup.querySelector(".slice-left-inner .layer-surface"),
  leftInnerLeftImage: hourGroup.querySelector(".slice-left-inner .digit-left"),
  leftInnerRightImage: hourGroup.querySelector(".slice-left-inner .digit-right"),
  rightInner: hourGroup.querySelector(".slice-right-inner"),
  rightInnerSurface: hourGroup.querySelector(".slice-right-inner .layer-surface"),
  rightInnerLeftImage: hourGroup.querySelector(".slice-right-inner .digit-left"),
  rightInnerRightImage: hourGroup.querySelector(".slice-right-inner .digit-right"),
  rightOuter: hourGroup.querySelector(".slice-right-outer"),
  rightOuterSurface: hourGroup.querySelector(".slice-right-outer .layer-surface"),
  rightOuterLeftImage: hourGroup.querySelector(".slice-right-outer .digit-left"),
  rightOuterRightImage: hourGroup.querySelector(".slice-right-outer .digit-right")
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
const DIGIT_ASSET_CANVAS = 220;
const DIGIT_BOUNDS = [
  { x: 73, y: 37, width: 75, height: 133 },
  { x: 88, y: 39, width: 37, height: 129 },
  { x: 72, y: 38, width: 76, height: 131 },
  { x: 70, y: 37, width: 76, height: 133 },
  { x: 69, y: 39, width: 84, height: 129 },
  { x: 74, y: 38, width: 73, height: 131 },
  { x: 74, y: 38, width: 73, height: 131 },
  { x: 73, y: 39, width: 72, height: 129 },
  { x: 71, y: 37, width: 79, height: 133 },
  { x: 74, y: 38, width: 73, height: 131 }
];

function digitAsset(digit) {
  return `../../../../assets/watchface_digits/diffusion_png/${digit}.png`;
}

function digitBounds(digit) {
  return DIGIT_BOUNDS[digit] || { x: 0, y: 0, width: DIGIT_ASSET_CANVAS, height: DIGIT_ASSET_CANVAS };
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

function createAuditCard(hour) {
  const card = document.createElement("article");
  card.className = "audit-card";
  card.innerHTML = `
    <div class="audit-card-header">
      <span>${hour}:24</span>
      <span class="audit-chip">${hour < 10 ? "single" : "dual"}</span>
    </div>
    <div class="audit-watch">
      <div class="battery-row"><span class="battery-icon" aria-hidden="true">
        <svg viewBox="0 0 12 18" focusable="false" aria-hidden="true">
          <path d="M6.9 0 1.8 9.1h3.1L3.8 18l6.4-9.6H7.1z" fill="currentColor"></path>
        </svg>
      </span><span>21%</span></div>
      <div class="hour-stage">
        <div class="digit-group">
          <div class="hour-slice slice-left-outer"><div class="layer-surface"><img class="digit-image digit-left digit-blur-outer" alt=""><img class="digit-image digit-right digit-blur-outer" alt=""></div></div>
          <div class="hour-slice slice-left-inner"><div class="layer-surface"><img class="digit-image digit-left digit-blur-inner" alt=""><img class="digit-image digit-right digit-blur-inner" alt=""></div></div>
          <div class="hour-core"><div class="layer-surface"><img class="digit-image digit-left digit-core-image" alt=""><img class="digit-image digit-right digit-core-image" alt=""></div></div>
          <div class="hour-slice slice-right-inner"><div class="layer-surface"><img class="digit-image digit-left digit-blur-inner" alt=""><img class="digit-image digit-right digit-blur-inner" alt=""></div></div>
          <div class="hour-slice slice-right-outer"><div class="layer-surface"><img class="digit-image digit-left digit-blur-outer" alt=""><img class="digit-image digit-right digit-blur-outer" alt=""></div></div>
        </div>
      </div>
      <div class="minute-label"><span class="node-text">24</span></div>
    </div>`;
  return card;
}

function ensureAuditGrid() {
  if (!hourAuditGrid || hourAuditGrid.childElementCount > 0) {
    return;
  }
  for (let hour = 1; hour <= 12; hour += 1) {
    hourAuditGrid.appendChild(createAuditCard(hour));
  }
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

function renderHourSurface(surfaceNode, leftImageNode, rightImageNode, digits, digitRect) {
  const clean = (digits || "").replace(/[^\d]/g, "").slice(0, 2);
  if (!clean) {
    surfaceNode.style.display = "none";
    return;
  }

  surfaceNode.style.display = "block";
  surfaceNode.style.left = "0px";
  surfaceNode.style.top = "0px";
  surfaceNode.style.width = `${digitRect.width}px`;
  surfaceNode.style.height = `${digitRect.height}px`;

  const leftDigit = Number(clean[0]);
  const rightDigit = clean.length > 1 ? Number(clean[1]) : null;
  const leftBounds = digitBounds(leftDigit);
  const rightBounds = rightDigit !== null ? digitBounds(rightDigit) : null;
  const singleVisibleWidth = state.layout.hourGlyph.singleTextLength;
  const dualVisibleWidth = state.layout.hourGlyph.dualTextLength;
  const targetVisibleWidth = clean.length > 1 ? dualVisibleWidth : singleVisibleWidth;
  const targetVisibleHeight = Math.min(digitRect.height, state.layout.hourGlyph.fontSize);
  const topInset = Math.round((digitRect.height - targetVisibleHeight) / 2);
  const leftScaleY = targetVisibleHeight / leftBounds.height;
  const rightScaleY = rightBounds ? targetVisibleHeight / rightBounds.height : 0;
  const normalizedTotalWidth =
    leftBounds.width * leftScaleY + (rightBounds ? rightBounds.width * rightScaleY : 0);
  const widthScale = normalizedTotalWidth > 0 ? targetVisibleWidth / normalizedTotalWidth : 1;
  const leftVisibleWidth = Math.round(leftBounds.width * leftScaleY * widthScale);
  const rightVisibleWidth = rightBounds ? Math.round(rightBounds.width * rightScaleY * widthScale) : 0;
  const groupVisibleWidth = leftVisibleWidth + rightVisibleWidth;
  const groupLeft = Math.round((digitRect.width - groupVisibleWidth) / 2);
  const leftRenderWidth = Math.round(DIGIT_ASSET_CANVAS * leftScaleY * widthScale);
  const rightRenderWidth = rightBounds ? Math.round(DIGIT_ASSET_CANVAS * rightScaleY * widthScale) : 0;
  const leftX = Math.round(groupLeft - leftBounds.x * leftScaleY * widthScale);
  const rightX = rightBounds
    ? Math.round(groupLeft + leftVisibleWidth - rightBounds.x * rightScaleY * widthScale)
    : 0;

  leftImageNode.src = digitAsset(leftDigit);
  leftImageNode.style.display = "block";
  leftImageNode.style.left = `${leftX}px`;
  leftImageNode.style.top = `${topInset}px`;
  leftImageNode.style.width = `${leftRenderWidth}px`;
  leftImageNode.style.height = `${targetVisibleHeight}px`;

  if (rightDigit !== null) {
    rightImageNode.src = digitAsset(rightDigit);
    rightImageNode.style.display = "block";
    rightImageNode.style.left = `${rightX}px`;
    rightImageNode.style.top = `${topInset}px`;
    rightImageNode.style.width = `${rightRenderWidth}px`;
    rightImageNode.style.height = `${targetVisibleHeight}px`;
  } else {
    rightImageNode.style.display = "none";
    rightImageNode.removeAttribute("src");
  }
}

function applySlice(slotNode, surfaceNode, slotX, slotWidth, imageX, opacity, digitRect) {
  slotNode.style.left = `${slotX}px`;
  slotNode.style.width = `${slotWidth}px`;
  slotNode.style.height = `${digitRect.height}px`;
  slotNode.style.opacity = String(opacity);
  surfaceNode.style.left = `${imageX}px`;
  surfaceNode.style.width = `${digitRect.width}px`;
  surfaceNode.style.height = `${digitRect.height}px`;
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
  renderHourSurface(hourLayers.coreSurface, hourLayers.coreLeftImage, hourLayers.coreRightImage, digits, digitRect);
  renderHourSurface(hourLayers.leftOuterSurface,
    hourLayers.leftOuterLeftImage,
    hourLayers.leftOuterRightImage,
    digits,
    digitRect);
  renderHourSurface(hourLayers.leftInnerSurface,
    hourLayers.leftInnerLeftImage,
    hourLayers.leftInnerRightImage,
    digits,
    digitRect);
  renderHourSurface(hourLayers.rightInnerSurface,
    hourLayers.rightInnerLeftImage,
    hourLayers.rightInnerRightImage,
    digits,
    digitRect);
  renderHourSurface(hourLayers.rightOuterSurface,
    hourLayers.rightOuterLeftImage,
    hourLayers.rightOuterRightImage,
    digits,
    digitRect);

  applySlice(
    hourLayers.leftOuter,
    hourLayers.leftOuterSurface,
    baseOverlap - outerShift,
    outerWidth,
    -cropInset,
    outerOpacity,
    digitRect
  );
  applySlice(
    hourLayers.leftInner,
    hourLayers.leftInnerSurface,
    baseOverlap - innerShift,
    innerWidth,
    -cropInset,
    innerOpacity,
    digitRect
  );
  applySlice(
    hourLayers.rightInner,
    hourLayers.rightInnerSurface,
    digitRect.width - innerWidth - baseOverlap + innerShift,
    innerWidth,
    -(digitRect.width - innerWidth - cropInset),
    innerOpacity,
    digitRect
  );
  applySlice(
    hourLayers.rightOuter,
    hourLayers.rightOuterSurface,
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
  renderAuditGrid();
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

function renderAuditGrid() {
  if (!hourAuditGrid) {
    return;
  }
  ensureAuditGrid();

  const cards = hourAuditGrid.querySelectorAll(".audit-card");
  cards.forEach((card, index) => {
    const hour = String(index + 1);
    const watch = card.querySelector(".audit-watch");
    const battery = card.querySelector(".battery-row");
    const icon = card.querySelector(".battery-icon");
    const minute = card.querySelector(".minute-label .node-text");
    const minuteWrap = card.querySelector(".minute-label");
    const stage = card.querySelector(".hour-stage");
    const group = card.querySelector(".digit-group");
    const layers = {
      core: card.querySelector(".hour-core"),
      coreSurface: card.querySelector(".hour-core .layer-surface"),
      coreLeft: card.querySelector(".hour-core .digit-left"),
      coreRight: card.querySelector(".hour-core .digit-right"),
      leftOuter: card.querySelector(".slice-left-outer"),
      leftOuterSurface: card.querySelector(".slice-left-outer .layer-surface"),
      leftOuterLeft: card.querySelector(".slice-left-outer .digit-left"),
      leftOuterRight: card.querySelector(".slice-left-outer .digit-right"),
      leftInner: card.querySelector(".slice-left-inner"),
      leftInnerSurface: card.querySelector(".slice-left-inner .layer-surface"),
      leftInnerLeft: card.querySelector(".slice-left-inner .digit-left"),
      leftInnerRight: card.querySelector(".slice-left-inner .digit-right"),
      rightInner: card.querySelector(".slice-right-inner"),
      rightInnerSurface: card.querySelector(".slice-right-inner .layer-surface"),
      rightInnerLeft: card.querySelector(".slice-right-inner .digit-left"),
      rightInnerRight: card.querySelector(".slice-right-inner .digit-right"),
      rightOuter: card.querySelector(".slice-right-outer"),
      rightOuterSurface: card.querySelector(".slice-right-outer .layer-surface"),
      rightOuterLeft: card.querySelector(".slice-right-outer .digit-left"),
      rightOuterRight: card.querySelector(".slice-right-outer .digit-right")
    };

    const stageRect = state.layout.hourStage;
    const digitRect = state.layout.digitBox;
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
    const startX = Math.round((stageRect.width - digitRect.width) / 2) + digitRect.x;
    const startY = Math.round((stageRect.height - digitRect.height) / 2) + digitRect.y;

    battery.style.left = `${state.layout.batteryRow.x}px`;
    battery.style.top = `${state.layout.batteryRow.y}px`;
    battery.style.width = `${state.layout.batteryRow.width}px`;
    battery.style.height = `${state.layout.batteryRow.height}px`;
    battery.style.fontSize = `${state.layout.batteryRow.fontSize}px`;
    battery.style.gap = `${state.layout.batteryRow.gap}px`;
    icon.style.width = `${state.layout.batteryRow.iconSize}px`;
    icon.style.height = `${state.layout.batteryRow.iconSize}px`;

    stage.style.left = `${stageRect.x}px`;
    stage.style.top = `${stageRect.y}px`;
    stage.style.width = `${stageRect.width}px`;
    stage.style.height = `${stageRect.height}px`;

    group.style.left = `${startX}px`;
    group.style.top = `${startY}px`;
    group.style.width = `${digitRect.width}px`;
    group.style.height = `${digitRect.height}px`;

    layers.core.style.left = "0px";
    layers.core.style.width = `${digitRect.width}px`;
    layers.core.style.height = `${digitRect.height}px`;
    renderHourSurface(layers.coreSurface, layers.coreLeft, layers.coreRight, hour, digitRect);
    renderHourSurface(layers.leftOuterSurface, layers.leftOuterLeft, layers.leftOuterRight, hour, digitRect);
    renderHourSurface(layers.leftInnerSurface, layers.leftInnerLeft, layers.leftInnerRight, hour, digitRect);
    renderHourSurface(layers.rightInnerSurface, layers.rightInnerLeft, layers.rightInnerRight, hour, digitRect);
    renderHourSurface(layers.rightOuterSurface, layers.rightOuterLeft, layers.rightOuterRight, hour, digitRect);

    applySlice(layers.leftOuter, layers.leftOuterSurface, baseOverlap - outerShift, outerWidth, -cropInset, outerOpacity, digitRect);
    applySlice(layers.leftInner, layers.leftInnerSurface, baseOverlap - innerShift, innerWidth, -cropInset, innerOpacity, digitRect);
    applySlice(
      layers.rightInner,
      layers.rightInnerSurface,
      digitRect.width - innerWidth - baseOverlap + innerShift,
      innerWidth,
      -(digitRect.width - innerWidth - cropInset),
      innerOpacity,
      digitRect
    );
    applySlice(
      layers.rightOuter,
      layers.rightOuterSurface,
      digitRect.width - outerWidth - baseOverlap + outerShift,
      outerWidth,
      -(digitRect.width - outerWidth - cropInset),
      outerOpacity,
      digitRect
    );

    minuteWrap.style.left = `${state.layout.minuteLabel.x}px`;
    minuteWrap.style.top = `${state.layout.minuteLabel.y}px`;
    minuteWrap.style.width = `${state.layout.minuteLabel.width}px`;
    minuteWrap.style.height = `${state.layout.minuteLabel.height}px`;
    minute.style.fontSize = `${state.layout.minuteLabel.fontSize}px`;
    minute.textContent = "24";
  });
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
  renderAuditGrid();
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
