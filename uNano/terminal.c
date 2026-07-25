#include "terminal.h"

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>


static struct termios original;


void terminal_disable_raw_mode(void)
{
    tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &original
    );
}


void terminal_enable_raw_mode(void)
{
    tcgetattr(
        STDIN_FILENO,
        &original
    );

    atexit(terminal_disable_raw_mode);

    struct termios raw = original;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);

    tcsetattr(
        STDIN_FILENO,
        TCSAFLUSH,
        &raw
    );
}


int terminal_read_key(void)
{
    char c;

    if (read(STDIN_FILENO, &c, 1) != 1)
        return -1;


    if (c == '\033')
    {
        char seq[2];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\033';

        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\033';


        if (seq[0] == '[')
        {
            switch (seq[1])
            {
                case 'A': return 1000; // up
                case 'B': return 1001; // down
                case 'C': return 1002; // right
                case 'D': return 1003; // left
            }
        }

        return '\033';
    }


    return c;
}


void terminal_clear_screen(void)
{
    write(
        STDOUT_FILENO,
        "\033[2J\033[H",
        7
    );
}
