// keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

// Legge un carattere. blocca finché l'utente non preme un tasto.
char kb_getchar(void);

// Legge una riga intera nel buffer. echo = funzione per stampare a video.
// Passa NULL come echo se non vuoi l'eco a schermo.
void kb_readline(char *buf, int max, void (*echo)(char));

#endif
