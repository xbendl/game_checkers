// clientcom.cc
// Modul zajistujiciho komunikaci klienta se serverem
// Autor: Jaroslav Bendl (xbendl00)

#include "clientcom.h"

bool ClientCom::ConnectToServer(wxIPV4address addr)
{
    mSock->Connect(addr, false);
    mSock->WaitOnConnect(10);

    if(mSock->IsConnected())
    {   // Test uspesnosti pripojeni
        return true;
    }
    else
    {
        mSock->Close();
        return false;
    }
}

bool ClientCom::DisconnectFromServer()
{
    if(mSock)
        mSock->Close();

    return true;
}

bool ClientCom::SignToServer(wxString &nameWx)
{
    std::string name = std::string(nameWx.mb_str());
    unsigned char jumper = SockCom::CLI_SIGN;
    int len = sizeof(jumper) + SockCom::PLAYER_LEN * sizeof(char);
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));             // 1B  = Typ zpravy: CLI_SIGN
    pos += sizeof(jumper);
    memcpy(&buffer[pos], name.c_str(), name.size() * sizeof(char));    // 8B  = Jmeno hrace

    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::SendAskPlayGame(wxString &challengerWx, wxString &opponentWx, int gameId, time_t date, bool onStep)
{
    std::string opponent = std::string(opponentWx.mb_str());

    unsigned char jumper = SockCom::CLI_ASK_GAME;
    unsigned char onStepChar = static_cast<unsigned char>(onStep);
    int len = sizeof(jumper) + SockCom::PLAYER_LEN * sizeof(char) + sizeof(gameId) + sizeof(date) + sizeof(onStepChar);
    char buffer[len];
    bzero(buffer, len);
    int i = 0;
    memcpy(&buffer[i], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: CLI_ASK_GAME
    i += sizeof(jumper);
    memcpy(&buffer[i], opponent.c_str(), opponent.size() * sizeof(char));  // 8B = Jmeno protihrace
    i += SockCom::PLAYER_LEN * sizeof(char);
    memcpy(&buffer[i], &gameId, sizeof(gameId));  // 4B  = ID rozehrane hry
    i += sizeof(gameId);
    memcpy(&buffer[i], &date, sizeof(date));      // 4B  = Datum rozehrani hry
    i += sizeof(date);
    memcpy(&buffer[i], &onStepChar, sizeof(onStepChar)); // 1B  = Urcuje hrace na tahu

    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetAskPlayGame(wxString &challengerName, int &gameId, time_t &date)
{
    // Nacteni jmena vyzyvatele ke hre
    int len = SockCom::PLAYER_LEN * sizeof(char);
    char buffer[SockCom::PLAYER_LEN + 1];
    bzero(buffer, (SockCom::PLAYER_LEN + 1) * sizeof(char));
    mSock->Read(buffer, SockCom::PLAYER_LEN * sizeof(char));
    if(mSock->Error())
        return false;

    challengerName = wxString(wxString::FromAscii(buffer));

    // Nacteni identifikatoru hry a data rozehrani hry
    len = sizeof(gameId) + sizeof(date);
    char buffer2[len];
    mSock->Read(buffer2, len);
    if(mSock->Error())
        return false;

    int pos = 0;
    memcpy(&gameId, &buffer2[pos], sizeof(gameId));
    pos += sizeof(int);
    memcpy(&date, &buffer2[pos], sizeof(date));

    return true;
}

bool ClientCom::GetAskContGame(wxString &challengerName, int &gameId, time_t &date, SockCom::StepVector &gameSteps, bool &onStep)
{
    // Nacteni delky datove casti zpravy
    int dataLen;
    mSock->Read(&dataLen, sizeof(dataLen));
    if(mSock->Error())
        return false;
    // Ziskani jmena vyzyvatele
    int len = SockCom::PLAYER_LEN * sizeof(char);
    char nameBuffer[SockCom::PLAYER_LEN + 1];
    bzero(nameBuffer, (SockCom::PLAYER_LEN + 1) * sizeof(char));
    mSock->Read(nameBuffer, SockCom::PLAYER_LEN * sizeof(char));
    if(mSock->Error())
        return false;

    challengerName = wxString(wxString::FromAscii(nameBuffer));
    // Prijmuti dalsich datovych slozek zpravy
    unsigned char onStepChar;
    char buffer[len];
    len = sizeof(gameId) + sizeof(date) + sizeof(onStepChar);
    mSock->Read(buffer, len);
    if(mSock->Error())
        return false;
    int pos = 0;
    memcpy(&gameId, &buffer[pos], sizeof(gameId));
    pos += sizeof(gameId);
    memcpy(&date, &buffer[pos], sizeof(date));
    pos += sizeof(date);
    memcpy(&onStepChar, &buffer[pos], sizeof(onStepChar));
    pos += sizeof(onStepChar);
    onStep = static_cast<bool>(onStepChar);
    // Prijmuti promenne casti zpravy
    char dataBuffer[dataLen];
    mSock->Read(dataBuffer, dataLen);
    if(mSock->Error())
        return false;
    SockCom::DataVector dataVector;
    SockCom::MovData movData;
    unsigned char stepLen;
    pos = 0;
    int i = 0;

    if(dataLen > 0)
    {
        while(pos < dataLen)
        {   // Prijimani jednotlivych tahu
            memcpy(&stepLen, &dataBuffer[pos], sizeof(stepLen));    // Pocet pohybu v ramci aktualniho tahu
            pos += sizeof(stepLen);
            dataVector.clear();
            for(int j = 0; j < stepLen; j++)
            {   // Ulozeni pohybu v ramci jednoho tahu do pomocneho vektoru
                memcpy(&movData, &dataBuffer[pos], sizeof(movData));
                pos += sizeof(movData);
                dataVector.push_back(movData);
            }

            i++;
            gameSteps.push_back(dataVector);    // Ulozeni aktualniho tahu do vektoru tahu hry
        }
    }
    return true;
}

bool ClientCom::SendAnswerPlayGame(bool answer, wxString &challengerNameWx, int gameId)
{
    std::string challengerName = std::string(challengerNameWx.mb_str());

    // Zaslani odpovedi na zadost o hru s vybranym protihracem
    unsigned char jumper;
    if(answer)
        jumper = SockCom::CLI_ACCEPT_GAME;
    else
        jumper = SockCom::CLI_REJECT_GAME;

    int len = SockCom::PLAYER_LEN * sizeof(char) + sizeof(int) + 1;
    char buffer[len];
    bzero(buffer, len);
    int i = 0;
    memcpy(&buffer[i], &jumper, sizeof(jumper));    // 1B  = Typ zpravy: CLI_ACCEPT_GAME / CLI_REJECT_GAME
    i += sizeof(jumper);
    memcpy(&buffer[i], challengerName.c_str(), challengerName.size() * sizeof(char));  // 8B = Jmeno protihrace
    i += SockCom::PLAYER_LEN * sizeof(char);
    memcpy(&buffer[i], &gameId, sizeof(int));       // 4B  = ID rozehrane hry

    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetAnswerPlayGame(int &gameId)
{
    // Ziskani ID hry, ke ktere byl ziskan souhlas
    mSock->Read(&gameId, sizeof(gameId));
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetGameId(int &gameId)
{
    // Ziskani ID nove hry
    mSock->Read(&gameId, sizeof(gameId));
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::SendEndGame(wxString &opponentNameWx, int &gameId)
{
    std::string opponentName = std::string(opponentNameWx.mb_str());

    // Vytvoreni bufferu
    unsigned char jumper = SockCom::CLI_END_GAME;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);

    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: CLI_END_GAME
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));  // 4B  = ID rozehrane hry

    mSock->Write(buffer, len);
    if(mSock->Error())
    {   // Test uspesnosti zaslani odpovedi na zadost o hru
        return false;
    }

    return true;
}

bool ClientCom::GetEndGame(int &gameId)
{
    // Ziskani ID ukoncovane hry
    mSock->Read(&gameId, sizeof(gameId));
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::SendStep(int gameId, SockCom::DataVector &dataVector)
{
    // Vytvoreni bufferu
    unsigned char jumper = SockCom::CLI_SEND_STEP;
    unsigned char numberOfMov = static_cast<unsigned char>(dataVector.size());
    int len = sizeof(jumper) + sizeof(numberOfMov) + sizeof(gameId) + dataVector.size() * sizeof(SockCom::MovData);
    char buffer[len];
    bzero(buffer, len);

    // Naplneni bufferu
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));            // 1B  = Typ zpravy: CLI_SEND_STEP
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &numberOfMov, sizeof(numberOfMov));  // 1B  = Pocet pohybu v ramci tahu
    pos += sizeof(numberOfMov);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));            // 4B  = ID hry
    pos += sizeof(gameId);
    for(int i = 0; i < numberOfMov; i++)
    {
        memcpy(&buffer[pos], &(dataVector[i]), sizeof(SockCom::MovData)); // 1B  = Pohyb v ramci tahu
        pos += sizeof(SockCom::MovData);
    }

    // Zaslani tahu protihraci
    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetStep(int &gameId, SockCom::DataVector &dataVector)
{
    // Ziskani poctu pohybu v ramci tahu
    unsigned char numberOfMov;
    mSock->Read(&numberOfMov, sizeof(numberOfMov));

    // Ziskani ID rozehrane hry a jednotlive tahy
    SockCom::MovData movData;
    int len = numberOfMov * sizeof(movData) + sizeof(int);
    char buffer[len];
    mSock->Read(buffer, len);
    int pos = 0;
    memcpy(&gameId, &buffer[pos], sizeof(gameId));
    pos += sizeof(gameId);

    for(int i = 0; i < numberOfMov; i++)
    {   // Postupne nacitani jednotlivych pohybu v ramci tahu
        memcpy(&movData, &buffer[pos], sizeof(movData));
        dataVector.push_back(movData);
        pos += sizeof(movData);
    }

    return true;
}

