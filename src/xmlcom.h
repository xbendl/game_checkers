/**
* \file xmlcom.h
* Poskytuje komunika�n� rozhran� pro pr�ci s XML na stran� serveru
* \author V�t Kop�iva (xkopri05)
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

/// Zaji��uje modifikaci z�znam� her v XML souborech
class XmlCom
{
private:
    TiXmlDocument *gamesDoc;
    TiXmlDocument *playersDoc;

    /// P��znaky ur�uj�c� typ obsahu XML souboru
    enum XML_TYPE_FLAGS
    {
        XML_GAMES_SERVER,    ///< XML obsahuje seznam her (na stran� serveru)
        XML_GAMES_CLIENT,    ///< XML obsahuje seznam her (na stran� klienta)
        XML_PLAYERS_SERVER,  ///< XML obsahuje seznam registrovan�ch hr��� (na stran� serveru)
        XML_CONFIG_CLIENT    ///< XML obsahuje konfigura�n� nastaven� aplikace (na stran� klienta)
    };

    /**
    * Aktulizuje hr��e na tahu a p��znak dohran� hry
    * @param gameId Identifik�tor ur�uj�c� hru
    * @return True, pokud se aktualizace z�znamu poda�ila, jinak false
    */
    bool ChangeGame(int gameId);

    /**
    * Ov��� zda hra existuje v XML souboru
    * @param gameId Identifik�tor ur�uj�c� hru
    * @return True, pokud se nalezen� hry poda�ilo, jinak false
    */
    bool IsExistsGame(int gameId);

public:
    /**
    * Konstruktor
    * @param gamesFile Cesta k souboru s ulo�en�mi hrami
    */
    XmlCom(wxString gamesFile);

    /**
    * Konstruktor
    * @param gamesFile Cesta k souboru s ulo�enymi hrami
    * @param playersFile Cesta k souboru se seznamem registrovan�ch hr���
    */
    XmlCom(wxString gamesFile, wxString playersFile);

    /**
    * Destruktor
    */
    ~XmlCom();

    /**
    * Z�sk� jm�na v�ech hr��� z XML soubor
    * @param playerNames Vektor jmen hr���
    * @return Po�et nalezen�ch jmen, v p��pad� chyby -1
    */
    int FindNames(SockCom::StringVector &playerNames);

    /**
    * Z�sk� seznam protihr��� z XML dokumentu na stran� klienta
    * @param challengerName Jm�no hr��e, pro kter�ho chceme nal�zt protihr��e
    * @param opponentNames Vektor jmen protihr���
    * @return Po�et nalezen�ch her, v p��pad� chyby -1
    */
    int FindOpponents(wxString &challengerName, SockCom::StringVector &opponentNames);

    /**
    * Vyhled� rozehran� hry s vybran�m protihr��em
    * @param challengerName Jm�no hledaj�c�ho hr��e
    * @param opponentName Jm�no vybran�ho protihr��e
    * @param opponentGames Nalezen� hry
    * @return Po�et nalezen�ch her, v p��pad� chyby -1
    */
    int FindOpponentGames(std::string &challengerName, std::string &opponentName, SockCom::GameVector &opponentGames);

    /**
    * Vyhled� seznam tah� k rozehran� h�e
    * @param gameId Identifik�tor rozehran� hry
    * @param stepVector Vektor tah� hry
    * @return Po�et nalezen�ch her, v p��pad� chyby -1
    */
    int FindGameSteps(int gameId, SockCom::StepVector &stepVector);

    /**
    * Vyhled� seznam tah� k rozehran� h�e
    * @param gameId Identifik�tor rozehran� hry
    * @param stepVector Vektor tah� hry
    * @param challengerName Jm�no hrace zadajiciho o seznam tah�
    * @return True, pokud hr�� ��daj�c� o seznam tah� byl vyzyvatelem ke h�e, jinak False
    */
    bool FindGameSteps(int gameId, SockCom::StepVector &stepVector, std::string challengerName);

    /**
    * Vytvo�� novou hru
    * @param challengerName Jm�no hr��e, kter� vyzval protihr��e ke h�e
    * @param opponentName Jm�no hr��e, kter� byl ke h�e vyzv�n
     * @return ID nov� vytvo�en� hry, v p��pad� chyby -1
    */
    int InsertGame(std::string &challengerName, std::string &opponentName);

    /**
     * Vytvo�� novou hru
    * @param challengerName Jm�no hr��e, kter� vyzval protihr��e ke h�e
     * @param opponentName Jm�no hr��e, kter� byl ke h�e vyzv�n
    * @param gameId ID, pod kter�m bude hra ulo�ena
    * @return ID nov� vytvo�en� hry, v p��pad� chyby -1
    */
    int InsertClientGame(wxString &challengerName, wxString &opponentName, int gameId);

    /**
    * Vlo�� nov� tah k dan� h�e
    * @param gameId Identifik�tor ur�uj�c� hru
    * @param dataVector Vektor s pohyby na �achovnici v r�mci vkl�dan�ho tahu
    * @return True, pokud se vlo�en� tahu poda�ilo, jinak false
    */
    bool InsertStep(int gameId, SockCom::DataVector dataVector);

    /**
    * Sma�e hru
    * @param gameId Identifik�tor ur�uj�c� hru
    * @return True, pokud se smaz�n� hry poda�ilo, jinak false
    */
    bool DeleteGame(int gameId);

    /**
    * Zjist� existenci dan�ho jm�na u registrovan�ch hr���
    * @param name Ov��ovan� jm�no
    * @return True, pokud se nalezen� hr��e poda�ilo, jinak false
    */
    bool IsExistsName(std::string &name);

    /**
    * Vlo�� jm�no nov�ho hr��e mezi registrovan� hr��e
    * @param playerName Jm�no vkladan�ho hr��e
     * @return True, pokud se vlo�en� hr��e poda�ilo, jinak false
    */
    bool InsertPlayer(std::string &playerName);

    /**
    * Vytvo�� nov� XML soubor zadan�ho typu
    * @param doc XML dokument, do kter�ho se nov� soubor na�te
    * @param fileType Typ XML dokumentu
    * @return True, pokud se vytvo�en� dokumentu poda�ilo, jinak false
    */
    bool CreateFile(TiXmlDocument *doc, XML_TYPE_FLAGS fileType);
};

#endif
