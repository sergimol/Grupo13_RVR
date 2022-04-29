#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* buffer = _data;
    memcpy(buffer, &type, sizeof(uint8_t));
    buffer += sizeof(uint8_t);
    nick[nick.size() + 1] = '\0';
    memcpy(buffer, nick.c_str(), 8 * sizeof(char));
    buffer += 8 * sizeof(char);
    message[message.size() + 1] = '\0';
    memcpy(buffer, message.c_str(), 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char * buffer = _data;
    memcpy(&type, buffer, sizeof(uint8_t));
    buffer += sizeof(uint8_t);
    memcpy(&nick[0], buffer, 8 * sizeof(char));
    buffer += 8 * sizeof(char);
    memcpy(&message[0], buffer, 80 * sizeof(char));
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        Socket* clnt;
        ChatMessage msg;
        socket.recv(msg, clnt);

        switch(msg.type){
            case ChatMessage::LOGIN:
                clients.push_back(clnt);int count = 0;
                std::cout << msg.nick.c_str() << " se conectó\n";
                break;
            case ChatMessage::LOGOUT:
                std::cout << msg.nick.c_str() << " se desconectó\n";
                int i = 0;
                for(Socket* sock: clients) {
                    if(*sock == *clnt) {
                        clients.erase(clients.begin() + i);
                        break;
                    }
                    i++;
                }
                break;
            case ChatMessage::MESSAGE:
                for(Socket* sock: clients) {
                    if(!(*sock == *clnt)){
                        socket.send(msg, *sock);
                        std::cout << msg.nick.c_str() << " envió un mensaje\n";
                        break;
                    }
                }
                break;
            default:
                break;
        }

        std::cout << "Conectados " << clients.size() << "\n";
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

