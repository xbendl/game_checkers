/**
* \file xmlconfig.h
* Poskytuje komunikaèní rozhraní pro práci s konfiguraèním XML souborem
* \author Vít Kopøiva (xkopri05)
*/


#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include "sockcom.h"
#include "tinyxml.h"

using namespace std;

/// Zaji¹»uje modifikaci konfiguraèního nastavení klienta v XML souboru
class XmlConfig
{
private:
    TiXmlDocument *configDoc;

public:
    /**
    * Konstruktor
    * @param configFile Cesta k souboru s konfigurací
    */
    XmlConfig(wxString configFile);

    /**
    * Destruktor
    */
    ~XmlConfig();

    /**
    * Nastaví ¾ádaný parametr zadané znaèky v konfiguraèním souboru
    * @param tagName Jméno hledaného tagu
     * @param paramName Jméno hledaného parametru
    * @param value Nová hodnota parametru
    * @return True, pokud se nastavení parametru podaøilo, jinak false
    */
    bool SetParam(const std::string tagName, const std::string paramName, wxString &value);

    /**
    * Získá ¾ádaný parametr zadané znaèky v konfiguraèním souboru
    * @param tagName Jméno hledaného tagu
    * @param paramName Jméno hledaného parametru
    * @param value Naplnìná hodnota nalezeného parametru
    * @return True, pokud se získání parametru podaøilo, jinak false
    */
    bool GetParam(const std::string tagName, const std::string paramName, wxString &value);

    /**
    * Vytvoøí nový XML
    * @param doc XML dokument, do kterého se nový soubor naète
    * @param startPath Cesta k adresáøi, do kterého chceme bude nový soubor umístìn
    * @return True, pokud se vytvoøení dokumentu podaøilo, jinak false
    */
    bool CreateFile(TiXmlDocument *doc, wxString startPath);
};

#endif
