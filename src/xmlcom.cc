// xmlcom.cc
// Poskytuje komunikacni rozhrani pro praci s XML na strane serveru
// Autor: Vit Kopriva (xkopri05)

#include "xmlcom.h"

XmlCom::XmlCom(wxString gamesFileName)
{
    wxFileName gamesFile(gamesFileName);
    std::string gamesPath, startPath;
    if(!gamesFile.IsAbsolute())
    {
      wxString executableFullPath = (wxStandardPaths::Get()).GetExecutablePath();
      wxString executablePath;
      wxFileName::SplitPath(executableFullPath, NULL, &executablePath, NULL, NULL);
      gamesPath = std::string(executablePath.mb_str()) + "/xml/" + std::string(gamesFileName.mb_str());
    }
    else
    {
      gamesPath = std::string(gamesFileName.mb_str());      
    }
    gamesDoc = new TiXmlDocument(gamesPath);      // Otevreni XML s ulozenymi hrami
    if(!gamesDoc->LoadFile())
    {   // Soubor nelze otevrit => Pravdepodobne neexistuje
        if(!CreateFile(gamesDoc, XML_GAMES_CLIENT))
        {   // Soubor nelze vytvorit
            std::cerr << "Soubor " << gamesPath << " neexistuje nebo nelze otevrit ci vytvorit." << std::endl;
            return;
        }
    }

    playersDoc = NULL;
}

XmlCom::XmlCom(wxString gamesFileName, wxString playersFileName)
{
    wxFileName gamesFile(gamesFileName);
    std::string gamesPath;
    if(!gamesFile.IsAbsolute())
    {
      wxString executableFullPath = (wxStandardPaths::Get()).GetExecutablePath();
      wxString executablePath;
      wxFileName::SplitPath(executableFullPath, NULL, &executablePath, NULL, NULL);
      gamesPath = std::string(executablePath.mb_str()) + "/xml/" + std::string(gamesFileName.mb_str());
    }
    else
    {
      gamesPath = std::string(gamesFileName.mb_str());
    }    
    gamesDoc = new TiXmlDocument(gamesPath);      // Otevreni XML s ulozenymi hrami
    if(!gamesDoc->LoadFile())
    {   // Soubor nelze otevrit => Pravdepodobne neexistuje
        if(!CreateFile(gamesDoc, XML_GAMES_SERVER))
        {   // Soubor nelze vytvorit
            std::cerr << "Soubor " << gamesPath << " neexistuje nebo nelze otevrit ci vytvorit." << std::endl;
            return;
        }
    }

    wxFileName playersFile(playersFileName);
    std::string playersPath;
    if(!playersFile.IsAbsolute())
    {
        wxString executableFullPath = (wxStandardPaths::Get()).GetExecutablePath();
        wxString executablePath;
        wxFileName::SplitPath(executableFullPath, NULL, &executablePath, NULL, NULL);
        playersPath = std::string(executablePath.mb_str()) + "/xml/" + std::string(playersFileName.mb_str());
    }
    else
    {
        playersPath = std::string(playersFileName.mb_str());
    }
    playersDoc = new TiXmlDocument(playersPath);      // Otevreni XML se jmeny registrovanych hracu
    if(!playersDoc->LoadFile())
    {
        if(!CreateFile(playersDoc, XML_PLAYERS_SERVER))
        {
            std::cerr << "Soubor " << playersPath << " neexistuje nebo nelze otevrit ci vytvorit." << std::endl;
            return;
        }
    }
}

XmlCom::~XmlCom()
{
    if(gamesDoc != NULL)
        delete gamesDoc;
//         free(gamesDoc);
    if(playersDoc != NULL)
        delete playersDoc;
//         free(playersDoc);
}

bool XmlCom::IsExistsGame(int gameId)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");

    std::string actString;
    int actGameId;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actString = gameNode->FirstChild("GAME_ID")->ToElement()->GetText();
        actGameId = atoi(actString.c_str());
        if(actGameId == gameId)
        {   // Prvek k odstraneni nalezen => Prerusime cyklus
            break;
        }
    }

    if(gameNode)        // Hra byla nalezena
        return true;

    return false;
}

int XmlCom::FindNames(SockCom::StringVector &playerNames)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");

    std::string player1, player2;
    int gameId, date;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        player1 = gameNode->FirstChild("PLAYER_1")->ToElement()->GetText();
        player2 = gameNode->FirstChild("PLAYER_2")->ToElement()->GetText();

        // Pridani jmena z tagu <PLAYER_1>
        int i;
        for(i = 0; i < playerNames.size(); i++)
            if(playerNames[i] == player1)
                break;
        if(i == playerNames.size())
            playerNames.push_back(player1);

        // Pridani jmena z tagu <PLAYER_2>
        for(i = 0; i < playerNames.size(); i++)
            if(playerNames[i] == player2)
                break;
        if(i == playerNames.size())
            playerNames.push_back(player2);
    }

    return playerNames.size();  // Vraci pocet nalenych protihracu
}

