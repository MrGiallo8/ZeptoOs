// keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

// Legge un carattere. blocca finché l'utente non preme un tasto.
char kb_getchar(void);

void kb_readline(char *buf, int max, void (*echo)(char));

#endif
