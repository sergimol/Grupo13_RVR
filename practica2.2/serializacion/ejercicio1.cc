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
        // Asigna memoria para los datos
        int32_t jugador_size = 2 * sizeof(int16_t) + MAX_NAME * sizeof(char);
        alloc_data(jugador_size);
        char * tmp = _data;
        // Copia el nombre
        memcpy(tmp, name, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);
        // Copia la x
        memcpy(tmp, &x, sizeof(int16_t));
        tmp += sizeof(int16_t);
        // Copia la y
        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        // Copia data en el buffer
        char* buffer = data;
        // Copia el nombre del buffer
        memcpy(name, buffer, MAX_NAME * sizeof(char));
        buffer += MAX_NAME * sizeof(char);
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
    file = open("./jugador.bin", O_RDONLY);
    read(file, buffer, 256);
    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);
    close(file);
    // 5. Mostrar el contenido de one_r
    std::cout << "Name: " << one_r.name << " X: " << one_r.x << " Y: " << one_r.y << "\n";

    return 0;
}

