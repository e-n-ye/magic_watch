#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_BUFFER_SIZE 64U
#define IDLE_SLEEP_SECONDS 10U

typedef enum {
    PAGE_HOME = 0,
    PAGE_APP_MENU,
    PAGE_HEART,
    PAGE_STEPS,
    PAGE_ABOUT,
    PAGE_NUM,
} page_t;

typedef enum {
    MENU_HEART = 0,
    MENU_STEPS,
    MENU_ABOUT,
    MENU_COUNT,
} menu_item_t;

typedef enum {
    SHARED_ACTION_NONE = 0,
    SHARED_ACTION_OPEN_MENU,
    SHARED_ACTION_OPEN_HEART_SHORTCUT,
    SHARED_ACTION_OPEN_STEPS_SHORTCUT,
    SHARED_ACTION_MOVE_NEXT,
    SHARED_ACTION_MOVE_PREV,
    SHARED_ACTION_ACTIVATE_SELECTION,
    SHARED_ACTION_NAV_BACK,
} shared_action_t;

typedef enum {
    REPLAY_STEP_OPEN_MENU = 0,
    REPLAY_STEP_MOVE_NEXT,
    REPLAY_STEP_ACTIVATE_SELECTION,
    REPLAY_STEP_NAV_BACK,
} replay_step_t;

typedef enum {
    POWER_ACTIVE = 0,
    POWER_SLEEP,
} power_state_t;

typedef enum {
    APP_EVENT_NONE = 0,
    APP_EVENT_USER_ACTION,
    APP_EVENT_TICK_1S,
    APP_EVENT_IDLE_TIMEOUT,
    APP_EVENT_BATTERY_CHANGED,
} app_event_type_t;

typedef struct {
    app_event_type_t type;
    shared_action_t action;
    unsigned int battery_percent;
} app_event_t;

static page_t s_current_page = PAGE_HOME;
static page_t s_detail_return_page = PAGE_HOME;
static menu_item_t s_menu_selection = MENU_HEART;
static power_state_t s_power_state = POWER_ACTIVE;
static unsigned int s_idle_seconds = 0U;
static unsigned int s_clock_seconds = 0U;
static unsigned int s_battery_percent = 100U;
static bool s_render_dirty = true;

static void render_current_state(void);
static void render_if_dirty(void);
static void mark_render_dirty(void);
static void trim_line_ending(char *text);
static void print_help(void);
static bool is_known_terminal_command(const char *command);
static bool is_key_input(const char *input);
static bool is_replay_command(const char *input);
static bool translate_terminal_command(const char *input, shared_action_t *action);
static bool translate_key_input(const char *input, shared_action_t *action);
static bool translate_replay_step(replay_step_t step, shared_action_t *action);
static bool parse_tick_command(const char *input, unsigned int *tick_count);
static bool parse_battery_command(const char *input, unsigned int *battery_percent);
static bool run_replay_step(replay_step_t step);
static void run_replay_command(const char *input);
static bool dispatch_app_event(app_event_t event);
static bool dispatch_user_action(shared_action_t action);
static bool dispatch_tick_count(unsigned int tick_count);
static bool apply_shared_action(shared_action_t action);
static const char *page_name(page_t page);
static const char *power_name(power_state_t power_state);
static const char *shared_action_name(shared_action_t action);

