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
    REPLAY_STEP_OPEN_MENU = 0,
    REPLAY_STEP_MOVE_NEXT,
    REPLAY_STEP_ACTIVATE_SELECTION,
    REPLAY_STEP_NAV_BACK,
} replay_step_t;

static page_t s_current_page = PAGE_HOME;
static page_t s_detail_return_page = PAGE_HOME;
static menu_item_t s_menu_selection = MENU_HEART;

static void handle_command(const char *command);
static void render_current_page(void);
static bool is_known_page_command(const char *command);
static bool is_key_input(const char *input);
static const char *translate_key_input(const char *input);
static bool is_replay_command(const char *input);
static void run_replay_command(const char *input);
static const char *translate_replay_step(replay_step_t step);
static bool run_replay_step(replay_step_t step);

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

    puts("Watch Architecture Lab - Stage 04 Command Language Pressure");
    print_help();
    render_current_page();

    while (true) {
        const char *translated_command;

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

        if (is_replay_command(input)) {
            run_replay_command(input);
            continue;
        }

        if (is_key_input(input)) {
            translated_command = translate_key_input(input);
            if (translated_command == NULL) {
                puts("Input unavailable on this page.");
                continue;
            }

            handle_command(translated_command);
            continue;
        }

        handle_command(input);
    }

    puts("Bye.");
    return 0;
}

static bool is_key_input(const char *input)
{
    return strcmp(input, "key_next") == 0 ||
           strcmp(input, "key_prev") == 0 ||
           strcmp(input, "key_select") == 0 ||
           strcmp(input, "key_back") == 0;
}

static const char *translate_key_input(const char *input)
{
    switch (s_current_page) {
        case PAGE_HOME:
            if (strcmp(input, "key_select") == 0) {
                return "menu";
            }
            if (strcmp(input, "key_back") == 0) {
                return "back";
            }
            return NULL;

        case PAGE_APP_MENU:
            if (strcmp(input, "key_next") == 0) {
                return "next";
            }
            if (strcmp(input, "key_prev") == 0) {
                return "prev";
            }
            if (strcmp(input, "key_select") == 0) {
                return "enter";
            }
            if (strcmp(input, "key_back") == 0) {
                return "back";
            }
            return NULL;

        case PAGE_HEART:
        case PAGE_STEPS:
        case PAGE_ABOUT:
            if (strcmp(input, "key_back") == 0) {
                return "back";
            }
            return NULL;

        case PAGE_NUM:
            return NULL;
    }

    return NULL;
}

static bool is_replay_command(const char *input)
{
    return strcmp(input, "replay_about_roundtrip") == 0 ||
           strcmp(input, "replay_steps_roundtrip") == 0;
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

static const char *translate_replay_step(replay_step_t step)
{
    switch (step) {
        case REPLAY_STEP_OPEN_MENU:
            if (s_current_page == PAGE_HOME) {
                return "menu";
            }
            return NULL;

        case REPLAY_STEP_MOVE_NEXT:
            if (s_current_page == PAGE_APP_MENU) {
                return "next";
            }
            return NULL;

        case REPLAY_STEP_ACTIVATE_SELECTION:
            if (s_current_page == PAGE_APP_MENU) {
                return "enter";
            }
            return NULL;

        case REPLAY_STEP_NAV_BACK:
            if (s_current_page == PAGE_HOME ||
                s_current_page == PAGE_APP_MENU ||
                s_current_page == PAGE_HEART ||
                s_current_page == PAGE_STEPS ||
                s_current_page == PAGE_ABOUT) {
                return "back";
            }
            return NULL;

        default:
            break;
    }

    return NULL;
}

static bool run_replay_step(replay_step_t step)
{
    const char *command = translate_replay_step(step);

    if (command == NULL) {
        puts("Replay step unavailable on this page.");
        return false;
    }

    printf("replay step -> %s\n", command);
    handle_command(command);
    return true;
}

static void handle_command(const char *command)
{
    switch (s_current_page) {
        case PAGE_HOME:
            if (strcmp(command, "heart") == 0) {
                s_detail_return_page = PAGE_HOME;
                s_current_page = PAGE_HEART;
                render_current_page();
                return;
            }
            if (strcmp(command, "steps") == 0) {
                s_detail_return_page = PAGE_HOME;
                s_current_page = PAGE_STEPS;
                render_current_page();
                return;
            }
            if (strcmp(command, "menu") == 0) {
                s_current_page = PAGE_APP_MENU;
                render_current_page();
                return;
            }
            if (strcmp(command, "back") == 0) {
                puts("Already on Home.");
                return;
            }
            break;

        case PAGE_APP_MENU:
            if (strcmp(command, "next") == 0) {
                s_menu_selection = (s_menu_selection + 1) % MENU_COUNT;
                render_current_page();
                return;
            }
            if (strcmp(command, "prev") == 0) {
                s_menu_selection =
                    (s_menu_selection + MENU_COUNT - 1) % MENU_COUNT;
                render_current_page();
                return;
            }
            if (strcmp(command, "enter") == 0) {
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
                        return;
                }

                render_current_page();
                return;
            }
            if (strcmp(command, "back") == 0) {
                s_current_page = PAGE_HOME;
                render_current_page();
                return;
            }
            break;

        case PAGE_HEART:
        case PAGE_STEPS:
        case PAGE_ABOUT:
            if (strcmp(command, "back") == 0) {
                s_current_page = s_detail_return_page;
                render_current_page();
                return;
            }
            break;

        case PAGE_NUM:
            return;
    }

    if (strcmp(command, "show") == 0) {
        render_current_page();
        return;
    }

    if (is_known_page_command(command)) {
        puts("Command unavailable on this page.");
    }
    else {
        puts("Unknown command.");
    }
}

static bool is_known_page_command(const char *command)
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
