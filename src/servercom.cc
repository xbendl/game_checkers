// servercom.cc
// Modul zajistujiciho komunikaci serveru s klientem
// Autor: Jaroslav Bendl (xbendl00)

#include "servercom.h"

bool ServerCom::SendGameId(int sock, int gameId)
{
    // Vytvoreni zpravy
    unsigned char jumper = SockCom::SERV_SEND_GAME_ID;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);
    int i = 0;
    memcpy(&buffer[i], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_SEND_GAME_ID
    i += sizeof(jumper);
    memcpy(&buffer[i], &gameId, sizeof(gameId));  // 4B  = ID hry

    // Zaslani zpravy
    if(send(sock, buffer, len, 0) <= 0)
        return false;
}

bool ServerCom::FindInPlayGames(PlayVector &playVector, PlayData &playData, int senderSock, int gameId)
{
    for(int i = 0; i < playVector.size(); i++)
    {   // Prochazeni aktualne hranymi hrami
        if(gameId == playVector[i].gameId)
        {   // Nalezeni hry podle identifikatoru
            playData.gameId = gameId;
            if(playVector[i].senderSock == senderSock)
            {   // Naplneni odkazem predane struktury detaily o hre, kontrola spravneho poradi odesilatel - prijemce
                playData.senderName = playVector[i].senderName;
                playData.senderSock = playVector[i].senderSock;
                playData.receiverName = playVector[i].receiverName;
                playData.receiverSock = playVector[i].receiverSock;
            }
            else
            {
                playData.senderName = playVector[i].receiverName;
                playData.senderSock = playVector[i].receiverSock;
                playData.receiverName = playVector[i].senderName;
                playData.receiverSock = playVector[i].senderSock;
            }
            return true;  // Po nalezeni detailu hry se metoda ukonci
        }
    }

    return false;
}

bool ServerCom::FindInPlayGames(PlayVector &playVector, PlayData &playData, std::string &playerName)
{
    for(int i = 0; i < playVector.size(); i++)
    {   // Prochazeni aktualne hranymi hrami
        if(playerName == playVector[i].senderName)
        {
            playData.gameId = playVector[i].gameId;
            playData.senderName = playVector[i].senderName;
            playData.senderSock = playVector[i].senderSock;
            playData.receiverName = playVector[i].receiverName;
            playData.receiverSock = playVector[i].receiverSock;
            return true;  // Po nalezeni detailu hry se metoda ukonci
        }
        else if(playerName == playVector[i].receiverName)
        {
            playData.gameId = playVector[i].gameId;
            playData.senderName = playVector[i].receiverName;
            playData.senderSock = playVector[i].receiverSock;
            playData.receiverName = playVector[i].senderName;
            playData.receiverSock = playVector[i].senderSock;
            return true;
        }
    }

    return false;
}

bool ServerCom::DeleteFromPlayVector(PlayVector &playVector, int gameId)
{
    for(int i = 0; i < playVector.size(); i++)
    {   // Prochazeni aktualne hranymi hrami
        if(gameId == playVector[i].gameId)
        {   // Nalezeni hry podle shody identifikatoru
            playVector.erase(playVector.begin() + i);   // Vymazani hry
            return true;
        }
    }

    return false;
}