int main(void)
{
    char input[COMMAND_BUFFER_SIZE];

    puts("Watch Architecture Lab - Stage 06 System Event Pressure");
    print_help();
    render_current_state();

    while (true) {
        shared_action_t action = SHARED_ACTION_NONE;
        unsigned int tick_count = 0U;
        unsigned int battery_percent = 0U;

        fputs("> ", stdout);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            putchar('\n');
            break;
        }

        trim_line_ending(input);

        if (strcmp(input, "quit") == 0) {
            break;
        }

        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }

        if (strcmp(input, "show") == 0) {
            render_current_state();
            continue;
        }

        if (parse_tick_command(input, &tick_count)) {
            dispatch_tick_count(tick_count);
            render_if_dirty();
            continue;
        }

        if (parse_battery_command(input, &battery_percent)) {
            app_event_t event = {
                .type = APP_EVENT_BATTERY_CHANGED,
                .action = SHARED_ACTION_NONE,
                .battery_percent = battery_percent,
            };
            dispatch_app_event(event);
            render_if_dirty();
            continue;
        }

        if (is_replay_command(input)) {
            run_replay_command(input);
            render_if_dirty();
            continue;
        }

        if (is_key_input(input)) {
            if (!translate_key_input(input, &action)) {
                puts("Input unavailable on this page.");
                continue;
            }

            dispatch_user_action(action);
            render_if_dirty();
            continue;
        }

        if (translate_terminal_command(input, &action)) {
            dispatch_user_action(action);
            render_if_dirty();
            continue;
        }

        if (is_known_terminal_command(input)) {
            puts("Command unavailable on this page.");
        }
        else {
            puts("Unknown command.");
        }
    }

    puts("Bye.");
    return 0;
}

static void trim_line_ending(char *text)
{
    size_t length = strlen(text);

    while (length > 0U &&
           (text[length - 1U] == '\n' || text[length - 1U] == '\r')) {
        text[length - 1U] = '\0';
        length--;
    }
}

static void print_help(void)
{
    puts("Terminal commands:");
    puts("  menu   open App Menu from Home");
    puts("  heart  open Heart directly from Home");
    puts("  steps  open Steps directly from Home");
    puts("  next   select next app in App Menu");
    puts("  prev   select previous app in App Menu");
    puts("  enter  open selected app in App Menu");
    puts("  back   return according to navigation context");
    puts("Simulated physical keys:");
    puts("  key_next");
    puts("  key_prev");
    puts("  key_select");
    puts("  key_back");
    puts("Replay entry:");
    puts("  replay_about_roundtrip");
    puts("  replay_steps_roundtrip");
    puts("System events:");
    puts("  tick       advance 1 second");
    puts("  tick N     advance N seconds");
    puts("  battery N  change battery percent to N");
    puts("Program controls:");
    puts("  show   print current full state");
    puts("  help   print this help");
    puts("  quit   exit");
}

static bool is_known_terminal_command(const char *command)
{
    return strcmp(command, "menu") == 0 ||
           strcmp(command, "heart") == 0 ||
           strcmp(command, "steps") == 0 ||
           strcmp(command, "next") == 0 ||
           strcmp(command, "prev") == 0 ||
           strcmp(command, "enter") == 0 ||
           strcmp(command, "back") == 0 ||
           strcmp(command, "show") == 0 ||
           strcmp(command, "tick") == 0;
}

static bool is_key_input(const char *input)
{
    return strcmp(input, "key_next") == 0 ||
           strcmp(input, "key_prev") == 0 ||
           strcmp(input, "key_select") == 0 ||
           strcmp(input, "key_back") == 0;
}

static bool is_replay_command(const char *input)
{
    return strcmp(input, "replay_about_roundtrip") == 0 ||
           strcmp(input, "replay_steps_roundtrip") == 0;
}

static bool translate_terminal_command(const char *input, shared_action_t *action)
{
    switch (s_current_page) {
        case PAGE_HOME:
            if (strcmp(input, "menu") == 0) {
                *action = SHARED_ACTION_OPEN_MENU;
                return true;
            }
            if (strcmp(input, "heart") == 0) {
                *action = SHARED_ACTION_OPEN_HEART_SHORTCUT;
                return true;
            }
            if (strcmp(input, "steps") == 0) {
                *action = SHARED_ACTION_OPEN_STEPS_SHORTCUT;
                return true;
            }
            if (strcmp(input, "back") == 0) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;

        case PAGE_APP_MENU:
            if (strcmp(input, "next") == 0) {
                *action = SHARED_ACTION_MOVE_NEXT;
                return true;
            }
            if (strcmp(input, "prev") == 0) {
                *action = SHARED_ACTION_MOVE_PREV;
                return true;
            }
            if (strcmp(input, "enter") == 0) {
                *action = SHARED_ACTION_ACTIVATE_SELECTION;
                return true;
            }
            if (strcmp(input, "back") == 0) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;

        case PAGE_HEART:
        case PAGE_STEPS:
        case PAGE_ABOUT:
            if (strcmp(input, "back") == 0) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;

        case PAGE_NUM:
            return false;
    }

    return false;
}

