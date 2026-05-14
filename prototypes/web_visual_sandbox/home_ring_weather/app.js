const presets = {
  "xiaomi-197": {
    watchWidth: 240,
    watchHeight: 296,
    safeInsetX: 10,
    safeInsetTop: 16,
    safeInsetBottom: 10,
    stageRadius: 28,
    cardGap: 8,
    heroRatio: 1,
    metricFontScale: 1,
  },
  "taller-alt": {
    watchWidth: 252,
    watchHeight: 312,
    safeInsetX: 11,
    safeInsetTop: 18,
    safeInsetBottom: 12,
    stageRadius: 29,
    cardGap: 9,
    heroRatio: 1,
    metricFontScale: 1.02,
  },
};

const root = document.documentElement;
const iconBindings = [
  {
    image: document.getElementById("weather-icon-image"),
    fallback: document.getElementById("weather-icon-fallback"),
    status: document.getElementById("weather-icon-status"),
    label: "Weather",
  },
  {
    image: document.getElementById("sleep-icon-image"),
    fallback: document.getElementById("sleep-icon-fallback"),
    status: document.getElementById("sleep-icon-status"),
    label: "Sleep",
  },
  {
    image: document.getElementById("steps-icon-image"),
    fallback: document.getElementById("steps-icon-fallback"),
    status: document.getElementById("steps-icon-status"),
    label: "Steps",
  },
];

function setCssVar(name, value) {
  root.style.setProperty(name, value);
}

function applyPreset(name) {
  const preset = presets[name];
  if (!preset) {
    return;
  }

  const stageHeight = preset.watchHeight - preset.safeInsetTop - preset.safeInsetBottom;
  const heroHeight = Math.round((preset.watchWidth - preset.safeInsetX * 2 - preset.cardGap) / 2);

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

document.querySelectorAll(".preset-button").forEach((button) => {
  button.addEventListener("click", () => applyPreset(button.dataset.preset));
});

for (const binding of iconBindings) {
  const showFallback = () => {
    binding.image.classList.add("is-hidden");
    binding.fallback.classList.remove("is-hidden");
    binding.status.textContent = `${binding.label} icon: fallback`;
  };

  const showGenerated = () => {
    binding.image.classList.remove("is-hidden");
    binding.fallback.classList.add("is-hidden");
    binding.status.textContent = `${binding.label} icon: generated asset`;
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

applyPreset("xiaomi-197");
