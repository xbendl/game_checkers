/**
* \file xmlconfig.h
* Poskytuje komunika�n� rozhran� pro pr�ci s konfigura�n�m XML souborem
* \author V�t Kop�iva (xkopri05)
*/


#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include "sockcom.h"
#include "tinyxml.h"

using namespace std;

/// Zaji��uje modifikaci konfigura�n�ho nastaven� klienta v XML souboru
class XmlConfig
{
private:
    TiXmlDocument *configDoc;

public:
    /**
    * Konstruktor
    * @param configFile Cesta k souboru s konfigurac�
    */
    XmlConfig(wxString configFile);

    /**
    * Destruktor
    */
    ~XmlConfig();

    /**
    * Nastav� ��dan� parametr zadan� zna�ky v konfigura�n�m souboru
    * @param tagName Jm�no hledan�ho tagu
     * @param paramName Jm�no hledan�ho parametru
    * @param value Nov� hodnota parametru
    * @return True, pokud se nastaven� parametru poda�ilo, jinak false
    */
    bool SetParam(const std::string tagName, const std::string paramName, wxString &value);

    /**
    * Z�sk� ��dan� parametr zadan� zna�ky v konfigura�n�m souboru
    * @param tagName Jm�no hledan�ho tagu
    * @param paramName Jm�no hledan�ho parametru
    * @param value Napln�n� hodnota nalezen�ho parametru
    * @return True, pokud se z�sk�n� parametru poda�ilo, jinak false
    */
    bool GetParam(const std::string tagName, const std::string paramName, wxString &value);

    /**
    * Vytvo�� nov� XML
    * @param doc XML dokument, do kter�ho se nov� soubor na�te
    * @param startPath Cesta k adres��i, do kter�ho chceme bude nov� soubor um�st�n
    * @return True, pokud se vytvo�en� dokumentu poda�ilo, jinak false
    */
    bool CreateFile(TiXmlDocument *doc, wxString startPath);
};

#endif
