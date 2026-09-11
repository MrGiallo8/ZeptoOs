#ifndef GAMES_H
#define GAMES_H

static inline void outb(unsigned short port, unsigned char val);
static inline unsigned char inb(unsigned short port);
extern volatile char last_key;

char get_key(void);

void game_menu();
void number_guesser();
void snake_game();

#endif
