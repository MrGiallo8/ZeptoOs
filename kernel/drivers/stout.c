#include "../includes/stdfuncs.h"

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)
#define va_arg(v, l)    __builtin_va_arg(v, l)

unsigned char current_color = 0x07;

#define video_mem 0xB8000
#define white_black 0x07 

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

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

// ----------------print-----------------

void write(const char *fmt , ...){ // il '...' dice che ci sono x argomenti
	va_list args; // legge le variabili inseriti nello stack uno alla volta
	va_start(args, fmt);
	// ciclo che prende fmt fino a quando non trova '/0'
	while (*fmt) { 
		if (*fmt == '%'){
			fmt++; //carattere dopo il '%'

			if(*fmt == 's'){
				char *str = va_arg(args, char*); // legge il valore di args e lo interperptreta come char
				pputs(str); //stampa la stringa
			}

			else if (*fmt == 'd') {
				int num = va_arg(args, int); // legge il valore di args e lo interperptreta come numero intero
				print_int(num);
			}

			else if (*fmt == 'u') {
				uint64_t num = va_arg(args, uint64_t );

				char buffer[32];
				int i=0;

				if(num == 0){
					print_int(0);
				} else {
					while (num > 0){
						buffer[i++] = '0' + (num%10);
						num/=10;
					}

					while (i > 0){
						pputc(buffer[--i]);
					}
				}
			}
			
			else if (*fmt == 'c') {
				char c = (char)va_arg(args, int); // legge il valore di args e lo interperptreta come int e poi char
				pputc(c);
			}

			else if (*fmt == 'k') { // colori
			    int testo = va_arg(args, int);
			    current_color = (NERO << 4) | testo;
			}

			else if (*fmt == 'g'){ // posizione (x,y)   
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
    // alto a sinistra
    write("%g", 0, 0); 
    pputc(0xC9);
    
    // superiore
    for (int i = 1; i < 79; i++) {
        write("%g", i, 0); pputc(0xCD);
    }
    
    // alto a destra
    write("%g", 79, 0); pputc(0xBB);

    // bordi laterali
    for (int i = 1; i < 24; i++) {
        write("%g", 0,  i); pputc(0xBA);  // sx
        write("%g", 79, i); pputc(0xBA);  // dx
    }

    // basso a sinistra
    write("%g", 0, 24); pputc(0xC8);

    // inferiore
    for (int i = 1; i < 79; i++) {
        write("%g", i, 24); pputc(0xCD);
    }

    // basso a destra
    write("%g", 79, 24); pputc(0xBC);
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