static bool translate_key_input(const char *input, shared_action_t *action)
{
    switch (s_current_page) {
        case PAGE_HOME:
            if (strcmp(input, "key_select") == 0) {
                *action = SHARED_ACTION_OPEN_MENU;
                return true;
            }
            if (strcmp(input, "key_back") == 0) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;

        case PAGE_APP_MENU:
            if (strcmp(input, "key_next") == 0) {
                *action = SHARED_ACTION_MOVE_NEXT;
                return true;
            }
            if (strcmp(input, "key_prev") == 0) {
                *action = SHARED_ACTION_MOVE_PREV;
                return true;
            }
            if (strcmp(input, "key_select") == 0) {
                *action = SHARED_ACTION_ACTIVATE_SELECTION;
                return true;
            }
            if (strcmp(input, "key_back") == 0) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;

        case PAGE_HEART:
        case PAGE_STEPS:
        case PAGE_ABOUT:
            if (strcmp(input, "key_back") == 0) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;

        case PAGE_NUM:
            return false;
    }

    return false;
}

static bool translate_replay_step(replay_step_t step, shared_action_t *action)
{
    switch (step) {
        case REPLAY_STEP_OPEN_MENU:
            if (s_current_page == PAGE_HOME) {
                *action = SHARED_ACTION_OPEN_MENU;
                return true;
            }
            return false;

        case REPLAY_STEP_MOVE_NEXT:
            if (s_current_page == PAGE_APP_MENU) {
                *action = SHARED_ACTION_MOVE_NEXT;
                return true;
            }
            return false;

        case REPLAY_STEP_ACTIVATE_SELECTION:
            if (s_current_page == PAGE_APP_MENU) {
                *action = SHARED_ACTION_ACTIVATE_SELECTION;
                return true;
            }
            return false;

        case REPLAY_STEP_NAV_BACK:
            if (s_current_page == PAGE_HOME ||
                s_current_page == PAGE_APP_MENU ||
                s_current_page == PAGE_HEART ||
                s_current_page == PAGE_STEPS ||
                s_current_page == PAGE_ABOUT) {
                *action = SHARED_ACTION_NAV_BACK;
                return true;
            }
            return false;
    }

    return false;
}

static bool parse_tick_command(const char *input, unsigned int *tick_count)
{
    if (strcmp(input, "tick") == 0) {
        *tick_count = 1U;
        return true;
    }

    if (sscanf(input, "tick %u", tick_count) == 1 && *tick_count > 0U) {
        return true;
    }

    return false;
}

static bool parse_battery_command(const char *input, unsigned int *battery_percent)
{
    if (sscanf(input, "battery %u", battery_percent) != 1) {
        return false;
    }

    if (*battery_percent > 100U) {
        puts("Battery percent must be 0..100.");
        return false;
    }

    return true;
}

static bool run_replay_step(replay_step_t step)
{
    shared_action_t action = SHARED_ACTION_NONE;

    if (!translate_replay_step(step, &action)) {
        puts("Replay step unavailable on this page.");
        return false;
    }

    printf("replay step -> action %d\n", (int)action);
    return dispatch_user_action(action);
}

static void run_replay_command(const char *input)
{
    menu_item_t target_item;

    if (strcmp(input, "replay_about_roundtrip") == 0) {
        target_item = MENU_ABOUT;
    }
    else if (strcmp(input, "replay_steps_roundtrip") == 0) {
        target_item = MENU_STEPS;
    }
    else {
        puts("Unknown replay command.");
        return;
    }

    puts("[REPLAY START]");
    if (!run_replay_step(REPLAY_STEP_OPEN_MENU)) {
        puts("[REPLAY ABORT]");
        return;
    }

    while (s_menu_selection != target_item) {
        if (!run_replay_step(REPLAY_STEP_MOVE_NEXT)) {
            puts("[REPLAY ABORT]");
            return;
        }
    }

    if (!run_replay_step(REPLAY_STEP_ACTIVATE_SELECTION) ||
        !run_replay_step(REPLAY_STEP_NAV_BACK) ||
        !run_replay_step(REPLAY_STEP_NAV_BACK)) {
        puts("[REPLAY ABORT]");
        return;
    }

    puts("[REPLAY END]");
}

