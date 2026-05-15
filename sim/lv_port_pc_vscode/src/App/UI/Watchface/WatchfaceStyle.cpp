#include "App/UI/Watchface/WatchfaceStyle.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <string>

#include "App/UI/UiStyles.h"
#include "lvgl/src/misc/lv_fs.h"

namespace twsim::app {

namespace {

constexpr int kMinSpreadIndex = 0;
constexpr int kMaxSpreadIndex = 8;
constexpr lv_coord_t kDigitSize = 162;
constexpr lv_coord_t kSliceHeight = 168;
constexpr lv_coord_t kInnerSliceWidth = 40;
constexpr lv_coord_t kOuterSliceWidth = 30;

bool has_text(const char* text) {
  return text != nullptr && text[0] != '\0';
}

std::string make_lvgl_stdio_path(const std::filesystem::path& absolute_path) {
  if (absolute_path.empty()) {
    return {};
  }
  return std::string("A:") + absolute_path.generic_string();
}

std::string resolve_lvgl_asset_path(const char* relative_asset_path) {
  if (!has_text(relative_asset_path)) {
    return {};
  }

  namespace fs = std::filesystem;
  const fs::path relative_path(relative_asset_path);
  const fs::path start = fs::current_path();
  const std::array<fs::path, 7> candidates {
      start,
      start / "..",
      start / ".." / "..",
      start / ".." / ".." / "..",
      start / ".." / ".." / ".." / "..",
      start / ".." / ".." / ".." / ".." / "..",
      fs::path("D:/MY_Desk/watch/magic_watch"),
  };

  for (const auto& base : candidates) {
    std::error_code ec;
    const fs::path normalized = fs::weakly_canonical(base / relative_path, ec);
    if (!ec && fs::exists(normalized)) {
      return make_lvgl_stdio_path(normalized);
    }
  }

  std::printf("[magic_watch] watchface asset not found: %s (cwd=%s)\n",
              relative_asset_path,
              start.generic_string().c_str());
  return {};
}

bool file_exists(const char* path) {
  if (!has_text(path)) {
    return false;
  }
  lv_fs_file_t file;
  if (lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) {
    return false;
  }
  lv_fs_close(&file);
  return true;
}

const char* diffusion_digit_asset_path(int digit) {
  static const std::array<std::string, 10> paths = []() {
    std::array<std::string, 10> generated {};
    for (std::size_t index = 0; index < generated.size(); ++index) {
      generated[index] = resolve_lvgl_asset_path(
          ("assets/watchface_digits/diffusion_png/" + std::to_string(index) + ".png").c_str());
    }
    return generated;
  }();

  if (digit < 0 || digit > 9) {
    return nullptr;
  }
  const auto& path = paths[static_cast<std::size_t>(digit)];
  return path.empty() ? nullptr : path.c_str();
}

int clamp_spread_index(int value) {
  return std::clamp(value, kMinSpreadIndex, kMaxSpreadIndex);
}

lv_coord_t lerp_coord(int spread_index, lv_coord_t minimum, lv_coord_t maximum) {
  const int clamped = clamp_spread_index(spread_index);
  return static_cast<lv_coord_t>(minimum + ((maximum - minimum) * clamped) / kMaxSpreadIndex);
}

lv_opa_t lerp_opa(int spread_index, lv_opa_t minimum, lv_opa_t maximum) {
  const int clamped = clamp_spread_index(spread_index);
  return static_cast<lv_opa_t>(minimum + ((maximum - minimum) * clamped) / kMaxSpreadIndex);
}

class DiffusionWatchfaceRenderer final : public IWatchfaceStyleRenderer {
 public:
  lv_obj_t* build(lv_obj_t* parent) override {
    root_ = lv_obj_create(parent);
    if (root_ == nullptr) {
      return nullptr;
    }
    ui_prepare_box(root_);
    lv_obj_set_size(root_, LV_PCT(100), LV_PCT(100));
    lv_obj_center(root_);
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);

