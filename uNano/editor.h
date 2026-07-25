#ifndef EDITOR_H
#define EDITOR_H

void editor_init(void);
void editor_draw(void);
void editor_process_key(int c);

void editor_clear(void);
void editor_append_row(const char *text);

int editor_row_count(void);
const char *editor_row_text(int row);

#endif
