const presets = {
  "square-197": {
    watchWidth: 240,
    watchHeight: 296,
    safeInsetX: 14,
    safeInsetTop: 40,
    safeInsetBottom: 22,
    stageRadius: 28,
    cardGap: 12,
    heroRatio: 0.45,
    metricFontScale: 1,
  },
  "taller-alt": {
    watchWidth: 252,
    watchHeight: 312,
    safeInsetX: 15,
    safeInsetTop: 42,
    safeInsetBottom: 24,
    stageRadius: 30,
    cardGap: 12,
    heroRatio: 0.44,
    metricFontScale: 1.03,
  },
};

const root = document.documentElement;
const iconImage = document.getElementById("weather-icon-image");
const iconFallback = document.getElementById("weather-icon-fallback");
const iconStatus = document.getElementById("icon-status");

function setCssVar(name, value) {
  root.style.setProperty(name, value);
}

function applyPreset(name) {
  const preset = presets[name];
  if (!preset) {
    return;
  }

  const stageHeight = preset.watchHeight - preset.safeInsetTop - preset.safeInsetBottom;
  const innerStageHeight = stageHeight - preset.cardGap * 2;
  const heroHeight = Math.round((innerStageHeight - preset.cardGap) * preset.heroRatio);

  setCssVar("--watch-width", preset.watchWidth);
  setCssVar("--watch-height", preset.watchHeight);
  setCssVar("--safe-inset-x", preset.safeInsetX);
  setCssVar("--safe-inset-top", preset.safeInsetTop);
  setCssVar("--safe-inset-bottom", preset.safeInsetBottom);
  setCssVar("--stage-radius", preset.stageRadius);
  setCssVar("--card-gap", preset.cardGap);
  setCssVar("--hero-ratio", preset.heroRatio);
  setCssVar("--hero-height", heroHeight);
  setCssVar("--metric-font-scale", preset.metricFontScale);

  document.getElementById("spec-watch").textContent = `${preset.watchWidth} x ${preset.watchHeight}`;
  document.getElementById("spec-safe").textContent =
    `${preset.safeInsetX} / ${preset.safeInsetTop} / ${preset.safeInsetBottom}`;
  document.getElementById("spec-hero").textContent = `${Math.round(preset.heroRatio * 100)}%`;
  document.getElementById("spec-gap").textContent = `${preset.cardGap}px`;
  document.getElementById("spec-type").textContent = `${preset.metricFontScale.toFixed(2)}x`;

  document.querySelectorAll(".preset-button").forEach((button) => {
    button.classList.toggle("is-active", button.dataset.preset === name);
  });
}

function showFallbackIcon() {
  iconImage.classList.add("is-hidden");
  iconFallback.classList.remove("is-hidden");
  iconStatus.textContent = "Weather icon: fallback";
}

function showGeneratedIcon() {
  iconImage.classList.remove("is-hidden");
  iconFallback.classList.add("is-hidden");
  iconStatus.textContent = "Weather icon: generated asset";
}

document.querySelectorAll(".preset-button").forEach((button) => {
  button.addEventListener("click", () => applyPreset(button.dataset.preset));
});

iconImage.addEventListener("load", showGeneratedIcon);
iconImage.addEventListener("error", showFallbackIcon);

applyPreset("square-197");
