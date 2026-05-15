#include "App/UI/Watchface/WatchfaceStyle.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <string>

#include "App/UI/UiStyles.h"
#include "lvgl/src/misc/lv_fs.h"

namespace twsim::app {

namespace {

constexpr int kMinSpreadIndex = 0;
constexpr int kMaxSpreadIndex = 8;
constexpr lv_coord_t kDigitAssetCanvas = 220;
constexpr lv_coord_t kGlyphStageWidth = 242;
constexpr lv_coord_t kGlyphStageHeight = 280;
constexpr lv_coord_t kDigitTop = 2;
constexpr lv_coord_t kDigitVisibleTop = 0;
constexpr lv_coord_t kDigitVisibleHeight = 280;
constexpr lv_coord_t kSingleVisibleWidth = 110;
constexpr lv_coord_t kDualVisibleWidth = 160;
constexpr float kInnerSliceRatio = 0.31f;
constexpr float kOuterSliceRatio = 0.28f;
constexpr float kSliceInsetRatio = 0.18f;
constexpr lv_coord_t kInnerMaxShift = 4;
constexpr lv_coord_t kOuterMaxShift = 16;
constexpr lv_coord_t kBaseOverlap = 28;
constexpr lv_opa_t kInnerMaxOpacity = static_cast<lv_opa_t>(128);
constexpr lv_opa_t kOuterMaxOpacity = static_cast<lv_opa_t>(74);

struct DigitBounds {
  lv_coord_t x;
  lv_coord_t y;
  lv_coord_t width;
  lv_coord_t height;
};

constexpr std::array<DigitBounds, 10> kDigitBounds {{
    {73, 37, 75, 133},
    {88, 39, 37, 129},
    {72, 38, 76, 131},
    {70, 37, 76, 133},
    {69, 39, 84, 129},
    {74, 38, 73, 131},
    {74, 38, 73, 131},
    {73, 39, 72, 129},
    {71, 37, 79, 133},
    {74, 38, 73, 131},
}};

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

DigitBounds digit_bounds_for(int digit) {
  if (digit < 0 || digit > 9) {
    return {0, 0, kDigitAssetCanvas, kDigitAssetCanvas};
  }
  return kDigitBounds[static_cast<std::size_t>(digit)];
}

lv_coord_t round_coord(float value) {
  return static_cast<lv_coord_t>(value >= 0.0f ? value + 0.5f : value - 0.5f);
}

std::string sanitize_hour_text(const std::string& source) {
  std::string digits;
  digits.reserve(2);
  for (const unsigned char ch : source) {
    if (std::isdigit(ch) == 0) {
      continue;
    }
    digits.push_back(static_cast<char>(ch));
    if (digits.size() == 2) {
      break;
    }
  }
  return digits;
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

    group_root_ = lv_obj_create(root_);
    if (group_root_ == nullptr) {
      return nullptr;
    }
    ui_prepare_box(group_root_);
    lv_obj_set_size(group_root_, kGlyphStageWidth, kGlyphStageHeight);
    lv_obj_set_style_bg_opa(group_root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(group_root_, 0, 0);
    lv_obj_set_style_pad_all(group_root_, 0, 0);

    core_image_ = create_hour_surface(group_root_);
    if (core_image_ == nullptr) {
      return nullptr;
    }

    for (std::size_t index = 0; index < slice_slots_.size(); ++index) {
      slice_slots_[index] = create_slice_slot(group_root_);
      if (slice_slots_[index] == nullptr) {
        return nullptr;
      }
      slice_images_[index] = create_hour_surface(slice_slots_[index]);
      if (slice_images_[index] == nullptr) {
        return nullptr;
      }
    }

    return root_;
  }

  void apply(const WatchfaceRenderState& state) override {
    if (root_ == nullptr || empty_label_ == nullptr || group_root_ == nullptr || core_image_ == nullptr) {
      return;
    }

    const std::string hour_digits = sanitize_hour_text(state.hour_text);
    if (hour_digits.empty()) {
      lv_obj_clear_flag(empty_label_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(group_root_, LV_OBJ_FLAG_HIDDEN);
      return;
    }

    lv_obj_add_flag(empty_label_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(group_root_, LV_OBJ_FLAG_HIDDEN);

    const int left_digit = hour_digits[0] - '0';
    const int right_digit = hour_digits.size() > 1 ? (hour_digits[1] - '0') : -1;
    const char* left_asset = diffusion_digit_asset_path(left_digit);
    const char* right_asset = right_digit >= 0 ? diffusion_digit_asset_path(right_digit) : nullptr;
    if (!file_exists(left_asset) || (right_digit >= 0 && !file_exists(right_asset))) {
      lv_obj_clear_flag(empty_label_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(group_root_, LV_OBJ_FLAG_HIDDEN);
      return;
    }

    const int spread = clamp_spread_index(state.spread_index);
    const lv_coord_t stage_width = lv_obj_get_content_width(root_);
    const lv_coord_t start_x = static_cast<lv_coord_t>((stage_width - kGlyphStageWidth) / 2);
    const lv_coord_t inner_width = static_cast<lv_coord_t>(kGlyphStageWidth * kInnerSliceRatio);
    const lv_coord_t outer_width = static_cast<lv_coord_t>(kGlyphStageWidth * kOuterSliceRatio);
    const lv_coord_t crop_inset = static_cast<lv_coord_t>(kGlyphStageWidth * kSliceInsetRatio);
    const lv_coord_t inner_shift = static_cast<lv_coord_t>((kInnerMaxShift * spread) / kMaxSpreadIndex);
    const lv_coord_t outer_shift = static_cast<lv_coord_t>((kOuterMaxShift * spread) / kMaxSpreadIndex);
    const lv_opa_t inner_opacity = static_cast<lv_opa_t>((kInnerMaxOpacity * spread) / kMaxSpreadIndex);
    const lv_opa_t outer_opacity = static_cast<lv_opa_t>((kOuterMaxOpacity * spread) / kMaxSpreadIndex);
    lv_obj_set_pos(group_root_, start_x, kDigitTop);
    lv_obj_set_size(group_root_, kGlyphStageWidth, kGlyphStageHeight);

    apply_hour_pair(core_image_, left_asset, left_digit, right_asset, right_digit, LV_OPA_COVER);
    apply_hour_pair(slice_images_[0], left_asset, left_digit, right_asset, right_digit, outer_opacity);
    apply_hour_pair(slice_images_[1], left_asset, left_digit, right_asset, right_digit, inner_opacity);
    apply_hour_pair(slice_images_[2], left_asset, left_digit, right_asset, right_digit, inner_opacity);
    apply_hour_pair(slice_images_[3], left_asset, left_digit, right_asset, right_digit, outer_opacity);

    apply_slice(slice_slots_[0],
                slice_images_[0],
                kBaseOverlap - outer_shift,
                outer_width,
                -crop_inset,
                outer_opacity);
    apply_slice(slice_slots_[1],
                slice_images_[1],
                kBaseOverlap - inner_shift,
                inner_width,
                -crop_inset,
                inner_opacity);
    apply_slice(slice_slots_[2],
                slice_images_[2],
                static_cast<lv_coord_t>(kGlyphStageWidth - inner_width - kBaseOverlap + inner_shift),
                inner_width,
                static_cast<lv_coord_t>(-(kGlyphStageWidth - inner_width - crop_inset)),
                inner_opacity);
    apply_slice(slice_slots_[3],
                slice_images_[3],
                static_cast<lv_coord_t>(kGlyphStageWidth - outer_width - kBaseOverlap + outer_shift),
                outer_width,
                static_cast<lv_coord_t>(-(kGlyphStageWidth - outer_width - crop_inset)),
                outer_opacity);
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
  lv_obj_t* create_hour_surface(lv_obj_t* parent) {
    lv_obj_t* surface = lv_obj_create(parent);
    if (surface == nullptr) {
      return nullptr;
    }
    ui_prepare_box(surface);
    lv_obj_set_size(surface, kGlyphStageWidth, kGlyphStageHeight);
    lv_obj_set_style_bg_opa(surface, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(surface, 0, 0);
    lv_obj_set_style_pad_all(surface, 0, 0);
    lv_obj_set_style_radius(surface, 0, 0);
    lv_obj_set_scrollbar_mode(surface, LV_SCROLLBAR_MODE_OFF);
    return surface;
  }

  lv_obj_t* create_slice_slot(lv_obj_t* parent) {
    lv_obj_t* slot = lv_obj_create(parent);
    if (slot == nullptr) {
      return nullptr;
    }
    ui_prepare_box(slot);
    lv_obj_set_size(slot, 1, kGlyphStageHeight);
    lv_obj_set_style_bg_opa(slot, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(slot, 0, 0);
    lv_obj_set_style_clip_corner(slot, true, 0);
    return slot;
  }

  void apply_hour_pair(lv_obj_t* target,
                       const char* left_asset,
                       int left_digit,
                       const char* right_asset,
                       int right_digit,
                       lv_opa_t opacity) {
    if (target == nullptr) {
      return;
    }
    lv_obj_clean(target);

    const DigitBounds left_bounds = digit_bounds_for(left_digit);
    const DigitBounds right_bounds = digit_bounds_for(right_digit);
    const bool is_dual = right_digit >= 0;
    const lv_coord_t target_visible_width = is_dual ? kDualVisibleWidth : kSingleVisibleWidth;
    const lv_coord_t target_visible_height = kDigitVisibleHeight;

    const float left_scale_y = static_cast<float>(target_visible_height) / static_cast<float>(left_bounds.height);
    const float right_scale_y = static_cast<float>(target_visible_height) / static_cast<float>(right_bounds.height);
    const float normalized_total_width =
        static_cast<float>(left_bounds.width) * left_scale_y +
        (is_dual ? static_cast<float>(right_bounds.width) * right_scale_y : 0.0f);
    const float width_scale =
        normalized_total_width > 0.0f ? static_cast<float>(target_visible_width) / normalized_total_width : 1.0f;

    const lv_coord_t left_visible_width = round_coord(static_cast<float>(left_bounds.width) * left_scale_y * width_scale);
    const lv_coord_t right_visible_width = is_dual
                                               ? round_coord(static_cast<float>(right_bounds.width) * right_scale_y * width_scale)
                                               : 0;
    const lv_coord_t group_visible_width = left_visible_width + right_visible_width;
    const lv_coord_t group_left = static_cast<lv_coord_t>((kGlyphStageWidth - group_visible_width) / 2);
    const lv_coord_t top = kDigitVisibleTop;

    const lv_coord_t left_render_width = round_coord(static_cast<float>(kDigitAssetCanvas) * left_scale_y * width_scale);
    const lv_coord_t right_render_width = is_dual
                                              ? round_coord(static_cast<float>(kDigitAssetCanvas) * right_scale_y * width_scale)
                                              : 0;
    const lv_coord_t left_render_height = target_visible_height;
    const lv_coord_t right_render_height = target_visible_height;
    const lv_coord_t left_x = static_cast<lv_coord_t>(group_left - round_coord(static_cast<float>(left_bounds.x) * left_scale_y * width_scale));
    const lv_coord_t right_x = is_dual
                                   ? static_cast<lv_coord_t>(group_left + left_visible_width -
                                                             round_coord(static_cast<float>(right_bounds.x) * right_scale_y * width_scale))
                                   : 0;

    lv_obj_t* left = lv_image_create(target);
    if (left == nullptr) {
      return;
    }

    lv_image_set_src(left, left_asset);
    lv_image_set_inner_align(left, LV_IMAGE_ALIGN_STRETCH);
    lv_obj_set_pos(left, left_x, top);
    lv_obj_set_size(left, left_render_width, left_render_height);
    lv_obj_set_style_opa(left, opacity, 0);

    if (is_dual) {
      lv_obj_t* right = lv_image_create(target);
      if (right == nullptr) {
        return;
      }
      lv_image_set_src(right, right_asset);
      lv_image_set_inner_align(right, LV_IMAGE_ALIGN_STRETCH);
      lv_obj_set_pos(right, right_x, top);
      lv_obj_set_size(right, right_render_width, right_render_height);
      lv_obj_set_style_opa(right, opacity, 0);
    }
  }

  void apply_slice(lv_obj_t* slot,
                   lv_obj_t* image,
                   lv_coord_t slot_x,
                   lv_coord_t slot_width,
                   lv_coord_t image_x,
                   lv_opa_t opacity) {
    lv_obj_set_pos(slot, slot_x, 0);
    lv_obj_set_size(slot, slot_width, kGlyphStageHeight);
    lv_obj_set_style_opa(image, opacity, 0);
    lv_obj_set_pos(image, image_x, 0);
    lv_obj_set_size(image, kGlyphStageWidth, kGlyphStageHeight);
  }

  lv_obj_t* root_ {nullptr};
  lv_obj_t* empty_label_ {nullptr};
  lv_obj_t* group_root_ {nullptr};
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