    empty_label_ = lv_label_create(root_);
    if (empty_label_ == nullptr) {
      return nullptr;
    }
    ui_prepare_label(empty_label_);
    ui_apply_text(empty_label_, TextStyle::HeroSoft);
    lv_obj_set_style_text_font(empty_label_, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(empty_label_, lv_color_hex(0x64748B), 0);
    lv_label_set_text(empty_label_, "--");
    lv_obj_align(empty_label_, LV_ALIGN_CENTER, 0, -4);

    for (auto* slot : slice_slots_) {
      slot = nullptr;
    }
    for (auto* image : slice_images_) {
      image = nullptr;
    }

    core_image_ = create_digit_image(root_);
    if (core_image_ == nullptr) {
      return nullptr;
    }
    lv_obj_align(core_image_, LV_ALIGN_CENTER, 0, -4);

    slice_slots_[0] = create_slice_slot(root_, kOuterSliceWidth);
    slice_slots_[1] = create_slice_slot(root_, kInnerSliceWidth);
    slice_slots_[2] = create_slice_slot(root_, kInnerSliceWidth);
    slice_slots_[3] = create_slice_slot(root_, kOuterSliceWidth);
    for (auto* slot : slice_slots_) {
      if (slot == nullptr) {
        return nullptr;
      }
    }

    for (std::size_t index = 0; index < slice_slots_.size(); ++index) {
      slice_images_[index] = create_digit_image(slice_slots_[index]);
      if (slice_images_[index] == nullptr) {
        return nullptr;
      }
      lv_obj_align(slice_images_[index], LV_ALIGN_CENTER, 0, -4);
    }

    return root_;
  }