int XmlCom::FindOpponents(wxString &challengerNameWx, SockCom::StringVector &opponentNames)
{
    std::string challengerName = std::string(challengerNameWx.mb_str());
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");

    std::string player1, player2;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        player1 = gameNode->FirstChild("PLAYER_1")->ToElement()->GetText();
        player2 = gameNode->FirstChild("PLAYER_2")->ToElement()->GetText();

        // Overeni, zda hra patri ke zvolenemu hraci
        if(player1 == challengerName)
        {
            // Overeni, ze jmeno protihrace jiz neni ve vektoru jmen protihracu
            int i;
            for(i = 0; i < opponentNames.size(); i++)
                if(opponentNames[i] == player2)
                    break;

            if(i != opponentNames.size())
                continue;

            // Vlozeni jmena protihrace do vektoru jmen
            opponentNames.push_back(player2);
        }
        else if(player2 == challengerName)
        {
            // Overeni, ze jmeno protihrace jiz neni ve vektoru jmen protihracu
            int i;
            for(i = 0; i < opponentNames.size(); i++)
                if(opponentNames[i] == player1)
                    break;

            if(i != opponentNames.size())
                continue;

            // Vlozeni jmena protihrace do vektoru jmen
            opponentNames.push_back(player1);
        }
    }

    return opponentNames.size();  // Vraci pocet nalenych protihracu
}

int XmlCom::FindOpponentGames(std::string &challengerName, std::string &opponentName, SockCom::GameVector &opponentGames)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");

    SockCom::GameData gameData;        // Pomocna struktura pro ulozeni dat k rozehrane hre
    std::string player1, player2, onStepPlayer;
    int gameId, date;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        player1 = gameNode->FirstChild("PLAYER_1")->ToElement()->GetText();
        player2 = gameNode->FirstChild("PLAYER_2")->ToElement()->GetText();

        // Overeni hry
        if((player1 != challengerName) && (player1 != opponentName))
            continue;
        if((player2 != challengerName) && (player2 != opponentName))
            continue;

        // Naplneni struktury
        bzero(gameData.onStepPlayer, ((SockCom::PLAYER_LEN+1) * sizeof(char)));
        gameData.gameId = atoi(gameNode->FirstChild("GAME_ID")->ToElement()->GetText());
        gameData.date = atoi(gameNode->FirstChild("DATE")->ToElement()->GetText());
        onStepPlayer = gameNode->FirstChild("ON_STEP")->ToElement()->GetText();
        strcpy(gameData.onStepPlayer, onStepPlayer.c_str());

        opponentGames.push_back(gameData);   // Ulozeni rozehrane hry do vysledneho vektoru
    }

    return opponentGames.size();  // Vraci pocet nalenych her
}

