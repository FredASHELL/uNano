#include "terminal.h"
#include "editor.h"
#include "file.h"

int main(int argc, char *argv[])
{
    terminal_enable_raw_mode();

    editor_init();

    if (argc > 1)
        file_open(argv[1]);

    while (1)
    {
        terminal_clear_screen();

        editor_draw();

        int c = terminal_read_key();

        if (c == 17)
    break;

if (c == 19)
{
    file_save();
    continue;
}

        editor_process_key(c);
    }

    /* Clean exit */
    terminal_clear_screen();

    return 0;
}
