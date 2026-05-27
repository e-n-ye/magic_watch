#include "App/UI/Pages/ShellPages.h"

#include "App/UI/Pages/Shell/ShellPagePrimitives.h"

namespace twsim::app {

PassiveShellPage::PassiveShellPage(DataCenter& data_center, PageId page_id, const char* title, const char* detail)
    : PageBase(data_center), page_id_(page_id), title_(title), detail_(detail) {}

PageId PassiveShellPage::id() const {
  return page_id_;
}

const char* PassiveShellPage::name() const {
  return page_name(page_id_);
}

lv_obj_t* PassiveShellPage::build() {
  lv_obj_t* root = lv_obj_create(nullptr);
  if (root == nullptr) {
    return nullptr;
  }
  style_root(root, 0x000000);

  if (page_id_ == PageId::ScreenOff) {
    lv_obj_t* debug_hint = lv_label_create(root);
    if (debug_hint == nullptr) {
      return root;
    }
    lv_label_set_text(debug_hint, "screen off");
    lv_obj_set_style_text_font(debug_hint, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(debug_hint, lv_color_hex(0x101010), 0);
    lv_obj_align(debug_hint, LV_ALIGN_BOTTOM_MID, 0, -8);
    return root;
  }

  lv_obj_t* title = lv_label_create(root);
  lv_obj_t* detail = lv_label_create(root);
  if (title == nullptr || detail == nullptr) {
    return nullptr;
  }

  lv_label_set_text(title, title_);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -18);

  lv_label_set_text(detail, detail_);
  lv_obj_set_width(detail, 180);
  lv_label_set_long_mode(detail, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_font(detail, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(detail, lv_color_hex(0x94A3B8), 0);
  lv_obj_align(detail, LV_ALIGN_CENTER, 0, 24);
  return root;
}

}  // namespace twsim::app
