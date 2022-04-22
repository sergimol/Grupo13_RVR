#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        // Reserva memoria para todos los datos
        alloc_data(sizeof(char) * MAX_NAME + sizeof(int16_t) * 2);
        char* buffer = _data;
        // Copìa el nombre en el buffer
        memcpy(buffer, (void*)name, sizeof(char) * MAX_NAME);
        buffer += sizeof(char) * MAX_NAME;
        // Copìa la posición de x en el buffer
        memcpy(buffer, &x, sizeof(int16_t));
        buffer += sizeof(int16_t);
        // Copìa la posición de x en el buffer
        memcpy(buffer, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        // Copia data en el buffer
        char* buffer = data + sizeof(int32_t);
        // Copia el nombre del buffer
        memcpy(name, buffer, MAX_NAME);
        buffer += MAX_NAME;
        // Copia x del buffer
        memcpy(&x, buffer, sizeof(int16_t));
        buffer += sizeof(int16_t);
        // Copia y del buffer
        memcpy(&y, buffer, sizeof(int16_t));
        return 0;
    }


public:
    const static size_t MAX_NAME = 80;
    char name[MAX_NAME];
    
    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int file = open("./jugador.bin", O_CREAT | O_TRUNC | O_RDWR, 0666);
    write(file, one_w.data(), one_w.size());
    close(file);
    // 3. Leer el fichero
    char buffer[256];
    file = ("./jugador.bin", O_RDONLY);
    read(file, buffer, 256);
    close(file);
    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);
    // 5. Mostrar el contenido de one_r
    std::cout << "Name: " << one_r.name << " X: " << one_r.x << " Y: " << one_r.y << "\n";

    return 0;
}

