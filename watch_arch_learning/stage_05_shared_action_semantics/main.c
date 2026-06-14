#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_BUFFER_SIZE 64U

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

static page_t s_current_page = PAGE_HOME;
static page_t s_detail_return_page = PAGE_HOME;
static menu_item_t s_menu_selection = MENU_HEART;

static void render_current_page(void);
static bool is_known_terminal_command(const char *command);
static bool is_key_input(const char *input);
static bool is_replay_command(const char *input);
static bool translate_terminal_command(const char *input, shared_action_t *action);
static bool translate_key_input(const char *input, shared_action_t *action);
static bool translate_replay_step(replay_step_t step, shared_action_t *action);
static bool run_replay_step(replay_step_t step);
static void run_replay_command(const char *input);
static bool apply_shared_action(shared_action_t action);

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
    puts("Program controls:");
    puts("  show   print current page");
    puts("  help   print this help");
    puts("  quit   exit");
}

int main(void)
{
    char input[COMMAND_BUFFER_SIZE];

    puts("Watch Architecture Lab - Stage 05 Shared Action Semantics");
    print_help();
    render_current_page();

    while (true) {
        shared_action_t action = SHARED_ACTION_NONE;

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
            render_current_page();
            continue;
        }

        if (is_replay_command(input)) {
            run_replay_command(input);
            continue;
        }

        if (is_key_input(input)) {
            if (!translate_key_input(input, &action)) {
                puts("Input unavailable on this page.");
                continue;
            }

            apply_shared_action(action);
            continue;
        }

        if (translate_terminal_command(input, &action)) {
            apply_shared_action(action);
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

static bool is_known_terminal_command(const char *command)
{
    return strcmp(command, "menu") == 0 ||
           strcmp(command, "heart") == 0 ||
           strcmp(command, "steps") == 0 ||
           strcmp(command, "next") == 0 ||
           strcmp(command, "prev") == 0 ||
           strcmp(command, "enter") == 0 ||
           strcmp(command, "back") == 0 ||
           strcmp(command, "show") == 0;
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

static bool run_replay_step(replay_step_t step)
{
    shared_action_t action = SHARED_ACTION_NONE;

    if (!translate_replay_step(step, &action)) {
        puts("Replay step unavailable on this page.");
        return false;
    }

    printf("replay step -> action %d\n", (int)action);
    return apply_shared_action(action);
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

static bool apply_shared_action(shared_action_t action)
{
    switch (action) {
        case SHARED_ACTION_OPEN_MENU:
            if (s_current_page != PAGE_HOME) {
                return false;
            }
            s_current_page = PAGE_APP_MENU;
            render_current_page();
            return true;

        case SHARED_ACTION_OPEN_HEART_SHORTCUT:
            if (s_current_page != PAGE_HOME) {
                return false;
            }
            s_detail_return_page = PAGE_HOME;
            s_current_page = PAGE_HEART;
            render_current_page();
            return true;

        case SHARED_ACTION_OPEN_STEPS_SHORTCUT:
            if (s_current_page != PAGE_HOME) {
                return false;
            }
            s_detail_return_page = PAGE_HOME;
            s_current_page = PAGE_STEPS;
            render_current_page();
            return true;

        case SHARED_ACTION_MOVE_NEXT:
            if (s_current_page != PAGE_APP_MENU) {
                return false;
            }
            s_menu_selection = (s_menu_selection + 1) % MENU_COUNT;
            render_current_page();
            return true;

        case SHARED_ACTION_MOVE_PREV:
            if (s_current_page != PAGE_APP_MENU) {
                return false;
            }
            s_menu_selection =
                (s_menu_selection + MENU_COUNT - 1) % MENU_COUNT;
            render_current_page();
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

            render_current_page();
            return true;

        case SHARED_ACTION_NAV_BACK:
            switch (s_current_page) {
                case PAGE_HOME:
                    puts("Already on Home.");
                    return true;

                case PAGE_APP_MENU:
                    s_current_page = PAGE_HOME;
                    render_current_page();
                    return true;

                case PAGE_HEART:
                case PAGE_STEPS:
                case PAGE_ABOUT:
                    s_current_page = s_detail_return_page;
                    render_current_page();
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

static void render_current_page(void)
{
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
}
