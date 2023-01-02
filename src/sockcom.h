/**
* \file sockcom.h
* Rozhran� pro moduly staraj�c� se o komunikaci mezi klientem a serverem
* \author Jaroslav Bendl (xbendl00)
*/


#ifndef SOCKCOMM_H
#define SOCKCOMM_H

#include <vector>
#include <map>

/// Definuje datov� typy pou��van� pro komunikaci mezi klientem a serverem
class SockCom
{
public:
    /// D�lka jm�na hr��e
    static const int PLAYER_LEN = 12;

    /// Struktura popisuj�c� rozehranou hru
    typedef struct gameData
    {
        int gameId;                         ///< ID jednozna�n� ur�uj�c� hru
        char onStepPlayer[PLAYER_LEN+1];    ///< Jm�no hr��e, kter� je na tahu
        time_t date;                        ///< Datum a �as, kdy byla hra rozehr�na
    } GameData;

    /// Vektor struktur popisuj�c� rozehran� hry
    typedef std::vector<GameData> GameVector;

    /// Struktura popisuj�c� pohyb na �achovnici (pohyb� m��e b�t v�ce v r�mci jednoho tahu)
    typedef unsigned char MovData;

    /// Vektor zm�n popisuj�c� tah hry
    typedef std::vector<MovData> DataVector;

    /// Vektor tah� popisuj�c� vybranou hru
    typedef std::vector<DataVector> StepVector;

    /// Vektor jmen protihr���
    typedef std::vector<std::string> StringVector;

    /// Komunika�n� p��znaky (prvn� bit zas�lan� zpr�vy v komunikaci klient - server)
    enum COM_FLAGS
    {
        CLI_SIGN = 0x10,               ///< ��dost o p�ihl�en� (Klient -> Server)
        SERV_SIGNED = 0x11,            ///< Pozitivn� odpov�� na ��dost o p�ihl�en� (Server -> Klient)
        SERV_UNSIGNED = 0x12,          ///< Negativn� odpov�� na ��dost o p�ihl�en� (Server -> Klient)
        CLI_ASK_GAME = 0x20,           ///< ��dost protihr��e o hru (Klient_1 -> Server)
        SERV_ASK_GAME = 0x21,          ///< ��dost protihr��e o hru (Server -> Klient_2)
        SERV_ASK_CONT_GAME = 0x22,     ///< ��dost protihr��e o dohr�ni hry (Server -> Klient_2)
        CLI_ACCEPT_GAME = 0x23,        ///< Pozitivn� odpov�� na ��dost o hru (Klient_2 -> Server)
        SERV_ACCEPT_GAME = 0x24,       ///< Pozitivn� odpov�� na ��dost o hru (Server -> Klient_1)
        CLI_REJECT_GAME = 0x26,        ///< Negativn� odpov�� na ��dost o hru (Klient_2 -> Server)
        SERV_REJECT_GAME = 0x27,       ///< Negativn� odpov�� na ��dost o hru (Server -> Klient1)
        SERV_SEND_GAME_ID = 0x28,      ///< ��dost ID vytvo�en� hry protihr��i (Server -> Klient2)
        CLI_END_GAME = 0x30,           ///< ��dost o p�eposl�n� zpr�vy o ukon�en� hry (Klient_1 -> Server)
        SERV_END_GAME = 0x31,          ///< ��dost o p�eposl�n� zpr�vy o ukon�en� hry (Server -> Klient_2)
        CLI_SEND_STEP = 0x40,          ///< ��dost o p�eposl�n� tahu protihr��i (Klient_1 -> Server)
        SERV_SEND_STEP = 0x41,         ///< ��dost o p�eposl�n� tahu protihr��i (Server -> Klient_2)
        CLI_SEND_WIN = 0x50,           ///< ��dost o p�eposl�n� zpr�vy o v�h�e protihr��i (Klient_1 -> Server)
        SERV_SEND_WIN = 0x51,          ///< ��dost o p�eposl�n� zpr�vy o v�h�e protihr��i (Server -> Klient_2)
        SERV_GET_PLAYER_LIST = 0x60,   ///< ��dost o seznam voln�ch hr��� (Server -> Klient)
        SERV_INSERT_PLAYER = 0x61,     ///< ��dost jm�na pr�v� p�ihla�en�ho hr��e ostatn�m p�ihl�en�m hr���m (Server -> Klient)
        SERV_DELETE_PLAYER = 0x62,     ///< Zasl�n� jm�na pr�v� odhl�en�ho hr��e ostatn�m p�ihl�en�m hr���m (Server -> Klient)
        CLI_GAME_LIST = 0x70,          ///< ��dost o rozehran� hry s vybran�m protihr��em (Klient -> Server)
        SERV_GAME_LIST = 0x71,         ///< Odpov�� na ��dost o rozehran� hry s vybran�m protihr��em (Server -> Klient)
        CLI_GAME_STEPS = 0x80,         ///< ��dost o tahy k rozehran� h�e (Klient -> Server)
        SERV_GAME_STEPS = 0x81,        ///< Odpov�� na ��dost o tahy k rozehran� h�e (Server -> Klient)
        CLI_REGISTER = 0x90,           ///< ��dost o registraci nov�ho hr��e (Klient -> Server)
        SERV_REGISTERED = 0x91,        ///< Pozitivn� odpov�� na ��dost o registraci nov�ho hr��e (Klient -> Server)
        SERV_NOT_REGISTERED = 0x92,    ///< Negativn� odpov�� na ��dost o registraci nov�ho hr��e (Server -> Klient)
        CLI_VISIBLE = 0xA0,            ///< ��dost o zviditeln�n� hr��e na serveru (Klient -> Server)
        CLI_INVISIBLE = 0xA1           ///< ��dost o zneviditeln�n� hr��e na serveru (Klient -> Server)
    };
};

#endif
