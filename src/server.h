/**
* \file server.h
* Rozhraní serveru, který slou¾í pro vyøizování po¾adavkù klientù
* \author Jaroslav Bendl (xbendl00)
*/


#ifndef SERVER_H
#define SERVER_H

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/file.h>
#include <iostream>
#include <fstream>
#include <string>
#include <errno.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iterator>
#include "sockcom.h"
#include "servercom.h"
#include "xmlcom.h"

/// Zaji¹»uje obsluhu událostí na pøipojených soketech
class DamaServer : public wxEvtHandler
{
public:
    /**
    * Konstruktor
    * @param port Èíslo portu, na kterém server pobe¾í
    */
    DamaServer(int port);

    /**
    * Destruktor
    */
    ~DamaServer();

private:
    /**
    * Vytvoøí soket, na kterém server naslouchá po¾adavkùm klientù
    * @return True, pokud se vytvoøení socketu podaøilo, jinak false
    */
    bool CreateSocket();

    /**
    * Získá po¾adavek od klienta
    * @return True, pokud se získání po¾adavku podaøilo, jinak false
    */
    bool ListenOnSocket();

    /**
    * Uzavøe hlavní soket a sokety pøipojených klientu
    */
    void ShutDownServer();

    /**
    * Zvolí dal¹í zpracování po¾adavkù klienta na základì øídicího (prvního) znaku pøijmuté zprávy
    * @return True, pokud bìhem obsluhy nedo¹lo k chybì, jinak false
    */
    bool SocketInput(int clientSock, unsigned char jumper);

    /**
    * Odstraní klienta ze serveru
    * @param sock Soket odpojeného klienta
    * @return True, pokud se odstranìní klienta podaøilo, jinak false
    */
    bool SocketLost(int sock);

    /**
    * Zapí¹e chybovou zprávu do logovacího souboru
    * @param message Zapisovaná zpráva
    * @return True, pokud se zapsaní zprávy podaøilo, jinak false
    */
    void WriteMessageToFile(std::string message);

    const static int capacityIncrement = 4;
    vector<int> clientVector;           ///< Sokety pøipojených hráèù
    int mPort;                          ///< Èíslo portu, na kterém server pobì¾í
    ServerCom *serverCom;               ///< Objekt obstarávající obsluhu po¾adavkù od klienta
    int mServer;                        ///< Soket serveru
    ServerCom::PlayerMap freePlayers;   ///< Pole volných hráèù
    ServerCom::PlayerMap logPlayers;    ///< Pole pøipojených hráèù
    ServerCom::PlayVector playVector;   ///< Vektor struktur s aktuálnì hranými hrami
};

#endif
