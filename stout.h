#ifndef STOUT_H 
#define STOUT_H

extern unsigned char current_color;

// pseudo librerie gcc 
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)

// tipo varibili
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

// ---------- colori ----------

enum colori {
    NERO        = 0,
    BLU         = 1,
    VERDE       = 2,
    CIANO       = 3,
    ROSSO       = 4,
    MAGENTA     = 5,
    MARRONE     = 6,
    GRIGIO      = 7,
    GRIGIO_SCURO = 8,
    BLU_CHIARO  = 9,
    VERDE_CHIARO = 10,
    CIANO_CHIARO = 11,
    ROSSO_CHIARO = 12,
    ROSA        = 13,
    GIALLO      = 14,
    BIANCO      = 15
};

extern int pos_char;

// unsigned char current_color = 0x07;
// bianco su nero di default

// indirizzo memoria video e colori
#define video_mem 0xB8000
#define white_black 0x07 

// dichiarazioni print putc puts
void pputc(char x);
void pputs(const char *s);
void print_int(int n);
void write(const char *fmt, ...);
void clear_screen();

//write center
void write_center(const char *str, int y, enum colori colore);

// dichiarazioni bordo 
void bordo();

// dichiarazioni stringe
int strleng(const char *s);
int strcmp(const char *a, const char *b);
int strcmp_n(const char *a, const char *b, int n);

void disable_cursor(void);

#endif
