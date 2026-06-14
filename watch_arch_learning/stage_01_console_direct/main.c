#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_BUFFER_SIZE 64U

/*
 * LEARN 1:
 * Replace this placeholder with PAGE_HOME, PAGE_HEART, and PAGE_STEPS.
 */
typedef enum {
    PAGE_HOME = 0,
    PAGE_HEART,
    PAGE_STEPS
} page_t;

static page_t s_current_page = PAGE_HOME;

static void handle_command(const char *command);
static void render_current_page(void);

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
    puts("  heart  enter Heart detail");
    puts("  steps  enter Steps detail");
    puts("  back   return to Home");
    puts("  show   print current page");
    puts("  help   print this help");
    puts("  quit   exit");
}

int main(void)
{
    char command[COMMAND_BUFFER_SIZE];

    puts("Watch Architecture Lab - Stage 01 Console Direct");
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
    /*
     * LEARN 2:
     * Implement heart, steps, back, and show.
     *
     * Decide:
     * - which commands change s_current_page;
     * - what back does on Home;
     * - whether an invalid command renders anything;
     * - when render_current_page() is called.
     */
    if (strcmp(command, "heart") == 0)
    {
        s_current_page = PAGE_HEART;
        render_current_page();
        return;
    }
    if (strcmp(command, "steps") == 0)
    {
        s_current_page = PAGE_STEPS;
        render_current_page();
        return;
    }
    if (strcmp(command, "back") == 0)
    {
        if(s_current_page == PAGE_HEART || s_current_page == PAGE_STEPS)
        {
            s_current_page = PAGE_HOME;
            render_current_page();
            return;
        }
        else
        {
            puts("Already on Home.");
            return;
        }
    }
    if (strcmp(command, "show") == 0)
    {
        render_current_page();
        return;
    }

    puts("Unknown command.");
}

static void render_current_page(void)
{
    /*
     * LEARN 3:
     * Print one of [HOME], [HEART], or [STEPS] from s_current_page.
     * This function reads state. It must not change s_current_page.
     */
    switch(s_current_page)
    {
        case PAGE_HOME:
            puts("It's PAGE_HOME");
        break;
        case PAGE_HEART:
            puts("It's PAGE_HEART");
        break;
        case PAGE_STEPS:
            puts("It's PAGE_STEPS");
        break;
    }
}
