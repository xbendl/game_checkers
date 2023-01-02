// server.cc
// Server slouzici pro vyrizovani pozadavku klientu
// Autor: Jaroslav Bendl (xbendl00)

#include "server.h"

using namespace std;

// --------------------------------------------------------------------------
// MAIN
// --------------------------------------------------------------------------

bool getParams(int argc, char **argv, int &port)
{
    if(argc == 1)
    {
        port = 3000;
    }
    else if(argc == 2)
    {
        char *wrongChar = NULL;   // Ukazatel na prvni nekorektni znak
        port = strtol(argv[1], &wrongChar, 0);

        if(*wrongChar != '\0')
        {   // Overeni spravneho zadani cisla portu
            cerr << "CHYBA: Port musi byt zadan jako cislo." << endl;
            return false;
        }
    }
    else
    {
        cerr << "CHYBA: Program volejte ve tvaru:\n./server [port]" << endl;
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    // Zpracovani parametru prikazoveho radku
    int port;
    if(!getParams(argc, argv, port))
        return false;

    // Vytvoreni serveru
    DamaServer *frame = new DamaServer(port);

    return true;
}

// --------------------------------------------------------------------------
// DAMASERVER
// --------------------------------------------------------------------------

DamaServer::DamaServer(int port)
{
    serverCom = new ServerCom();
    mPort = port;

    // Vytvoreni socketu
    if(!CreateSocket())
        return;

    // Naslouchani na soketu
    if(!ListenOnSocket())
        return;
}

DamaServer::~DamaServer()
{
    delete serverCom;
}

bool DamaServer::CreateSocket()
{
    struct sockaddr_in serverAddr;

    // Vytvoreni soketu
    if((mServer = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Chyba pri tvorbe socketu: ");
        return false;
    }

    // Umozneni provest znovu bind stejneho soketu (eliminuje casovou prodlevu mezi starty aplikace)
    int wf = 1;
    setsockopt(mServer, SOL_SOCKET, SO_REUSEADDR, &wf, sizeof(wf));

    // Nastaveni parametru pripojeni k serveru
    serverAddr.sin_family = AF_INET;           // Urcime rodinu protokolu
    serverAddr.sin_port = htons(mPort);        // Urcime cislo portu
    serverAddr.sin_addr.s_addr = INADDR_ANY;   // Urcime cilovou IP adresu
    memset(&(serverAddr.sin_zero), '\0', 8);   // Zbytek vynulujeme

    // Prirazeni jmena socketu
    if(bind(mServer, (sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Chyba pri pojmenovani soketu: ");
        return false;
    }

    // Vytvoreni fronty pozadavku na spojeni
    if(listen(mServer, 10) == -1)
    {
        perror("Chyba pri vytvareni fronty: ");
        return false;
    }

    return true;
}

bool DamaServer::ListenOnSocket()
{
    struct sockaddr_in clientAddr;
    int sock;
    vector<int> childSock;  // Identifikatory soketu pripojenych klientu
    pollfd *group = NULL, *temp;
    int socketCount = 0, actSocketCount = 0, allocCount = 0, j;
    unsigned char jumper;


    // Hlavni smycka obsluhujici udalosti na hlavnim soketu i pripojenych klientskych soketech
    while(true)
    {
        actSocketCount = clientVector.size() + 1;
        if(socketCount != actSocketCount)
        {   // Pocet soketu pripojenych klientu se zmenil
            socketCount = actSocketCount;
            if(allocCount < actSocketCount)
            {   // Je nutne prialokovat prostor pro soket nove pripojeneho klienta (alokuje se po blocich)
                allocCount = actSocketCount + DamaServer::capacityIncrement;
                temp = group;
                if((group = (pollfd *)realloc(group, (allocCount * sizeof(pollfd)))) == NULL)
                {   // Chyba pri alokaci pameti
                    free(temp);
                    ShutDownServer();
                    cerr << "Chyba pri alokaci pameti." << endl;
                    return false;
                }
            }

            // Naplneni vytvoreneho pole
            group[0].fd = mServer;       // Nulty prvek je pro naslouchaci soket na serveru
            group[0].events = POLLIN;    // Zajimaji nas pouze prichozi pozadavky na tento soket
            group[0].revents = 0;
            for(int i = 0, p = 1; i != clientVector.size(); i++, p++)
            {   // Vlozeni soketu pripojenych klientu
                group[p].fd = clientVector[i];
                group[p].events = POLLIN;
                group[p].revents = 0;
            }
        }

        // Chyba na hlavnim soketu
        if(poll(group, socketCount, -1) < 0)
        {
            free(group);
            ShutDownServer();
            cerr << "Selhani funkce poll." << endl;
            return -1;
        }

        // Kontrola zmeny na hlavnim soketu
        if((group[0].revents != 0 & POLLIN) != 0)
        {
            socklen_t clientLen = sizeof(clientAddr);
            int client = accept(mServer, (sockaddr*) &clientAddr, &clientLen);
            clientVector.push_back(client);
        }

        // Projdu vsechna spojeni a zkontroluji, jestli na nich nedoslo k zmene
        for(j = 1; j < socketCount; j++)
        {   // Na soketu jsou prichozi data
            if((group[j].revents & (POLLIN)) != 0)
            {   // Na soketu jsou prichozi data
                if(recv(group[j].fd, &jumper, 1, 0) <= 0)
                {   // Nekorektni obsah
                    if(errno != EAGAIN && errno != EWOULDBLOCK)
                    {   // Dany soket neni dostupny
                        clientVector.erase(find(clientVector.begin(), clientVector.end(), group[j].fd));
                        SocketLost(group[j].fd);
                        break;
                    }
                }
                else
                {   // Korektni obsah
                    SocketInput(group[j].fd, jumper);
                }
                group[j].revents = 0;
            }
        }
    }

    return true;
}

void DamaServer::ShutDownServer()
{
    close(mServer);     // Zavreni naslouchaciho soketu serveru
    for(int i = 0; i < clientVector.size(); i++)
    {   // Zavreni soketu pripojenych klientu
        close(clientVector[i]);
    }
    clientVector.erase(clientVector.begin(), clientVector.end());
}

// Obsluha udalosti na soketu: Na soketu jsou nova data
bool DamaServer::SocketInput(int clientSock, unsigned char jumper)
{
    switch(jumper)
    {   // Rozhodnuti podle typu pozadavku
        case SockCom::CLI_SIGN:        // Pozadavek na zarazeni do seznamu volnych hracu
        {
            // Overeni prihlasovaciho jmena uzivatele + odpoved
            bool playerSigned = false;
            if(!serverCom->SignToServer(clientSock, logPlayers, freePlayers, playerSigned))
                WriteMessageToFile("Vyrizeni pozadavku na prihlaseni hrace se nepodarilo.");

            if(!playerSigned)
            {   // Pokud bylo prihlaseni neuspesne, dale nepokracuji
                break;
            }

            // Zaslani seznamu volnych hracu pripojenemu uzivateli
            if(!serverCom->SendPlayerList(clientSock, freePlayers))
                WriteMessageToFile("Vyrizeni pozadavku na zaslani seznamu volnych hracu se nepodarilo.");

            break;
        }
        case SockCom::CLI_ASK_GAME:    // Pozadavek na preposlani zadosti protihrace o hru
        {
            if(!serverCom->SendAskPlayGame(clientSock, freePlayers, logPlayers))
                WriteMessageToFile("Vyrizeni pozadavku na preposlani zadosti protihrace o hru se nepodarilo.");
            break;
        }
        case SockCom::CLI_ACCEPT_GAME: // Pozadavek na preposlani souhlasu na zadost protihrace o hru
        {
            if(!serverCom->SendAnswerPlayGame(clientSock, true, freePlayers, logPlayers, playVector))
                WriteMessageToFile("Vyrizeni pozadavku na preposlani souhlasu ke hre se nepodarilo.");
            break;
        }
        case SockCom::CLI_REJECT_GAME: // Pozadavek na preposlani nesouhlasu na zadost protihrace o hru
        {
            if(!serverCom->SendAnswerPlayGame(clientSock, false, freePlayers, logPlayers, playVector))
                WriteMessageToFile("Vyrizeni pozadavku na preposlani nesouhlasu se hrou se nepodarilo.");
            break;
        }
        case SockCom::CLI_END_GAME:    // Pozadavek na preposlani zpravy o ukonceni hry
        {
            if(!serverCom->SendEndGame(clientSock, freePlayers, logPlayers, playVector))
                WriteMessageToFile("Vyrizeni pozadavku na preposlani zpravy o ukonceni hry ke hre se nepodarilo.");
            break;
        }
        case SockCom::CLI_SEND_STEP:   // Pozadavek na preposlani tahu protihraci
        {
            if(!serverCom->SendStep(clientSock, freePlayers, logPlayers, playVector))
                WriteMessageToFile("Vyrizeni pozadavku na preposlani tahu protihraci se nepodarilo.");
            break;
        }
        case SockCom::CLI_SEND_WIN:    // Pozadavek na preposlani zpravy urcujici vitezstvi ve hre
        {
            if(!serverCom->SendWinInfo(clientSock, freePlayers, logPlayers, playVector))
                WriteMessageToFile("Vyrizeni pozadavku na preposlani zpravy urcujici vitezstvi ve hre se nepodarilo.");
            break;
        }
        case SockCom::CLI_GAME_LIST:   // Pozadavek na zaslani rozehranych her s vybranym spoluhracem
        {
            if(!serverCom->SendGameList(clientSock, logPlayers))
                WriteMessageToFile("Vyrizeni pozadavku na zaslani rozehranych her s vybranym spoluhracem se nepodarilo.");
            break;
        }
        case SockCom::CLI_GAME_STEPS:  // Pozadavek na zaslani tahu k vybrane rozehrane hre
        {
            if(!serverCom->SendGameSteps(clientSock))
                WriteMessageToFile("Vyrizeni pozadavku na zaslani tahu k vybrane rozehrane hre se nepodarilo.");
            break;
        }
        case SockCom::CLI_REGISTER:    // Pozadavek na registraci noveho hrace
        {
            // Overeni registrovaneho jmena + odpoved
            bool playerRegistered = false;
            if(!serverCom->RegisterToServer(clientSock, logPlayers, freePlayers, playerRegistered))
                WriteMessageToFile("Vyrizeni pozadavku na registraci hrace se nepodarilo.");

            if(!playerRegistered)
            {   // Pokud byla registrace neuspesna, dale nepokracuji
                break;
            }

            // Zaslani seznamu volnych hracu pripojenemu uzivateli
            if(!serverCom->SendPlayerList(clientSock, freePlayers))
                WriteMessageToFile("Vyrizeni pozadavku na zaslani volnych hracu se nepodarilo.");

            break;
        }
        case SockCom::CLI_VISIBLE:     // Pozadavek o zviditelneni hrace
        {
            if(!serverCom->SetPlayerVisibility(clientSock, logPlayers, freePlayers, true))
                WriteMessageToFile("Vyrizeni pozadavku na zviditelneni hrace se nepodarilo.");
            break;
        }
        case SockCom::CLI_INVISIBLE:   // Pozadavek o zneviditelneni
        {
            if(!serverCom->SetPlayerVisibility(clientSock, logPlayers, freePlayers, false))
                WriteMessageToFile("Vyrizeni pozadavku na zviditelneni hrace se podarilo.");
            break;
        }
    }
}

// Obsluha udalosti na soketu: Socket rozvazal spojeni
bool DamaServer::SocketLost(int sock)
{
    // Zaslani zpravy o odpojeni hrace vsem ostatnim pripojenym hracum
    ServerCom::PlayData playData;
    playData.gameId = -1;
    if(!serverCom->SendDeleteName(sock, freePlayers, logPlayers, playVector, playData))
        WriteMessageToFile("Vyrizeni pozadavku na vyjmuti hrace ze seznamu volnych hracu se nepodarilo.");

    // Odstraneni odpojeneho hrace ze seznamu prihlasenych hracu
    for(ServerCom::itPlayerMap it = logPlayers.begin(); it != logPlayers.end(); ++it)
    {   // Prochazeni seznamem prihlasenych hracu
        if(it->second == sock)
        {   // Pro Soket, s kterym bylo rozvazano spojeni vymazeme odpovidajici polozku ze seznamu prihlasenych hracu
            logPlayers.erase(it);
            break;
        }
    }

    // Odstraneni odpojeneho hrace ze seznamu volnych hracu
    for(ServerCom::itPlayerMap it = freePlayers.begin(); it != freePlayers.end(); ++it)
    {   // Prochazeni seznamem volnych hracu
        if(it->second == sock)
        {   // Pro Soket, s kterym bylo rozvazano spojeni vymazeme odpovidajici polozku ze seznamu volnych hracu
            freePlayers.erase(it);
            break;
        }
    }

    // Pokud hrac v dobe odpojeni hral hru, pak i ta bude prerusena
    if(playData.gameId != -1)
    {
        if(!serverCom->SendInsertName(playData.receiverSock, playData.receiverSock, logPlayers))
            WriteMessageToFile("Vyrizeni pozadavku na vlozeni hrace do seznamu volnych hracu se nepodarilo.");
        if(!serverCom->SendEndGame(playData.receiverSock, playData.gameId))
            WriteMessageToFile("Vyrizeni pozadavku na zaslani zpravy o ukonceni hry se nepodarilo.");
        serverCom->DeleteFromPlayVector(playVector, playData.gameId);
    }

    close(sock);

    return true;
}

void DamaServer::WriteMessageToFile(std::string logMessage)
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    //printf("Current local time and date: %s", asctime (timeinfo));
    std::stringstream date;
    date << timeinfo->tm_mday << "." << (timeinfo->tm_mon + 1) << "." << (timeinfo->tm_year + 1900) << ", " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec;
    std::string completeMessage = date.str() + " - " + logMessage + "\n";

    ofstream outFile("serverlog.txt", ofstream::app);
    if(outFile.fail())
    {   // Log soubor se nepodarilo otevrit
        cerr << "Log soubor se nepodarilo otevrit pro zapis." << endl;
        return;
    }

    copy(completeMessage.begin(), completeMessage.end(), ostream_iterator<char>(outFile,""));

    return;
}
