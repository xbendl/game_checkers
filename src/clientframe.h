/**
* \file clientframe.h
* Okno klientsk� aplikace
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

/// Vlastn� aplikace
class ClientApp : public wxApp
{
    wxLocale appLocale;
    
public:
    virtual bool OnInit();
};

class ClientBoard;

/// Hlavn� okno aplikace
class ClientFrame : public wxFrame
{
    DECLARE_EVENT_TABLE()

    ClientBoard *m_board;
    ClientCom *clientCom;        //< Objekt zaji��uj�c� soketovou komunikaci se serverem
    XmlConfig *xmlConfig;        //< Objekt modifikuj�c� XML s konfigurac� klientsk� aplikace
    XmlCom *xmlCom;              //< Objekt modifikuj�c� XML s ulo�en�mi hrami
    wxIPV4address mAddr;         //< Ur�uje adresu vzd�len�ho serveru
    wxSocketClient *mSock;       //< Socket, ke kter�mu je v�z�no p�ipojen� na vzd�len� server
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
    int mGameId;                 //< Ur�uje ID moment�ln� hran� hry
    bool mLog;                   //< Ur�uje zda je zapnut� logov�n�
    bool mConstructed;
    bool mPlayGame;              //< Ur�uje zda hr�� moment�ln� hraje hru
    bool mOnStep;                //< Ur�uje zda je hr�� na tahu
    bool mConnected;             //< Ur�uje zda je hr�� p�ipojen k serveru
    int mDelayTime;              //< Ur�uje prodlevu mezi kroky p�i plynul�m p�ehr�v�n� hry
    wxString mName;              //< Ur�uje jm�no hr��e
    wxString mOpponentName;      //< Ur�uje jm�no protihr��e
    wxString mOnStepPlayer;      //< Ur�uje jm�no hr��e na tahu
    SockCom::GameVector mOpponentGames; //< Vektor s rozehran�mi hrami s vybran�m protihr��em
    SockCom::StepVector mGameSteps;     //< Vektor s tahy rozehran� hry s vybran�m protihr��em

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
    * Vyvol� p�ihla�ovac� dialog
    * @return True, pokud se u�ivatel �sp�n� p�ihl�sil / registroval, jinak False
    */    
    bool LogUser();
    
    /**
    * Za�le ke vzd�len�mu serveru p�ihla�ovac� �daje
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false
    */
    bool SignToServer();

    /**
    * Za�le ke vzd�len�mu serveru registrovan� jm�no
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool RegisterToServer();
        
    /**
    * Zpracuje p��choz� zpr�vu
    */
    void SocketInput();

public:
    ClientFrame();
    ~ClientFrame();
    
    /**
    * Za�le u�ivatel�m tah protihr��i a z�rove� jej vlo�� do lok�ln�ho XML
    * @param dataVector Vektor se zas�lan�mi pohyby na �achovnici v r�mci tahu
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false
    */
    bool SendStep(SockCom::DataVector &dataVector);

    /**
    * Z�sk� tah od protihr��e a z�rove� jej vlo�� do lok�ln�ho XML
    * @param dataVector Vektor se zas�lan�mi pohyby na �achovnici v r�mci tahu
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false
    */
    bool GetStep(SockCom::DataVector &dataVector);

    /**
    * Za�le protihr��i zpr�vu o jeho v�t�zstv�
    * @return True, pokud se zasl�n� zpr�vy poda�ilo, jinak false.
    */
    bool SendWinInfo();
    
    /**
    * Z�sk� zpr�vu od protihr��e ur�uj�c�, �e je v�t�zem hry
    * @return True, pokud se z�sk�n� zpr�vy poda�ilo, jinak false.
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
    * @param parent Okno, ke kter�mu je dialog v�z�n
    * @param xmlConfig Konfigura�n� soubor
    */
    ConfigDialog(ClientFrame *parent, XmlConfig *xmlConfig);

    void OnConfigSave(wxCommandEvent& event);
};

// --------------------------------------------------------------------------
// SELECTPLAYERDIALOG
// --------------------------------------------------------------------------

/// Dialog pro v�b�r hr��e z ulo�en�ho XML
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
    * @param parent Okno, ke kteremu je dialog v�z�n
    * @param xmlCom Vybran� XML soubor
    */
    SelectPlayerDialog(ClientFrame *parent, XmlCom *xmlCom);

    void OnPlayerSelect(wxCommandEvent& event);
};

// --------------------------------------------------------------------------
// LOGDIALOG
// --------------------------------------------------------------------------

/// Dialog pro p�ihl�en� u�ivatele ke vzd�len�mu serveru
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
    * @param parent Okno, ke kter�mu je dialog v�zan
    * @param xmlConfig Konfiguraèní soubor
    */
    LogDialog(ClientFrame *parent, XmlConfig *xmlConfig);

    void OnRegister(wxCommandEvent& event);

    void OnLog(wxCommandEvent& event);

    void SaveConfig();
};

#endif // CLIENTFRAME_H