static bool dispatch_user_action(shared_action_t action)
{
    app_event_t event = {
        .type = APP_EVENT_USER_ACTION,
        .action = action,
        .battery_percent = 0U,
    };

    return dispatch_app_event(event);
}

static bool dispatch_tick_count(unsigned int tick_count)
{
    unsigned int index;
    bool handled_any = false;

    for (index = 0U; index < tick_count; index++) {
        app_event_t event = {
            .type = APP_EVENT_TICK_1S,
            .action = SHARED_ACTION_NONE,
            .battery_percent = 0U,
        };
        handled_any = dispatch_app_event(event) || handled_any;
    }

    return handled_any;
}

static bool dispatch_app_event(app_event_t event)
{
    switch (event.type) {
        case APP_EVENT_USER_ACTION:
            printf("event -> user action %s\n", shared_action_name(event.action));

            if (s_power_state == POWER_SLEEP) {
                s_power_state = POWER_ACTIVE;
                s_idle_seconds = 0U;
                mark_render_dirty();
                puts("sleep gate -> wake only");
                return true;
            }

            s_idle_seconds = 0U;
            return apply_shared_action(event.action);

        case APP_EVENT_TICK_1S:
            printf("event -> tick 1s\n");
            s_clock_seconds++;

            if (s_power_state == POWER_ACTIVE) {
                s_idle_seconds++;
                mark_render_dirty();

                if (s_idle_seconds >= IDLE_SLEEP_SECONDS) {
                    app_event_t idle_timeout_event = {
                        .type = APP_EVENT_IDLE_TIMEOUT,
                        .action = SHARED_ACTION_NONE,
                        .battery_percent = 0U,
                    };
                    dispatch_app_event(idle_timeout_event);
                }
            }

            return true;

        case APP_EVENT_IDLE_TIMEOUT:
            printf("event -> idle timeout\n");
            if (s_power_state == POWER_ACTIVE) {
                s_power_state = POWER_SLEEP;
                mark_render_dirty();
                return true;
            }
            return false;

        case APP_EVENT_BATTERY_CHANGED:
            printf("event -> battery changed %u\n", event.battery_percent);
            s_battery_percent = event.battery_percent;
            mark_render_dirty();
            return true;

        case APP_EVENT_NONE:
            return false;
    }

    return false;
}

static bool apply_shared_action(shared_action_t action)
{
    switch (action) {
        case SHARED_ACTION_OPEN_MENU:
            if (s_current_page != PAGE_HOME) {
                return false;
            }
            s_current_page = PAGE_APP_MENU;
            mark_render_dirty();
            return true;

        case SHARED_ACTION_OPEN_HEART_SHORTCUT:
            if (s_current_page != PAGE_HOME) {
                return false;
            }
            s_detail_return_page = PAGE_HOME;
            s_current_page = PAGE_HEART;
            mark_render_dirty();
            return true;

        case SHARED_ACTION_OPEN_STEPS_SHORTCUT:
            if (s_current_page != PAGE_HOME) {
                return false;
            }
            s_detail_return_page = PAGE_HOME;
            s_current_page = PAGE_STEPS;
            mark_render_dirty();
            return true;

        case SHARED_ACTION_MOVE_NEXT:
            if (s_current_page != PAGE_APP_MENU) {
                return false;
            }
            s_menu_selection = (s_menu_selection + 1) % MENU_COUNT;
            mark_render_dirty();
            return true;

        case SHARED_ACTION_MOVE_PREV:
            if (s_current_page != PAGE_APP_MENU) {
                return false;
            }
            s_menu_selection =
                (s_menu_selection + MENU_COUNT - 1) % MENU_COUNT;
            mark_render_dirty();
            return true;

        case SHARED_ACTION_ACTIVATE_SELECTION:
            if (s_current_page != PAGE_APP_MENU) {
                return false;
            }
            s_detail_return_page = PAGE_APP_MENU;

            switch (s_menu_selection) {
                case MENU_HEART:
                    s_current_page = PAGE_HEART;
                    break;
                case MENU_STEPS:
                    s_current_page = PAGE_STEPS;
                    break;
                case MENU_ABOUT:
                    s_current_page = PAGE_ABOUT;
                    break;
                case MENU_COUNT:
                    return false;
            }

            mark_render_dirty();
            return true;

        case SHARED_ACTION_NAV_BACK:
            switch (s_current_page) {
                case PAGE_HOME:
                    puts("Already on Home.");
                    return true;

                case PAGE_APP_MENU:
                    s_current_page = PAGE_HOME;
                    mark_render_dirty();
                    return true;

                case PAGE_HEART:
                case PAGE_STEPS:
                case PAGE_ABOUT:
                    s_current_page = s_detail_return_page;
                    mark_render_dirty();
                    return true;

                case PAGE_NUM:
                    return false;
            }
            return false;

        case SHARED_ACTION_NONE:
            return false;
    }

    return false;
}