int XmlCom::InsertGame(std::string &challengerName, std::string &opponentName)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");

    // Vytvoreni znacky <GAME>
    TiXmlElement *newGame = new TiXmlElement("GAME");
    std::stringstream out;

    // Zjisteni id pro novou znacku <GAME>
    TiXmlAttribute *lastIdAttribute = root->ToElement()->FirstAttribute();   // Ziskani odkazu na atribut urcujici lastId
    int lastId = lastIdAttribute->IntValue() + 1;  // Vypocet nove hodnoty lastId
    lastIdAttribute->SetIntValue(lastId);          // Nastaveni nove hodnoty lastId

    // Vytvoreni znacky <GAME_ID>
    TiXmlElement *newGameId = new TiXmlElement("GAME_ID");
    out << lastId;
    TiXmlText *newGameIdValue = new TiXmlText(out.str());
    newGameId->LinkEndChild(newGameIdValue);
    newGame->LinkEndChild(newGameId);

    // Vytvoreni znacky <PLAYER_1>
    TiXmlElement *newPlayer1 = new TiXmlElement("PLAYER_1");
    TiXmlText *newPlayer1Value = new TiXmlText(challengerName);
    newPlayer1->LinkEndChild(newPlayer1Value);
    newGame->LinkEndChild(newPlayer1);

    // Vytvoreni znacky <PLAYER_2>
    TiXmlElement *newPlayer2 = new TiXmlElement("PLAYER_2");
    TiXmlText *newPlayer2Value = new TiXmlText(opponentName);
    newPlayer2->LinkEndChild(newPlayer2Value);
    newGame->LinkEndChild(newPlayer2);

    // Vytvoreni znacky <DATE>
    TiXmlElement *newDate = new TiXmlElement("DATE");
    out.str("");
    out << time(NULL);
    TiXmlText *newDateValue = new TiXmlText(out.str());
    newDate->LinkEndChild(newDateValue);
    newGame->LinkEndChild(newDate);

    // Vytvoreni znacky <ON_STEP>
    TiXmlElement *onStepPlayer = new TiXmlElement("ON_STEP");
    TiXmlText *newOnStepPlayerValue = new TiXmlText(challengerName);
    onStepPlayer->LinkEndChild(newOnStepPlayerValue);
    newGame->LinkEndChild(onStepPlayer);

    // Vytvoreni znacky <FINISHED>
    TiXmlElement *newFin = new TiXmlElement("FINISHED");
    out.str("");
    out << 0;
    TiXmlText *newFinValue = new TiXmlText(out.str());
    newFin->LinkEndChild(newFinValue);
    newGame->LinkEndChild(newFin);

    // Vytvoreni znacky <GAMESTEPS>
    TiXmlElement *newGameSteps = new TiXmlElement("GAMESTEPS");
    newGameSteps->ToElement()->SetAttribute("last_step_id", 0);
    newGame->LinkEndChild(newGameSteps);

    root->LinkEndChild(newGame);

    if(!gamesDoc->SaveFile())
        return false;              // Ulozeni zmen

    return lastId;
}

int XmlCom::InsertClientGame(wxString &challengerNameWx, wxString &opponentNameWx, int gameId)
{
    std::string challengerName = std::string(challengerNameWx.mb_str());
    std::string opponentName = std::string(opponentNameWx.mb_str());

    if(IsExistsGame(gameId))
    {   // Existuje-li jiz hra s danym ID, vkladani neprobehne
        return false;
    }

    TiXmlNode *root = gamesDoc->FirstChild("GAMES");

    // Vytvoreni znacky <GAME>
    TiXmlElement *newGame = new TiXmlElement("GAME");
    std::stringstream out;

    // Vytvoreni znacky <GAME_ID>
    TiXmlElement *newGameId = new TiXmlElement("GAME_ID");
    out << gameId;
    TiXmlText *newGameIdValue = new TiXmlText(out.str());
    newGameId->LinkEndChild(newGameIdValue);
    newGame->LinkEndChild(newGameId);

    // Vytvoreni znacky <PLAYER_1>
    TiXmlElement *newPlayer1 = new TiXmlElement("PLAYER_1");
    TiXmlText *newPlayer1Value = new TiXmlText(challengerName);
    newPlayer1->LinkEndChild(newPlayer1Value);
    newGame->LinkEndChild(newPlayer1);

    // Vytvoreni znacky <PLAYER_2>
    TiXmlElement *newPlayer2 = new TiXmlElement("PLAYER_2");
    TiXmlText *newPlayer2Value = new TiXmlText(opponentName);
    newPlayer2->LinkEndChild(newPlayer2Value);
    newGame->LinkEndChild(newPlayer2);

    // Vytvoreni znacky <DATE>
    TiXmlElement *newDate = new TiXmlElement("DATE");
    out.str("");
    out << time(NULL);
    TiXmlText *newDateValue = new TiXmlText(out.str());
    newDate->LinkEndChild(newDateValue);
    newGame->LinkEndChild(newDate);

    // Vytvoreni znacky <ON_STEP>
    TiXmlElement *onStepPlayer = new TiXmlElement("ON_STEP");
    TiXmlText *newOnStepPlayerValue = new TiXmlText(challengerName);
    onStepPlayer->LinkEndChild(newOnStepPlayerValue);
    newGame->LinkEndChild(onStepPlayer);

    // Vytvoreni znacky <FINISHED>
    TiXmlElement *newFin = new TiXmlElement("FINISHED");
    out.str("");
    out << 0;
    TiXmlText *newFinValue = new TiXmlText(out.str());
    newFin->LinkEndChild(newFinValue);
    newGame->LinkEndChild(newFin);

    // Vytvoreni znacky <GAMESTEPS>
    TiXmlElement *newGameSteps = new TiXmlElement("GAMESTEPS");
    newGameSteps->ToElement()->SetAttribute("last_step_id", 0);
    newGame->LinkEndChild(newGameSteps);

    root->LinkEndChild(newGame);

    if(!gamesDoc->SaveFile())
        return false;           // Ulozeni zmen

    return gameId;
}

