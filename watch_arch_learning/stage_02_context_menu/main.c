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

static page_t s_current_page = PAGE_HOME;
static page_t s_detail_return_page = PAGE_HOME;
static menu_item_t s_menu_selection = MENU_HEART;

static void handle_command(const char *command);
static void render_current_page(void);
static bool is_known_page_command(const char *command);

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
    puts("Commands:");
    puts("  menu   open App Menu from Home");
    puts("  heart  open Heart directly from Home");
    puts("  steps  open Steps directly from Home");
    puts("  next   select next app in App Menu");
    puts("  prev   select previous app in App Menu");
    puts("  enter  open selected app in App Menu");
    puts("  back   return according to navigation context");
    puts("  show   print current page");
    puts("  help   print this help");
    puts("  quit   exit");
}

int main(void)
{
    char command[COMMAND_BUFFER_SIZE];

    puts("Watch Architecture Lab - Stage 02 Context Menu");
    print_help();
    render_current_page();

    while (true) {
        fputs("> ", stdout);
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            putchar('\n');
            break;
        }

        trim_line_ending(command);

        if (strcmp(command, "quit") == 0) {
            break;
        }

        if (strcmp(command, "help") == 0) {
            print_help();
            continue;
        }

        handle_command(command);
    }

    puts("Bye.");
    return 0;
}

static void handle_command(const char *command)
{
    switch(s_current_page)
    {
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
            switch (s_menu_selection)
            {
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