static void mark_render_dirty(void)
{
    s_render_dirty = true;
}

static void render_if_dirty(void)
{
    if (!s_render_dirty) {
        puts("[NO RENDER] state unchanged");
        return;
    }

    render_current_state();
}

static void render_current_state(void)
{
    printf("[POWER=%s] [CLOCK=%u] [IDLE=%u] [BATTERY=%u]\n",
           power_name(s_power_state),
           s_clock_seconds,
           s_idle_seconds,
           s_battery_percent);

    switch (s_current_page) {
        case PAGE_HOME:
            puts("[HOME]");
            break;

        case PAGE_HEART:
            puts("[HEART]");
            break;

        case PAGE_STEPS:
            puts("[STEPS]");
            break;

        case PAGE_APP_MENU:
            puts("[HEART] [STEPS] [ABOUT]");
            switch (s_menu_selection) {
                case MENU_HEART:
                    puts("[SELECT] [      ] [      ]");
                    break;
                case MENU_STEPS:
                    puts("[      ] [SELECT] [      ]");
                    break;
                case MENU_ABOUT:
                    puts("[      ] [      ] [SELECT]");
                    break;
                case MENU_COUNT:
                    break;
            }
            break;

        case PAGE_ABOUT:
            puts("[ABOUT]");
            break;

        case PAGE_NUM:
            break;
    }

    printf("[RETURN_TARGET=%s]\n", page_name(s_detail_return_page));
    s_render_dirty = false;
}

static const char *page_name(page_t page)
{
    switch (page) {
        case PAGE_HOME:
            return "HOME";
        case PAGE_APP_MENU:
            return "APP_MENU";
        case PAGE_HEART:
            return "HEART";
        case PAGE_STEPS:
            return "STEPS";
        case PAGE_ABOUT:
            return "ABOUT";
        case PAGE_NUM:
            return "INVALID";
    }

    return "INVALID";
}

static const char *power_name(power_state_t power_state)
{
    switch (power_state) {
        case POWER_ACTIVE:
            return "ACTIVE";
        case POWER_SLEEP:
            return "SLEEP";
    }

    return "UNKNOWN";
}

static const char *shared_action_name(shared_action_t action)
{
    switch (action) {
        case SHARED_ACTION_NONE:
            return "NONE";
        case SHARED_ACTION_OPEN_MENU:
            return "OPEN_MENU";
        case SHARED_ACTION_OPEN_HEART_SHORTCUT:
            return "OPEN_HEART_SHORTCUT";
        case SHARED_ACTION_OPEN_STEPS_SHORTCUT:
            return "OPEN_STEPS_SHORTCUT";
        case SHARED_ACTION_MOVE_NEXT:
            return "MOVE_NEXT";
        case SHARED_ACTION_MOVE_PREV:
            return "MOVE_PREV";
        case SHARED_ACTION_ACTIVATE_SELECTION:
            return "ACTIVATE_SELECTION";
        case SHARED_ACTION_NAV_BACK:
            return "NAV_BACK";
    }

    return "UNKNOWN";
}
