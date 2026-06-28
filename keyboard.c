// definizioni

typedef unsigned char      uint8_t;   // 8 bit senza segno  
typedef unsigned short     uint16_t;  // 16 bit senza segno 
typedef unsigned int       uint32_t;  // 32 bit senza segno
typedef unsigned long long uint64_t;  // 64 bit senza segno

typedef signed char        int8_t;    // 8 bit con segno  
typedef signed short       int16_t;   // 16 bit con segno
typedef signed int         int32_t;   // 32 bit con segno
typedef signed long long   int64_t;   // 64 bit con segno

// ------------ I/O ------------
// input
static inline uint8_t inb(uint16_t p) {
    uint8_t v;
    __asm__ volatile("inb %1,%0":"=a"(v):"Nd"(p));
    return v;
}
//output
static inline void outb(uint16_t p, uint8_t v) {
    __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p));
}

// ------------ Costanti porte ------------
#define KB_DATA   0x60 // da dove arrivano gli scan codes
#define KB_STATUS 0x64 // porta stato 

// ------------ Keymaps ------------
static const char km_norm[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,9,
    'q','w','e','r','t','y','u','i','o','p','[',']',13,0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
// per maiuscolo
static const char km_shft[128] = {
    0,27,'!','@','#','$','%','^','&','*','(',')','+',0,8,9,
    'Q','W','E','R','T','Y','U','I','O','P','{','}',13,0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// ------------ Stato modificatori ------------
static int kb_shift = 0, kb_ctrl = 0, kb_caps = 0;

// ------------ API pubblica ------------
static inline int  kb_ready(void)     { return inb(KB_STATUS) & 1; }
static inline uint8_t kb_scan(void)   { while(!kb_ready()); return inb(KB_DATA); }

char kb_getchar(void) {
    for(;;) {
        uint8_t sc = kb_scan();
        switch(sc) {
            case 0x2A: case 0x36: kb_shift=1; continue;  
            case 0xAA: case 0xB6: kb_shift=0; continue;  
            case 0x1D: kb_ctrl=1; continue;              
            case 0x9D: kb_ctrl=0; continue;             
            case 0x3A: kb_caps=!kb_caps; continue;       
        }
        if (sc & 0x80) continue;  // altri break code: ignora
        if (sc >= 128)  continue;  // fuori range: ignora
        const char *m = (kb_shift ^ kb_caps) ? km_shft : km_norm;
        if (m[sc]) return m[sc];
    }
}

void kb_readline(char *buf, int max, void (*echo)(char)) {
    int n = 0;
    for(;;) {
        char c = kb_getchar();
        if (c == 13) { buf[n]=0; if(echo) echo('\n'); return; }
        if (c == 8 && n > 0) {
            n--;
            if(echo){ echo('\b'); echo(' '); echo('\b'); }
            continue;
        }
        if (n < max-1) { buf[n++]=c; if(echo) echo(c); }
    }
}