bool ClientCom::SendWinInfo(int gameId)
{
    // Vytvoreni bufferu
    unsigned char jumper = SockCom::CLI_SEND_WIN;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);

    // Naplneni bufferu
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));           // 1B = Typ zpravy: CLI_SEND_WIN
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));           // 4B = ID hry
    pos += sizeof(gameId);

    // Zaslani tahu protihraci
    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetWinInfo(int &gameId)
{
    mSock->Read(&gameId, sizeof(gameId));
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetFreePlayers(SockCom::StringVector &freePlayers)
{
    // Prijmuti poctu jmen v odpovedi
    int numberOfNames;
    mSock->Read(&numberOfNames, sizeof(numberOfNames));
    if(mSock->Error())
        return false;

    if(numberOfNames > 0)
    {   // Prijmuti samotnych jmen
        char buffer[(SockCom::PLAYER_LEN * numberOfNames) + 1];
        mSock->Read(buffer, (SockCom::PLAYER_LEN * numberOfNames) * sizeof(char));
        if(mSock->Error())
            return false;

        // Zapsani jmen hracu do vectoru
        std::string playerName;
        for(int i = 0; i < numberOfNames; i++)
        {
            playerName = std::string(&(buffer[SockCom::PLAYER_LEN * sizeof(char) * i]), SockCom::PLAYER_LEN * sizeof(char));

            freePlayers.push_back(playerName);
        }
    }

    return true;
}

bool ClientCom::GetInsertName(wxString &insertPlayer)
{
    unsigned int len = SockCom::PLAYER_LEN * sizeof(char);
    char playerName[SockCom::PLAYER_LEN + 1];
    bzero(playerName, ((SockCom::PLAYER_LEN + 1) * sizeof(char)));
    mSock->Read(playerName, len);
    if(mSock->Error())
        return false;

    insertPlayer = wxString(wxString::FromAscii(playerName));

    return true;
}

bool ClientCom::GetDeleteName(wxString &deletePlayer)
{
    unsigned int len  = (SockCom::PLAYER_LEN) * sizeof(char);
    char playerName[SockCom::PLAYER_LEN + 1];
    bzero(playerName, (SockCom::PLAYER_LEN + 1) * sizeof(char));

    mSock->Read(playerName, len);
    if(mSock->Error())
        return false;

    deletePlayer = wxString(wxString::FromAscii(playerName));

    return true;
}

bool ClientCom::SendAskGameList(wxString &opponentNameWx)
{
    // Zaslani prihlasovaciho jmena
    std::string opponentName = std::string(opponentNameWx.mb_str());

    unsigned char jumper = SockCom::CLI_GAME_LIST;
    int len = sizeof(jumper) + SockCom::PLAYER_LEN * sizeof(char);
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));                           // 1B  = Typ zpravy: CLI_GAME_LIST
    pos += sizeof(jumper);
    memcpy(&buffer[pos], opponentName.c_str(), opponentName.size() * sizeof(char));  // 8B = Jmeno protihrace

    // Zaslani zadosti na server
    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetGameList(SockCom::GameVector &opponentGames)
{
    int numberOfGames;

    // Prijmuti poctu rozehranych her v odpovedi
    mSock->Read(&numberOfGames, sizeof(numberOfGames));
    if(mSock->Error())
        return false;

    if(numberOfGames > 0)
    {   // Prijmuti samotnych struktur s rozehranymi hrami
        unsigned int len = numberOfGames * sizeof(SockCom::GameData);
        char buffer[len +1];
        bzero(buffer, len);
        mSock->Read(buffer, len);
        if(mSock->Error())
            return false;

        // Ulozeni struktur rozehranych her do prislusneho vektoru v klientovi
        SockCom::GameData gameData;
        int pos = 0;
        for(int i = 0; i < numberOfGames; i++)
        {   // Zapsani jmen hracu do vectoru
            memcpy(&gameData, &buffer[pos], sizeof(SockCom::GameData));

            opponentGames.push_back(gameData);
            pos += sizeof(SockCom::GameData);
        }
    }

    return true;
}