  void apply(const WatchfaceRenderState& state) override {
    if (root_ == nullptr || core_image_ == nullptr || empty_label_ == nullptr) {
      return;
    }

    const char* asset_path = diffusion_digit_asset_path(state.hour_digit);
    const bool valid_digit = file_exists(asset_path);
    if (!valid_digit) {
      lv_obj_clear_flag(empty_label_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(core_image_, LV_OBJ_FLAG_HIDDEN);
      for (auto* slot : slice_slots_) {
        if (slot != nullptr) {
          lv_obj_add_flag(slot, LV_OBJ_FLAG_HIDDEN);
        }
      }
      return;
    }

    lv_obj_add_flag(empty_label_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(core_image_, LV_OBJ_FLAG_HIDDEN);
    lv_image_set_src(core_image_, asset_path);
    lv_image_set_scale(core_image_, LV_SCALE_NONE);

    for (std::size_t index = 0; index < slice_images_.size(); ++index) {
      lv_obj_clear_flag(slice_slots_[index], LV_OBJ_FLAG_HIDDEN);
      lv_image_set_src(slice_images_[index], asset_path);
    }

    const int spread = clamp_spread_index(state.spread_index);
    const lv_coord_t inner_offset = lerp_coord(spread, 34, 58);
    const lv_coord_t outer_offset = lerp_coord(spread, 62, 92);
    const std::uint32_t inner_scale = static_cast<std::uint32_t>(LV_SCALE_NONE + spread * 5);
    const std::uint32_t outer_scale = static_cast<std::uint32_t>(LV_SCALE_NONE + spread * 8);

    lv_obj_align(slice_slots_[0], LV_ALIGN_CENTER, -outer_offset, -4);
    lv_obj_align(slice_slots_[1], LV_ALIGN_CENTER, -inner_offset, -4);
    lv_obj_align(slice_slots_[2], LV_ALIGN_CENTER, inner_offset, -4);
    lv_obj_align(slice_slots_[3], LV_ALIGN_CENTER, outer_offset, -4);

    lv_image_set_scale(slice_images_[0], outer_scale);
    lv_image_set_scale(slice_images_[1], inner_scale);
    lv_image_set_scale(slice_images_[2], inner_scale);
    lv_image_set_scale(slice_images_[3], outer_scale);

    lv_obj_set_style_opa(slice_images_[0], lerp_opa(spread, static_cast<lv_opa_t>(16), static_cast<lv_opa_t>(44)), 0);
    lv_obj_set_style_opa(slice_images_[1], lerp_opa(spread, static_cast<lv_opa_t>(24), static_cast<lv_opa_t>(78)), 0);
    lv_obj_set_style_opa(slice_images_[2], lerp_opa(spread, static_cast<lv_opa_t>(24), static_cast<lv_opa_t>(78)), 0);
    lv_obj_set_style_opa(slice_images_[3], lerp_opa(spread, static_cast<lv_opa_t>(16), static_cast<lv_opa_t>(44)), 0);
    lv_obj_set_style_opa(core_image_, LV_OPA_COVER, 0);
  }

  bool on_crown_delta(int delta, WatchfaceConfig& config) override {
    const int previous = config.spread_index;
    config.spread_index = clamp_spread_index(config.spread_index + delta);
    return previous != config.spread_index;
  }

  bool supports_spread_control() const override {
    return true;
  }

 private:
  lv_obj_t* create_digit_image(lv_obj_t* parent) {
    lv_obj_t* image = lv_image_create(parent);
    if (image == nullptr) {
      return nullptr;
    }
    lv_obj_set_size(image, kDigitSize, kDigitSize);
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
    return image;
  }

  lv_obj_t* create_slice_slot(lv_obj_t* parent, lv_coord_t width) {
    lv_obj_t* slot = lv_obj_create(parent);
    if (slot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(slot);
    lv_obj_set_size(slot, width, kSliceHeight);
    lv_obj_set_style_bg_opa(slot, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(slot, 0, 0);
    lv_obj_set_style_clip_corner(slot, true, 0);
    return slot;
  }

  lv_obj_t* root_ {nullptr};
  lv_obj_t* empty_label_ {nullptr};
  lv_obj_t* core_image_ {nullptr};
  std::array<lv_obj_t*, 4> slice_slots_ {};
  std::array<lv_obj_t*, 4> slice_images_ {};
};

class ReservedWatchfaceRenderer final : public IWatchfaceStyleRenderer {
 public:
  explicit ReservedWatchfaceRenderer(const char* label_text) : label_text_(label_text) {}

  lv_obj_t* build(lv_obj_t* parent) override {
    root_ = lv_obj_create(parent);
    if (root_ == nullptr) {
      return nullptr;
    }
    ui_prepare_box(root_);
    lv_obj_set_size(root_, LV_PCT(100), LV_PCT(100));
    lv_obj_center(root_);
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);

    label_ = lv_label_create(root_);
    if (label_ == nullptr) {
      return nullptr;
    }
    ui_prepare_label(label_);
    ui_apply_text(label_, TextStyle::HeroSoft);
    lv_obj_set_width(label_, 180);
    lv_obj_set_style_text_align(label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label_, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label_, lv_color_hex(0x94A3B8), 0);
    lv_label_set_text(label_, label_text_);
    lv_obj_center(label_);
    return root_;
  }

  void apply(const WatchfaceRenderState&) override {}

  bool on_crown_delta(int, WatchfaceConfig&) override {
    return false;
  }

  bool supports_spread_control() const override {
    return false;
  }

 private:
  const char* label_text_ {nullptr};
  lv_obj_t* root_ {nullptr};
  lv_obj_t* label_ {nullptr};
};

}  // namespace

std::unique_ptr<IWatchfaceStyleRenderer> create_watchface_style_renderer(const WatchfaceConfig& config) {
  switch (config.style_id) {
    case WatchfaceStyleId::Diffusion:
      return std::make_unique<DiffusionWatchfaceRenderer>();
    case WatchfaceStyleId::CoolDigitsReserved:
    default:
      return std::make_unique<ReservedWatchfaceRenderer>("Cool Digits\nReserved");
  }
}

WatchfaceConfig default_watchface_config() {
  return {};
}

const char* watchface_style_name(WatchfaceStyleId style_id) {
  switch (style_id) {
    case WatchfaceStyleId::Diffusion:
      return "Diffusion";
    case WatchfaceStyleId::CoolDigitsReserved:
      return "CoolDigitsReserved";
    default:
      return "Unknown";
  }
}

}  // namespace twsim::app
