/**
* \file sockcom.h
* Rozhraní pro moduly starající se o komunikaci mezi klientem a serverem
* \author Jaroslav Bendl (xbendl00)
*/


#ifndef SOCKCOMM_H
#define SOCKCOMM_H

#include <vector>
#include <map>

/// Definuje datové typy pou¾ívané pro komunikaci mezi klientem a serverem
class SockCom
{
public:
    /// Délka jména hráèe
    static const int PLAYER_LEN = 12;

    /// Struktura popisující rozehranou hru
    typedef struct gameData
    {
        int gameId;                         ///< ID jednoznaènì urèující hru
        char onStepPlayer[PLAYER_LEN+1];    ///< Jméno hráèe, který je na tahu
        time_t date;                        ///< Datum a èas, kdy byla hra rozehrána
    } GameData;

    /// Vektor struktur popisující rozehrané hry
    typedef std::vector<GameData> GameVector;

    /// Struktura popisující pohyb na ¹achovnici (pohybù mù¾e být více v rámci jednoho tahu)
    typedef unsigned char MovData;

    /// Vektor zmìn popisující tah hry
    typedef std::vector<MovData> DataVector;

    /// Vektor tahù popisující vybranou hru
    typedef std::vector<DataVector> StepVector;

    /// Vektor jmen protihráèù
    typedef std::vector<std::string> StringVector;

    /// Komunikaèní pøíznaky (první bit zasílané zprávy v komunikaci klient - server)
    enum COM_FLAGS
    {
        CLI_SIGN = 0x10,               ///< ®ádost o pøihlá¹ení (Klient -> Server)
        SERV_SIGNED = 0x11,            ///< Pozitivní odpovìï na ¾ádost o pøihlá¹ení (Server -> Klient)
        SERV_UNSIGNED = 0x12,          ///< Negativní odpovìï na ¾ádost o pøihlá¹ení (Server -> Klient)
        CLI_ASK_GAME = 0x20,           ///< ®ádost protihráèe o hru (Klient_1 -> Server)
        SERV_ASK_GAME = 0x21,          ///< ®ádost protihráèe o hru (Server -> Klient_2)
        SERV_ASK_CONT_GAME = 0x22,     ///< ®ádost protihráèe o dohráni hry (Server -> Klient_2)
        CLI_ACCEPT_GAME = 0x23,        ///< Pozitivní odpovìï na ¾ádost o hru (Klient_2 -> Server)
        SERV_ACCEPT_GAME = 0x24,       ///< Pozitivní odpovìï na ¾ádost o hru (Server -> Klient_1)
        CLI_REJECT_GAME = 0x26,        ///< Negativní odpovìï na ¾ádost o hru (Klient_2 -> Server)
        SERV_REJECT_GAME = 0x27,       ///< Negativní odpovìï na ¾ádost o hru (Server -> Klient1)
        SERV_SEND_GAME_ID = 0x28,      ///< ®ádost ID vytvoøené hry protihráèi (Server -> Klient2)
        CLI_END_GAME = 0x30,           ///< ®ádost o pøeposlání zprávy o ukonèení hry (Klient_1 -> Server)
        SERV_END_GAME = 0x31,          ///< ®ádost o pøeposlání zprávy o ukonèení hry (Server -> Klient_2)
        CLI_SEND_STEP = 0x40,          ///< ®ádost o pøeposlání tahu protihráèi (Klient_1 -> Server)
        SERV_SEND_STEP = 0x41,         ///< ®ádost o pøeposlání tahu protihráèi (Server -> Klient_2)
        CLI_SEND_WIN = 0x50,           ///< ®ádost o pøeposlání zprávy o výhøe protihráèi (Klient_1 -> Server)
        SERV_SEND_WIN = 0x51,          ///< ®ádost o pøeposlání zprávy o výhøe protihráèi (Server -> Klient_2)
        SERV_GET_PLAYER_LIST = 0x60,   ///< ®ádost o seznam volných hráèù (Server -> Klient)
        SERV_INSERT_PLAYER = 0x61,     ///< ®ádost jména právì pøihla¹eného hráèe ostatním pøihlá¹eným hráèùm (Server -> Klient)
        SERV_DELETE_PLAYER = 0x62,     ///< Zaslání jména právì odhlá¹eného hráèe ostatním pøihlá¹eným hráèùm (Server -> Klient)
        CLI_GAME_LIST = 0x70,          ///< ®ádost o rozehrané hry s vybraným protihráèem (Klient -> Server)
        SERV_GAME_LIST = 0x71,         ///< Odpovìï na ¾ádost o rozehrané hry s vybraným protihráèem (Server -> Klient)
        CLI_GAME_STEPS = 0x80,         ///< ®ádost o tahy k rozehrané høe (Klient -> Server)
        SERV_GAME_STEPS = 0x81,        ///< Odpovìï na ¾ádost o tahy k rozehrané høe (Server -> Klient)
        CLI_REGISTER = 0x90,           ///< ®ádost o registraci nového hráèe (Klient -> Server)
        SERV_REGISTERED = 0x91,        ///< Pozitivní odpovìï na ¾ádost o registraci nového hráèe (Klient -> Server)
        SERV_NOT_REGISTERED = 0x92,    ///< Negativní odpovìï na ¾ádost o registraci nového hráèe (Server -> Klient)
        CLI_VISIBLE = 0xA0,            ///< ®ádost o zviditelnìní hráèe na serveru (Klient -> Server)
        CLI_INVISIBLE = 0xA1           ///< ®ádost o zneviditelnìní hráèe na serveru (Klient -> Server)
    };
};

#endif
