#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define waddstr_(text) printf("%s", text)
#define waddch_(ch) putchar(ch)
#define wprintw_(format, data) printf(format, data)
#define newline printf("\r\n")

#define repeat(i, n) for (int i = 0; i < n; ++i)

#define get_tile_at(k) (grid + y * ((x + size) % (size + 1) + k * x) + (size + 1 - y) * i)

#define CLEAR_SCREEN "\033[2J\033[H"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

int size, new_tile_pos, num_empty_slots, changed, last_empty_tile_index, *last_non_empty_tile, *this_tile,
    grid_buf[128], *grid = grid_buf, did_restore_from_alt_buffer, did_disable_raw_mode;

void restore_from_alt_buffer(void) {
    if (did_restore_from_alt_buffer++)
        return;
    printf("\033[?1049l");
}

void switch_to_alt_buffer(void) {
    atexit(restore_from_alt_buffer);
    printf("\033[?1049h");
}

struct termios original_termios;

void disable_raw_mode(void) {
    if (did_disable_raw_mode++)
        return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    printf(SHOW_CURSOR);
}

void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);

    struct termios raw = original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf(HIDE_CURSOR);
}

char get_key(void) {
    char c;
    while (read(STDIN_FILENO, &c, 1) != 1)
        ;
    return c;
}

void draw_frame_1(void) {
    waddch_('+');
    repeat(i, 8 * size - 1) {
        waddch_('-');
    }
    waddch_('+');
    newline;
}

void draw_frame_2(void) {
    waddch_('|');
    repeat(i, size) {
        repeat(j, 7) {
            waddch_(32);
        }
        waddch_('|');
    }
    newline;
}

void show_grid(void) {
    draw_frame_1();
    repeat(i, size) {
        draw_frame_2();

        waddstr_("| ");
        repeat(j, size) {
            if (!*grid) {
                waddstr_("      | ");
            } else if (*grid >= 1024) {
                wprintw_(" %2dk  | ", *grid / 1024);
            } else {
                wprintw_("%4d  | ", *grid);
            }
            ++grid;
        }
        newline;

        draw_frame_2();

        if (i < size - 1) {
            waddch_('|');
            repeat(j, size) {
                repeat(j, 7) {
                    waddch_('-');
                }
                if (j < size - 1) {
                    waddch_('+');
                }
            }
            waddch_('|');
            newline;
        }
    }
    draw_frame_1();
    grid -= size * size;
}

int move_grid(int x, int y, int do_change) {
    changed = 0;
    repeat(i, size) {
        last_empty_tile_index = 0;
        last_non_empty_tile = 0;
        repeat(j, size) {
            this_tile = get_tile_at(j);
            if (*this_tile) {
                if (last_non_empty_tile && *this_tile == *last_non_empty_tile) {
                    if (do_change) {
                        *last_non_empty_tile *= 2;
                        *this_tile = 0;
                    }
                    last_non_empty_tile = 0;
                    changed = 1;
                } else {
                    last_non_empty_tile = get_tile_at(last_empty_tile_index);
                    if (do_change) {
                        *last_non_empty_tile = *this_tile;
                    }
                    ++last_empty_tile_index;
                }
            }
        }
        for (; last_empty_tile_index < size; ++last_empty_tile_index) {
            this_tile = get_tile_at(last_empty_tile_index);
            changed |= *this_tile;
            if (do_change) {
                *this_tile = 0;
            }
        }
    }
    return !changed;
}

void put_new_tile(void) {
    num_empty_slots = 0;
    repeat(i, size * size) {
        if (!grid[i]) {
            ++num_empty_slots;
        }
    }
    if (num_empty_slots == 0) {
        return;
    }
    new_tile_pos = rand() % num_empty_slots;
    repeat(i, size * size) {
        if (!grid[i] && --num_empty_slots == new_tile_pos) {
            grid[i] = rand() % 8 ? 2 : 4;
        }
    }
}

int main(int argc, char **argv) {
    srand(time(0));

    size = argc > 1 ? (*argv[1] - '0') : 4;
    if (size <= 1 || size > 8) {
        fputs("invalid board size\n", stderr);
        return 1;
    }

    switch_to_alt_buffer();
    enable_raw_mode();

    put_new_tile();
    put_new_tile();

    while (1) {
        printf(CLEAR_SCREEN);
        show_grid();
        waddstr_("  h,j,k,l: move         q: quit");
        fflush(stdout);

        switch (get_key()) {
        case 'q':
            goto quit;

        case 'h':
            move_grid(1, 1, 1);
            goto move_end;
        case 'j':
            move_grid(-1, size, 1);
            goto move_end;
        case 'k':
            move_grid(1, size, 1);
            goto move_end;
        case 'l':
            move_grid(-1, 1, 1);
            goto move_end;

        move_end:
            if (changed) {
                put_new_tile();
            }
            if (move_grid(1, 1, 0) & move_grid(-1, 1, 0) & move_grid(1, size, 0) & move_grid(-1, size, 0)) {
                goto quit;
            }
            usleep(2048 * 8);
            break;
        }
    }

quit:
    disable_raw_mode();
    restore_from_alt_buffer();
    printf(CLEAR_SCREEN);
}
