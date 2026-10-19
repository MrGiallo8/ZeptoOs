#include <stout.h>
#include <time.h>
#include <keyboard.h>
#include <stdfuncs.h>
#include <fat12.h>
#include <ata.h>
#include <shell.h>
#include <games.h>

void kernel_main() {
    time_init();
    startOS();
    terminal_run();
}
