/*
 * Phase 2 test.
 * Prints a message using system call
 */

#include <uARMconst.h>
#include <libuarm.h>

#include "const.h"
#include "utils.h"
#include "ssi.h"

//use term 1 because term0 is used for debug prints
#define TERM1ADDR               0x25C

//type that support a device address
typedef uintptr_t devaddr;

#define do_terminal_io(dev, cmd) do_io((dev),(cmd),0,0)

static inline unsigned int do_io(devaddr device,  uintptr_t command, uintptr_t data1, uintptr_t data2) {
    uintptr_t retval;
    struct {
        uintptr_t reqtag;
        devaddr device;
        uintptr_t command;
        uintptr_t data1;
        uintptr_t data2;
    } req = { DO_IO, device, command, data1, data2};
    MsgSend(SSI_addr, &req);
    MsgRecv(SSI_addr, &retval);
    return retval;
}

static void ttyprintstring(devaddr device, char *s) {
    uintptr_t status;
    //until the current char is not 0
    for (; *s; s++) {
        status = do_terminal_io(device, DEV_TTRS_C_TRSMCHAR | (*s << 8));
        switch (status & 0x0F) {
            case DEV_S_READY:
            case DEV_TTRS_S_CHARTRSM: break;
            default: return;
        }
    }
}

void test(void){
    ttyprintstring(TERM1ADDR, "NUCLEUS TEST: starting...\n");
    HALT();
}

