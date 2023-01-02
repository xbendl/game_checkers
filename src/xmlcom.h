/**
* \file xmlcom.h
* Poskytuje komunikaèní rozhraní pro práci s XML na stranì serveru
* \author Vít Kopøiva (xkopri05)
*/


#ifndef XMLCOM_H
#define XMLCOM_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include "sockcom.h"
#include "tinyxml.h"

using namespace std;

/// Zaji¹»uje modifikaci záznamé her v XML souborech
class XmlCom
{
private:
    TiXmlDocument *gamesDoc;
    TiXmlDocument *playersDoc;

    /// Pøíznaky urèující typ obsahu XML souboru
    enum XML_TYPE_FLAGS
    {
        XML_GAMES_SERVER,    ///< XML obsahuje seznam her (na stranì serveru)
        XML_GAMES_CLIENT,    ///< XML obsahuje seznam her (na stranì klienta)
        XML_PLAYERS_SERVER,  ///< XML obsahuje seznam registrovaných hráèé (na stranì serveru)
        XML_CONFIG_CLIENT    ///< XML obsahuje konfiguraèní nastavení aplikace (na stranì klienta)
    };

    /**
    * Aktulizuje hráèe na tahu a pøíznak dohrané hry
    * @param gameId Identifikátor urèující hru
    * @return True, pokud se aktualizace záznamu podaøila, jinak false
    */
    bool ChangeGame(int gameId);

    /**
    * Ovìøí zda hra existuje v XML souboru
    * @param gameId Identifikátor urèující hru
    * @return True, pokud se nalezení hry podaøilo, jinak false
    */
    bool IsExistsGame(int gameId);

public:
    /**
    * Konstruktor
    * @param gamesFile Cesta k souboru s ulo¾enými hrami
    */
    XmlCom(wxString gamesFile);

    /**
    * Konstruktor
    * @param gamesFile Cesta k souboru s ulo¾enymi hrami
    * @param playersFile Cesta k souboru se seznamem registrovaných hráèé
    */
    XmlCom(wxString gamesFile, wxString playersFile);

    /**
    * Destruktor
    */
    ~XmlCom();

    /**
    * Získá jména v¹ech hráèù z XML soubor
    * @param playerNames Vektor jmen hráèù
    * @return Poèet nalezených jmen, v pøípadì chyby -1
    */
    int FindNames(SockCom::StringVector &playerNames);

    /**
    * Získá seznam protihráèù z XML dokumentu na stranì klienta
    * @param challengerName Jméno hráèe, pro kterého chceme nalézt protihráèe
    * @param opponentNames Vektor jmen protihráèù
    * @return Poèet nalezených her, v pøípadì chyby -1
    */
    int FindOpponents(wxString &challengerName, SockCom::StringVector &opponentNames);

    /**
    * Vyhledá rozehrané hry s vybraným protihráèem
    * @param challengerName Jméno hledajícího hráèe
    * @param opponentName Jméno vybraného protihráèe
    * @param opponentGames Nalezené hry
    * @return Poèet nalezených her, v pøípadì chyby -1
    */
    int FindOpponentGames(std::string &challengerName, std::string &opponentName, SockCom::GameVector &opponentGames);

    /**
    * Vyhledá seznam tahù k rozehrané høe
    * @param gameId Identifikátor rozehrané hry
    * @param stepVector Vektor tahù hry
    * @return Poèet nalezených her, v pøípadì chyby -1
    */
    int FindGameSteps(int gameId, SockCom::StepVector &stepVector);

    /**
    * Vyhledá seznam tahù k rozehrané høe
    * @param gameId Identifikátor rozehrané hry
    * @param stepVector Vektor tahù hry
    * @param challengerName Jméno hrace zadajiciho o seznam tahù
    * @return True, pokud hráè ¾ádající o seznam tahù byl vyzyvatelem ke høe, jinak False
    */
    bool FindGameSteps(int gameId, SockCom::StepVector &stepVector, std::string challengerName);

    /**
    * Vytvoøí novou hru
    * @param challengerName Jméno hráèe, který vyzval protihráèe ke høe
    * @param opponentName Jméno hráèe, který byl ke høe vyzván
     * @return ID novì vytvoøené hry, v pøípadì chyby -1
    */
    int InsertGame(std::string &challengerName, std::string &opponentName);

    /**
     * Vytvoøí novou hru
    * @param challengerName Jméno hráèe, který vyzval protihráèe ke høe
     * @param opponentName Jméno hráèe, který byl ke høe vyzván
    * @param gameId ID, pod kterým bude hra ulo¾ena
    * @return ID novì vytvoøené hry, v pøípadì chyby -1
    */
    int InsertClientGame(wxString &challengerName, wxString &opponentName, int gameId);

    /**
    * Vlo¾í nový tah k dané høe
    * @param gameId Identifikátor urèující hru
    * @param dataVector Vektor s pohyby na ¹achovnici v rámci vkládaného tahu
    * @return True, pokud se vlo¾ení tahu podaøilo, jinak false
    */
    bool InsertStep(int gameId, SockCom::DataVector dataVector);

    /**
    * Sma¾e hru
    * @param gameId Identifikátor urèující hru
    * @return True, pokud se smazání hry podaøilo, jinak false
    */
    bool DeleteGame(int gameId);

    /**
    * Zjistí existenci daného jména u registrovaných hráèù
    * @param name Ovìøované jméno
    * @return True, pokud se nalezení hráèe podaøilo, jinak false
    */
    bool IsExistsName(std::string &name);

    /**
    * Vlo¾í jméno nového hráèe mezi registrované hráèe
    * @param playerName Jméno vkladaného hráèe
     * @return True, pokud se vlo¾ení hráèe podaøilo, jinak false
    */
    bool InsertPlayer(std::string &playerName);

    /**
    * Vytvoøí nový XML soubor zadaného typu
    * @param doc XML dokument, do kterého se nový soubor naète
    * @param fileType Typ XML dokumentu
    * @return True, pokud se vytvoøení dokumentu podaøilo, jinak false
    */
    bool CreateFile(TiXmlDocument *doc, XML_TYPE_FLAGS fileType);
};

#endif
