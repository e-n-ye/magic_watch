#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Shell/ShellAssetHelpers.h"
#include "App/UI/Pages/Shell/ShellFontHelpers.h"
#include "App/UI/Pages/Shell/ShellPagePrimitives.h"
#include "App/UI/UiStyles.h"

#include <algorithm>
#include <array>
#include <cstdint>

namespace twsim::app {
namespace {

using shell_asset::file_exists;
using shell_asset::weather_icon_asset_path;
using shell_font::cjk_font_14;
using shell_font::cjk_font_16;
using shell_font::cjk_font_72;

constexpr lv_coord_t kWeatherSectionGap = 10;
constexpr std::int32_t kWeatherSectionCount = 5;

lv_obj_t* create_weather_label(lv_obj_t* parent,
                               const char* text,
                               const lv_font_t* font,
                               std::uint32_t color,
                               lv_coord_t width,
                               lv_label_long_mode_t long_mode = LV_LABEL_LONG_DOT) {
  lv_obj_t* label = lv_label_create(parent);
  if (label == nullptr) {
    return nullptr;
  }
  ui_prepare_label(label);
  lv_obj_set_width(label, width);
  lv_label_set_long_mode(label, long_mode);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_text(label, text);
  return label;
}

lv_obj_t* create_weather_panel(lv_obj_t* parent,
                               lv_coord_t width,
                               lv_coord_t height,
                               std::uint32_t bg,
                               std::uint32_t border,
                               lv_coord_t radius) {
  lv_obj_t* panel = lv_obj_create(parent);
  if (panel == nullptr) {
    return nullptr;
  }
  ui_prepare_box(panel);
  ui_apply_surface(panel, SurfaceStyle::PanelSubtle);
  lv_obj_set_size(panel, width, height);
  lv_obj_set_style_bg_color(panel, lv_color_hex(bg), 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(border), 0);
  lv_obj_set_style_border_width(panel, 1, 0);
  lv_obj_set_style_radius(panel, radius, 0);
  lv_obj_set_style_shadow_width(panel, 0, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_TRANSP, 0);
  return panel;
}

lv_obj_t* create_weather_section(lv_obj_t* parent,
                                 const char* title,
                                 const char* time_text,
                                 lv_coord_t width,
                                 lv_coord_t height) {
  lv_obj_t* section = create_weather_panel(parent, width, height, 0x071E3A, 0x12385F, 30);
  if (section == nullptr) {
    return nullptr;
  }
  lv_obj_remove_flag(section, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* title_label = create_weather_label(section, title, cjk_font_16(), 0xF8FAFC, 112);
  lv_obj_t* time_label = create_weather_label(section, time_text, &lv_font_montserrat_16, 0xD8E9FF, 72);
  if (title_label == nullptr || time_label == nullptr) {
    return nullptr;
  }
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 18, 16);
  lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -18, 16);
  return section;
}

lv_obj_t* create_weather_horizontal_row(lv_obj_t* parent, lv_coord_t width, lv_coord_t height) {
  lv_obj_t* row = lv_obj_create(parent);
  if (row == nullptr) {
    return nullptr;
  }
  ui_prepare_box(row);
  lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_set_size(row, width, height);
  lv_obj_set_scroll_dir(row, LV_DIR_HOR);
  lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  ui_set_flex_row(row, 0, 8, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  return row;
}

void create_weather_mini_icon(lv_obj_t* parent, lv_coord_t x, lv_coord_t y, lv_coord_t size) {
  const char* weather_icon_path = weather_icon_asset_path();
  if (file_exists(weather_icon_path)) {
    lv_obj_t* image = lv_image_create(parent);
    if (image == nullptr) {
      return;
    }
    lv_obj_set_size(image, size, size);
    lv_image_set_src(image, weather_icon_path);
    lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_align(image, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    return;
  }

  lv_obj_t* sun = lv_obj_create(parent);
  lv_obj_t* cloud = lv_obj_create(parent);
  if (sun == nullptr || cloud == nullptr) {
    return;
  }
  for (lv_obj_t* part : {sun, cloud}) {
    ui_prepare_box(part);
    lv_obj_remove_flag(part, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_border_width(part, 0, 0);
  }
  lv_obj_set_size(sun, size / 2, size / 2);
  lv_obj_set_style_bg_color(sun, lv_color_hex(0xFFE082), 0);
  lv_obj_set_style_bg_opa(sun, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(sun, LV_RADIUS_CIRCLE, 0);
  lv_obj_align(sun, LV_ALIGN_TOP_LEFT, x + size / 3, y + 2);

  lv_obj_set_size(cloud, size, size / 2);
  lv_obj_set_style_bg_color(cloud, lv_color_hex(0xEAF6FF), 0);
  lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(cloud, size / 4, 0);
  lv_obj_align(cloud, LV_ALIGN_TOP_LEFT, x, y + size / 3);
}

}  // namespace

WeatherAppPage::WeatherAppPage(DataCenter& data_center) : PageBase(data_center) {}

PageId WeatherAppPage::id() const {
  return PageId::AppWeather;
}

const char* WeatherAppPage::name() const {
  return page_name(PageId::AppWeather);
}

lv_obj_t* WeatherAppPage::build() {
  struct HourForecast {
    const char* time;
    const char* temp;
    const char* wind;
    const char* air;
  };

  struct DayForecast {
    const char* day;
    const char* temp;
    const char* weather;
  };

  struct WeatherIndex {
    const char* value;
    const char* label;
    std::uint32_t accent;
  };

  static const std::array<HourForecast, 8> kHourly {{
      {"\xE7\x8E\xB0\xE5\x9C\xA8", "28C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE4\xBC\x98"},
      {"12:00", "26C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE8\x89\xAF"},
      {"13:00", "26C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE8\x89\xAF"},
      {"14:00", "27C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE8\x89\xAF"},
      {"15:00", "28C", "\xE8\xA5\xBF\xE9\xA3\x8E 2", "\xE8\x89\xAF"},
      {"16:00", "27C", "\xE8\xA5\xBF\xE9\xA3\x8E 2", "\xE8\x89\xAF"},
      {"17:00", "26C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE4\xBC\x98"},
      {"18:00", "25C", "\xE8\xA5\xBF\xE9\xA3\x8E 1", "\xE4\xBC\x98"},
  }};

  static const std::array<DayForecast, 7> kDaily {{
      {"\xE4\xBB\x8A\xE5\xA4\xA9", "28/22", "\xE5\xB0\x8F\xE9\x9B\xA8"},
      {"\xE6\x98\x8E\xE5\xA4\xA9", "26/22", "\xE5\xB0\x8F\xE9\x9B\xA8"},
      {"\xE5\x91\xA8\xE5\x85\xAD", "31/23", "\xE9\x98\xB5\xE9\x9B\xA8"},
      {"\xE5\x91\xA8\xE6\x97\xA5", "29/24", "\xE5\xA4\x9A\xE4\xBA\x91"},
      {"\xE5\x91\xA8\xE4\xB8\x80", "33/25", "\xE5\xA4\x9A\xE4\xBA\x91"},
      {"\xE5\x91\xA8\xE4\xBA\x8C", "32/24", "\xE9\x98\xB4"},
      {"\xE5\x91\xA8\xE4\xB8\x89", "30/23", "\xE5\xB0\x8F\xE9\x9B\xA8"},
  }};

  static const std::array<WeatherIndex, 4> kIndices {{
      {"46", "\xE7\xA9\xBA\xE6\xB0\x94\xE8\xB4\xA8\xE9\x87\x8F", 0xA5E9FF},
      {"79", "\xE7\x9B\xB8\xE5\xAF\xB9\xE6\xB9\xBF\xE5\xBA\xA6", 0xBFE7FF},
      {"1", "\xE8\xA5\xBF\xE9\xA3\x8E", 0xD8E9FF},
      {"1", "\xE7\xB4\xAB\xE5\xA4\x96\xE7\xBA\xBF\xE6\x8C\x87\xE6\x95\xB0", 0xFFE082},
  }};

  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x02070D);

  const lv_coord_t screen_w = static_cast<lv_coord_t>(lv_display_get_horizontal_resolution(nullptr));
  const lv_coord_t screen_h = static_cast<lv_coord_t>(lv_display_get_vertical_resolution(nullptr));
  const lv_coord_t section_w = std::min<lv_coord_t>(screen_w - 16, 224);
  const lv_coord_t section_h = screen_h - 16;
  page_pitch_ = section_h + kWeatherSectionGap;

  scroll_root_ = lv_obj_create(root);
  if (scroll_root_ == nullptr) {
    return nullptr;
  }
  ui_prepare_box(scroll_root_);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_add_flag(scroll_root_, LV_OBJ_FLAG_SCROLL_ONE);
  lv_obj_set_size(scroll_root_, screen_w, screen_h);
  lv_obj_set_scroll_dir(scroll_root_, LV_DIR_VER);
  lv_obj_set_scroll_snap_y(scroll_root_, LV_SCROLL_SNAP_START);
  lv_obj_set_scrollbar_mode(scroll_root_, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_bg_opa(scroll_root_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(scroll_root_, 0, 0);
  ui_set_flex_column(scroll_root_, 8, kWeatherSectionGap);
  lv_obj_set_style_pad_bottom(scroll_root_, 8, 0);

  lv_obj_t* current = create_weather_section(
      scroll_root_, "\xE9\xBE\x99\xE6\xB9\xBE", "11:11", section_w, section_h);
  lv_obj_t* hourly = create_weather_section(
      scroll_root_, "\xE5\xA4\xA9\xE6\xB0\x94\xE9\xA2\x84\xE6\xB5\x8B", "11:10", section_w, section_h);
  lv_obj_t* daily = create_weather_section(
      scroll_root_, "\xE6\x9C\xAA\xE6\x9D\xA5\xE5\xA4\xA9\xE6\xB0\x94", "11:11", section_w, section_h);
  lv_obj_t* indices = create_weather_section(
      scroll_root_, "\xE5\xA4\xA9\xE6\xB0\x94\xE6\x8C\x87\xE6\x95\xB0", "11:11", section_w, section_h);
  lv_obj_t* sunrise = create_weather_section(
      scroll_root_, "\xE6\x97\xA5\xE5\x8D\x87\xE6\x97\xA5\xE8\x90\xBD", "11:11", section_w, section_h);
  if (current == nullptr || hourly == nullptr || daily == nullptr || indices == nullptr || sunrise == nullptr) {
    return nullptr;
  }

  {
    lv_obj_t* temp = create_weather_label(current, "28", cjk_font_72(), 0xF8FAFC, 128);
    lv_obj_t* degree = create_weather_label(current, "C", &lv_font_montserrat_34, 0xE8F5FF, 26);
    lv_obj_t* updated =
        create_weather_label(current, "\xE5\x88\x9A\xE5\x88\x9A\xE6\x9B\xB4\xE6\x96\xB0", cjk_font_14(), 0xB9D7F2, 96);
    lv_obj_t* condition =
        create_weather_label(current, "\xE5\xB0\x8F\xE9\x9B\xA8", cjk_font_16(), 0xF8FAFC, 100);
    lv_obj_t* range = create_weather_label(current, "28C / 22C", &lv_font_montserrat_20, 0xD8E9FF, 130);
    lv_obj_t* chip = create_weather_panel(current, 128, 28, 0x143B63, 0x275F8C, 14);
    lv_obj_t* air = chip == nullptr
                        ? nullptr
                        : create_weather_label(chip,
                                               "\xE7\xA9\xBA\xE6\xB0\x94\xE8\xB4\xA8\xE9\x87\x8F  \xE4\xBC\x98",
                                               cjk_font_16(),
                                               0xD8E9FF,
                                               112);
    if (temp == nullptr || degree == nullptr || updated == nullptr || condition == nullptr || range == nullptr ||
        air == nullptr || chip == nullptr) {
      return nullptr;
    }
    lv_obj_align(updated, LV_ALIGN_TOP_LEFT, 18, 42);
    lv_obj_align(temp, LV_ALIGN_TOP_MID, -24, 84);
    lv_obj_align_to(degree, temp, LV_ALIGN_OUT_RIGHT_TOP, -6, 12);
    lv_obj_align(condition, LV_ALIGN_TOP_MID, 0, 154);
    lv_obj_align(range, LV_ALIGN_TOP_MID, 0, 184);
    lv_obj_align(chip, LV_ALIGN_BOTTOM_MID, 0, -22);
    lv_obj_align(air, LV_ALIGN_CENTER, 0, 0);
  }

  {
    lv_obj_t* row = create_weather_horizontal_row(hourly, section_w - 28, 156);
    if (row == nullptr) {
      return nullptr;
    }
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -12);

    for (const auto& item : kHourly) {
      lv_obj_t* card = create_weather_panel(row, 62, 142, 0x0D3158, 0x1A527F, 20);
      if (card == nullptr) {
        return nullptr;
      }
      lv_obj_t* time = create_weather_label(card, item.time, cjk_font_14(), 0xE8F5FF, 54);
      lv_obj_t* temp = create_weather_label(card, item.temp, &lv_font_montserrat_20, 0xF8FAFC, 54);
      lv_obj_t* wind = create_weather_label(card, item.wind, cjk_font_14(), 0xCDE6FF, 54);
      lv_obj_t* air = create_weather_label(card, item.air, cjk_font_14(), 0xD7FBE8, 54);
      if (time == nullptr || temp == nullptr || wind == nullptr || air == nullptr) {
        return nullptr;
      }
      lv_obj_align(time, LV_ALIGN_TOP_MID, 0, 10);
      lv_obj_align(temp, LV_ALIGN_TOP_MID, 0, 38);
      create_weather_mini_icon(card, 16, 66, 30);
      lv_obj_align(wind, LV_ALIGN_TOP_MID, 0, 102);
      lv_obj_align(air, LV_ALIGN_TOP_MID, 0, 122);
    }
  }

  {
    lv_obj_t* row = create_weather_horizontal_row(daily, section_w - 28, 158);
    if (row == nullptr) {
      return nullptr;
    }
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -12);

    for (const auto& item : kDaily) {
      lv_obj_t* card = create_weather_panel(row, 54, 144, 0x0D3158, 0x1A527F, 18);
      if (card == nullptr) {
        return nullptr;
      }
      lv_obj_t* day = create_weather_label(card, item.day, cjk_font_14(), 0xF8FAFC, 46);
      lv_obj_t* weather = create_weather_label(card, item.weather, cjk_font_14(), 0xCDE6FF, 46);
      lv_obj_t* temp = create_weather_label(card, item.temp, &lv_font_montserrat_16, 0xF8FAFC, 48);
      if (day == nullptr || weather == nullptr || temp == nullptr) {
        return nullptr;
      }
      lv_obj_align(day, LV_ALIGN_TOP_MID, 0, 12);
      create_weather_mini_icon(card, 13, 44, 30);
      lv_obj_align(weather, LV_ALIGN_TOP_MID, 0, 82);
      lv_obj_align(temp, LV_ALIGN_TOP_MID, 0, 112);
    }
  }

  {
    for (std::size_t index = 0; index < kIndices.size(); ++index) {
      const auto& item = kIndices[index];
      const lv_coord_t x = static_cast<lv_coord_t>(14 + (index % 2) * 100);
      const lv_coord_t y = static_cast<lv_coord_t>(60 + (index / 2) * 78);
      lv_obj_t* card = create_weather_panel(indices, 92, 68, 0x0D3158, 0x1A527F, 18);
      lv_obj_t* value = create_weather_label(card, item.value, &lv_font_montserrat_34, 0xF8FAFC, 62);
      lv_obj_t* marker = lv_obj_create(card);
      lv_obj_t* label = create_weather_label(card, item.label, cjk_font_14(), 0xCDE6FF, 84);
      if (card == nullptr || value == nullptr || marker == nullptr || label == nullptr) {
        return nullptr;
      }
      lv_obj_align(card, LV_ALIGN_TOP_LEFT, x, y);
      lv_obj_align(value, LV_ALIGN_TOP_LEFT, 8, 7);
      ui_prepare_box(marker);
      lv_obj_set_size(marker, 8, 8);
      lv_obj_set_style_bg_color(marker, lv_color_hex(item.accent), 0);
      lv_obj_set_style_bg_opa(marker, LV_OPA_COVER, 0);
      lv_obj_set_style_radius(marker, LV_RADIUS_CIRCLE, 0);
      lv_obj_remove_flag(marker, LV_OBJ_FLAG_CLICKABLE);
      lv_obj_align(marker, LV_ALIGN_TOP_RIGHT, -18, 18);
      lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, 8, -8);
    }
  }

  {
    static const lv_point_precise_t kSunPath[] = {{28, 112}, {58, 72}, {96, 52}, {134, 72}, {164, 112}};
    static const lv_point_precise_t kHorizon[] = {{22, 112}, {170, 112}};

    lv_obj_t* graph = create_weather_panel(sunrise, section_w - 38, 126, 0x0D3158, 0x1A527F, 22);
    if (graph == nullptr) {
      return nullptr;
    }
    lv_obj_align(graph, LV_ALIGN_TOP_MID, 0, 58);

    lv_obj_t* sun_path = lv_line_create(graph);
    lv_obj_t* horizon = lv_line_create(graph);
    lv_obj_t* sun = lv_obj_create(graph);
    lv_obj_t* rise = create_weather_label(sunrise, "\xE6\x97\xA5\xE5\x87\xBA  05:05", cjk_font_16(), 0xF8FAFC, 150);
    lv_obj_t* set = create_weather_label(sunrise, "\xE6\x97\xA5\xE8\x90\xBD  18:42", cjk_font_16(), 0xF8FAFC, 150);
    if (sun_path == nullptr || horizon == nullptr || sun == nullptr || rise == nullptr || set == nullptr) {
      return nullptr;
    }

    lv_line_set_points(sun_path, kSunPath, 5);
    lv_obj_set_size(sun_path, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_line_width(sun_path, 5, 0);
    lv_obj_set_style_line_color(sun_path, lv_color_hex(0x9BD7FF), 0);
    lv_obj_set_style_line_opa(sun_path, LV_OPA_70, 0);
    lv_obj_set_style_line_rounded(sun_path, true, 0);

    lv_line_set_points(horizon, kHorizon, 2);
    lv_obj_set_size(horizon, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_line_width(horizon, 2, 0);
    lv_obj_set_style_line_color(horizon, lv_color_hex(0xD8E9FF), 0);
    lv_obj_set_style_line_opa(horizon, LV_OPA_50, 0);

    ui_prepare_box(sun);
    lv_obj_set_size(sun, 22, 22);
    lv_obj_set_style_bg_color(sun, lv_color_hex(0xF8FAFC), 0);
    lv_obj_set_style_bg_opa(sun, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(sun, 0, 0);
    lv_obj_set_style_radius(sun, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(sun, LV_ALIGN_TOP_LEFT, 86, 48);

    lv_obj_align(rise, LV_ALIGN_BOTTOM_MID, 0, -38);
    lv_obj_align(set, LV_ALIGN_BOTTOM_MID, 0, -14);
  }

  bind_input();
  return root;
}

void WeatherAppPage::bind_input() {
  track(data_center_.subscribe(EventId::InputRequested,
                               [this](const Event& event) {
                                 if (root_ == nullptr || lv_screen_active() != root_ || scroll_root_ == nullptr) {
                                   return;
                                 }

                                 const auto* command = std::get_if<InputCommand>(&event.payload);
                                 if (command == nullptr) {
                                   return;
                                 }

                                 const std::int32_t current_y = lv_obj_get_scroll_y(scroll_root_);
                                 const std::int32_t page_pitch = page_pitch_ > 0 ? page_pitch_ : 1;
                                 const std::int32_t detents = std::max<std::int16_t>(1, command->value);
                                 auto scroll_to_page = [&](bool forward) {
                                   const std::int32_t current_index = forward
                                                                          ? current_y / page_pitch
                                                                          : (current_y + page_pitch - 1) / page_pitch;
                                   const std::int32_t target_index =
                                       std::clamp(current_index + (forward ? detents : -detents),
                                                  0,
                                                  kWeatherSectionCount - 1);
                                   lv_obj_scroll_to_y(scroll_root_, target_index * page_pitch, LV_ANIM_ON);
                                 };
                                 switch (command->action) {
                                   case InputAction::CrownRotateCW:
                                     scroll_to_page(true);
                                     break;
                                   case InputAction::CrownRotateCCW:
                                     scroll_to_page(false);
                                     break;
                                   default:
                                     break;
                                 }
                               }));
}

}  // namespace twsim::app
