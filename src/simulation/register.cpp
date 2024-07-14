
#include "register.hpp"


REGISTER ::REGISTER(sc_module_name name) : sc_module(name) {
    SC_THREAD(update);
    sensitive << we.pos();
}


void REGISTER ::update() {
    while (true) {
        wait();
        q.write(d.read());
    }
}