bool ServerCom::SignToServer(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool &playerSigned)
{
    // Precteni jmena hrace
    unsigned int len  = (SockCom::PLAYER_LEN) * sizeof(char);
    char buffer[SockCom::PLAYER_LEN + 1];
    bzero(buffer, SockCom:: PLAYER_LEN * sizeof(char));

    if(recv(sock, buffer, len, 0) <= 0)
        return false;

    std::string playerName = std::string(buffer);
    itPlayerMap found=logPlayers.find(playerName);

    // Nastaveni odpovedi danemu hraci
    unsigned char jumper;
    if((found == logPlayers.end()) && (xmlCom->IsExistsName(playerName)))
    {   // Pokud hrac je v tabulce registrovanych hracu a zaroven neni v tabulce prihlasenych hracu
        logPlayers[playerName] = sock;           // Pridani jmena do seznamu prihlasenych hracu
        jumper = SockCom::SERV_SIGNED;
        playerSigned = true;        // Nastaveni odkazem predaneho parametru indikujiciho uspesne / neuspesne prihlaseni
    }
    else
    {
        jumper = SockCom::SERV_UNSIGNED;
        playerSigned = false;
    }

    // Zaslani odpovedi danemu hraci
    if(send(sock, &jumper, sizeof(jumper), 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SendAskPlayGame(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers)
{
    // Ziskani jmena vyzivatele ke hre (od nej pozadavek pochazi)
    std::string challengerName;
    for(PlayerMap::iterator it = freePlayers.begin(); it != freePlayers.end(); ++it)
    {   // Prochazeni seznamem volnych hracu
        if(it->second == sock)
        {
            challengerName = std::string(it->first);
            freePlayers.erase(it);  // Vyjmuti vyzyvatele ze seznamu volnych hracu

            // Zaslani zpravy o odpojeni vyzyvatele z listiny volnych hracu
            bool boolSendDelete = ServerCom::SendDeleteName(sock, freePlayers, logPlayers);
            if(!boolSendDelete)
                return false;

            break;
        }
    }

    // Ziskani jmena protihrace
    int gameId;
    time_t date;
    int len = SockCom::PLAYER_LEN * sizeof(char);
    unsigned char onStepChar;
    char buffer[SockCom::PLAYER_LEN + 1];
    bzero(buffer, (SockCom::PLAYER_LEN + 1) * sizeof(char));

    if(recv(sock, buffer, SockCom::PLAYER_LEN * sizeof(char), 0) <= 0)
        return false;
    std::string opponentName = std::string(buffer);

    // Ziskani ID rozehrane hry (nebo -1 pokud jde o novou hru), data rozehrani hry a hrace na tahu
    len = sizeof(gameId) + sizeof(date) + sizeof(onStepChar);
    char buffer2[len];

    if(recv(sock, buffer2, len, 0) <= 0)
        return false;
    int pos = 0;
    memcpy(&gameId, &buffer2[pos], sizeof(gameId));
    pos += sizeof(gameId);
    memcpy(&date, &buffer2[pos], sizeof(date));
    pos += sizeof(date);
    memcpy(&onStepChar, &buffer2[pos], sizeof(onStepChar));

    // Ziskani soketu protihrace (tomu pozadavek zasleme)
    int opponentSock;
    for(PlayerMap::iterator it = freePlayers.begin(); it != freePlayers.end(); ++it)
    { // Prochazeni seznamem volnych hracu
        if(it->first == opponentName)
        {
            opponentSock = it->second;
            freePlayers.erase(it);  // Vyjmuti protihrace ze seznamu volnych hracu

            // Zaslani zpravy o odpojeni vyzyvatele z listiny volnych hracu
            bool boolSendDelete = ServerCom::SendDeleteName(opponentSock, freePlayers, logPlayers);
            if(!boolSendDelete)
            {   // Test uspesnosti zaslani zpravy
                return false;
            }
            break;
        }
    }

    // Vytvoreni preposilane zpravy
    if(gameId == -1)
    {   // Jedna se o novou hru
        unsigned char jumper = SockCom::SERV_ASK_GAME;
        len = sizeof(jumper) + SockCom::PLAYER_LEN * sizeof(char) + sizeof(gameId) + sizeof(date);
        char buffer3[len];
        bzero(buffer3, len);
        pos = 0;
        memcpy(&buffer3[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_ASK_GAME
        pos += sizeof(jumper);
        memcpy(&buffer3[pos], challengerName.c_str(), challengerName.size() * sizeof(char));  // 8B = Jmeno protihrace
        pos += SockCom::PLAYER_LEN * sizeof(char);
        memcpy(&buffer3[pos], &gameId, sizeof(gameId));  // 4B  = ID rozehrane hry
        pos += sizeof(gameId);
        memcpy(&buffer3[pos], &date, sizeof(date));      // XB  = Datum rozehrani hry

        if(send(opponentSock, buffer3, len, 0) <= 0)
            return false;
    }
    else
    {   // Jedna se o rozehranou hru
        SockCom::StepVector stepVector;
        xmlCom->FindGameSteps(gameId, stepVector);

        // Ziskani delky datove casti zpravy
        int dataLen = 0;
        for(int i = 0; i < stepVector.size(); i++)
        {
            dataLen += stepVector[i].size() * sizeof(SockCom::MovData) + sizeof(SockCom::MovData);
        }

        // Vytvoreni pevne casti zpravy
        unsigned char jumper = SockCom::SERV_ASK_CONT_GAME;
        len = sizeof(jumper) + sizeof(dataLen) + SockCom::PLAYER_LEN * sizeof(char) + sizeof(gameId) + sizeof(date) + sizeof(onStepChar) + dataLen * sizeof(SockCom::MovData);
        char buffer3[len];
        bzero(buffer3, len);
        pos = 0;
        memcpy(&buffer3[pos], &jumper, sizeof(jumper));     // 1B = Typ zpravy: SERV_ASK_CONT_GAME
        pos += sizeof(jumper);
        memcpy(&buffer3[pos], &dataLen, sizeof(dataLen));   // 4B = Delka datove casti zpravy
        pos += sizeof(dataLen);
        memcpy(&buffer3[pos], challengerName.c_str(), challengerName.size() * sizeof(char));  // 8B = Jmeno protihrace
        pos += SockCom::PLAYER_LEN * sizeof(char);
        memcpy(&buffer3[pos], &gameId, sizeof(gameId));     // 4B = Identifikator rozehrane hry
        pos += sizeof(gameId);
        memcpy(&buffer3[pos], &date, sizeof(date));             // 4B = Datum rozehrani hry
        pos += sizeof(date);
        memcpy(&buffer3[pos], &onStepChar, sizeof(onStepChar)); // 1B = Urceni hrace na tahu
        pos += sizeof(onStepChar);

        // Vytvoreni datove casti zpravy promenne delky
        unsigned char stepLen = 0;
        for(int i = 0; i < stepVector.size(); i++)
        {
            stepLen = static_cast<unsigned char>(stepVector[i].size());
            memcpy(&buffer3[pos], &stepLen, sizeof(stepLen));  // 1B = Pocet zmen v ramci jednoho tahu
            pos += sizeof(stepLen);

            for(int j = 0; j < stepVector[i].size(); j++)
            {
                memcpy(&buffer3[pos], &(stepVector[i][j]), sizeof(SockCom::MovData));  // 1B = Zmena v ramci aktualniho tahu
                pos += sizeof(SockCom::MovData);
            }
        }

        if(send(opponentSock, buffer3, len, 0) <= 0)
            return false;
    }

    return true;
}

bool ServerCom::SendAnswerPlayGame(int sock, bool answer, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector)
{
    // Ziskani jmena protihrace (ten, kdo zaslal odpoved)
    std::string opponentName;
    PlayerMap::iterator it;
    for(it = logPlayers.begin(); it != logPlayers.end(); ++it)
    { // Nalezeni protihrace v seznamu volnych hracu
        if(it->second == sock)
        {
            opponentName = std::string(it->first);
            break;
        }
    }
    if(!answer)
    {   // Pokud protihrac zadost o hru zamitnul
        freePlayers[opponentName] = sock;          // Pridani jmena do seznamu volnych hracu
        if(!SendInsertName(sock, sock, logPlayers))
        {   // Test uspesnosti zaslani zpravy o vlozeni protihrace do seznamu volnych hracu
            return false;
        }
    }

    // Ziskani jmena vyzyvatele ke hre
    int gameId;
    int len = SockCom::PLAYER_LEN * sizeof(char);
    char buffer[SockCom::PLAYER_LEN + 1];
    bzero(buffer, (SockCom::PLAYER_LEN + 1) * sizeof(char));
    if(recv(sock, buffer, SockCom::PLAYER_LEN * sizeof(char), 0) <= 0)
        return false;

    std::string challengerName = std::string(buffer);

    // Ziskani ID rozehrane hry (nebo -1 pokud jde o novou hru)
    len = sizeof(int);
    char buffer2[len];

    if(recv(sock, buffer2, len, 0) <= 0)
        return false;

    memcpy(&gameId, &buffer2[0], sizeof(gameId));

    // Ziskani soketu vyzyvatele
    int challengerSock;
    for(it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {
        if(it->first == challengerName)
        {
            challengerSock = it->second;
            break;
        }
    }
    if(!answer)
    {   // Pokud protihrac zadost o hru zamitnul
        freePlayers[challengerName] = challengerSock;  // Pridani jmena do seznamu volnych hracu

        // Zaslani zpravy vsem ostatnim pripojenym hracum o "novem" volnem hraci
        if(!ServerCom::SendInsertName(challengerSock, challengerSock, logPlayers))
            return false;
    }


    // Vytvoreni zaznamu o nove hre v XML na serveru
    if(answer && gameId == -1)
    {   // Pokud protihrac souhlasil s hrou a jednalo-li se o novou hru
        gameId = xmlCom->InsertGame(challengerName, opponentName);  // Vytvoreni zaznamu v tabulce her v XML
        if(gameId == -1)
            return false;

        SendGameId(sock, gameId);  // Zaslani identifikatoru vytvoreneho zaznamu zpet protihraci
    }

    // Ulozeni do vektoru s aktualne hranymi hrami
    PlayData playData;
    playData.gameId = gameId;
    playData.senderName = challengerName;
    playData.senderSock = challengerSock;
    playData.receiverName = opponentName;
    playData.receiverSock = sock;
    playVector.push_back(playData);

    // Rozhodnuti o typu zpravy
    unsigned char jumper;
    if(answer)
    {   // Pokud protihrac zadost o hru odsouhlasil
        jumper = SockCom::SERV_ACCEPT_GAME;
    }
    else
    {   // Pokud protihrac zadost o hru zamitnul
        jumper = SockCom::SERV_REJECT_GAME;
    }

    // Vytvoreni preposilane zpravy
    len = sizeof(gameId) + sizeof(jumper);
    char sendBuffer[len];
    bzero(sendBuffer, len);
    int i = 0;
    memcpy(&sendBuffer[i], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_ACCEPT_GAME / SERV_REJECT_GAME
    i += sizeof(jumper);
    memcpy(&sendBuffer[i], &gameId, sizeof(gameId));  // 4B  = ID rozehrane hry

    // Preposlani zpravy
    if(send(challengerSock, sendBuffer, len, 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SendEndGame(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector)
{
    // Ziskani ID ukoncovane hry
    int gameId;
    if(recv(sock, &gameId, sizeof(gameId), 0) <= 0)
        return false;

    // Ziskani detailu k ukoncovane hre
    PlayData playData;
    if(!FindInPlayGames(playVector, playData, sock, gameId))
    {   // Nebylo mozne hru najit ve vektoru aktualne hranych her
        return false;
    }

    // Vytvoreni bufferu
    unsigned char jumper = SockCom::SERV_END_GAME;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);

    // Naplneni bufferu
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_END_GAME
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));  // 4B  = ID hry

    // Rozsireni seznamu volnych her o hrace, kteri hrali prave ukoncovanou hru
    freePlayers[playData.senderName] = playData.senderSock;
    freePlayers[playData.receiverName] = playData.receiverSock;

    if(!DeleteFromPlayVector(playVector, gameId))
    {   // Test uspesnosti odstraneni hry ze seznamu aktualne hranych her
        return false;
    }

    if(!ServerCom::SendInsertName(playData.senderSock, playData.senderSock, logPlayers))
    {   // Test uspesnosti zaslani zpravy o vlozeni noveho hrace do seznamu volnych hracu
        return false;
    }

    if(!ServerCom::SendInsertName(playData.receiverSock, playData.receiverSock, logPlayers))
    {   // Test uspesnosti zaslani zpravy o vlozeni noveho hrace do seznamu volnych hracu
        return false;
    }

    if(send(playData.receiverSock, buffer, len, 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SendEndGame(int sock, int gameId)
{
    // Vytvoreni bufferu
    unsigned char jumper = SockCom::SERV_END_GAME;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);

    // Naplneni bufferu
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_END_GAME
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));  // 4B  = ID hry

    if(send(sock, buffer, len, 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SendStep(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector)
{
    // Ziskani poctu pohybu v ramci tahu
    unsigned char numberOfMov;
    if(recv(sock, &numberOfMov, sizeof(numberOfMov), 0) <= 0)
        return false;

    // Ziskani ID rozehrane hry a jednotlive tahy
    int gameId;
    SockCom::MovData movData;
    SockCom::DataVector dataVector;
    int len = sizeof(gameId) + numberOfMov * sizeof(SockCom::MovData);
    char buffer[len];
    if(recv(sock, buffer, len, 0) <= 0)
        return false;
    int pos = 0;
    memcpy(&gameId, &buffer[pos], sizeof(gameId));
    pos += sizeof(gameId);
    for(int i = 0; i < numberOfMov; i++)
    {   // Postupne nacitani jednotlivych pohybu v ramci tahu
        memcpy(&movData, &buffer[pos], sizeof(movData));
        dataVector.push_back(movData);
        pos += sizeof(movData);
    }

    // Ziskani jmena odesilatele
    std::string senderName;
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem volnych hracu pro nalezeni jmena asociovaneho k tomuto soketu
        if(it->second == sock)
        {
            senderName = it->first;
            break;
        }
    }
    PlayData playData;
    if(!FindInPlayGames(playVector, playData, senderName))
    {   // Nebylo mozne hru najit ve vektoru aktualne hranych her
        return false;
    }
    gameId = playData.gameId;

    // Vytvoreni bufferu
    unsigned char jumper = SockCom::SERV_SEND_STEP;
    len = + sizeof(jumper) + sizeof(numberOfMov) + sizeof(gameId) + dataVector.size() * sizeof(SockCom::MovData);
    char sendBuffer[len];
    bzero(sendBuffer, len);

    // Naplneni bufferu
    pos = 0;
    memcpy(&sendBuffer[pos], &jumper, sizeof(jumper));           // 1B  = Typ zpravy: SERV_SEND_STEP
    pos += sizeof(jumper);
    memcpy(&sendBuffer[pos], &numberOfMov, sizeof(numberOfMov)); // 1B  = Pocet pohybu v ramci tahu
    pos += sizeof(numberOfMov);
    memcpy(&sendBuffer[pos], &gameId, sizeof(int));              // 4B  = ID hry
    pos += sizeof(gameId);
    for(int i = 0; i < numberOfMov; i++)
    {
        memcpy(&sendBuffer[pos], &(dataVector[i]), sizeof(SockCom::MovData)); // 1B  = Pohyb v ramci tahu
        pos += sizeof(SockCom::MovData);
    }

    if(send(playData.receiverSock, sendBuffer, len, 0) <= 0)
        return false;

    // Tah nevedl k ukonceni hry, proto bude vlozen do XML
    if(!xmlCom->InsertStep(gameId, dataVector))
        return false;

    return true;
}

bool ServerCom::SendWinInfo(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector)
{
    // Ziskani ID rozehrane hry
    int gameId;
    if(recv(sock, &gameId, sizeof(gameId), 0) <= 0)
        return false;

    // Ziskani jmena odesilatele
    std::string senderName;
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem volnych hracu pro nalezeni jmena asociovaneho k tomuto soketu
        if(it->second == sock)
        {
            senderName = it->first;
            break;
        }
    }

    PlayData playData;
    if(!FindInPlayGames(playVector, playData, senderName))
    {   // Nebylo mozne hru najit ve vektoru aktualne hranych her
        return false;
    }

    // Vytvoreni bufferu
    unsigned char jumper = SockCom::SERV_SEND_WIN;
    int len = sizeof(jumper) + sizeof(gameId);
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_SEND_WIN
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &gameId, sizeof(gameId));  // 4B  = ID hry

    // Zaslani zpravy o vlozeni noveho hrace (odesilatele "vitezneho tahu") do listiny volnych hracu
    if(!ServerCom::SendInsertName(playData.receiverSock, playData.senderSock, logPlayers))
        return false;

    // Zaslani zpravy o vlozeni noveho hrace (prijemce "vitezneho tahu") do listiny volnych hracu
    if(!ServerCom::SendInsertName(playData.receiverSock, playData.senderSock, logPlayers))
        return false;

    // Zaslani zpravy o vitezstvi protihraci
    if(send(playData.receiverSock, buffer, len, 0) <= 0)
        return false;

    // Ziskani detailu k ukoncovane hre
    if(!FindInPlayGames(playVector, playData, sock, gameId))
        return false;

    // Smazani ukoncene hry a vsech jejich tahu z XML
    if(!xmlCom->DeleteGame(gameId))
        return false;

    // Rozsireni seznamu volnych her o hrace, kteri hrali prave ukoncovanou hru
    freePlayers[playData.senderName] = playData.senderSock;
    freePlayers[playData.receiverName] = playData.receiverSock;

    // Odstraneni hry ze seznamu aktualne hranych her
    if(!DeleteFromPlayVector(playVector, gameId))
        return false;

    return true;
}

bool ServerCom::SendPlayerList(int sock, PlayerMap &freePlayers)
{
    // Vytvoreni bufferu
    unsigned char jumper = SockCom::SERV_GET_PLAYER_LIST;
    int numberOfPlayers = freePlayers.size();
    int len = sizeof(jumper) + sizeof(numberOfPlayers) + numberOfPlayers * sizeof(char) * SockCom::PLAYER_LEN;  // Delka zasilane zpravy
    char buffer[len];
    bzero(buffer, len);

    // Plneni bufferu
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));                    // 1B = Typ zpravy: SERV_PLAYER_LIST
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &numberOfPlayers, sizeof(numberOfPlayers));  // 4B = Pocet jmen hracu ve zprave
    pos += sizeof(numberOfPlayers);

    for(PlayerMap::iterator it = freePlayers.begin(); it != freePlayers.end(); ++it)
    { // Prochazeni seznamem volnych hracu a jejich umistovani do bufferu
        if(it->second == sock)
            continue;
        memcpy(&buffer[pos], (it->first).c_str(), (it->first).size() * sizeof(char));  // 8B = Jmeno volneho hrace
        pos += SockCom::PLAYER_LEN * sizeof(char);
    }

    // Zaslani zpravy hraci
    if(send(sock, buffer, len, 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SendInsertName(int sock, int ignoreSock, PlayerMap &logPlayers)
{
    // Jsou-li pripojeni dalsi hraci, posle se jim zprava o pripojeni tohoto hrace
    std::string insertPlayerName;
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem volnych hracu pro nalezeni jmena asociovaneho k tomuto soketu
        if(it->second == sock)
        {
            insertPlayerName = it->first;
            break;
        }
    }
    if(logPlayers.empty())
    {   // Uzivatel byl na serveru sam - nikomu nic neposilame
        return true;
    }

    // Nakopirovani jmena hrace k vlozeni do pomocneho buffeu
    unsigned char jumper = SockCom::SERV_INSERT_PLAYER;
    int len = SockCom::PLAYER_LEN * sizeof(char) + 1;  // Delka zasilane zpravy
    int pos = 0;
    char buffer[len];
    bzero(buffer, len);
    memcpy(&buffer[pos], &jumper, sizeof(jumper));
    pos += sizeof(jumper);                                  // 1B  = Typ zpravy: SERV_INSERT_PLAYER
    memcpy(&buffer[pos], (insertPlayerName).c_str(), insertPlayerName.size() * sizeof(char));    // 8B = Jmeno noveho hrace

    // Zaslani jmena hrace vyjmuteho ze seznamu volnych hracu pripojenym uzivatelum
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem volnych hracu a zasilani zprav na jednotlive sokety
        if(it->second == sock || it->second == ignoreSock)
            continue;

        // Zaslani zpravy
        if(send(it->second, buffer, len, 0) <= 0)
            return false;
    }
    return true;
}

bool ServerCom::SendDeleteName(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers)
{
    // Byl-li uzivatel na seznamu pripojenych hracu, rozesle se ostatnim zprava o jeho odpojeni
    std::string deletePlayerName;
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem pripojenych hracu pro nalezeni jmena asociovaneho k tomuto soketu
        if(it->second == sock)
        {
            deletePlayerName = it->first;
            break;
        }
    }
    if(deletePlayerName.empty())
    {   // Uzivatel byl na serveru sam - nikomu nic neposilame
        return true;
    }

    unsigned char jumper = SockCom::SERV_DELETE_PLAYER;
    int len = SockCom::PLAYER_LEN * sizeof(char) + 1;  // Delka zasilane zpravy
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));   // 1B  = Typ zpravy: SERV_DELETE_PLAYER
    pos += sizeof(jumper);
    memcpy(&buffer[pos], deletePlayerName.c_str(), deletePlayerName.size() * sizeof(char));  // 32B = Jmeno noveho hrace

    // Zaslani jmena hrace vyjmuteho ze seznamu volnych hracu pripojenym uzivatelum
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    { // Prochazeni seznamem volnych hracu a zasilani zprav na jednotlive sokety
        if(it->second == sock)
            continue;

        if(send(it->second, buffer, len, 0) <= 0)
            return false;
    }

    return true;
}

bool ServerCom::SendDeleteName(int sock, PlayerMap &freePlayers, PlayerMap &logPlayers, PlayVector &playVector, PlayData &playData)
{
    // Byl-li uzivatel na seznamu pripojenych hracu, rozesle se ostatnim zprava o jeho odpojeni
    std::string deletePlayerName;
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem pripojenych hracu pro nalezeni jmena asociovaneho k tomuto soketu
        if(it->second == sock)
        {
            deletePlayerName = it->first;
            break;
        }
    }
    if(deletePlayerName.empty())
    {   // Uzivatel byl na serveru sam - nikomu nic neposilame
        return true;
    }

    // Zjisteni, zda odpojeny uzivatel nehral v dobe odpojeni hru
    FindInPlayGames(playVector, playData, deletePlayerName);  // Naplni stukturu playData popisujici detaily rozehrane hry
    if(playData.gameId != -1)
    {   // Uzivatel hral v dobe odpojeni nejakou hru
        freePlayers[playData.receiverName] = logPlayers[playData.receiverName];
    }

    unsigned char jumper = SockCom::SERV_DELETE_PLAYER;
    int len = sizeof(jumper) + SockCom::PLAYER_LEN * sizeof(char);  // Delka zasilane zpravy
    char buffer[len];
    bzero(buffer, len);
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));  // 1B  = Typ zpravy: SERV_DELETE_PLAYER
    pos += sizeof(jumper);
    memcpy(&buffer[pos], deletePlayerName.c_str(), deletePlayerName.size() * sizeof(char));  // 8B = Jmeno noveho hrace

    // Zaslani jmena hrace vyjmuteho ze seznamu volnych hracu pripojenym uzivatelum
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    { // Prochazeni seznamem volnych hracu a zasilani zprav na jednotlive sokety
        if(it->second == sock)
            continue;

        if(send(it->second, buffer, len, 0) <= 0)
            return false;
    }

    return true;
}

