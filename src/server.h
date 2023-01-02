/**
* \file server.h
* Rozhran� serveru, kter� slou�� pro vy�izov�n� po�adavk� klient�
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

/// Zaji��uje obsluhu ud�lost� na p�ipojen�ch soketech
class DamaServer : public wxEvtHandler
{
public:
    /**
    * Konstruktor
    * @param port ��slo portu, na kter�m server pobe��
    */
    DamaServer(int port);

    /**
    * Destruktor
    */
    ~DamaServer();

private:
    /**
    * Vytvo�� soket, na kter�m server naslouch� po�adavk�m klient�
    * @return True, pokud se vytvo�en� socketu poda�ilo, jinak false
    */
    bool CreateSocket();

    /**
    * Z�sk� po�adavek od klienta
    * @return True, pokud se z�sk�n� po�adavku poda�ilo, jinak false
    */
    bool ListenOnSocket();

    /**
    * Uzav�e hlavn� soket a sokety p�ipojen�ch klientu
    */
    void ShutDownServer();

    /**
    * Zvol� dal�� zpracov�n� po�adavk� klienta na z�klad� ��dic�ho (prvn�ho) znaku p�ijmut� zpr�vy
    * @return True, pokud b�hem obsluhy nedo�lo k chyb�, jinak false
    */
    bool SocketInput(int clientSock, unsigned char jumper);

    /**
    * Odstran� klienta ze serveru
    * @param sock Soket odpojen�ho klienta
    * @return True, pokud se odstran�n� klienta poda�ilo, jinak false
    */
    bool SocketLost(int sock);

    /**
    * Zap�e chybovou zpr�vu do logovac�ho souboru
    * @param message Zapisovan� zpr�va
    * @return True, pokud se zapsan� zpr�vy poda�ilo, jinak false
    */
    void WriteMessageToFile(std::string message);

    const static int capacityIncrement = 4;
    vector<int> clientVector;           ///< Sokety p�ipojen�ch hr���
    int mPort;                          ///< ��slo portu, na kter�m server pob��
    ServerCom *serverCom;               ///< Objekt obstar�vaj�c� obsluhu po�adavk� od klienta
    int mServer;                        ///< Soket serveru
    ServerCom::PlayerMap freePlayers;   ///< Pole voln�ch hr���
    ServerCom::PlayerMap logPlayers;    ///< Pole p�ipojen�ch hr���
    ServerCom::PlayVector playVector;   ///< Vektor struktur s aktu�ln� hran�mi hrami
};

#endif
