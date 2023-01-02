/**
* \file servercom.h
* Rozhraní modulu, který zaji¹tuje komunikaci serveru s klientem
* \author Jaroslav Bendl (xbendl00)
*/


#ifndef SERVERCOM_H
#define SERVERCOM_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <errno.h>
#include <map>
#include "sockcom.h"
#include "xmlcom.h"

using namespace std;

/// Zaji¹»uje pøíjem a zasílání zpráv na stranì serveru
class ServerCom
{
public:
    /// Asociativní pole se jménem pøipojeného hráèe a k nìmu nále¾ícímu soketu
    typedef std::map<std::string,int> PlayerMap;

    /// Iterátor pro typ PlayerMap
    typedef PlayerMap::iterator itPlayerMap;

    /// Struktura popisující aktuálnì hrané hry
    typedef struct pData
    {
        int gameId;                ///< ID Hry
        std::string senderName;    ///< Jméno vyzyvatele ke høe
        int senderSock;            ///< Socket vyzyvatele ke høe
        std::string receiverName;  ///< Jméno protihráèe
        int receiverSock;          ///< Socket protihráèe
    } PlayData;

    /// Asociativní pole s identifikátory aktuálnì hraných her a jmény zúèastnìných hráèù
    typedef std::vector<PlayData> PlayVector;

private:
    XmlCom *xmlCom;

    /**
    * Za¹le protihráèi identifikátor novì vytvoøené hry poté, co odsouhlasí její vytvoøení
    * @param sock Soket hráèe, kterému se zpráva bude zasílat
    * @param gameId Identifikátor hry
    * @return True, pokud se zaslání zprávy podaøilo, jinak false
    */
    bool SendGameId(int sock, int gameId);

    /**
    * Nalezne detaily hry v seznamu aktuálnì hraných her.
    * @param playVector Vektor struktur s aktuálnì hranými hrami
    * @param playData Struktura popisující hru
    * @param gameId Identifikátor hry
    * @param senderSock Socket odesilatele
    * @return True, pokud se vyhledání podaøilo, jinak false
    */
    bool FindInPlayGames(PlayVector &playVector, PlayData &playData, int senderSock, int gameId);

    /**
    * Vyhledá podle jména hráèe a detaily o jeho spoluhráèi ze seznamu aktuálnì hraných her
    * @param playVector Vektor s aktuálnì hranými hrami
    * @param playData Struktura popisující hru
    * @param playerName Socket hráèe, kterému hledáme protihráèe
    * @return True, pokud se vyhledání podaøilo, jinak false
    */
    bool FindInPlayGames(PlayVector &playVector, PlayData &playData, std::string &playerName);

public:
    /**
    * Konstruktor
    */
    ServerCom()
    {
        xmlCom = new XmlCom(_("games_server.xml"), _("players_server.xml"));
    }

    /**
    * Destruktor
    */
    ~ServerCom()
    {
        delete xmlCom;
    }

    /**
    * Sma¾e hru urèenou jejím identifikátorem ze seznamu aktuálnì hraných her
    * @param playVector Vektor struktur s aktuálnì hranými hrami
    * @param gameId Identifikátor hry, která má být smazána
    * @return True, pokud se smazání podaøilo, jinak false
    */
    bool DeleteFromPlayVector(PlayVector &playVector, int gameId);

    /**
    * Pøihlásí hráèe k serveru a za¹le mu zpìt odpovìï
    * @param sock Soket hráèe, který po¾aduje pøipojení k serveru
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param freePlayers Pole jmen s volnými hráèi
    * @param playerSigned Urèuje,zda-li se registrace hráèe podarila
    * @return True, pokud se pøihlá¹eni hráèe podaøilo, jinak false
    */
    bool SignToServer(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool &playerSigned);

    /**
    * Pøepo¹le ¾ádost o hru vybranému protihráèi
    * @param sock Soket hráèe, který po¾aduje pøeposlat ¾ádost o hru
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @return True, pokud se pøeposlání ¾ádosti o hru podaøilo, jinak false
    */
    bool SendAskPlayGame(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers);