bool ServerCom::SendGameList(int sock, PlayerMap &logPlayers)
{
    // Precteni jmena protihrace
    int len  = (SockCom::PLAYER_LEN) * sizeof(char);
    char buffer[SockCom::PLAYER_LEN + 1];
    bzero(buffer, (SockCom::PLAYER_LEN + 1) * sizeof(char));

    if(recv(sock, buffer, len, 0) <= 0)
        return false;

    std::string opponentName = std::string(buffer);

    // Ziskani jmena vyzivatele (ten, kdo zaslal pozadavek)
    std::string challengerName;
    for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
    { // Prochazeni seznamem volnych hracu a jejich umistovani do bufferu
        if(it->second == sock)
        {
            challengerName = std::string(it->first);
            break;
        }
    }

    // Naplneni vektoru slouziciho pro ulozeni rozehranych her s vybranym protihracem
    SockCom::GameVector opponentGames;
    xmlCom->FindOpponentGames(challengerName, opponentName, opponentGames);

    // Zaslani seznamu rozehranych her s vybranym protihracem
    unsigned char jumper = SockCom::SERV_GAME_LIST;
    int numberOfGames = opponentGames.size();
    len = sizeof(jumper) + sizeof(numberOfGames) + numberOfGames * sizeof(SockCom::GameData);  // Delka zasilane zpravy
    char sendBuffer[len];
    bzero(sendBuffer, len);

    int pos = 0;
    memcpy(&sendBuffer[pos], &jumper, sizeof(jumper));                // 1B = Typ zpravy: SERV_GAME_LIST
    pos = sizeof(jumper);
    memcpy(&sendBuffer[pos], &numberOfGames, sizeof(numberOfGames));  // 4B = Pocet rozehranych her ve zprave
    pos += sizeof(numberOfGames);
    for(int i = 0; i < opponentGames.size(); i++)
    {   // Prochazeni seznamem rozehranych her a jejich umistovani do bufferu
        memcpy(&sendBuffer[pos], &(opponentGames[i]), sizeof(SockCom::GameData));  // XB = Struktura popisujici rozehranou hru
        pos += sizeof(SockCom::GameData);
    }

    if(send(sock, sendBuffer, len, 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SendGameSteps(int sock)
{
    // Precteni ID rozehrane hry
    int gameId;
    if(recv(sock, &gameId, sizeof(gameId), 0) <= 0)
        return false;

    // Naplneni vektoru slouziciho pro ulozeni tahu k rozehrane hre
    SockCom::StepVector stepVector;
    xmlCom->FindGameSteps(gameId, stepVector);

    // Ziskani celkove delky zpravy
    int dataLen = 0;           // Delka zasilane zpravy
    for(int i = 0; i < stepVector.size(); i++)
    {
        dataLen += stepVector[i].size() + 1;
    }

    unsigned char stepLen;
    unsigned char jumper = SockCom::SERV_GAME_STEPS;
    unsigned int len = sizeof(jumper) + sizeof(dataLen) + dataLen * sizeof(SockCom::MovData);  // Delka zasilane zpravy
    char buffer[len];
    bzero(buffer, len);

    SockCom::MovData movData;
    int pos = 0;
    memcpy(&buffer[pos], &jumper, sizeof(jumper));       // 1B = Typ zpravy: SERV_GAME_STEPS
    pos += sizeof(jumper);
    memcpy(&buffer[pos], &dataLen, sizeof(dataLen));     // 4B = Delka datove casti zpravy
    pos += sizeof(dataLen);

    for(int i = 0; i < stepVector.size(); i++)
    {
        stepLen = static_cast<unsigned char>(stepVector[i].size());
        memcpy(&buffer[pos], &stepLen, sizeof(stepLen));  // 1B = Pocet zmen v ramci jednoho tahu
        pos += sizeof(stepLen);


        for(int j = 0; j < stepVector[i].size(); j++)
        {
            memcpy(&buffer[pos], &(stepVector[i][j]), sizeof(SockCom::MovData));  // 1B = Zmena v ramci aktualniho tahu
            pos += sizeof(SockCom::MovData);
        }
    }

    if(send(sock, buffer, len, 0) <= 0)
        return false;

    return true;
}

bool ServerCom::RegisterToServer(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool &playerRegistered)
{
    // Precteni jmena hrace
    unsigned int len  = (SockCom::PLAYER_LEN) * sizeof(char);
    char buffer[SockCom::PLAYER_LEN + 1];
    bzero(buffer, (SockCom::PLAYER_LEN + 1) * sizeof(char));

    if(recv(sock, buffer, len, 0) <= 0)
        return false;
    std::string playerName = std::string(buffer);
    itPlayerMap found=logPlayers.find(playerName);

    // Nastaveni odpovedi danemu hraci
    unsigned char jumper;
    if(!xmlCom->IsExistsName(playerName))
    {   // Pokud hrac nebyl v seznamu registrovanych hracu nalezen
        logPlayers[playerName] = sock;     // Pridani hrace do seznamu prihlasenych hracu
        jumper = SockCom::SERV_REGISTERED;
        playerRegistered = true;     // Nastaveni odkazem predaneho parametru indikujiciho uspesnou / neuspesnou registraci

        if(!xmlCom->InsertPlayer(playerName))
        {   // Test uspesnosti vlozeni hrace do seznamu registrovanych hracu
            return false;
        }
    }
    else
    {
        jumper = SockCom::SERV_NOT_REGISTERED;
        playerRegistered = false;
    }

    if(send(sock, &jumper, sizeof(jumper), 0) <= 0)
        return false;

    return true;
}

bool ServerCom::SetPlayerVisibility(int sock, PlayerMap &logPlayers, PlayerMap &freePlayers, bool visibility)
{
    if(visibility)
    {   // Hrac ma byt zviditelnen
        std::string playerName;
        for(PlayerMap::iterator it = logPlayers.begin(); it != logPlayers.end(); ++it)
        { // Prochazeni seznamem volnych hracu a jejich umistovani do bufferu
            if(it->second == sock)
            {
                playerName = std::string(it->first);
                freePlayers[playerName] = sock;        // Pridani jmena do seznamu volnych hracu

                if(!SendInsertName(sock, sock, logPlayers))  // Zaslani zpravy o pridani do volnych hracu vsem ostatnim hracum
                    return false;

                break;
            }
        }
    }
    else
    {   // Hrac ma byt zneviditelnen
        for(PlayerMap::iterator it = freePlayers.begin(); it != freePlayers.end(); ++it)
        {   // Prochazeni seznamem volnych hracu
            if(it->second == sock)
            {
                freePlayers.erase(it);  // Vyjmuti jmena ze seznamu volnych hracu

                if(!ServerCom::SendDeleteName(sock, freePlayers, logPlayers))  // Zaslani zpravy o odstraneni do volnych hracu vsem ostatnim hracum
                    return false;

                break;
            }
        }
    }

    return true;
}
