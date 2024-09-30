//questo serve per poter utilizzare le socket su windows
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <thread>
#include <stdlib.h>
#include "threads_main.h"

int main(){
    printf("Main: creazione gateway\n");
    std::thread gatewayT(gateway_run);
    while (true);
}