bool XmlCom::DeleteGame(int gameId)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");

    std::string actString;
    int actGameId;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actString = gameNode->FirstChild("GAME_ID")->ToElement()->GetText();
        actGameId = atoi(actString.c_str());
        if(actGameId == gameId)
        {   // Prvek k odstraneni nalezen => Prerusime cyklus
            break;
        }
    }

    if(gameNode)
    {   // Odstraneni zaznamu o hre
        if(!root->RemoveChild(gameNode))
            return false;
    }

    if(!gamesDoc->SaveFile())
        return false;           // Ulozeni zmen

    return true;
}

bool XmlCom::ChangeGame(int gameId)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");

    std::string actString;
    int actGameId;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actString = gameNode->FirstChild("GAME_ID")->ToElement()->GetText();
        actGameId = atoi(actString.c_str());
        if(actGameId == gameId)
        {   // Prvek k odstraneni nalezen => Prerusime cyklus
            break;
        }
    }

    if(gameNode == NULL)
        return false;

    std::string player1 = gameNode->FirstChild("PLAYER_1")->ToElement()->GetText();
    std::string player2 = gameNode->FirstChild("PLAYER_2")->ToElement()->GetText();
    std::string onStepPlayer = gameNode->FirstChild("ON_STEP")->ToElement()->GetText();

    // Aktualizace hrace na tahu
    if(player1 == onStepPlayer)
        gameNode->FirstChild("ON_STEP")->FirstChild()->SetValue(player2);
    else
        gameNode->FirstChild("ON_STEP")->FirstChild()->SetValue(player1);

    // Aktualizace casu posledniho tahu
    std::stringstream out;
    out << time(NULL);
    gameNode->FirstChild("DATE")->FirstChild()->SetValue(out.str());

    if(!gamesDoc->SaveFile())
        return false;           // Ulozeni zmen

    return true;
}

int XmlCom::FindGameSteps(int gameId, SockCom::StepVector &stepVector)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");
    std::string actString;
    int actGameId;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actString = gameNode->FirstChild("GAME_ID")->ToElement()->GetText();
        actGameId = atoi(actString.c_str());
        if(actGameId == gameId)
        {   // Uzel s danou hrou nalezen => Prerusime cyklus
            break;
        }
    }
    std::string challengerName;
    TiXmlNode *stepsNode, *dataNode;
    int movInt;
    SockCom::MovData movData;             // Struktura popisujici zmenu na sachovnici
    SockCom::DataVector dataVector;       // Vektor zmen popisujici tah hry
    std::string movString;
    if(gameNode)
    {   // Prohledani znacky <GAMESTEPS> dane znacky <GAME>
        stepsNode = gameNode->FirstChild("GAMESTEPS")->FirstChild("STEP");
        for(stepsNode; stepsNode; stepsNode = stepsNode->NextSibling())
        {   // Prochazeni pres znacky <STEP> daneho <GAME>
            dataVector.clear();
            dataNode = stepsNode->FirstChild("DATA");
            for(dataNode; dataNode; dataNode = dataNode->NextSibling())
            {   // Prochazeni pres znacky <DATA> daneho <STEP> urceneho <GAME>
                movString = string(dataNode->ToElement()->GetText());
                movInt = atoi(movString.c_str());
                movData = static_cast<SockCom::MovData>(movInt);
                dataVector.push_back(movData);
            }
            stepVector.push_back(dataVector);
        }
        challengerName = gameNode->FirstChild("PLAYER_1")->ToElement()->GetText();
    }

    return stepVector.size();
}

bool XmlCom::FindGameSteps(int gameId, SockCom::StepVector &stepVector, std::string name)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");
    std::string actString;
    int actGameId;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actString = gameNode->FirstChild("GAME_ID")->ToElement()->GetText();
        actGameId = atoi(actString.c_str());
        if(actGameId == gameId)
        {   // Uzel s danou hrou nalezen => Prerusime cyklus
            break;
        }
    }
    std::string challengerName;
    TiXmlNode *stepsNode, *dataNode;
    int movInt;
    SockCom::MovData movData;             // Struktura popisujici zmenu na sachovnici
    SockCom::DataVector dataVector;       // Vektor zmen popisujici tah hry
    std::string movString;
    if(gameNode)
    {   // Prohledani znacky <GAMESTEPS> dane znacky <GAME>
        stepsNode = gameNode->FirstChild("GAMESTEPS")->FirstChild("STEP");
        for(stepsNode; stepsNode; stepsNode = stepsNode->NextSibling())
        {   // Prochazeni pres znacky <STEP> daneho <GAME>
            dataVector.clear();
            dataNode = stepsNode->FirstChild("DATA");
            for(dataNode; dataNode; dataNode = dataNode->NextSibling())
            {   // Prochazeni pres znacky <DATA> daneho <STEP> urceneho <GAME>
                movString = string(dataNode->ToElement()->GetText());
                movInt = atoi(movString.c_str());
                movData = static_cast<SockCom::MovData>(movInt);
                dataVector.push_back(movData);
            }
            stepVector.push_back(dataVector);
        }
        challengerName = gameNode->FirstChild("PLAYER_1")->ToElement()->GetText();
    }

    if(challengerName == name)
        return true;

    return false;
}

