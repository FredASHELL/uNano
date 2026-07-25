#ifndef TERMINAL_H
#define TERMINAL_H

void terminal_enable_raw_mode(void);
void terminal_disable_raw_mode(void);
int terminal_read_key(void);
void terminal_clear_screen(void);

#endif
