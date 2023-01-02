/**
* \file clientcom.h
* Rozhraní modulu zaji¹»ujícího komunikaci klienta se serverem
* \author Jaroslav Bendl (xbendl00)
*/

#ifndef CLIENTCOM_H
#define CLIENTCOM_H

#include <wx/wx.h>
#include <wx/socket.h>
#include <strings.h>
#include <vector>
#include "sockcom.h"

/// Zaji¹»uje pøíjem a zasílání zpráv na stranì klienta
class ClientCom
{
    wxSocketClient *mSock;  ///< Soket, ke kterému je pøipojení vázáno

public:
    /**
    * Konstruktor
    * @param sock Soket, ke kterému je pøipojení vázáno
    */
    ClientCom(wxSocketClient *sock)
    {
        mSock = sock;
    }

    /**
    * Pøipojí hráèe ke vzdálenému serveru
    * @param addr Jednoznaènì urèuje adresu vzdáleného serveru
    * @return True, pokud se pøipojení podaøilo, jinak false
    */
    bool ConnectToServer(wxIPV4address addr);

    /**
    * Odpojí hráèe od vzdáleného serveru
    * @return True, pokud se odpojení hráèe podaøilo, jinak false
    */
    bool DisconnectFromServer();

    /**
    * Za¹le ke vzdálenému serveru pøihla¹ovací údaje
    * @param name Zasílané pøihla¹ovací jméno
    * @return True, pokud se zaslání zprávy podaøilo, jinak false
    */
    bool SignToServer(wxString &name);

    /**
    * Za¹le ¾ádost o hru s vybraným protihráèem
    * @param challenger Jméno vyzyvatele
    * @param opponent Jméno vyzyvatele
    * @param gameId Jde-li o ¾ádost k dohrání ji¾ rozehrané hry, udává její identifikátor, jinak má hodnotu -1
    * @param date Datum, kdy byla hra rozehrána
    * @param onStep Urèuje, zda-li bude pøíjmece zprávy na tahu
    * @return True, pokud se Zaslání zprávy podaøilo, jinak false
    */
    bool SendAskPlayGame(wxString &challenger, wxString &opponent, int gameId, time_t date, bool onStep);

    /**
    * Získá po¾adavek o novu hru s daným vyzyvatelem
    * @param challengerName Jméno vyzyvatele
    * @param gameId Identifikátor hry
    * @param date Datum, kdy byla hra rozehrána
    * @return True, pokud se získání zprávy podaøilo, jinak false
    */
    bool GetAskPlayGame(wxString &challengerName, int &gameId, time_t &date);

    /**
    * Získá po¾adavek o dohrání rozehrané hry s daným vyzyvatelem
    * @param challengerName Jméno vyzyvatele
    * @param gameId Identifikátor hry
    * @param date Datum, kdy byla hra rozehrána
    * @param gameSteps Vektor jednotlivých tahù rozehrané hry
    * @param onStep Urèuje hráèe na tahu
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetAskContGame(wxString &challengerName, int &gameId, time_t &date, SockCom::StepVector &gameSteps, bool &onStep);

    /**
    * Za¹le odpovìÃ¯ na po¾adavek o hru s daným protihráèem
    * @param answer Vyjadøuje souhlas èi nesouhlas hráèe se hrou
    * @param challengerName Jméno vyzivatele ke høe
    * @param gameId Jde-li o ¾ádost k dohrání ji¾ rozehrané hry, udává její ID, jinak má hodnotu -1
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendAnswerPlayGame(bool answer, wxString &challengerName, int gameId);

    /**
    * Získá odpovìd na ¾ádost o hru, pokud byla ¾ádost pøijata.
    * @param gameId Jde-li o ¾ádost k dohrání ji¾ rozehrané hry, udává jeji ID, jinak má hodnotu -1
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetAnswerPlayGame(int &gameId);

    /**
    * Získá na serveru vygenerovaný identifikátor novì vytvoøené hry
    * @param gameId Identifikátor nové vytvoøené hry
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetGameId(int &gameId);

    /**
    * Za¹le protihráèi zprávu o ukonèeni hry
    * @param opponentName Jméno protihráèe
    * @param gameId Identifikátor ukonèované hry
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendEndGame(wxString &opponentName, int &gameId);

    /**
    * Získá zprávu o ukonèení hry
    * @param gameId Identifikátor ukonèované hry
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetEndGame(int &gameId);

    /**
    * Za¹le u¾ivatelév tah protihráèi
    * @param gameId Identifikátor hry
    * @param dataVector Vektor se zasílanými pohyby na ¹achovnici v rámci tahu
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendStep(int gameId, SockCom::DataVector &dataVector);

    /**
    * Získá tah od protihráèe
    * @param gameId Identifikátor hry
    * @param steps Vektor s pøijimanými pohyby na ¹achovnici v rámci tahu
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetStep(int &gameId, SockCom::DataVector &steps);

    /**
    * Za¹le protihráèi zprávu o jeho vítìzství
    * @param gameId Identifikátor rozehrané hry
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendWinInfo(int gameId);

    /**
    * Získá zprávu od protihráèe urèující, ¾e je vítìzem hry
    * @param gameId Identifikátor rozehrané hry
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetWinInfo(int &gameId);

    /**
    * Získá seznam volných hráèé
    * @param freePlayers Vektor jmen volných hráèé
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetFreePlayers(SockCom::StringVector &freePlayers);

    /**
    * Získá jméno novì pøipojeného hráèe
    * @param insertPlayer Jméno pøipojovaného hráèe
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetInsertName(wxString &insertPlayer);

    /**
    * Získá jméno právì odpojeného hráèe a odstraní jej ze seznamu
    * @param deletePlayer Jméno odpojovaného hráèe
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetDeleteName(wxString &deletePlayer);

    /**
    * Za¹le ke vzdálenému serveru po¾adavek na seznam rozehraných her s vybraným protihráèem
    * @param opponentName Jméno protihráèe
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendAskGameList(wxString &opponentName);

    /**
    * Získá seznam rozehraných her s vybraným protihráèem
    * @param opponentGames Vektor her rozehranýmch s vybraným protihráèem
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetGameList(SockCom::GameVector &opponentGames);

    /**
    * Za¹le ke vzdálenému serveru po¾adavek na seznam tahù k rozehrané høe
    * @param gameId Identifikátor rozehrané hry
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendAskGameSteps(int gameId);

    /**
    * Získá seznam tahù k rozehrané høe
    * @param gameSteps Vektor jednotlivých tahù rozehrané hry
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetGameSteps(SockCom::StepVector &gameSteps);

    /**
    * Za¹le ke vzdálenému serveru registrované jméno
    * @param registeredName Registrované jméno
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool RegisterToServer(wxString &registeredName);

    /**
    * Nastaví viditelnost hráèe
    * @param visibility Po¾adovaná viditelnost
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendVisibility(bool visibility);
};

#endif
