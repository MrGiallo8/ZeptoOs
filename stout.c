typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)

unsigned char current_color = 0x07;

#define video_mem 0xB8000
#define white_black 0x07 

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

//funzione putc , driver video per impostazione casella = carattere
int pos_char = 0;

// ----------------scrolling-----------------

void scroll() {
    volatile char *video = (volatile char*)video_mem;
    // copia ogni riga nella riga prima
    for (int i = 0; i < 80 * 24; i++) {
        video[i * 2]     = video[(i + 80) * 2];
        video[i * 2 + 1] = video[(i + 80) * 2 + 1];
    }
    // pulisci l'ultima riga
    for (int i = 80 * 24; i < 80 * 25; i++) {
        video[i * 2]     = ' '; // carattere
        video[i * 2 + 1] = 0x07; // colore
    }
    // cursore torna all'inizio dell'ultima riga
    pos_char = 80 * 24;
}

// ---------- print , put char e string ----------

void pputc(char x) {
	volatile char *video = (volatile char*)video_mem;
	
	if (x == '\n') {
		int riga = pos_char / 80; // posizione
		pos_char=(riga + 1) * 80;
		if (pos_char >= 80 * 25) {
        scroll();
    	}
		return;// esce dalla funzione
		
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
	char buffer[20]; //array da 20 byte ( quindi 20 caratteri) basta 
	int i = 0;

	// se il numero è 0 allora scrivi 0
	if (n==0){
		pputc('0');
		return;
	}

	// se il numero è negativo metti un 'meno' davanti
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
// printf finale 
void write(const char *fmt , ...){ // il '...' dice che ci sono x argomenti
	va_list args; // legge le variabili inseriti nello stack uno alla volta
	va_start(args, fmt);
	// ciclo che prende fmt fino a quando non trova '/0'
	while (*fmt) { 
		if (*fmt == '%'){
			fmt++;//carattere dopo il '%'

			if(*fmt == 's'){
				char *str = va_arg(args, char*); // legge il valore di args e lo interperptreta come char
				pputs(str); //stampa la stringa
			}

			else if (*fmt == 'd') {
				int num = va_arg(args, int); // legge il valore di args e lo interperptreta come numero intero
				print_int(num);
			}
			
			else if (*fmt == 'c') {
				 char c = (char)va_arg(args, int); // legge il valore di args e lo interperptreta come int e poi char
				 pputc(c);
			}

			else if (*fmt == 'k') { // colori
			    int testo = va_arg(args, int);
			    current_color = (NERO << 4) | testo;
			}

			else if (*fmt == 'g'){ //posizione (x,y)   
				int x = va_arg(args, int);
				int y = va_arg(args, int);
				pos_char = y * 80 + x;
			}
			
		}else {
			// se non ce '%' semplicemente stampa il carattere
			pputc(*fmt);
		}
		
		fmt++;
	}
	
	va_end(args);
}

// ---------- strlen ----------

int strleng(const char *str){
	int i=0;
	while(str[i] != '\0') i++;
	return i;
}

// ---------- write_center ----------
void write_center(const char *str, int y, enum colori colore){
	int x = (78 - strleng(str)) /2 +1;
	write("%k%g%s",colore,x,y,str);
}
// ---------- clear screen ----------

#define vga_buffer ((volatile unsigned short*)video_mem)

void clear_screen(){
	volatile unsigned short *vga = (volatile unsigned short*)video_mem;
	for (int i= 0;i < 80 * 25 ; i++){
		vga[i]= (current_color << 8) | ' ';
	}
	pos_char = 0;
}

// ----------  bordo ----------

void bordo() {
    // angolo in alto a sinistra
    write("%g", 0, 0); 
    pputc(0xC9);
    
    // riga superiore
    for (int i = 1; i < 79; i++) {
        write("%g", i, 0); pputc(0xCD);
    }
    
    // angolo in alto a destra
    write("%g", 79, 0); pputc(0xBB);

    // bordi laterali
    for (int i = 1; i < 24; i++) {
        write("%g", 0,  i); pputc(0xBA);  // sinistro
        write("%g", 79, i); pputc(0xBA);  // destro
    }

    // angolo in basso a sinistra
    write("%g", 0, 24); pputc(0xC8);

    // riga inferiore
    for (int i = 1; i < 79; i++) {
        write("%g", i, 24); pputc(0xCD);
    }

    // angolo in basso a destra
    write("%g", 79, 24); pputc(0xBC);
}

// Confronta due stringhe. Restituisce 0 se sono uguali.
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
    return 0; // I primi n caratteri sono identici!
}

// disabilitare cursore
void disable_cursor(void) {
    // Seleziona il registro 0x0A (Cursor Start Register)
    __asm__ volatile ("outb %0, %1" :: "a"((uint8_t)0x0A), "Nd"((uint16_t)0x3D4));
    // Bit 5 = 1 allora cursore disabilitato
    __asm__ volatile ("outb %0, %1" :: "a"((uint8_t)0x20), "Nd"((uint16_t)0x3D5));
}

//---------------- STARTING OS ----------------------------------

void startOS (){
	disable_cursor();
	clear_screen();
	bordo();
	write_center("#####  #####  ####   #####   ###          ###     ###", 2, BLU_CHIARO);
	delay_ms(200);
	write_center("   #   #      #   #    #    #   #        #   #   #   ", 3, BLU_CHIARO);
	delay_ms(200);
	write_center("  #    #####  ####     #    #   #        #   #    ## ", 4, BLU_CHIARO);
	delay_ms(200);
	write_center(" #     #      #        #    #   #        #   #      #", 5, BLU_CHIARO);
	delay_ms(200);
	write_center("####   #####  #        #     ###          ###    ### ", 6, BLU_CHIARO);
	delay_ms(200);
	delay_s(1);
	write_center("v0.2.0",  9, VERDE_CHIARO);
	delay_s(1);
	write_center("Don't fall for who wants you to believe them , learn yourself",11, BIANCO);
	delay_s(1);
	for(int i=0;i<3;i++){
		write_center("|", 13, BIANCO);
		delay_ms(200);
		write_center("\b", 13, BIANCO);
		write_center("\\", 13, BIANCO);
		delay_ms(200);
		write_center("\b", 13, BIANCO);
		write_center("-", 13, BIANCO);
		delay_ms(200);
		write_center("\b", 13, BIANCO);
		write_center("/", 13, BIANCO);
		delay_ms(200);
		write_center("\b", 13, BIANCO);
	} 
	
	write_center("Starting OS...",  16, BIANCO);
	delay_s(1);
	write_center("ZeptoOS ready!",  16, BIANCO);
	delay_ms(500);
	clear_screen();
}