bool XmlCom::InsertStep(int gameId, SockCom::DataVector dataVector)
{
    TiXmlNode *root = gamesDoc->FirstChild("GAMES");
    TiXmlNode *gameNode = root->FirstChild("GAME");
    int actGameId;
    int *lastStepId = new int;
    std::stringstream out;
    std::string actString;

    for(gameNode; gameNode; gameNode=gameNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actString = gameNode->FirstChild("GAME_ID")->ToElement()->GetText();
        actGameId = atoi(actString.c_str());
        if(actGameId == gameId)
        {   // Uzel s hrou nalezen => Prerusime cyklus
            break;
        }
    }

    if(gameNode == NULL)
    {
        return false;
    }
    TiXmlNode *stepsNode = gameNode->FirstChild("GAMESTEPS");

    // Zjisteni step id pro novou znacku <STEP>
    stepsNode->ToElement()->Attribute("last_step_id", lastStepId);   // Ziskani odkazu na atribut urcujici lastId
    stepsNode->ToElement()->SetAttribute("last_step_id", *lastStepId+1);          // Nastaveni nove hodnoty last_step_id

    // Vytvoreni znacky <STEP>
    TiXmlElement *newStep = new TiXmlElement("STEP");
    newStep->ToElement()->SetAttribute("step_id", *lastStepId);          // Nastaveni nove hodnoty last_step_id
    stepsNode->LinkEndChild(newStep);

    // Vytvoreni znacek <DATA> k danemumu <STEP>
    for(int i = 0; i < dataVector.size(); i++)
    {
        TiXmlElement *dataNode = new TiXmlElement("DATA");
        out.str("");
        out << static_cast<int>(dataVector[i]);
        TiXmlText *dataNodeValue = new TiXmlText(out.str());
        dataNode->LinkEndChild(dataNodeValue);
        newStep->LinkEndChild(dataNode);
    }

    if(!ChangeGame(gameId))
        return false;          // Aktualizace zaznamu ve hre

    if(!gamesDoc->SaveFile())
        return false;          // Ulozeni zmen

    return true;
}

bool XmlCom::InsertPlayer(std::string &playerName)
{
    TiXmlNode *root = playersDoc->FirstChild("PLAYERS");

    // Vytvoreni znacky <PLAYER>
    TiXmlElement *newPlayerNode = new TiXmlElement("PLAYER");
    newPlayerNode->SetAttribute("name", playerName);
    root->LinkEndChild(newPlayerNode);

    if(!playersDoc->SaveFile())     // Ulozeni zmen
        return false;

    return true;
}

bool XmlCom::IsExistsName(std::string &name)
{
    TiXmlNode *root = playersDoc->FirstChild("PLAYERS");
    TiXmlNode *playerNode = root->FirstChild("PLAYER");
    std::string actName;

    for(playerNode; playerNode; playerNode = playerNode->NextSibling())
    {   // Prochazeni pres znacky <GAME>
        actName = std::string(playerNode->ToElement()->Attribute("name"));
        if(actName == name)
        {   // Uzel s jmenem hledaneho hrace nalezen
            return true;
        }
    }

    return false;
}

bool XmlCom::CreateFile(TiXmlDocument *doc, XML_TYPE_FLAGS fileType)
{
    TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement* rootElement;
    std::string filePath;

    switch(fileType)
    {
        case XML_GAMES_SERVER:
            rootElement = new TiXmlElement("GAMES");
            rootElement->SetAttribute("last", "0");
            break;
        case XML_GAMES_CLIENT:
            rootElement = new TiXmlElement("GAMES");
            break;
        case XML_PLAYERS_SERVER:
            rootElement = new TiXmlElement("PLAYERS");
            break;
    }

    doc->LinkEndChild(declaration);
    doc->LinkEndChild(rootElement);

    if(!doc->SaveFile()) // Ulozeni vytvorene struktury
        return false;

    return true;
}


