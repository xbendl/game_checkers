/**
* \file clientframe.h
* Okno klientské aplikace
* \author Maksym Kryzhanovskyy (xkryzh00)
*/

#ifndef CLIENTFRAME_H
#define CLIENTFRAME_H 1

#include "wx/file.h"
#include <wx/utils.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/aboutdlg.h>
#include <wx/timer.h>
#include <wx/spinctrl.h>
#include "wx/app.h"
#include "wx/frame.h"
#include "clientcom.h"
#include "xmlconfig.h"
#include "xmlcom.h"
#include "wx/notebook.h"

/// Vlastní aplikace
class ClientApp : public wxApp
{
    wxLocale appLocale;
    
public:
    virtual bool OnInit();
};

class ClientBoard;

/// Hlavní okno aplikace
class ClientFrame : public wxFrame
{
    DECLARE_EVENT_TABLE()

    ClientBoard *m_board;
    ClientCom *clientCom;        //< Objekt zaji¹»ující soketovou komunikaci se serverem
    XmlConfig *xmlConfig;        //< Objekt modifikující XML s konfigurací klientské aplikace
    XmlCom *xmlCom;              //< Objekt modifikující XML s ulo¾enými hrami
    wxIPV4address mAddr;         //< Urèuje adresu vzdáleného serveru
    wxSocketClient *mSock;       //< Socket, ke kterému je vázáno pøipojení na vzdálený server
    wxToolBar *mToolBar;
    wxNotebook *mNotebook;
    wxListBox *mBoxFreePlayers;
    wxListBox *mBoxSavedPlayers;
    wxListBox *mBoxGameList;
    wxMenuBar *mMenu;
    wxMenu *mMenuFile;
    wxMenu *mMenuGame;
    wxMenu *mMenuServer;
    wxMenu *mMenuReplay;
    wxMenu *mMenuHelp;
    wxTimer mTimer;
    int mGameId;                 //< Urèuje ID momentálnì hrané hry
    bool mLog;                   //< Urèuje zda je zapnuté logování
    bool mConstructed;
    bool mPlayGame;              //< Urèuje zda hráè momentálnì hraje hru
    bool mOnStep;                //< Urèuje zda je hráè na tahu
    bool mConnected;             //< Urèuje zda je hráè pøipojen k serveru
    int mDelayTime;              //< Urèuje prodlevu mezi kroky pøi plynulém pøehrávání hry
    wxString mName;              //< Urèuje jméno hráèe
    wxString mOpponentName;      //< Urèuje jméno protihráèe
    wxString mOnStepPlayer;      //< Urèuje jméno hráèe na tahu
    SockCom::GameVector mOpponentGames; //< Vektor s rozehranými hrami s vybraným protihráèem
    SockCom::StepVector mGameSteps;     //< Vektor s tahy rozehrané hry s vybraným protihráèem

    friend class LogDialog;
    friend class SelectPlayerDialog;
    friend class ConfigDialog;

    // Menu
    void OnRelog(wxCommandEvent& event);
    void OnConfig(wxCommandEvent& event);
    void OnAbout(wxCommandEvent&);
    void OnHelp(wxCommandEvent& event);

    // Toolbar
    void OnNewGame(wxCommandEvent&);
    void OnContinueGame(wxCommandEvent&);
    void OnEndGame(wxCommandEvent&);
    void OnQuit(wxCommandEvent&);
    void OnOpenConnection(wxCommandEvent&);
    void OnCloseConnection(wxCommandEvent&);
    void OnStartPlay(wxCommandEvent& event);
    void OnStopPlay(wxCommandEvent& event);
    void OnBackPlay(wxCommandEvent& event);
    void OnForwardPlay(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& WXUNUSED(event));
    void OnClosePlay(wxCommandEvent& event);
    void OnPausePlay(wxCommandEvent& event);

    // Notebook
    void OnNotebookPageChanged(wxNotebookEvent& event);
    void OnNotebookPageChanging(wxNotebookEvent& event);

    // Listboxy
    void OnPlayerSelect(wxCommandEvent&);
    void OnSavedPlayerSelect(wxCommandEvent& event);
    void OnGameSelect(wxCommandEvent&);
    void OnInitPlay(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);

    // Socket
    void OnSocketEvent(wxSocketEvent&);

    
    // Uzivatelske funkce
    void UpdateStatusBar();
    void PrepareToRelog();
    void FillBox();

    /**
    * Vyvolá pøihla¹ovací dialog
    * @return True, pokud se u¾ivatel úspì¹nì pøihlásil / registroval, jinak False
    */    
    bool LogUser();
    