    /**
    * Pøepo¹le odpovìï na ¾ádost o hru vyzyvateli ke høe
    * @param sock Soket hráèe, který po¾aduje pøeposlat ¾ádost o hru
    * @param answer Odpovìï na ¾ádost o hru
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param playVector Vektor struktur s detaily aktuálnì hraných her
    * @return True, pokud se pøeposlaní odpovìdi na ¾ádost o hru podaøilo, jinak false
    */
    bool SendAnswerPlayGame(int sock, bool answer, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * Pøepo¹le zprávu o ukonèení hry
    * @param sock Soket hráèe, kterému bude ¾ádost o ukonèení hry doruèena
    * @param gameId Identifikátor ukonèované hry hry
    * @return True, pokud se pøeposlání zprávy podaøilo, jinak false
    */
    bool SendEndGame(int sock, int gameId);

    /**
    * Pøepo¹le zprávu o ukonèení hry
    * @param sock Soket hráèe, který po¾aduje pøeposlat zprávu o ukonèení hry
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param playVector Vektor struktur s detaily aktuálnì hraných her
    * @return True, pokud se pøeposlání zprávy podaøilo, jinak false
    */
    bool SendEndGame(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * Pøepo¹le tah protihráèi
    * @param sock Soket hráèe, který po¾aduje pøeposlat tah
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param playVector Vektor s detaily aktuálnì hraných her
    * @return True, pokud se pøeposlání tahu protihráèi podaøilo, jinak false
    */
    bool SendStep(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * Pøepo¹le zprávu urèující vítìzství ve høe
    * @param sock Soket hráèe, který po¾aduje pøeposlat tah
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param playVector Vektor s detaily aktuálnì hraných her
    * @return True, pokud se pøeposlání zprávy podaøilo, jinak false
    */
    bool SendWinInfo(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * Za¹le seznam volných hráèù
    * @param sock Soket hráèe, kterému je seznam zasilán
    * @param freePlayers Pole jmen s volnými hráèi
    * @return True, pokud se zaslání seznamu volných hráèù podaøilo, jinak false
    */
    bool SendPlayerList(int sock, PlayerMap &freePlayers);

    /**
    * Za¹le jméno nové pøihlá¹eného hráèe v¹em ostatnim pøipojeným hráèùm
    * @param sock Soket hráèe, který se pøihlá¹il
    * @param ignoreSock Soket hráèe, jemu¾ jméno zasílat nechceme
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @return True, pokud se zaslání jména podaøilo, jinak false
    */
    bool SendInsertName(int sock, int ignoreSock, PlayerMap &logPlayers);

    /**
    * Za¹le jméno právì odhlá¹eného hráèe v¹em ostatním pøipojeným hráèùm
    * @param sock Soket hráèe, který se odhlásil
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @return True, pokud se zaslání jména podaøilo, jinak false
    */
    bool SendDeleteName(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers);

    /**
    * Za¹le jméno právì odhlá¹eného hráèe v¹em ostatnim pøipojeným hráèùm
    * @param sock Soket hráèe, který se odhlásil
    * @param freePlayers Pole jmen s volnými hráèi
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param playVector Vektor struktur s detaily aktuálnì hraných her
    * @param playData Detaily hry, kterou aktuálnì odhla¹ováný hráè v dobì odpojení hrál
    * @return True, pokud se zaslání jména podaøilo, jinak false
    */
    bool SendDeleteName(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector, PlayData &playData);

    /**
    * Za¹le rozehrané hry s vybraným protihráèem
    * @param sock Soket hráèe, který po¾aduje nalézt hry s protihráèem
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @return True, pokud se zaslání seznamu volných hráèù podaøilo, jinak false
    */
    bool SendGameList(int sock, PlayerMap &logPlayers);

    /**
    * Za¹le tahy k rozehrané høe s vybraným protihráèem
    * @param sock Soket hráèe, který po¾aduje nalézt tahy k rozehrané høe
    * @return True, pokud se zaslání seznamu tahù podaøilo, jinak false
    */
    bool SendGameSteps(int sock);

    /**
    * Registruje hráèe k serveru a za¹le mu zpìt odpovìï
    * @param sock Soket hráèe, který po¾aduje pøipojení k serveru
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param freePlayers Pole jmen s volnými hráèi
    * @param playerRegistered Urèuje,zda-li se registrace hráèe podaøila
    * @return True, pokud se registrace hráèe podaøila, jinak false
    */
    bool RegisterToServer(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool &playerRegistered);

    /**
    * Nastaví viditelnost hráèe na serveru
    * @param sock Soket hráèe, který po¾aduje provést zmìnu viditelnosti
    * @param logPlayers Pole jmen s pøihlá¹enými hráèi
    * @param freePlayers Pole jmen s volnými hráèi
    * @param visibility Po¾adovaná viditelnost hráèe
    * @return True, pokud se zmìna viditelnosti podaøila, jinak false
    */
    bool SetPlayerVisibility(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool visibility);
};

#endif
