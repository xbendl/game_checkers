/**
* \file clientcom.h
* Rozhran� modulu zaji��uj�c�ho komunikaci klienta se serverem
* \author Jaroslav Bendl (xbendl00)
*/

#ifndef CLIENTCOM_H
#define CLIENTCOM_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <strings.h>
#include <vector>
#include "sockcom.h"

/// Zaji��uje p��jem a zas�l�n� zpr�v na stran� klienta
class ClientCom
{
    wxSocketClient *mSock;  ///< Soket, ke kter�mu je p�ipojen� v�z�no

public:
    /**
    * Konstruktor
    * @param sock Soket, ke kter�mu je p�ipojen� v�z�no
    */
    ClientCom(wxSocketClient *sock)
    {
        mSock = sock;
    }

    /**
    * P�ipoj� hr��e ke vzd�len�mu serveru
    * @param addr Jednozna�n� ur�uje adresu vzd�len�ho serveru
    * @return True, pokud se p�ipojen� poda�ilo, jinak false
    */
    bool ConnectToServer(wxIPV4address addr);

    /**
    * Odpoj� hr��e od vzd�len�ho serveru
    * @return True, pokud se odpojen� hr��e poda�ilo, jinak false
    */
    bool DisconnectFromServer();

    /**
    * Za�le ke vzd�len�mu serveru p�ihla�ovac� �daje
    * @param name Zas�lan� p�ihla�ovac� jm�no
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false
    */
    bool SignToServer(wxString &name);

    /**
    * Za�le ��dost o hru s vybran�m protihr��em
    * @param challenger Jm�no vyzyvatele
    * @param opponent Jm�no vyzyvatele
    * @param gameId Jde-li o ��dost k dohr�n� ji� rozehran� hry, ud�v� jej� identifik�tor, jinak m� hodnotu -1
    * @param date Datum, kdy byla hra rozehr�na
    * @param onStep Ur�uje, zda-li bude p��jmece zpr�vy na tahu
    * @return True, pokud se Zasl�n� zpr�vy poda�ilo, jinak false
    */
    bool SendAskPlayGame(wxString &challenger, wxString &opponent, int gameId, time_t date, bool onStep);

    /**
    * Z�sk� po�adavek o novu hru s dan�m vyzyvatelem
    * @param challengerName Jm�no vyzyvatele
    * @param gameId Identifik�tor hry
    * @param date Datum, kdy byla hra rozehr�na
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false
    */
    bool GetAskPlayGame(wxString &challengerName, int &gameId, time_t &date);

    /**
    * Z�sk� po�adavek o dohr�n� rozehran� hry s dan�m vyzyvatelem
    * @param challengerName Jm�no vyzyvatele
    * @param gameId Identifik�tor hry
    * @param date Datum, kdy byla hra rozehr�na
    * @param gameSteps Vektor jednotliv�ch tah� rozehran� hry
    * @param onStep Ur�uje hr��e na tahu
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetAskContGame(wxString &challengerName, int &gameId, time_t &date, SockCom::StepVector &gameSteps, bool &onStep);

    /**
    * Za�le odpov�ï na po�adavek o hru s dan�m protihr��em
    * @param answer Vyjad�uje souhlas �i nesouhlas hr��e se hrou
    * @param challengerName Jm�no vyzivatele ke h�e
    * @param gameId Jde-li o ��dost k dohr�n� ji� rozehran� hry, ud�v� jej� ID, jinak m� hodnotu -1
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendAnswerPlayGame(bool answer, wxString &challengerName, int gameId);

    /**
    * Z�sk� odpov�d na ��dost o hru, pokud byla ��dost p�ijata.
    * @param gameId Jde-li o ��dost k dohr�n� ji� rozehran� hry, ud�v� jeji ID, jinak m� hodnotu -1
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetAnswerPlayGame(int &gameId);

    /**
    * Z�sk� na serveru vygenerovan� identifik�tor nov� vytvo�en� hry
    * @param gameId Identifik�tor nov� vytvo�en� hry
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetGameId(int &gameId);

    /**
    * Za�le protihr��i zpr�vu o ukon�eni hry
    * @param opponentName Jm�no protihr��e
    * @param gameId Identifik�tor ukon�ovan� hry
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendEndGame(wxString &opponentName, int &gameId);

    /**
    * Z�sk� zpr�vu o ukon�en� hry
    * @param gameId Identifik�tor ukon�ovan� hry
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetEndGame(int &gameId);

    /**
    * Za�le u�ivatel�v tah protihr��i
    * @param gameId Identifik�tor hry
    * @param dataVector Vektor se zas�lan�mi pohyby na �achovnici v r�mci tahu
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendStep(int gameId, SockCom::DataVector &dataVector);

    /**
    * Z�sk� tah od protihr��e
    * @param gameId Identifik�tor hry
    * @param steps Vektor s p�ijiman�mi pohyby na �achovnici v r�mci tahu
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetStep(int &gameId, SockCom::DataVector &steps);

    /**
    * Za�le protihr��i zpr�vu o jeho v�t�zstv�
    * @param gameId Identifik�tor rozehran� hry
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendWinInfo(int gameId);

    /**
    * Z�sk� zpr�vu od protihr��e ur�uj�c�, �e je v�t�zem hry
    * @param gameId Identifik�tor rozehran� hry
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetWinInfo(int &gameId);

    /**
    * Z�sk� seznam voln�ch hr���
    * @param freePlayers Vektor jmen voln�ch hr���
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetFreePlayers(SockCom::StringVector &freePlayers);

    /**
    * Z�sk� jm�no nov� p�ipojen�ho hr��e
    * @param insertPlayer Jm�no p�ipojovan�ho hr��e
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetInsertName(wxString &insertPlayer);

    /**
    * Z�sk� jm�no pr�v� odpojen�ho hr��e a odstran� jej ze seznamu
    * @param deletePlayer Jm�no odpojovan�ho hr��e
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetDeleteName(wxString &deletePlayer);

    /**
    * Za�le ke vzd�len�mu serveru po�adavek na seznam rozehran�ch her s vybran�m protihr��em
    * @param opponentName Jm�no protihr��e
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendAskGameList(wxString &opponentName);

    /**
    * Z�sk� seznam rozehran�ch her s vybran�m protihr��em
    * @param opponentGames Vektor her rozehran�mch s vybran�m protihr��em
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetGameList(SockCom::GameVector &opponentGames);

    /**
    * Za�le ke vzd�len�mu serveru po�adavek na seznam tah� k rozehran� h�e
    * @param gameId Identifik�tor rozehran� hry
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendAskGameSteps(int gameId);

    /**
    * Z�sk� seznam tah� k rozehran� h�e
    * @param gameSteps Vektor jednotliv�ch tah� rozehran� hry
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
    */
    bool GetGameSteps(SockCom::StepVector &gameSteps);

    /**
    * Za�le ke vzd�len�mu serveru registrovan� jm�no
    * @param registeredName Registrovan� jm�no
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool RegisterToServer(wxString &registeredName);

    /**
    * Nastav� viditelnost hr��e
    * @param visibility Po�adovan� viditelnost
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendVisibility(bool visibility);
};

#endif