    /**
    * Za¹le ke vzdálenému serveru pøihla¹ovací údaje
    * @return True, pokud se zaslání zprávy podaøilo, jinak false
    */
    bool SignToServer();

    /**
    * Za¹le ke vzdálenému serveru registrované jméno
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool RegisterToServer();
        
    /**
    * Zpracuje pøíchozí zprávu
    */
    void SocketInput();

public:
    ClientFrame();
    ~ClientFrame();
    
    /**
    * Za¹le u¾ivatelùm tah protihráèi a zároveò jej vlo¾í do lokálního XML
    * @param dataVector Vektor se zasílanými pohyby na ¹achovnici v rámci tahu
    * @return True, pokud se zaslání zprávy podaøilo, jinak false
    */
    bool SendStep(SockCom::DataVector &dataVector);

    /**
    * Získá tah od protihráèe a zároveò jej vlo¾í do lokálního XML
    * @param dataVector Vektor se zasílanými pohyby na ¹achovnici v rámci tahu
    * @return True, pokud se získání zprávy podaøilo, jinak false
    */
    bool GetStep(SockCom::DataVector &dataVector);

    /**
    * Za¹le protihráèi zprávu o jeho vítìzství
    * @return True, pokud se zaslání zprávy podaøilo, jinak false.
    */
    bool SendWinInfo();
    
    /**
    * Získá zprávu od protihráèe urèující, ¾e je vítìzem hry
    * @return True, pokud se získání zprávy podaøilo, jinak false.
    */
    bool GetWinInfo();
};

// --------------------------------------------------------------------------
// CONFIGDIALOG
// --------------------------------------------------------------------------

/// Dialog pro konfiguraci hry
class ConfigDialog: public wxDialog
{
private:
    ClientFrame *myParent;
    XmlConfig *xmlConfig;
    wxButton *mButtonSave;
    wxButton *mButtonClose;
    wxTextCtrl *mInputHostname;
    wxSpinCtrl *mSpinPort;
    wxSpinCtrl *mSpinDelay;
    wxStaticText *mTitleHostname;
    wxStaticText *mTitlePort;
    wxStaticText *mTitleLog;
    wxStaticText *mTitleDelay;
    wxCheckBox *mLog;

    DECLARE_EVENT_TABLE()

public:
    /**
    * Konstruktor
    * @param parent Okno, ke kterému je dialog vázán
    * @param xmlConfig Konfiguraèní soubor
    */
    ConfigDialog(ClientFrame *parent, XmlConfig *xmlConfig);

    void OnConfigSave(wxCommandEvent& event);
};

// --------------------------------------------------------------------------
// SELECTPLAYERDIALOG
// --------------------------------------------------------------------------

/// Dialog pro výbìr hráèe z ulo¾eného XML
class SelectPlayerDialog: public wxDialog
{
private:
    ClientFrame *myParent;
    wxListBox *mBoxPlayers;
    wxButton *mButtonOk;
    wxButton *mButtonClose;
    XmlCom *xmlCom;

    DECLARE_EVENT_TABLE()

public:
    /**
    * Konstruktor
    * @param parent Okno, ke kteremu je dialog vázán
    * @param xmlCom Vybraný XML soubor
    */
    SelectPlayerDialog(ClientFrame *parent, XmlCom *xmlCom);

    void OnPlayerSelect(wxCommandEvent& event);
};

// --------------------------------------------------------------------------
// LOGDIALOG
// --------------------------------------------------------------------------

/// Dialog pro pøihlá¹ení u¾ivatele ke vzdálenému serveru
class LogDialog: public wxDialog
{
private:
    ClientFrame *myParent;
    wxTextCtrl *mInputName;
    wxTextCtrl *mInputHostname;
    wxSpinCtrl *mSpinPort;
    wxStaticText *mTitleName;
    wxStaticText *mTitleHostname;
    wxStaticText *mTitlePort;
    wxStaticText *mTitleLog;
    wxButton *mButtonLog;
    wxButton *mButtonRegister;
    wxButton *mButtonClose;
    wxCheckBox *mLogCheckBox;
    XmlConfig *xmlConfig;

    DECLARE_EVENT_TABLE()

public:
    /**
    * Konstruktor
    * @param parent Okno, ke kterému je dialog vázan
    * @param xmlConfig KonfiguraÃ¨nÃ­ soubor
    */
    LogDialog(ClientFrame *parent, XmlConfig *xmlConfig);

    void OnRegister(wxCommandEvent& event);

    void OnLog(wxCommandEvent& event);

    void SaveConfig();
};

#endif // CLIENTFRAME_H

