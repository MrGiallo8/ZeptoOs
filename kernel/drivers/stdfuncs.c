#include "../includes/stout.h"

// ----------------pputc & pputs & print_int-----------------

void pputc(char x) {
	volatile char *video = (volatile char*)video_mem;
	
	if (x == '\n'){ 
		int riga = pos_char / 80; // posizione
		pos_char=(riga + 1) * 80;
		if (pos_char >= 80 * 25) {
        scroll();
    	}
		return;
		
	} else if (x == '\t') {
		int colonna = pos_char % 80; // posizione 
		int spazi = 8 - (colonna % 8); // tab
		for (int y=0;y < spazi ; y++){
			pputc(' ');
		}
		return;
		
	} else if (x == '\r'){
		pos_char =( pos_char / 80 ) * 80;
		return;
		
	} else if (x == '\b'){
		if (pos_char > 0){
			pos_char--;
			video[pos_char * 2] = ' '; // sovrascrive con uno spazio
			video[pos_char * 2 + 1] = current_color;
		}
		return;
	}
	
	video[pos_char * 2] = x; // primo carattere schermo (0) , e prende i 2 byte
	video[pos_char * 2 + 1] = current_color; // byte successivo a quello prima
	// [carattere][colore][carattere][colore] ; colore sempre bianco (0x07)

	if (pos_char >= 80*25){
		scroll();
	}

	pos_char++; // cella successiva
}
// put per le stringe
void pputs(const char *s) {
	while(*s){
		pputc(*s);
		s++;
	}
}

void print_int(int n){
	char buffer[20]; //array da 20 caratteri
	int i = 0;

	// se il numero è 0 allora scrivi 0
	if (n==0){
		pputc('0');
		return;
	}

	// se il numero è negativo metti un meno davanti
	if (n<0){
		pputc('-');
		n=-n; // n positivo
	}

	// se invece è positivo 
	while (n>0){
		buffer[i++] = '0' + (n%10);// n%10 prende l'ultima cifra
		n/=10;// n/10 prende tutto tranne l'ultima cifra
	}

	while (i--) {
	        pputc(buffer[i]); // stampa le cifre al contario 
	}
}

// ----------------gestione stringhe -----------------

int strleng(const char *str){
	int i=0;
	while(str[i] != '\0') i++;
	return i;
}

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

int strcmp_n(const char *a, const char *b, int n) {
    while (n > 0) {
        if (*a != *b) {
            return (uint8_t)*a - (uint8_t)*b;
        }
        if (*a == '\0') {
            return 0; // Entrambe finite prima di n caratteri
        }
        a++;
        b++;
        n--;
    }
    return 0; // I primi n caratteri sono identici
}

// ---------------- sys -----------------

void disable_cursor(void) {
    // Seleziona il registro 0x0A (Cursor Start Register)
    __asm volatile ("outb %0, %1" :: "a"((uint8_t)0x0A), "Nd"((uint16_t)0x3D4));
    // Bit 5 = 1 allora cursore disabilitato
    __asm volatile ("outb %0, %1" :: "a"((uint8_t)0x20), "Nd"((uint16_t)0x3D5));
}


// ---------------- Random func-----------------
uint64_t uptime ; 
uint64_t min ;
uint64_t sec ;
uint64_t hrs ;

uint64_t random64(){
	uptime = get_uptime_sec(); 
 	min = get_rtc_minutes() ;
 	sec = get_rtc_seconds();
	hrs = get_rtc_hours();

    uint64_t valore_random = ((uptime * 3600) + (hrs * 60) + min + sec) * 8740652293ULL + 62773ULL;

    valore_random = (valore_random << 45) | (valore_random >> 19); // rotazione bits
    valore_random ^= 0xA5B3E299;

	uint64_t x = valore_random;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    valore_random = x;

    return valore_random;
}

uint64_t random_dig(int cifre){
	
	if (cifre <= 0) return 0;  
	if (cifre > 19) cifre = 19 ;

	if (cifre == 1) {
        return random64() % 10;
    }
	
	uint64_t min = 1; uint64_t max;
	for (int i=0; i < cifre-1; i++){
		min*=10;
	}
	max = (min * 10) -1;

	uint64_t range = (max - min) + 1;
    return min + (random64() % range);
}

int random_between(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    uint64_t range = (uint64_t)(max - min + 1);
    return min + (int)(random64() % range);
}

int str_to_int(char str[]){
	int integer=0;
	for(int i=0; str[i] != '\0'; i++){
		integer = (integer * 10) + (str[i] - '0');
	}
	return integer;
}

// ------------------ sound --------------------

// divisore = 1193189 / freq(440) 

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void sound(uint32_t freq){
	if (freq == 0) {
        no_sound();
        return;
    }

	uint32_t div = 1193180 / freq;

	outb(0x43, 0xB6);
	outb(0x42, (uint8_t)(div & 0xFF));      
    outb(0x42, (uint8_t)((div >> 8) & 0xFF));

	// Abilita uscita speaker impostando bit 0 e 1 della porta 0x61
	uint8_t tmp = inb(0x61);
	if(tmp != (tmp | 3)){
		outb(0x61, tmp | 3);
	}
}

void no_sound(){
	uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

void beep(uint32_t freq, uint32_t durata) {
    sound(freq);
    delay_ms(durata);
    no_sound();
}

