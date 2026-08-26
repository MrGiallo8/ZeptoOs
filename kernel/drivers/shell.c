#include "../includes/stout.h"
#include "../includes/keyboard.h"
#include "../includes/time.h"

#define MAX_LINES 64
#define MAX_LINE_LEN 256

char editor_buffer[MAX_LINES][MAX_LINE_LEN];
int  editor_line_count = 0;

void text_editor() {
    clear_screen();
    char input[MAX_LINE_LEN];
    int line = 0;

    write("\n%k====================================%k[EDITOR]%k====================================\n",
          BIANCO, VERDE, BIANCO);

    // Mostra il contenuto già salvato (se si riapre)
    if (editor_line_count > 0) {
        write("%k-- contenuto salvato --%k\n", GIALLO, BIANCO);
        for (int i = 0; i < editor_line_count; i++) {
            write("  %d: %s\n", i + 1, editor_buffer[i]);
        }
        write("%k-- fine -- continua a scrivere, ***quit per uscire --%k\n", GIALLO, BIANCO);
        line = editor_line_count;  // continua dall'ultima riga 
    }

    while (1) {
        kb_readline(input, MAX_LINE_LEN, pputc);

        if (strcmp(input, "***quit") == 0) {
            // Salva in RAM
            editor_line_count = line;
            return;

        } else if (strcmp(input, "***show") == 0) {
            // Mostra il testo scritto fino ad ora
            for (int i = 0; i < line; i++) {
                write("  %d: %s\n", i + 1, editor_buffer[i]);
            }

        } else if (strcmp(input, "***clear") == 0) {
            // Svuota il buffer
            line = 0;
            editor_line_count = 0;
            write("%k[buffer svuotato]%k\n", ROSSO, BIANCO);

        } else if (line < MAX_LINES) {
            // Salva la riga nel buffer
            int i;
            for (i = 0; input[i] && i < MAX_LINE_LEN - 1; i++)
                editor_buffer[line][i] = input[i];
            editor_buffer[line][i] = '\0';
            line++;
        } else {
            write("%k[buffer pieno!]%k\n", ROSSO, BIANCO);
        }
    }
}

void terminal_base(){
	write("\n%k\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB", BIANCO);
	write("\n%k\xBA                        %kZeptoOS Kernel v0.2.1 alpha                         %k\xBA", BIANCO, BLU_CHIARO, BIANCO);
	write("\n%k\xCC\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB9", BIANCO);
	write("\n%k\xBA  Mode: Protected 32-bit       | CPU: x86 fam                               \xBA", BIANCO);
	write("\n%k\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC", BIANCO);
	write("\n");
}

void terminal_run(){
	char input[256];
	clear_screen();
	
	terminal_base();
	int line_terminal = 7; 

	pos_char = line_terminal * 80;

	while (1) {
		write("%kZeptoOS > ",BIANCO);
		kb_readline(input, 256, pputc);
		write("\n");
		    
		if(strcmp(input, "help") == 0){
			write("%k help     - show commands\n",BIANCO);
			write("%k clear    - clear the screen\n",BIANCO);
			write("%k quit     - exit the shell\n",BIANCO);
			write("%k print    - print the following string",BIANCO);
			write("%k sysinfo  - system infos",BIANCO);
			write("%k uptime   - shows kernel uptime");
			write("%k time     - shows time");
			
		}
		else if (strcmp(input, "clear") == 0){
			clear_screen();
			pos_char = line_terminal * 80;
			terminal_base();

		}
		else if (strcmp_n(input, "print ",6) == 0){
			write( input + 6 );
			write("\n\n");
		}
		else if(strcmp(input, "sysinfo")== 0){

			write("%k [ OS ]       %kZeptoOS v0.2.1 alpha\n", GRIGIO_SCURO, BIANCO);
    		write("%k [ Mode ]     %kx86 (32-bit Protected Mode)\n", GRIGIO_SCURO, BIANCO);
    		write("%k [ Display ]  %kVGA Text Mode (80x25)\n", GRIGIO_SCURO, BIANCO);
    		write("%k [ Storage ]  %kFAT12 (fs.img)\n\n", GRIGIO_SCURO, BIANCO);

		}
		else if(strcmp(input, "uptime") == 0){
			int uptime = get_uptime_sec();
			write("%kUptime: %d sec\n\n", BIANCO, uptime);
		}
		else if(strcmp(input, "time")== 0){
			uint8_t hrs = get_rtc_hours();
    		uint8_t min = get_rtc_minutes();
    		uint8_t sec = get_rtc_seconds();

			if(min<10) {write("0");} write("%k[ %d:",BIANCO,hrs+2);

			if(min<10) {write("0");} write("%k%d:",BIANCO,min);

			if(sec<10) {write("0");} write("%k%d ]\n\n",BIANCO,sec);
		}
		else if(strcmp(input, "") == 0){
			// Nulla
		}
		else {
			write("%k Unknown command: ",ROSSO);
			write(input);
			write("\n\n");
		}
	}
	
}

