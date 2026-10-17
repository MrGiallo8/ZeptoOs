#include "../includes/stdfuncs.h"
#include "../includes/stout.h"
#include "../includes/keyboard.h"
#include "../includes/time.h"
#include "../includes/shell.h"
#include "../includes/ata.h"

void kernel_main() {
    time_init();
    startOS();
    terminal_run();
}
