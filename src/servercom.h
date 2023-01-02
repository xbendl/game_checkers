/**
* \file servercom.h
* Rozhran� modulu, kter� zaji�tuje komunikaci serveru s klientem
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

/// Zaji��uje p��jem a zas�l�n� zpr�v na stran� serveru
class ServerCom
{
public:
    /// Asociativn� pole se jm�nem p�ipojen�ho hr��e a k n�mu n�le��c�mu soketu
    typedef std::map<std::string,int> PlayerMap;

    /// Iter�tor pro typ PlayerMap
    typedef PlayerMap::iterator itPlayerMap;

    /// Struktura popisuj�c� aktu�ln� hran� hry
    typedef struct pData
    {
        int gameId;                ///< ID Hry
        std::string senderName;    ///< Jm�no vyzyvatele ke h�e
        int senderSock;            ///< Socket vyzyvatele ke h�e
        std::string receiverName;  ///< Jm�no protihr��e
        int receiverSock;          ///< Socket protihr��e
    } PlayData;

    /// Asociativn� pole s identifik�tory aktu�ln� hran�ch her a jm�ny z��astn�n�ch hr���
    typedef std::vector<PlayData> PlayVector;

private:
    XmlCom *xmlCom;

    /**
    * Za�le protihr��i identifik�tor nov� vytvo�en� hry pot�, co odsouhlas� jej� vytvo�en�
    * @param sock Soket hr��e, kter�mu se zpr�va bude zas�lat
    * @param gameId Identifik�tor hry
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false
    */
    bool SendGameId(int sock, int gameId);

    /**
    * Nalezne detaily hry v seznamu aktu�ln� hran�ch her.
    * @param playVector Vektor struktur s aktu�ln� hran�mi hrami
    * @param playData Struktura popisuj�c� hru
    * @param gameId Identifik�tor hry
    * @param senderSock Socket odesilatele
    * @return True, pokud se vyhled�n� poda�ilo, jinak false
    */
    bool FindInPlayGames(PlayVector &playVector, PlayData &playData, int senderSock, int gameId);

    /**
    * Vyhled� podle jm�na hr��e a detaily o jeho spoluhr��i ze seznamu aktu�ln� hran�ch her
    * @param playVector Vektor s aktu�ln� hran�mi hrami
    * @param playData Struktura popisuj�c� hru
    * @param playerName Socket hr��e, kter�mu hled�me protihr��e
    * @return True, pokud se vyhled�n� poda�ilo, jinak false
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
    * Sma�e hru ur�enou jej�m identifik�torem ze seznamu aktu�ln� hran�ch her
    * @param playVector Vektor struktur s aktu�ln� hran�mi hrami
    * @param gameId Identifik�tor hry, kter� m� b�t smaz�na
    * @return True, pokud se smaz�n� poda�ilo, jinak false
    */
    bool DeleteFromPlayVector(PlayVector &playVector, int gameId);

    /**
    * P�ihl�s� hr��e k serveru a za�le mu zp�t odpov��
    * @param sock Soket hr��e, kter� po�aduje p�ipojen� k serveru
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param playerSigned Ur�uje,zda-li se registrace hr��e podarila
    * @return True, pokud se p�ihl�eni hr��e poda�ilo, jinak false
    */
    bool SignToServer(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool &playerSigned);

    /**
    * P�epo�le ��dost o hru vybran�mu protihr��i
    * @param sock Soket hr��e, kter� po�aduje p�eposlat ��dost o hru
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @return True, pokud se p�eposl�n� ��dosti o hru poda�ilo, jinak false
    */
    bool SendAskPlayGame(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers);

    /**
    * P�epo�le odpov�� na ��dost o hru vyzyvateli ke h�e
    * @param sock Soket hr��e, kter� po�aduje p�eposlat ��dost o hru
    * @param answer Odpov�� na ��dost o hru
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param playVector Vektor struktur s detaily aktu�ln� hran�ch her
    * @return True, pokud se p�eposlan� odpov�di na ��dost o hru poda�ilo, jinak false
    */
    bool SendAnswerPlayGame(int sock, bool answer, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * P�epo�le zpr�vu o ukon�en� hry
    * @param sock Soket hr��e, kter�mu bude ��dost o ukon�en� hry doru�ena
    * @param gameId Identifik�tor ukon�ovan� hry hry
    * @return True, pokud se p�eposl�n� zpr�vy poda�ilo, jinak false
    */
    bool SendEndGame(int sock, int gameId);

    /**
    * P�epo�le zpr�vu o ukon�en� hry
    * @param sock Soket hr��e, kter� po�aduje p�eposlat zpr�vu o ukon�en� hry
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param playVector Vektor struktur s detaily aktu�ln� hran�ch her
    * @return True, pokud se p�eposl�n� zpr�vy poda�ilo, jinak false
    */
    bool SendEndGame(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * P�epo�le tah protihr��i
    * @param sock Soket hr��e, kter� po�aduje p�eposlat tah
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param playVector Vektor s detaily aktu�ln� hran�ch her
    * @return True, pokud se p�eposl�n� tahu protihr��i poda�ilo, jinak false
    */
    bool SendStep(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * P�epo�le zpr�vu ur�uj�c� v�t�zstv� ve h�e
    * @param sock Soket hr��e, kter� po�aduje p�eposlat tah
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param playVector Vektor s detaily aktu�ln� hran�ch her
    * @return True, pokud se p�eposl�n� zpr�vy poda�ilo, jinak false
    */
    bool SendWinInfo(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector);

    /**
    * Za�le seznam voln�ch hr���
    * @param sock Soket hr��e, kter�mu je seznam zasil�n
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @return True, pokud se zasl�n� seznamu voln�ch hr��� poda�ilo, jinak false
    */
    bool SendPlayerList(int sock, PlayerMap &freePlayers);

    /**
    * Za�le jm�no nov� p�ihl�en�ho hr��e v�em ostatnim p�ipojen�m hr���m
    * @param sock Soket hr��e, kter� se p�ihl�il
    * @param ignoreSock Soket hr��e, jemu� jm�no zas�lat nechceme
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @return True, pokud se zasl�n� jm�na poda�ilo, jinak false
    */
    bool SendInsertName(int sock, int ignoreSock, PlayerMap &logPlayers);

    /**
    * Za�le jm�no pr�v� odhl�en�ho hr��e v�em ostatn�m p�ipojen�m hr���m
    * @param sock Soket hr��e, kter� se odhl�sil
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @return True, pokud se zasl�n� jm�na poda�ilo, jinak false
    */
    bool SendDeleteName(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers);

    /**
    * Za�le jm�no pr�v� odhl�en�ho hr��e v�em ostatnim p�ipojen�m hr���m
    * @param sock Soket hr��e, kter� se odhl�sil
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param playVector Vektor struktur s detaily aktu�ln� hran�ch her
    * @param playData Detaily hry, kterou aktu�ln� odhla�ov�n� hr�� v dob� odpojen� hr�l
    * @return True, pokud se zasl�n� jm�na poda�ilo, jinak false
    */
    bool SendDeleteName(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector, PlayData &playData);

    /**
    * Za�le rozehran� hry s vybran�m protihr��em
    * @param sock Soket hr��e, kter� po�aduje nal�zt hry s protihr��em
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @return True, pokud se zasl�n� seznamu voln�ch hr��� poda�ilo, jinak false
    */
    bool SendGameList(int sock, PlayerMap &logPlayers);

    /**
    * Za�le tahy k rozehran� h�e s vybran�m protihr��em
    * @param sock Soket hr��e, kter� po�aduje nal�zt tahy k rozehran� h�e
    * @return True, pokud se zasl�n� seznamu tah� poda�ilo, jinak false
    */
    bool SendGameSteps(int sock);

    /**
    * Registruje hr��e k serveru a za�le mu zp�t odpov��
    * @param sock Soket hr��e, kter� po�aduje p�ipojen� k serveru
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param playerRegistered Ur�uje,zda-li se registrace hr��e poda�ila
    * @return True, pokud se registrace hr��e poda�ila, jinak false
    */
    bool RegisterToServer(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool &playerRegistered);

    /**
    * Nastav� viditelnost hr��e na serveru
    * @param sock Soket hr��e, kter� po�aduje prov�st zm�nu viditelnosti
    * @param logPlayers Pole jmen s p�ihl�en�mi hr��i
    * @param freePlayers Pole jmen s voln�mi hr��i
    * @param visibility Po�adovan� viditelnost hr��e
    * @return True, pokud se zm�na viditelnosti poda�ila, jinak false
    */
    bool SetPlayerVisibility(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool visibility);
};

#endif
