
#include "register.hpp"

template<int size>
REGISTER<size>::REGISTER(sc_module_name name) : sc_module(name) {
    SC_THREAD(update);
    sensitive << we.pos();
}

template<int size>
void REGISTER<size>::update() {
    while (true) {
        wait();
        q.write(d.read());
    }
}




