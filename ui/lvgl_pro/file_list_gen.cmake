set(LV_EDITOR_PROJECT_SOURCES)
list(
  APPEND
  LV_EDITOR_PROJECT_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/components/health_shortcut_card_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/magic_watch_ui_gen.c
  ${CMAKE_CURRENT_LIST_DIR}/magic_watch_ui.c
  ${CMAKE_CURRENT_LIST_DIR}/screens/screen_health_shortcuts_gen.c)