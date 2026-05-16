const assetsBase = "../../../assets/generated_icons/";

const state = {
  topInset: 10,
  bottomInset: 10,
  tileSize: 71,
  iconSize: 60,
  rowGap: 4,
  colGap: 2,
  items: [
    { id: "settings", label: "Settings", bg: "#201c22", fallback: "⚙", asset: null, fg: "#c7ced8" },
    { id: "weather", label: "Weather", bg: "#1fa3e5", asset: `${assetsBase}weather_cloud_sun.png` },
    { id: "steps", label: "Steps", bg: "#2a5a2e", asset: `${assetsBase}steps_foot.png` },
    { id: "heart", label: "Heart", bg: "#fafafa", asset: `${assetsBase}health_heart.png` },
    { id: "spo2", label: "SpO2", bg: "#26d9d6", asset: `${assetsBase}health_spo2.png` },
    { id: "sleep", label: "Sleep", bg: "#9162c0", asset: `${assetsBase}sleep_bed.png` },
    { id: "stress", label: "Stress", bg: "#23433a", asset: `${assetsBase}health_stress.png` },
    { id: "breathe", label: "Breathe", bg: "#8f8f8f", asset: `${assetsBase}health_breathe.png` },
    { id: "nfc", label: "NFC", bg: "#8e9b50", fallback: "卡", asset: null, fg: "#f8fafc" },
    { id: "alipay", label: "Alipay", bg: "#008cff", asset: `${assetsBase}payment_alipay.png` },
    { id: "wechat", label: "WeChat", bg: "#ebebeb", asset: `${assetsBase}payment_wechat_green.png` }
  ]
};

const grid = document.getElementById("launcherGrid");
const paletteList = document.getElementById("paletteList");
const exportBox = document.getElementById("exportBox");

function renderGrid() {
  grid.innerHTML = "";
  document.documentElement.style.setProperty("--top-inset", `${state.topInset}px`);
  document.documentElement.style.setProperty("--bottom-inset", `${state.bottomInset}px`);
  document.documentElement.style.setProperty("--tile-size", `${state.tileSize}px`);
  document.documentElement.style.setProperty("--icon-size", `${state.iconSize}px`);
  document.documentElement.style.setProperty("--row-gap", `${state.rowGap}px`);
  document.documentElement.style.setProperty("--col-gap", `${state.colGap}px`);

  state.items.forEach((item) => {
    const tile = document.createElement("div");
    tile.className = "launcher-tile";

    const icon = document.createElement("div");
    icon.className = "launcher-icon";
    icon.style.background = item.bg;

    if (item.asset) {
      const img = document.createElement("img");
      img.src = item.asset;
      img.alt = item.label;
      icon.appendChild(img);
    } else {
      const fallback = document.createElement("span");
      fallback.className = "launcher-fallback";
      fallback.textContent = item.fallback ?? "?";
      fallback.style.color = item.fg ?? "#f8fafc";
      icon.appendChild(fallback);
    }

    tile.appendChild(icon);
    grid.appendChild(tile);
  });
}

function exportState() {
  exportBox.value = JSON.stringify(
    {
      layout: {
        topInset: state.topInset,
        bottomInset: state.bottomInset,
        tileSize: state.tileSize,
        iconSize: state.iconSize,
        rowGap: state.rowGap,
        colGap: state.colGap
      },
      items: state.items.map(({ id, label, bg }) => ({ id, label, bg }))
    },
    null,
    2
  );
}

function bindRange(id, key) {
  const input = document.getElementById(id);
  input.value = state[key];
  input.addEventListener("input", () => {
    state[key] = Number(input.value);
    renderGrid();
    exportState();
  });
}

function renderPalette() {
  paletteList.innerHTML = "";
  state.items.forEach((item, index) => {
    const row = document.createElement("label");
    row.className = "palette-row";

    const title = document.createElement("span");
    title.textContent = item.label;

    const input = document.createElement("input");
    input.type = "color";
    input.value = item.bg;
    input.addEventListener("input", () => {
      state.items[index].bg = input.value;
      swatch.style.background = input.value;
      renderGrid();
      exportState();
    });

    const swatch = document.createElement("span");
    swatch.className = "swatch-preview";
    swatch.style.background = item.bg;

    row.appendChild(title);
    row.appendChild(input);
    row.appendChild(swatch);
    paletteList.appendChild(row);
  });
}

bindRange("topInset", "topInset");
bindRange("bottomInset", "bottomInset");
bindRange("tileSize", "tileSize");
bindRange("iconSize", "iconSize");
bindRange("rowGap", "rowGap");
bindRange("colGap", "colGap");

renderPalette();
renderGrid();
exportState();