bool ClientCom::SendAskGameSteps(int gameId)
{
    unsigned char jumper = SockCom::CLI_GAME_STEPS;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: CLI_GAME_STEPS
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));  // 4B = Identifikator hry
    mSock->Write(buffer, len);

    // Zaslani zadosti na server
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::GetGameSteps(SockCom::StepVector &gameSteps)
{
    // Prijmuti delky datove casti zpravy
    int dataLen;
    mSock->Read(&dataLen, sizeof(dataLen));
    if(mSock->Error())
    {
        return false;
    }

    // Prijmuti zbytku zpravy
    if(dataLen > 0)
    {   // V pripade, ze hra obsahovala alespon jeden tah
        char buffer[dataLen];
        mSock->Read(buffer, dataLen);
        if(mSock->Error())
        {
            return false;
        }

        SockCom::DataVector dataVector;
        SockCom::MovData movData;
        unsigned char stepLen;
        int pos = 0;
        int i = 0;

        if(dataLen > 0)
        {
            while(pos < dataLen)
            {   // Prijimani jednotlivych tahu
                memcpy(&stepLen, &buffer[pos], sizeof(stepLen));    // Pocet pohybu v ramci aktualniho tahu
                pos += sizeof(stepLen);
                dataVector.erase(dataVector.begin(), dataVector.end());

                for(int j = 0; j < stepLen; j++)
                {   // Ulozeni pohybu v ramci jednoho tahu do pomocneho vektoru
                    memcpy(&movData, &buffer[pos], sizeof(movData));
                    pos += sizeof(movData);
                    dataVector.push_back(movData);
                }

                i++;
                gameSteps.push_back(dataVector);    // Ulozeni aktualniho tahu do vektoru tahu hry
            }
        }
    }
    return true;
}

bool ClientCom::RegisterToServer(wxString &registeredNameWx)
{
    // Zaslani prihlasovaciho jmena
    std::string registeredName = std::string(registeredNameWx.mb_str());
    unsigned char jumper = SockCom::CLI_REGISTER;
    int len = SockCom::PLAYER_LEN * sizeof(char) + sizeof(jumper);
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));    // 1B  = Typ zpravy: CLI_REGISTER
    pos += sizeof(jumper);
    memcpy(&buffer[pos], registeredName.c_str(), registeredName.size() * sizeof(char));  // 32B  = Registrovane jmeno
    mSock->Write(buffer, len);
    if(mSock->Error())
        return false;

    return true;
}

bool ClientCom::SendVisibility(bool visibility)
{
    unsigned char jumper;
    if(visibility)
        jumper = SockCom::CLI_VISIBLE;
    else
        jumper = SockCom::CLI_INVISIBLE;

    mSock->Write(&jumper, sizeof(jumper));
    if(mSock->Error())
        return false;

    return true;
}
