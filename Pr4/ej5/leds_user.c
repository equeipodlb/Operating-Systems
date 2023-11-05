#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int fd;
    char* led_state;

    /* Abrir el fichero de dispositivo */
    fd = open("/dev/leds", O_WRONLY);
    if (fd < 0) {
        perror("Error al abrir el fichero de dispositivo");
        return 1;
    }

    /* Realizar una secuencia de encendido y apagado de los leds */
    led_state = "1";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "2";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "3";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "1";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "2";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "3";
    write(fd, led_state, 1);
    sleep(1);
    led_state = "";
    write(fd, led_state, 1);

    /* Cerrar el fichero de dispositivo */
    close(fd);

    return 0;
}