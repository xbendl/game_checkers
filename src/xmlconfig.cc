// clientconfig.h
// Poskytuje komunikacni rozhrani pro praci s konfiguracnim XML souborem
// Autor: Vit Kopriva (xkopri05)

#include "xmlconfig.h"

XmlConfig::XmlConfig(wxString configFileName)
{
    wxFileName configFile(configFileName);

    wxString executableFullPath = (wxStandardPaths::Get()).GetExecutablePath();
    wxString executablePath;
    wxFileName::SplitPath(executableFullPath, NULL, &executablePath, NULL, NULL);
    std::string configPath = std::string(executablePath.mb_str()) + "/xml/" + std::string(configFileName.mb_str());

    configDoc = new TiXmlDocument(configPath);    // Otevreni XML s konfiguraci
    if(!configDoc->LoadFile())
    {
        if(!CreateFile(configDoc, executablePath))
        {   // Soubor nelze vytvorit
            std::cerr << "Soubor " << configFileName << " neexistuje nebo nelze otevrit ci vytvorit." << std::endl;
            return;
        }
    }
}

XmlConfig::~XmlConfig()
{
    if(configDoc)
    delete configDoc;
//         free(configDoc);
}

bool XmlConfig::SetParam(std::string tagName, std::string paramName, wxString &value)
{
    std::string valueAscii = std::string(value.mb_str());
    TiXmlNode *root = configDoc->FirstChild("CONFIG");
    TiXmlNode *configNode = root->FirstChild(tagName);
    if(configNode)
        configNode->ToElement()->SetAttribute(paramName, valueAscii);
    else
        return false;

    if(!configDoc->SaveFile())
        return false;
}

bool XmlConfig::GetParam(std::string tagName, std::string paramName, wxString &value)
{
    std::string valueAscii;
    TiXmlNode *root = configDoc->FirstChild("CONFIG");
    TiXmlNode *configNode = root->FirstChild(tagName);
    if(configNode)
        valueAscii = std::string(*(configNode->ToElement()->Attribute(paramName)));
    else
        return false;

    value = wxString(wxString::FromAscii(valueAscii.c_str()));

    return true;
}

bool XmlConfig::CreateFile(TiXmlDocument *doc, wxString startPathWx)
{
    std::string startPath = std::string(startPathWx.mb_str());
    TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement *rootElement, *serverElement, *logElement, *replayElement, *helpElement;
    std::string fileName = startPath + "/xml/config_client.xml";
    rootElement = new TiXmlElement("CONFIG");
    rootElement->SetAttribute("last", "0");
    serverElement = new TiXmlElement("SERVER");
    serverElement->SetAttribute("host", "localhost");
    serverElement->SetAttribute("port", "3000");
    rootElement->LinkEndChild(serverElement);
    logElement = new TiXmlElement("LOG");
    logElement->SetAttribute("enable", "1");
    rootElement->LinkEndChild(logElement);
    replayElement = new TiXmlElement("REPLAY");
    replayElement->SetAttribute("delay", "1");
    rootElement->LinkEndChild(replayElement);
    helpElement = new TiXmlElement("HELP");
    helpElement->SetAttribute("file", "doc/help/index.html");
    rootElement->LinkEndChild(helpElement);
    doc->LinkEndChild(declaration);
    doc->LinkEndChild(rootElement);

    if(!doc->SaveFile(fileName)) // Ulozeni vytvorene struktury
        return false;

    return true;
}
