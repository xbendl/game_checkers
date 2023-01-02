// clientframe.h
// Okno klientske aplikace
// Autor: Maxim Kryzhanovskyy (xkryzh00)

#define wxUSE_UNICODE 1

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "clientframe.h"
#include "clientboard.h"
#include "clientgame.h"

#include "xpms/newgame.xpm"
#include "xpms/continuegame.xpm"
#include "xpms/endgame.xpm"
#include "xpms/connect.xpm"
#include "xpms/disconnect.xpm"
#include "xpms/maximize.xpm"
#include "xpms/delete.xpm"
#include "xpms/play.xpm"
#include "xpms/pause.xpm"
#include "xpms/stop.xpm"
#include "xpms/backward.xpm"
#include "xpms/forward.xpm"
#include "xpms/quit.xpm"


IMPLEMENT_APP(ClientApp)

enum
{
    ID_DEBUG,

    // Menu
    ID_QUIT = wxID_EXIT,
    ID_RELOGIN = 1000,
    ID_EXAMPLES,
    ID_CONFIGURATION,
    ID_NEW_GAME_MENU,
    ID_CONT_GAME_MENU,
    ID_END_GAME_MENU,
    ID_CONNECT_MENU,
    ID_DISCONNECT_MENU,
    ID_INIT_PLAY_MENU,
    ID_CLOSE_PLAY_MENU,
    ID_PLAY_MENU,
    ID_PAUSE_MENU,
    ID_STOP_MENU,
    ID_BACK_MENU,
    ID_FORWARD_MENU,
    ID_ABOUT,
    ID_HELP,

    // ConfigDialog
    ID_MODAL,
    ID_CONFIG_SAVE,

    // SelectPlayerDialog
    ID_PLAYER_SELECT,
    ID_LISTBOX_SELECT,

    // LogDialog
    ID_BUTTON_LOG,
    ID_BUTTON_REGISTER,

    // Panely
    ID_OFFL_BOX,

    // Socket
    ID_SOCKET,

    // Listboxy
    ID_ONL_BOX,
    ID_GAME_BOX,
    mID_LISTBOXV,
    mID_LISTBOXH,

    // Toolbar
    ID_NEW_GAME,
    ID_CONT_GAME,
    ID_END_GAME,

    ID_CONNECT,
    ID_DISCONNECT,

    ID_INIT_PLAY,
    ID_CLOSE_PLAY,
    ID_PLAY,
    ID_PAUSE,
    ID_STOP,
    ID_BACK,
    ID_FORWARD,

    // Ostatni
    ID_NOTEBOOK,
    ID_TIMER,

    ID_BOARD
};

bool ClientApp::OnInit()
{
    ClientFrame* frame;
    frame = new ClientFrame();
    SetTopWindow(frame);
    return true;
}

BEGIN_EVENT_TABLE(ClientFrame, wxFrame)
    EVT_TOOL(ID_NEW_GAME, ClientFrame::OnNewGame)
    EVT_TOOL(ID_CONT_GAME, ClientFrame::OnContinueGame)
    EVT_TOOL(ID_END_GAME, ClientFrame::OnEndGame)
    EVT_TOOL(ID_CONNECT, ClientFrame::OnOpenConnection)
    EVT_TOOL(ID_DISCONNECT, ClientFrame::OnCloseConnection)
    EVT_TOOL(ID_INIT_PLAY, ClientFrame::OnInitPlay)
    EVT_TOOL(ID_CLOSE_PLAY, ClientFrame::OnClosePlay)
    EVT_TOOL(ID_PLAY, ClientFrame::OnStartPlay)
    EVT_TOOL(ID_STOP, ClientFrame::OnStopPlay)
    EVT_TOOL(ID_PAUSE, ClientFrame::OnPausePlay)
    EVT_TOOL(ID_BACK, ClientFrame::OnBackPlay)
    EVT_TOOL(ID_FORWARD, ClientFrame::OnForwardPlay)
    EVT_TOOL(ID_QUIT, ClientFrame::OnQuit)
    EVT_SOCKET(ID_SOCKET, ClientFrame::OnSocketEvent)
    EVT_LISTBOX(ID_ONL_BOX, ClientFrame::OnPlayerSelect)
    EVT_LISTBOX(ID_OFFL_BOX, ClientFrame::OnSavedPlayerSelect)
    EVT_LISTBOX(ID_GAME_BOX, ClientFrame::OnGameSelect)
    EVT_LISTBOX_DCLICK(ID_GAME_BOX, ClientFrame::OnInitPlay)
    EVT_MENU(ID_RELOGIN, ClientFrame::OnRelog)
    EVT_MENU(ID_EXAMPLES, ClientFrame::OnFileOpen)
    EVT_MENU(ID_CONFIGURATION, ClientFrame::OnConfig)
    EVT_MENU(ID_NEW_GAME_MENU, ClientFrame::OnNewGame)
    EVT_MENU(ID_CONT_GAME_MENU, ClientFrame::OnContinueGame)
    EVT_MENU(ID_END_GAME_MENU, ClientFrame::OnEndGame)
    EVT_MENU(ID_CONNECT_MENU, ClientFrame::OnOpenConnection)
    EVT_MENU(ID_DISCONNECT_MENU, ClientFrame::OnCloseConnection)
    EVT_MENU(ID_INIT_PLAY_MENU, ClientFrame::OnInitPlay)
    EVT_MENU(ID_CLOSE_PLAY_MENU, ClientFrame::OnClosePlay)
    EVT_MENU(ID_PLAY_MENU, ClientFrame::OnStartPlay)
    EVT_MENU(ID_STOP_MENU, ClientFrame::OnStopPlay)
    EVT_MENU(ID_PAUSE_MENU, ClientFrame::OnPausePlay)
    EVT_MENU(ID_BACK_MENU, ClientFrame::OnBackPlay)
    EVT_MENU(ID_FORWARD_MENU, ClientFrame::OnForwardPlay)
    EVT_MENU(ID_HELP, ClientFrame::OnHelp)
    EVT_MENU(ID_ABOUT, ClientFrame::OnAbout)
    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, ClientFrame::OnNotebookPageChanged)
    EVT_NOTEBOOK_PAGE_CHANGING(ID_NOTEBOOK, ClientFrame::OnNotebookPageChanging)
    EVT_TIMER(ID_TIMER, ClientFrame::OnTimer)
END_EVENT_TABLE()

ClientFrame::ClientFrame() try
    : wxFrame(),
      mTimer(this, ID_TIMER),
      mGameId(-1),
      mPlayGame(false),
      mOnStep(false),
      mConnected(false),
      mConstructed(false),
      mBoxSavedPlayers(NULL),
      mBoxFreePlayers(NULL)
{
    wxString title(_("Dama"));
    wxPoint pos(wxDefaultPosition);
    wxSize size(600,600);
    // Vytvoreni okna
    this->Create(NULL, wxID_ANY, title, pos, size);
    this->SetSizeHints(size, wxDefaultSize);
    // Vytvoreni menu

    mMenu = new wxMenuBar();
    mMenuFile = new wxMenu;
    mMenuFile->Append(ID_RELOGIN, wxT("Zmenit login"),
        wxT("Provest opetovne pripojeni (jiny server, login, ...)"));
    mMenuFile->Append(ID_EXAMPLES, wxT("Archiv"),
        wxT("Vyber souboru s rozehranymi hrami (pro testy)"));
    mMenuFile->Append(ID_CONFIGURATION, wxT("Konfigurace"),
        wxT("Konfigurace programu"));
    mMenuFile->AppendSeparator();
    mMenuFile->Append(wxID_EXIT, wxT("Konec"),
        wxT("Konec programu"));
    mMenu->Append(mMenuFile, wxT("&Soubor"));
    mMenuGame = new wxMenu;
    mMenuGame->Append(ID_NEW_GAME_MENU, wxT("Nova hra"),
        wxT("Nova hra"));
    mMenuGame->Append(ID_CONT_GAME_MENU, wxT("Obnovit hru"),
        wxT("Obnovit hru"));
    mMenuGame->Append(ID_END_GAME_MENU, wxT("Ukoncit hru"),
        wxT("Ukoncit hru"));
    mMenu->Append(mMenuGame, wxT("&Hra"));
    mMenuServer = new wxMenu;
    mMenuServer->Append(ID_CONNECT_MENU, wxT("Pripojit"),
        wxT("Pripojit k serveru"));
    mMenuServer->Append(ID_DISCONNECT_MENU, wxT("Odpojit"),
        wxT("Odpojit od serveru"));
    mMenu->Append(mMenuServer, wxT("&Server"));
    mMenuReplay = new wxMenu;
    mMenuReplay->Append(ID_INIT_PLAY_MENU, wxT("Prohlizet hru"),
        wxT("Spustit prohlizeni hry"));
    mMenuReplay->Append(ID_CLOSE_PLAY_MENU, wxT("Ukoncit prohlizeni"),
        wxT("Ukoncit prohlizeni hry"));
    mMenuReplay->Append(ID_PLAY_MENU, wxT("Sputit krokovani"),
        wxT("Sputit krokovani"));
    mMenuReplay->Append(ID_STOP_MENU, wxT("Zastavit"),
        wxT("Zastavit prehravani"));
    mMenuReplay->Append(ID_PAUSE_MENU, wxT("Pozastavit"),
        wxT("Pozastavi prehravani hry"));
    mMenuReplay->Append(ID_BACK_MENU, wxT("Predchozi"),
        wxT("Predchozi krok"));
    mMenuReplay->Append(ID_FORWARD_MENU, wxT("Nasledujici"),
        wxT("Nasledujici krok"));
    mMenu->Append(mMenuReplay, wxT("&Prehravani"));
    mMenuHelp = new wxMenu;
    mMenuHelp->Append(ID_ABOUT, wxT("O hre"),
        wxT("Zakladni informace o hre"));
    mMenuHelp->Append(ID_HELP, wxT("Napoveda"),
        wxT("Externi napoveda v HTML"));
    mMenu->Append(mMenuHelp, wxT("&Napoveda"));
    this->SetMenuBar(mMenu);
    // Vytvoreni toolbaru
    mToolBar = this->CreateToolBar(wxTB_FLAT, wxID_ANY);
    mToolBar->AddTool(ID_NEW_GAME, _("tool"), wxBitmap(newgame_xpm),
        wxBitmap(newgame_xpm), wxITEM_NORMAL,
        _("Nova hra"), _("Nova hra"));
    mToolBar->AddTool(ID_CONT_GAME, _("tool"), wxBitmap(continuegame_xpm),
        wxBitmap(continuegame_xpm), wxITEM_NORMAL,
        _("Obnovit hru"), _("Obnovit hru"));
    mToolBar->AddTool(ID_END_GAME, _("tool"), wxBitmap(endgame_xpm),
        wxBitmap(endgame_xpm), wxITEM_NORMAL,
        _("Ukoncit hru"), _("Ukoncit hru"));
    mToolBar->AddSeparator();
    mToolBar->AddTool(ID_CONNECT, _("tool"), wxBitmap(connect_xpm),
        wxBitmap(connect_xpm), wxITEM_NORMAL,
        _("Pripojit k serveru"), _("Pripojit k serveru"));
    mToolBar->AddTool(ID_DISCONNECT, _("tool"), wxBitmap(disconnect_xpm),
        wxBitmap(disconnect_xpm), wxITEM_NORMAL,
        _("Odpojit od serveru"), _("Odpojit od serveru"));
    mToolBar->AddSeparator();
    mToolBar->AddTool(ID_INIT_PLAY, _("tool"), wxBitmap(maximize_xpm),
        wxBitmap(maximize_xpm), wxITEM_NORMAL,
        _("Prohlizet hru"), _("Spustit prohlizeni hry"));
    mToolBar->AddTool(ID_CLOSE_PLAY, _("tool"), wxBitmap(delete_xpm),
        wxBitmap(delete_xpm), wxITEM_NORMAL,
        _("Ukoncit prohlizeni hry"), _("Spustit prehravani"));
    mToolBar->AddTool(ID_PLAY, _("tool"), wxBitmap(play_xpm),
        wxBitmap(play_xpm), wxITEM_NORMAL,
        _("Sputit krokovani"), _("Spustit prehravani"));
    mToolBar->AddTool(ID_STOP, _("tool"), wxBitmap(stop_xpm),
        wxBitmap(stop_xpm), wxITEM_NORMAL,
        _("Zastavit"), _("Zastavit prehravani"));
    mToolBar->AddTool(ID_PAUSE, _("tool"), wxBitmap(pause_xpm),
        wxBitmap(pause_xpm), wxITEM_NORMAL,
        _("Pozastavit"), _("Pozastavi prehravani hry"));
    mToolBar->AddTool(ID_BACK, _("tool"), wxBitmap(backward_xpm),
        wxBitmap(backward_xpm), wxITEM_NORMAL,
        _("Predchozi"), _("Na zacatek"));
    mToolBar->AddTool(ID_FORWARD, _("tool"), wxBitmap(forward_xpm),
        wxBitmap(forward_xpm), wxITEM_NORMAL,
        _("Vpred"), _("O krok dopredu"));
    mToolBar->AddSeparator();
    mToolBar->AddTool(wxID_EXIT, _("tool"), wxBitmap(quit_xpm),
        wxBitmap(quit_xpm), wxITEM_NORMAL,
        _("Ukoncit program"), _("Vypnout program"));
    mToolBar->Realize();

    // Prvky typu 'sizer'
    wxBoxSizer* frame_sizer;
    frame_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* top_sizer;
    top_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Spolecny seznam pro prvni a druhou zalozku
    mBoxGameList = new wxListBox(this, ID_GAME_BOX, wxDefaultPosition, wxDefaultSize);
    // Vytvoreni zalozek
    mNotebook = new wxNotebook(this, ID_NOTEBOOK);
    // Prvni zalozka obsahuje seznam lokalne ulozenych her
    wxPanel* offl_panel = new wxPanel(mNotebook, wxID_ANY);
    mBoxSavedPlayers = new wxListBox(offl_panel, ID_OFFL_BOX,
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
    wxBoxSizer* offl_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    offl_panel_sizer->Add(mBoxSavedPlayers, 1, wxALL|wxEXPAND, 0);
    offl_panel->SetSizer(offl_panel_sizer);
    offl_panel->Layout();
    mNotebook->AddPage(offl_panel, wxT("Off-line"), false);
    // Druha zalozka obsahuje seznam hracu pripravenych hrat
    wxPanel* onl_panel = new wxPanel(mNotebook, wxID_ANY);
    mBoxFreePlayers = new wxListBox(onl_panel, ID_ONL_BOX,
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
    wxBoxSizer* onl_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    onl_panel_sizer->Add(mBoxFreePlayers, 1, wxALL|wxEXPAND, 0);
    onl_panel->SetSizer(onl_panel_sizer);
    onl_panel->Layout();
    mNotebook->AddPage(onl_panel, wxT("On-line"), false);
    // Komponovani prvku hlavniho okna
    top_sizer->AddSpacer(10);
    top_sizer->Add(mNotebook, 1, wxALL|wxEXPAND, 0);
    top_sizer->AddSpacer(10);
    m_board = new ClientBoard(this, ID_BOARD);
    m_board->SetMinSize(wxSize(412,412));
    m_board->SetMaxSize(wxSize(412,412));
    top_sizer->Add(m_board, 0, wxALIGN_RIGHT|wxTOP|wxRIGHT, 0);
    frame_sizer->Add(top_sizer, 0, wxTOP|wxRIGHT|wxEXPAND, 10);
    frame_sizer->Add(mBoxGameList, 1, wxALL|wxEXPAND, 10);
    this->SetSizer(frame_sizer);
    this->Layout();

    this->CreateStatusBar(2);

    this->Centre();
    this->Show();

    // Vytvoreni socketu a nastaveni event handleru
    mSock = new wxSocketClient();
    mSock->SetEventHandler(*this, ID_SOCKET);
    mSock->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    mSock->Notify(true);

    //Vytvoreni objektu komunikacniho rozhrani
    clientCom = new ClientCom(mSock);

    // Otevreni konfiguracniho souboru
    xmlConfig = new XmlConfig(_("config_client.xml"));

    // Vytvoreni komunikacnich objektu
    xmlCom = new XmlCom(_("games_client.xml"));

    // Prihlaseni / Registrace uzivatele
    if(!LogUser())
        Close();

    // Ziskani prodlevy pro plynule prehravani
    wxString valueStr;
    xmlConfig->GetParam("REPLAY", "delay" ,valueStr);
    mDelayTime = atoi(valueStr.mb_str()) * 1000;

    mSock->SetEventHandler(*this, ID_SOCKET);
    mSock->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    mSock->Notify(true);

    mConstructed = true;
    UpdateStatusBar();
}
catch(...) {
    std::cerr << "Nastala vyjimka" << std::endl;
}

ClientFrame::~ClientFrame()
{
    delete mSock;
    delete clientCom;
    delete xmlConfig;
    delete xmlCom;
    m_board->Destroy();
    this->Destroy();
}

// --------------------------------------------------------------------------
// EVENT HANDLERY
// --------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Implementace krokovani ulozene hry
//---------------------------------------------------------------------------

// Inicializace modu prehravani
void ClientFrame::OnInitPlay(wxCommandEvent& event)
{
    if(mNotebook->GetSelection() == 0)
    {
        m_board->StartPlay(mOnStep, mGameSteps);

        mToolBar->EnableTool(ID_INIT_PLAY, false);
        mToolBar->EnableTool(ID_CLOSE_PLAY, true);
        mToolBar->EnableTool(ID_PLAY, true);
        mToolBar->EnableTool(ID_STOP, false);
        mToolBar->EnableTool(ID_PAUSE, false);
        mToolBar->EnableTool(ID_BACK, false);
        mToolBar->EnableTool(ID_FORWARD, true);
        mMenuReplay->Enable(ID_INIT_PLAY_MENU, false);
        mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, true);
        mMenuReplay->Enable(ID_PLAY_MENU, true);
        mMenuReplay->Enable(ID_STOP_MENU, false);
        mMenuReplay->Enable(ID_PAUSE_MENU, false);
        mMenuReplay->Enable(ID_BACK_MENU, false);
        mMenuReplay->Enable(ID_FORWARD_MENU, true);
    }
    return;
}
// Ukonceni modu prehravani
void ClientFrame::OnClosePlay(wxCommandEvent& event)
{
    if(mTimer.IsRunning())
        mTimer.Stop();

    m_board->EndPlay();

    mToolBar->EnableTool(ID_INIT_PLAY, true);
    mToolBar->EnableTool(ID_CLOSE_PLAY, false);
    mToolBar->EnableTool(ID_PLAY, false);
    mToolBar->EnableTool(ID_STOP, false);
    mToolBar->EnableTool(ID_PAUSE, false);
    mToolBar->EnableTool(ID_BACK, false);
    mToolBar->EnableTool(ID_FORWARD, false);
    mMenuReplay->Enable(ID_INIT_PLAY_MENU, true);
    mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, false);
    mMenuReplay->Enable(ID_PLAY_MENU, false);
    mMenuReplay->Enable(ID_STOP_MENU, false);
    mMenuReplay->Enable(ID_PAUSE_MENU, false);
    mMenuReplay->Enable(ID_BACK_MENU, false);
    mMenuReplay->Enable(ID_FORWARD_MENU, false);

    return;
}
// Zahajit krokovani v casovych intervalaech
void ClientFrame::OnStartPlay(wxCommandEvent& event)
{
    mTimer.Start(mDelayTime);

    mToolBar->EnableTool(ID_INIT_PLAY, false);
    mToolBar->EnableTool(ID_CLOSE_PLAY, true);
    mToolBar->EnableTool(ID_PLAY, false);
    mToolBar->EnableTool(ID_STOP, true);
    mToolBar->EnableTool(ID_PAUSE, true);
    mToolBar->EnableTool(ID_BACK, false);
    mToolBar->EnableTool(ID_FORWARD, false);
    mMenuReplay->Enable(ID_INIT_PLAY_MENU, false);
    mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, true);
    mMenuReplay->Enable(ID_PLAY_MENU, false);
    mMenuReplay->Enable(ID_STOP_MENU, true);
    mMenuReplay->Enable(ID_PAUSE_MENU, true);
    mMenuReplay->Enable(ID_BACK_MENU, false);
    mMenuReplay->Enable(ID_FORWARD_MENU, false);

}
// Zastavit krokovani v casovych intervalaech
void ClientFrame::OnStopPlay(wxCommandEvent& event)
{
    mTimer.Stop();

    m_board->StopPlay();

    mToolBar->EnableTool(ID_INIT_PLAY, false);
    mToolBar->EnableTool(ID_CLOSE_PLAY, true);
    mToolBar->EnableTool(ID_PLAY, true);
    mToolBar->EnableTool(ID_STOP, false);
    mToolBar->EnableTool(ID_PAUSE, false);
    mToolBar->EnableTool(ID_BACK, false);
    mToolBar->EnableTool(ID_FORWARD, true);
    mMenuReplay->Enable(ID_INIT_PLAY_MENU, false);
    mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, true);
    mMenuReplay->Enable(ID_PLAY_MENU, true);
    mMenuReplay->Enable(ID_STOP_MENU, false);
    mMenuReplay->Enable(ID_PAUSE_MENU, false);
    mMenuReplay->Enable(ID_BACK_MENU, false);
    mMenuReplay->Enable(ID_FORWARD_MENU, true);

    return;
}
// Krok dopredu
void ClientFrame::OnForwardPlay(wxCommandEvent& event)
{
    if(!m_board->PlayFwd())
    {
        mToolBar->EnableTool(ID_FORWARD, false);
        mMenuReplay->Enable(ID_FORWARD_MENU, false);
        mToolBar->EnableTool(ID_PLAY, false);
        mMenuReplay->Enable(ID_PLAY_MENU, false);
    }
    mToolBar->EnableTool(ID_BACK, true);
    mMenuReplay->Enable(ID_BACK_MENU, true);
    return;
}
// Krok zpet
void ClientFrame::OnBackPlay(wxCommandEvent& event)
{
    // Zobrazeni sachovnice
    mToolBar->EnableTool(ID_FORWARD, true);
    mMenuReplay->Enable(ID_FORWARD_MENU, true);
        mToolBar->EnableTool(ID_PLAY, true);
        mMenuReplay->Enable(ID_PLAY_MENU, true);
    if(!m_board->PlayBack())
    {
        mToolBar->EnableTool(ID_BACK, false);
        mMenuReplay->Enable(ID_BACK_MENU, false);
    }
    return;
}
// Pozastavit krokovani v casovych intervalaech
void ClientFrame::OnPausePlay(wxCommandEvent& event)
{
    mTimer.Stop();

    mToolBar->EnableTool(ID_INIT_PLAY, false);
    mToolBar->EnableTool(ID_CLOSE_PLAY, true);
    mToolBar->EnableTool(ID_PLAY, true);
    mToolBar->EnableTool(ID_STOP, true);
    mToolBar->EnableTool(ID_PAUSE, false);
    mToolBar->EnableTool(ID_BACK, false);
    mToolBar->EnableTool(ID_FORWARD, false);
    mMenuReplay->Enable(ID_INIT_PLAY_MENU, false);
    mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, true);
    mMenuReplay->Enable(ID_PLAY_MENU, true);
    mMenuReplay->Enable(ID_STOP_MENU, true);
    mMenuReplay->Enable(ID_PAUSE_MENU, false);
    mMenuReplay->Enable(ID_BACK_MENU, false);
    mMenuReplay->Enable(ID_FORWARD_MENU, false);

    return;
}
// Casovana udalost - krok dopredu
void ClientFrame::OnTimer(wxTimerEvent& event)
{
    if(!(m_board->PlayFwd())) {
        SetStatusText(_("Hra byla prehrana."), 1);
        mTimer.Stop();
        m_board->StopPlay();
        UpdateStatusBar();

        mToolBar->EnableTool(ID_INIT_PLAY, false);
        mToolBar->EnableTool(ID_CLOSE_PLAY, true);
        mToolBar->EnableTool(ID_PLAY, true);
        mToolBar->EnableTool(ID_STOP, false);
        mToolBar->EnableTool(ID_PAUSE, false);
        mToolBar->EnableTool(ID_BACK, false);
        mToolBar->EnableTool(ID_FORWARD, true);
        mMenuReplay->Enable(ID_INIT_PLAY_MENU, false);
        mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, true);
        mMenuReplay->Enable(ID_PLAY_MENU, true);
        mMenuReplay->Enable(ID_STOP_MENU, false);
        mMenuReplay->Enable(ID_PAUSE_MENU, false);
        mMenuReplay->Enable(ID_BACK_MENU, false);
        mMenuReplay->Enable(ID_FORWARD_MENU, true);
    }
}

void ClientFrame::OnNotebookPageChanged(wxNotebookEvent& event)
{
    int sel;
    if(mBoxSavedPlayers)
    {
        sel = mBoxSavedPlayers->GetSelection();
        if(sel >= 0)
            mBoxSavedPlayers->Deselect(sel);
    }
    if(mBoxFreePlayers)
    {
        sel = mBoxFreePlayers->GetSelection();
        if(sel >= 0)
            mBoxFreePlayers->Deselect(sel);
    }
    if(mConstructed && (mNotebook->GetSelection() == 0))
    {
        FillBox();
    }

    mBoxGameList->Clear();

    if(mConstructed)
        UpdateStatusBar();

    return;
}

void ClientFrame::OnNotebookPageChanging(wxNotebookEvent& event)
{
    if(mConstructed && mPlayGame)
    {
        wxMessageBox(_("Behem hrani hry nesmite prechazet k ulozenym hram v offline modu."), _("Upozorneni"), wxICON_ERROR);
        event.Veto();
    }
    if(!mConnected && (mNotebook->GetSelection() == 0))
    {
        wxMessageBox(_("Nejprve se musite pripojit k serveru."), _("Upozorneni"), wxICON_ERROR);
        event.Veto();
    }

    if(mConstructed)
    {
        if(mTimer.IsRunning())
            mTimer.Stop();

        m_board->EndPlay();
    }

    return;
}

// Ukonci program
void ClientFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    int answer = wxMessageBox(_("Opravdu chcete ukoncit program?"), _("Ukonceni programu"), wxYES_NO, this, wxICON_QUESTION);
    if(answer == wxYES)
        Close(true);
}

// Vybere protihrace a zasle zadost o seznam her s vybranym protihracem
void ClientFrame::OnPlayerSelect(wxCommandEvent& event)
{
    if(mBoxFreePlayers->GetSelection() >= 0)
    {
        mOpponentName = wxString(mBoxFreePlayers->GetString(mBoxFreePlayers->GetSelection()));
        if(!clientCom->SendAskGameList(mOpponentName))
        {
            wxMessageBox(_("Zaslani zadosti o rozehrane hry se nepodarilo."), _("Chyba"), wxICON_ERROR);
        }

        mGameId = -1;
    }

    UpdateStatusBar();
}

// Vybere protihrace a zasle zadost o seznam her s vybranym protihracem
void ClientFrame::OnSavedPlayerSelect(wxCommandEvent& event)
{
    // Naplneni vektoru slouziciho pro ulozeni rozehranych her s vybranym protihracem
    if(mBoxSavedPlayers->GetSelection() >= 0)
    {
        mOpponentGames.clear();
        mOpponentName = wxString(mBoxSavedPlayers->GetString(mBoxSavedPlayers->GetSelection()));
        std::string challengerName = std::string(mName.mb_str());
        std::string opponentName = std::string(mOpponentName.mb_str());
        if(!xmlCom->FindOpponentGames(challengerName, opponentName, mOpponentGames))
            wxMessageBox(_("Zadost o lokalni rozehrane hry s vybranym protihracem se nepodarila."), _("Chyba"), wxICON_ERROR);
        else
        {
            mBoxGameList->Clear();
            for(int i = 0; i < mOpponentGames.size(); i++)
            {   // Zobrazeni seznamu rozehranych her s danym hracem
                wxDateTime date(mOpponentGames[i].date);
                wxString datestring = date.Format(_("%d.%m.%Y, %H:%M"), wxDateTime::CET);
                mBoxGameList->Append(datestring);
            }
        }
        mGameId = -1;
    }
    UpdateStatusBar();
}

// Vybere hru a zasle zadost o seznam tahu k vybrane hre
void ClientFrame::OnGameSelect(wxCommandEvent& event)
{
    if(mBoxFreePlayers->GetSelection() != wxNOT_FOUND)
    {
        if(mBoxGameList->GetSelection() >= 0)
        {
            mGameId = mOpponentGames[mBoxGameList->GetSelection()].gameId;
            if(!clientCom->SendAskGameSteps(mGameId))
                wxMessageBox(_("Zaslani zadosti o tahy k rozehrane hre se nepodarilo."), _("Chyba"), wxICON_ERROR);
        }
    }
    else
    {
        if(mTimer.IsRunning())
            mTimer.Stop();

        m_board->EndPlay();

        mToolBar->EnableTool(ID_INIT_PLAY, true);
        mToolBar->EnableTool(ID_CLOSE_PLAY, false);
        mToolBar->EnableTool(ID_PLAY, false);
        mToolBar->EnableTool(ID_STOP, false);
        mToolBar->EnableTool(ID_PAUSE, false);
        mToolBar->EnableTool(ID_BACK, false);
        mToolBar->EnableTool(ID_FORWARD, false);
        mMenuReplay->Enable(ID_INIT_PLAY_MENU, true);
        mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, false);
        mMenuReplay->Enable(ID_PLAY_MENU, false);
        mMenuReplay->Enable(ID_STOP_MENU, false);
        mMenuReplay->Enable(ID_PAUSE_MENU, false);
        mMenuReplay->Enable(ID_BACK_MENU, false);
        mMenuReplay->Enable(ID_FORWARD_MENU, false);

        mGameSteps.clear();
        if(mBoxGameList->GetSelection() >= 0)
        {
            mGameSteps.clear();  // Seznam tahu rozehrane hry musi byt pred novym plnenim prazdny
            mGameId = mOpponentGames[mBoxGameList->GetSelection()].gameId;
            mOnStep = xmlCom->FindGameSteps(mGameId, mGameSteps, std::string(mName.mb_str()));

            UpdateStatusBar();
        }
    }
    UpdateStatusBar();
}

bool ClientFrame::LogUser()
{
    int returnValue;
    unsigned char jumper;
    PrepareToRelog();

    LogDialog *logDialog = new LogDialog(this, xmlConfig);
    while(true)
    {
        returnValue = logDialog->ShowModal();
        if(returnValue == ID_BUTTON_LOG)
        {
            if(!SignToServer())
            {
                wxMessageBox(_("Prihlaseni neprobehlo uspesne. Server je nedostupny."), _("Neuspesne prihlaseni"), wxICON_ERROR);
                continue;
            }
            break;
        }
        else if(returnValue == ID_BUTTON_REGISTER)
        {
            if(!RegisterToServer())
            {
                wxMessageBox(_("Registrace neprobehla uspesne. Server je nedostupny."), _("Neuspesna registrace"), wxICON_ERROR);
                continue;
            }
            break;
        }
        else
        {
            delete logDialog;
            FillBox();
            return false;
        }
    }
    FillBox();

    delete logDialog;
    return true;
}

// Prihlasi hrace na server
bool ClientFrame::SignToServer()
{
    // Pripojeni k serveru
    if(!clientCom->ConnectToServer(mAddr))
        return false;

    // Zaslani pozadavku k prihlaseni
    if(!clientCom->SignToServer(mName))
        return false;

    UpdateStatusBar();

    return true;
}

// Prihlasi hrace na server
void ClientFrame::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
    clientCom->SendVisibility(true);
    mConnected = true;
    mNotebook->SetSelection(1);
    UpdateStatusBar();
}

// Registruje hrace na server
bool ClientFrame::RegisterToServer()
{
    // Pripojeni k serveru
    if(!clientCom->ConnectToServer(mAddr))
        return false;

    // Zaslani pozadavku k registraci
    if(!clientCom->RegisterToServer(mName))
        return false;

    return true;
}

// Uzavre spojeni se serverem
void ClientFrame::OnCloseConnection(wxCommandEvent& WXUNUSED(event))
{
    mNotebook->SetSelection(0);
    clientCom->SendVisibility(false);
    mConnected = false;
    UpdateStatusBar();
}

// Zobrazi okno pro zadani
void ClientFrame::OnConfig(wxCommandEvent& WXUNUSED(event))
{
    ConfigDialog *configDialog = new ConfigDialog(this, xmlConfig);
    configDialog->ShowModal();
    delete configDialog;
}

// Vytvori novou hru
void ClientFrame::OnNewGame(wxCommandEvent& WXUNUSED(event))
{
    mGameSteps.clear();
    int selection = mBoxFreePlayers->GetSelection();
    if(selection != -1)
    {   // Musi byt oznaceny hrac, s kterym bude hra probihat
        wxString selectName = mBoxFreePlayers->GetString(selection);
        mGameId = -1;
        time_t date = 0;

        // Zaslani pozadavku o vytvoreni nove hry
        if(!clientCom->SendAskPlayGame(mName, selectName, mGameId, date, false))
        {
            wxMessageBox(_("Zaslani zadosti o novou hru se nepodarilo."), _("Chyba"), wxICON_ERROR);
        }
        else
        {
            mBoxGameList->Clear();
        }
    }
    UpdateStatusBar();
}

// Obnovi rozehranou hru
void ClientFrame::OnContinueGame(wxCommandEvent& WXUNUSED(event))
{
    int selection = mBoxGameList->GetSelection();
    if(selection != -1)
    {   // Musi byt oznacena hra, ktera bude dohravana
        time_t date = mOpponentGames[selection].date;

        // Nastaveni hrace na tahu
        mOnStepPlayer = wxString(wxString::FromAscii(mOpponentGames[selection].onStepPlayer));
        bool onStep = false;
        if((mOnStepPlayer != mName))
        {   // Pokud slo o rozehranou hru, musim o hraci na tahu rozhodnout podle posledniho stavu hry
            onStep = true;
        }

        if(!clientCom->SendAskPlayGame(mName, mOpponentName, mGameId, date, onStep))
        {
            wxMessageBox(_("Zaslani zadosti o dohrani rozehrane hry se nepodarilo."), _("Chyba"), wxICON_ERROR);
        }
        else
        {
            mBoxGameList->Clear();
        }
    }
    UpdateStatusBar();
}

// Ukonci aktualne hranou hru
void ClientFrame::OnEndGame(wxCommandEvent& WXUNUSED(event))
{
    int answer = wxMessageBox(_("Opravdu chcete ukoncit hru?"), _("Ukonceni hry"), wxYES_NO, this, wxICON_QUESTION);
    if(answer == wxNO)
        return;

    if(!clientCom->SendEndGame(mOpponentName, mGameId))
    {
        wxMessageBox(_("Zaslani zpravy o ukonceni hry se nepodarilo."), _("Chyba"), wxICON_ERROR);
    }

    // Nastaveni ridicich promennych
    mGameId = -1;
    mPlayGame = false;
    mOnStep = false;

    mGameSteps.clear();
    m_board->EndGame();

    UpdateStatusBar();
}

// Obsluha udalosti na soketu
void ClientFrame::OnSocketEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {   // Rozhodnuti podle typu udalosti
        case wxSOCKET_INPUT:
            SocketInput();
            break;
        case wxSOCKET_LOST:
            break;
        case wxSOCKET_CONNECTION:
            break;
        default:
            break;
    }
    UpdateStatusBar();
}

// Obsluha udalosti na soketu: Na soketu jsou nova data
void ClientFrame::SocketInput()
{
    mSock->SetNotify(wxSOCKET_LOST_FLAG);    // Behem vyrizovani udalosti prijimam pouze vyjimku oznacujici uzavreni spojeni se serverem

    unsigned char jumper;
    mSock->Read(&jumper, sizeof(jumper));    // Prvni byte urcuje typ zpravy
    switch(jumper)
    { // Rozhodnuti podle typu zpravy
        case SockCom::SERV_SIGNED:           // Pozitivni odpoved na pozadavek o prihlaseni
        {
            break;
        }
        case SockCom::SERV_UNSIGNED:         // Negativni odpoved na pozadavek o prihlaseni
        {
            clientCom->DisconnectFromServer();
            wxMessageBox(_("Prihlaseni neprobehlo uspesne. Ucet s danym jmenem na serveru pravdepodobne neexistuje."), _("Neuspesne prihlaseni"), wxICON_ERROR);
            if(!LogUser())
                Close();
            break;
        }
        case SockCom::SERV_ASK_GAME:         // Pozadavek o hru od protihrace
        {
            int gameId;
            time_t date;
            wxString challengerName;

            if(!clientCom->GetAskPlayGame(challengerName, gameId, date))
            {   // Test uspesnosti prijmuti pozadavku o hru
                wxMessageBox(_("Ziskani pozadavku o hru se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }

            // Rozhodovaci formular ohledne prijeti zadosti o novou hru
            bool answer = false;
            wxString question = wxString(_("Chcete hrat novou hru s uzivatelem ")) + challengerName + wxString(_("?"));
            int answerWx = wxMessageBox(question, _("Vyzva ke hre"), wxYES_NO, this, wxICON_QUESTION);
            if(answerWx == wxYES)
                answer = true;

            if(answer)
            {   // Pokud uzivatel odsouhlasil zadost o hru
                mNotebook->SetSelection(1);
                mGameId = gameId;
                mPlayGame = true;
                mOpponentName = challengerName;

                m_board ->NewGame("BLACK", "WHITE");
            }

            // Zaslani odpovedi na pozadavek o hru
            if(!clientCom->SendAnswerPlayGame(answer, challengerName, mGameId))
            {
                wxMessageBox(_("Zaslani odpovedi na pozadavek o hru hraci se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }

            break;
        }
        case SockCom::SERV_ASK_CONT_GAME:   // Pozadavek o dohrani rozehrane hry od protihrace
        {
            // Ziskani pozadavku o dohrani rozehrane hry
            int gameId;
            time_t date;
            wxString challengerName;
            std::string challengerNameAscii;
            bool onStep;
            mGameSteps.clear();

            if(!clientCom->GetAskContGame(challengerName, gameId, date, mGameSteps, onStep))
            {
                wxMessageBox(_("Ziskani pozadavku o dohrani rozehrane hry se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }

            // Rozhodovaci formular ohledne prijeti zadosti o dohrani rozehrane hry
            bool answer = false;
            wxDateTime dateWx(date);
            wxString datestring = dateWx.Format(_("%d.%m.%Y, %H:%M"), wxDateTime::CET);
            wxString question = wxString(_("Chcete dohrat hru s uzivatelem ")) + challengerName + wxString(_("? (naposledy hrana ")) + datestring + wxString(_(")"));
            int answerWx = wxMessageBox(question, _("Vyzva ke hre"), wxYES_NO, this, wxICON_QUESTION);
            if(answerWx == wxYES)
                answer = true;

            if(answer)
            {   // Pokud uzivatel odsouhlasil zadost o hru
                mNotebook->SetSelection(1);
                mGameId = gameId;
                mPlayGame = true;
                mOpponentName = challengerName;
                mOnStep = onStep;

                // Zobrazeni sachovnice
                SockCom::DataVector helpVector;
                SockCom::MovData movData;
                if((mOnStep && (mGameSteps.size() % 2 == 0)) || (!mOnStep && (mGameSteps.size() % 2 == 1)))
                {
                    m_board->NewGame("WHITE", "BLACK");
                    for(int i = 0; i < mGameSteps.size(); i++)
                    {
                        if((i % 2) == 0)
                        {
                            for(int j = 0; j < mGameSteps[i].size(); j++)
                            {
                                movData = 31 - mGameSteps[i][j];
                                helpVector.push_back(movData);
                            }
                            m_board->MoveChecker(helpVector);
                            helpVector.clear();
                        }
                        else
                            m_board->MoveChecker(mGameSteps[i]);
                    }
                }
                else
                {
                    m_board->NewGame("BLACK", "WHITE");
                    for(int i = 0; i < mGameSteps.size(); i++)
                    {
                        if((i % 2) == 1)
                        {
                            for(int j = 0; j < mGameSteps[i].size(); j++)
                            {
                                movData = 31 - mGameSteps[i][j];
                                helpVector.push_back(movData);
                            }
                            m_board->MoveChecker(helpVector);
                            helpVector.clear();
                        }
                        else
                            m_board->MoveChecker(mGameSteps[i]);
                    }
                }

                // Vlozeni zaznamu o hre do XML
                if(mLog)
                {   // Je-li zapnute logovani
                    xmlCom->DeleteGame(mGameId);  // Hru smazu
                    if((mOnStep && ((mGameSteps.size() % 2) == 0)) || (!mOnStep && ((mGameSteps.size() % 2) == 1)))
                        xmlCom->InsertClientGame(mName, mOpponentName, mGameId);  // Hru znovu vytvorim (nema zadne tahy)
                    else
                        xmlCom->InsertClientGame(mOpponentName, mName, mGameId);

                    for(int i = 0; i < mGameSteps.size(); i++)  // Aplikuji vsechny tahy
                    {
                        xmlCom->InsertStep(gameId, mGameSteps[i]);
                    }
                }
            }

            // Zaslani odpovedi na pozadavek o dohrani rozehrane hry
            if(!clientCom->SendAnswerPlayGame(answer, challengerName, mGameId))
            {
                wxMessageBox(_("Ziskani pozadavku o dohrani rozehrane hry se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }

            break;
        }
        case SockCom::SERV_ACCEPT_GAME:         // Pozitivni odpoved na pozadavek o hru od protihrace
        {
            int gameId = mGameId;

            if(!clientCom->GetAnswerPlayGame(gameId))
            {
                wxMessageBox(_("Ziskani pozitivni odpovedi na pozadavek o hru se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                // Nastaveni hrace na tahu
                if((mGameId != -1) && (mOnStepPlayer == mName))
                {   // Pokud slo o rozehranou hru, musim o hraci na tahu rozhodnout podle toho, kdo byl na tahu naposledy
                    mOnStep = true;
                }
                else if(mGameId == -1)
                {   // V pripade nove hry je na tahu vzdy vyzyvatel
                    mOnStep = true;
                }
                mPlayGame = true;

                if(mLog)
                {   // Je-li zapnute logovani
                    if(mGameId != -1)
                    {   // Jedna-li se o rozehranou hru
                        mGameId = gameId;
                        xmlCom->DeleteGame(gameId);  // Hru smazu
                        xmlCom->DeleteGame(mGameId);  // Hru smazu
                        if((mOnStep && ((mGameSteps.size() % 2) == 0)) || (!mOnStep && ((mGameSteps.size() % 2) == 1)))
                            xmlCom->InsertClientGame(mName, mOpponentName, mGameId);  // Hru znovu vytvorim (nema zadne tahy)
                        else
                            xmlCom->InsertClientGame(mOpponentName, mName, mGameId);

                        // Aktualizace zaznamu o hre v XML
                        for(int i = 0; i < mGameSteps.size(); i++)  // Aplikuji vsechny tahy
                        {
                            xmlCom->InsertStep(gameId, mGameSteps[i]);
                        }

                    }
                    else
                    {   // Jedna-li se o novou hru
                        mGameId = gameId;
                        xmlCom->InsertClientGame(mName, mOpponentName, mGameId);
                    }
                }

                // Zobrazeni sachovnice
                SockCom::DataVector helpVector;
                SockCom::MovData movData;
                if((mOnStep && (mGameSteps.size() % 2 == 1)) || (!mOnStep && (mGameSteps.size() % 2 == 0)))
                {
                    m_board->NewGame("BLACK", "WHITE");
                    for(int i = 0; i < mGameSteps.size(); i++)
                    {
                        if((i % 2) == 1)
                        {
                            for(int j = 0; j < mGameSteps[i].size(); j++)
                            {
                                movData = 31 - mGameSteps[i][j];
                                helpVector.push_back(movData);
                            }
                            m_board->MoveChecker(helpVector);
                            helpVector.clear();
                        }
                        else
                            m_board->MoveChecker(mGameSteps[i]);
                    }
                }
                else
                {
                    m_board->NewGame("WHITE", "BLACK");
                    for(int i = 0; i < mGameSteps.size(); i++)
                    {
                        if((i % 2) == 0)
                        {
                            for(int j = 0; j < mGameSteps[i].size(); j++)
                            {
                                movData = 31 - mGameSteps[i][j];
                                helpVector.push_back(movData);
                             }
                             m_board->MoveChecker(helpVector);
                             helpVector.clear();
                        }
                        else
                            m_board->MoveChecker(mGameSteps[i]);
                    }
                }
                mGameId = gameId;
            }
            break;
        }
        case SockCom::SERV_REJECT_GAME:         // Negativni odpoved na pozadavek o hru od protihrace
        {
            if(!clientCom->GetAnswerPlayGame(mGameId))
            {
                wxMessageBox(_("Ziskani negativni odpovedi na pozadavek o hru se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            break;
        }
        case SockCom::SERV_SEND_GAME_ID:        // Prijmuti ID pro vytvoreni noveho zaznamu v tabulce GAMES v XML
        {
            if(!clientCom->GetGameId(mGameId))
            {
                wxMessageBox(_("Ziskani ID pro vytvoreni noveho zaznamu se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else if(mLog)
            {   // Vlozeni hry do tabulky her
                 xmlCom->InsertClientGame(mOpponentName, mName, mGameId);
            }
            break;
        }
        case SockCom::SERV_END_GAME:            // Prijmuti zpravy o ukonceni hry
        {
            if(!clientCom->GetEndGame(mGameId))
            {
                wxMessageBox(_("Prijmuti zpravy o ukonceni hry se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                m_board->EndGame();

                wxMessageBox(_("Protihrac ukoncil hru. Hra zustava ulozena na serveru a bude mozne ji pozdeji dohrat."),
                    _("Ukonceni hry"), wxICON_INFORMATION);

                mPlayGame = false;
                mOnStep = false;
                mGameId = -1;
            }
            break;
        }
        case SockCom::SERV_SEND_STEP:           // Prijmuti tahu od protihrace
        {
            wxString opponentName;
            SockCom::DataVector dataVector;
            bool finished;

            // Zaslani tahu protihraci a vlozeni do XML
            if(!GetStep(dataVector))
            {
                wxMessageBox(_("Prijmuti tahu od  protihraci se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {   // Vlozeni tahu na sachovnici
                m_board->MoveChecker(dataVector);

                if(!mPlayGame)
                {
                    m_board->EndGame();
                }
                mOnStep = true;
            }

            break;
        }
        case SockCom::SERV_SEND_WIN:            // Prijmuti tahu od protihrace
        {
            if(!clientCom->GetWinInfo(mGameId))
            {
                wxMessageBox(_("Prijmuti zpravy o  vyhre se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                m_board->EndGame();

                mBoxFreePlayers->Append(mOpponentName);
                wxMessageBox(_("Vyhral jste. Hra bude nyni ukoncena"), _("Ukonceni hry"), wxICON_INFORMATION);

                mPlayGame = false;
                mOnStep = false;
                mGameId = -1;
            }

            break;
        }
        case SockCom::SERV_GET_PLAYER_LIST:     // Prijmuti seznamu volnych hracu
        {
            SockCom::StringVector freePlayers;

            if(!clientCom->GetFreePlayers(freePlayers))
            {
                wxMessageBox(_("Ziskani seznamu volnych hracu se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                for(int i = 0; i < freePlayers.size(); i++)
                {   // Zobrazeni volnych hracu
                    mBoxFreePlayers->Append(wxString(wxString::FromAscii(freePlayers.at(i).c_str())));
                }
            }
            break;
        }
        case SockCom::SERV_INSERT_PLAYER:       // Prijmuti jmena nove prihlaseneho hrace
        {
            std::string insertPlayerAscii;
            wxString insertPlayer;

            if(!clientCom->GetInsertName(insertPlayer))
            {
                wxMessageBox(_("Vlozeni noveho jmena do seznamu volnych hracu se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                // Pridani hrace do boxu s volnymi hraci
                int i;
                for(i = 0; i < mBoxFreePlayers->GetCount(); i++)
                {
                    if(insertPlayer < mBoxFreePlayers->GetString(i))
                    {   // Nalezeni pozice kam abecedne nove jmeno patri
                        break;
                    }
                }
                mBoxFreePlayers->Insert(insertPlayer, i);  // Vlozeni jmena do boxu s volnymi hraci
            }
            break;
        }
        case SockCom::SERV_DELETE_PLAYER:       // Prijmuti jmena nove odhlaseneho hrace
        {   // Odstraneni zaslaneho jmena uzivatele ze seznamu volnych hracu
            wxString deletePlayer;

            if(!clientCom->GetDeleteName(deletePlayer))
            {
                wxMessageBox(_("Odstraneni uzivatele ze seznamu volnych hracu se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                // Odstraneni hrace z boxu s volnymi hraci
                for(int i = 0; i < mBoxFreePlayers->GetCount(); i++)
                {
                    if(deletePlayer == mBoxFreePlayers->GetString(i))
                    {   // Nalezeni pozice jmena hrace, ktery ma byt odstranen
                        mBoxFreePlayers->Delete(i);  // Vyjmuti jmena z boxu s volnymi hraci
                        break;
                    }
                }
                if(mPlayGame && mOpponentName == deletePlayer)
                {   // Pokud uzivatel hral hru s timto protihracem v okamziku odpojeni protihrace
                    mPlayGame = false;
                    mOnStep = false;
                    mGameId = -1;
                }
            }
            break;
        }
        case SockCom::SERV_GAME_LIST:           // Prijmuti seznamu rozehranych her s vybranym protihracem
        {
            mBoxGameList->Clear();
            mOpponentGames.clear();  // Seznam rozehranych her musi byt pred novym plnenim prazdny

            if(!clientCom->GetGameList(mOpponentGames))
            {
                wxMessageBox(_("Ziskani seznamu rozehranych her s vybranym protihracem se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }
            else
            {
                for(int i = 0; i < mOpponentGames.size(); i++)
                {   // Zobrazeni seznamu rozehranych her hracu
                    wxDateTime date(mOpponentGames[i].date);
                    wxString datestring = date.Format(_("%d.%m.%Y, %H:%M"), wxDateTime::CET);
                    mBoxGameList->Append(datestring);
                }
            }
            break;
        }
        case SockCom::SERV_GAME_STEPS:          // Prijmuti seznamu tahu k rozehrane hre
        {
            mGameSteps.clear();  // Seznam tahu rozehrane hry musi byt pred novym plnenim prazdny

            if(!clientCom->GetGameSteps(mGameSteps))
            {
                wxMessageBox(_("Ziskani seznamu tahu k rozehrane hre se nepodarilo."), _("Chyba"), wxICON_ERROR);
            }

            break;
        }
        case SockCom::SERV_REGISTERED:       // Pozitivni odpoved na pozadavek k registraci
        {
            break;
        }
        case SockCom::SERV_NOT_REGISTERED:   // Negativni odpoved na pozadavek k registraci
        {
            clientCom->DisconnectFromServer();
            wxMessageBox(_("Registrace neprobehla uspesne. Ucet s danym jmenem na serveru pravdepodobne jiz existuje."), _("Neuspesna registrace"), wxICON_ERROR);
            if(!LogUser())
                Close();
            break;
        }
        default:
            break;
    }

    UpdateStatusBar();
    mSock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
}

// --------------------------------------------------------------------------
// UZIVATELSKE FUNKCE
// --------------------------------------------------------------------------

bool ClientFrame::SendStep(SockCom::DataVector &dataVector)
{
    // Zaslani tahu protihraci
    if(!clientCom->SendStep(mGameId, dataVector))
        return false;

    // Vlozeni tahu do XML
    if(mLog)
        if(!xmlCom->InsertStep(mGameId, dataVector))
            return false;

    dataVector.clear();
    mOnStep = false;
    UpdateStatusBar();

    return true;
}

bool ClientFrame::GetStep(SockCom::DataVector &dataVector)
{
    // Zaslani tahu protihraci
    if(!clientCom->GetStep(mGameId, dataVector))
        return false;


    // Vlozeni tahu do XML
    if(mLog)
        if(!xmlCom->InsertStep(mGameId, dataVector))
            return false;

    return true;
}

bool ClientFrame::SendWinInfo()
{
    if(!clientCom->SendWinInfo(mGameId))
    {
        return false;
    }

    mBoxFreePlayers->Append(mOpponentName);
    mPlayGame = false;
    mOnStep = false;
    mGameId = -1;
    UpdateStatusBar();
    wxMessageBox(_("Prohral jste. Hra bude nyni ukoncena. "), _("Upozorneni"), wxICON_INFORMATION);

    return true;
}

bool ClientFrame::GetWinInfo()
{
    // Zaslani tahu protihraci
    if(!clientCom->GetWinInfo(mGameId))
        return false;

    UpdateStatusBar();

    return true;
}

void ClientFrame::UpdateStatusBar()
{
    if(mConnected)
    {
        mToolBar->EnableTool(ID_CONNECT, false);
        mToolBar->EnableTool(ID_DISCONNECT, true);
        mMenuServer->Enable(ID_CONNECT_MENU, false);
        mMenuServer->Enable(ID_DISCONNECT_MENU, true);
    }
    else
    {
        mToolBar->EnableTool(ID_CONNECT, true);
        mToolBar->EnableTool(ID_DISCONNECT, false);
        mMenuServer->Enable(ID_CONNECT_MENU, true);
        mMenuServer->Enable(ID_DISCONNECT_MENU, false);
    }

    // Zobrazeni moznosti znovupripojeni v menu
    if(mPlayGame)
    {
        mMenuFile->Enable(ID_RELOGIN, false);
        mMenuFile->Enable(ID_EXAMPLES, false);
    }
    else
    {
        mMenuFile->Enable(ID_RELOGIN, true);
        mMenuFile->Enable(ID_EXAMPLES, true);
    }

    // Nastaveni vyberoveho boxu s rozehranymi hrami pri aktivnim listboxu SavedPlayers
    if((mBoxSavedPlayers->GetSelection() != wxNOT_FOUND) && (mBoxGameList->GetSelection() != wxNOT_FOUND))
    {
        mToolBar->EnableTool(ID_NEW_GAME, false);
        mToolBar->EnableTool(ID_CONT_GAME, false);
        mToolBar->EnableTool(ID_END_GAME, false);
        mToolBar->EnableTool(ID_INIT_PLAY, true);
        mMenuGame->Enable(ID_NEW_GAME_MENU, false);
        mMenuGame->Enable(ID_CONT_GAME_MENU, false);
        mMenuGame->Enable(ID_END_GAME_MENU, false);
        mMenuReplay->Enable(ID_INIT_PLAY_MENU, true);
    }
    else
    {
        mToolBar->EnableTool(ID_INIT_PLAY, false);
        mToolBar->EnableTool(ID_CLOSE_PLAY, false);
        mToolBar->EnableTool(ID_PLAY, false);
        mToolBar->EnableTool(ID_STOP, false);
        mToolBar->EnableTool(ID_PAUSE, false);
        mToolBar->EnableTool(ID_BACK, false);
        mToolBar->EnableTool(ID_FORWARD, false);
        mMenuReplay->Enable(ID_INIT_PLAY_MENU, false);
        mMenuReplay->Enable(ID_CLOSE_PLAY_MENU, false);
        mMenuReplay->Enable(ID_PLAY_MENU, false);
        mMenuReplay->Enable(ID_STOP_MENU, false);
        mMenuReplay->Enable(ID_PAUSE_MENU, false);
        mMenuReplay->Enable(ID_BACK_MENU, false);
        mMenuReplay->Enable(ID_FORWARD_MENU, false);
    }

    // Nastaveni vyberoveho boxu s rozehranymi hrami pri aktivnim listboxu FreePlayers
    if((mBoxFreePlayers->GetSelection() != wxNOT_FOUND))
    {
        mToolBar->EnableTool(ID_NEW_GAME, true);
        mMenuGame->Enable(ID_NEW_GAME_MENU, true);
        if(mBoxGameList->GetSelection() != wxNOT_FOUND)
        {
            mToolBar->EnableTool(ID_CONT_GAME, true);
            mMenuGame->Enable(ID_CONT_GAME_MENU, true);
        }
    }
    else
    {
        mToolBar->EnableTool(ID_NEW_GAME, false);
        mToolBar->EnableTool(ID_CONT_GAME, false);
        mToolBar->EnableTool(ID_END_GAME, false);
        mMenuGame->Enable(ID_NEW_GAME_MENU, false);
        mMenuGame->Enable(ID_CONT_GAME_MENU, false);
        mMenuGame->Enable(ID_END_GAME_MENU, false);
    }

    // Nastaveni tlacitka "Odpojit od serveru"
    if(mConnected && !mPlayGame) {
        mToolBar->EnableTool(ID_DISCONNECT, true);
        mMenuServer->Enable(ID_DISCONNECT_MENU, true);
    }
    else {
        mToolBar->EnableTool(ID_DISCONNECT, false);
        mMenuServer->Enable(ID_DISCONNECT_MENU, false);
    }

    // Zobrazeni "Jste / Nejste na tahu"
    if(!mPlayGame)
        SetStatusText(wxEmptyString, 1);
    else if(mOnStep)
        SetStatusText(_("Jste na tahu"), 1);
    else
        SetStatusText(_("Nejste na tahu"), 1);

    // Nastaveni ovladacich prvku v pripade, ze hrac hraje hru
    if(mPlayGame)
    {
        mToolBar->EnableTool(ID_NEW_GAME, false);
        mToolBar->EnableTool(ID_CONT_GAME, false);
        mToolBar->EnableTool(ID_END_GAME, true);
        mMenuGame->Enable(ID_NEW_GAME_MENU, false);
        mMenuGame->Enable(ID_CONT_GAME_MENU, false);
        mMenuGame->Enable(ID_END_GAME_MENU, true);
        mBoxFreePlayers->Enable(false);
        mBoxGameList->Enable(false);
        mBoxGameList->Clear();
    }
    else
    {
        mToolBar->EnableTool(ID_END_GAME, false);
        mMenuGame->Enable(ID_END_GAME_MENU, false);
        mBoxFreePlayers->Enable(true);
        mBoxGameList->Enable(true);
    }
}

void ClientFrame::OnRelog(wxCommandEvent& event)
{
    clientCom->DisconnectFromServer();
    LogUser();
    UpdateStatusBar();
}

void ClientFrame::OnAbout(wxCommandEvent& event)
{
    // Vytvoreni dialogu s odkazem na napovedu
    wxAboutDialogInfo info;
    info.SetName(_("DAMA"));
    info.SetDescription(_("Ceska dama podle pravidel Ceske federace damy"));
    info.AddDeveloper(_("Jaroslav  Bendl"));
    info.AddDeveloper(_("Vit Kopriva"));
    info.AddDeveloper(_("Maksim Kryzhanovsky"));
    info.AddDeveloper(_("Milos Chmel"));
    wxAboutBox(info);
}

void ClientFrame::OnHelp(wxCommandEvent& event)
{
    wxString helpFileName, helpPath;
    xmlConfig->GetParam("HELP", "file", helpFileName);
    wxFileName helpFile(helpFileName);
    wxString executableFullPath = (wxStandardPaths::Get()).GetExecutablePath();
    wxString executablePath;
    wxFileName::SplitPath(executableFullPath, NULL, &executablePath, NULL, NULL);
    helpPath = wxT("file://") + executablePath + _("/") + helpFileName;
    wxLaunchDefaultBrowser(helpPath);
}

void ClientFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fileDialog(this, wxT("Otevrit soubor"), ::wxGetCwd(), wxEmptyString, wxT("XML soubory (*.xml)|*.xml"), wxOPEN);
    fileDialog.CentreOnParent();

    if(fileDialog.ShowModal() == wxID_OK)
    {
        //std::string filePath = std::string(fileDialog.GetPath().mb_str());
        xmlCom = new XmlCom(fileDialog.GetPath());
        SelectPlayerDialog *selectPlayerDialog = new SelectPlayerDialog(this, xmlCom);
        if(selectPlayerDialog->ShowModal() == wxID_OK)
        {
            PrepareToRelog();
            clientCom->DisconnectFromServer();
            wxMilliSleep(250);
            SignToServer();
            wxMilliSleep(250);
            FillBox();
        }
        delete selectPlayerDialog;
    }
}

void ClientFrame::FillBox()
{
    // Naplneni mBoxFreePlayers
    mBoxSavedPlayers->Clear();
    mBoxGameList->Clear();
    SockCom::StringVector playerVector;
    xmlCom->FindOpponents(mName, playerVector);
    for(int i = 0; i < playerVector.size(); i++)
    {
        mBoxSavedPlayers->Append(wxString(wxString::FromAscii(playerVector[i].c_str())));
    }
}

void ClientFrame::PrepareToRelog()
{
    mGameId = -1;
    mPlayGame = false;
    mOnStep = false;
    mConnected = false;
    mNotebook->SetSelection(0);
    mBoxFreePlayers->Clear();
    mBoxSavedPlayers->Clear();
    mBoxGameList->Clear();
    UpdateStatusBar();
}

// ----------------------------------------------------------------------------
// SELECTPLAYERDIALOG
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SelectPlayerDialog, wxDialog)
    EVT_BUTTON(wxID_OK, SelectPlayerDialog::OnPlayerSelect)
END_EVENT_TABLE()

SelectPlayerDialog::SelectPlayerDialog(ClientFrame *parent, XmlCom *xmlComParent) : wxDialog(parent, ID_MODAL, wxString(_T("Vyber hrace")), wxDefaultPosition, wxSize(150, 240), wxID_OK|wxID_CANCEL)
{
    myParent = parent;
    xmlCom = xmlComParent;
    mBoxPlayers = new wxListBox(this, ID_LISTBOX_SELECT, wxPoint(10, 10), wxSize(130, 140), 0, NULL, wxLB_SINGLE);
    mBoxPlayers->SetFocus();
    SockCom::StringVector playerVector;
    xmlCom->FindNames(playerVector);
    for(int i = 0; i < playerVector.size(); i++)
    {
        mBoxPlayers->Append(wxString(wxString::FromAscii(playerVector[i].c_str())));
    }
    mButtonOk = new wxButton(this, wxID_OK, _("Vybrat"), wxPoint(10, 160), wxSize(130, 30));
    mButtonClose = new wxButton(this, wxID_CANCEL, _("Zavrit"), wxPoint(10, 200), wxSize(130, 30));
    mButtonOk->SetDefault();
}

void SelectPlayerDialog::OnPlayerSelect(wxCommandEvent& event)
{
    myParent->mName = wxString(mBoxPlayers->GetString(mBoxPlayers->GetSelection()));
    EndModal(wxID_OK);
}

// ----------------------------------------------------------------------------
// CONFIGDIALOG
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ConfigDialog, wxDialog)
    EVT_BUTTON(wxID_OK, ConfigDialog::OnConfigSave)
END_EVENT_TABLE()

ConfigDialog::ConfigDialog(ClientFrame *parent, XmlConfig *xmlConfigDoc) : wxDialog(parent, ID_MODAL, wxString(_T("Nastaveni")), wxDefaultPosition, wxSize(400,175))
{
    myParent = parent;

    xmlConfig = xmlConfigDoc;

    wxString valueStr;
    // Nastaveni hostname
    xmlConfig->GetParam("SERVER", "host", valueStr);   // Zisk hodnoty predchoziho nastaveni z clientconfig.xml
    mTitleHostname = new wxStaticText(this, wxID_STATIC, wxT("Adresa serveru:"), wxPoint(10, 15), wxDefaultSize, wxALIGN_LEFT);
    mInputHostname = new wxTextCtrl(this, wxID_ANY, valueStr, wxPoint(200, 10), wxSize(190, 25));

    // Nastaveni portu
    xmlConfig->GetParam("SERVER", "port", valueStr);
    mTitlePort = new wxStaticText(this, wxID_STATIC, wxT("Port:"), wxPoint(10, 45), wxDefaultSize, wxALIGN_LEFT);
    mSpinPort = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxPoint(200, 40), wxSize(190, 25));
    mSpinPort->SetRange(1, 99000);
    mSpinPort->SetValue(atoi(valueStr.mb_str()));

    // Nastaveni zpozdeni pri prehravani
    xmlConfig->GetParam("REPLAY", "delay" ,valueStr);
    mTitleDelay = new wxStaticText(this, wxID_STATIC, wxT("Zpozdeni prehravani:"), wxPoint(10, 70), wxDefaultSize, wxALIGN_LEFT);
    mSpinDelay = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxPoint(200, 70), wxSize(190, 25));
    mSpinDelay->SetRange(1, 100);
    mSpinDelay->SetValue(atoi(valueStr.mb_str()));

    // Nastaveni logovani her
    xmlConfig->GetParam("LOG", "enable" ,valueStr);
    mTitleLog = new wxStaticText(this, wxID_STATIC, wxT("Logovani:"), wxPoint(10, 105), wxDefaultSize, wxALIGN_LEFT);
    mLog = new wxCheckBox(this, wxID_ANY , _("Logovani"), wxPoint(200, 105), wxDefaultSize);
    if(valueStr == _("1"))  // Nastaveni podle ulozeneho
    {
        mLog->SetValue(true);
    }

    if(valueStr == _("1"))  // Nastaveni podle ulozeneho
    {
        mLog->SetValue(true);
    }

    mButtonSave = new wxButton(this, wxID_OK, _T("Ulozit"), wxPoint(10,135), wxSize(180,30));
    mButtonClose = new wxButton(this, wxID_CANCEL, _T("Zavrit"), wxPoint(200,135), wxSize(190,30));
    mButtonSave->SetDefault();

    mTitleHostname->SetFocus();
}

void ConfigDialog::OnConfigSave(wxCommandEvent& event)
{
    // Ulozeni hostname
    wxString valueStr = wxString(mInputHostname->GetValue());
    xmlConfig->SetParam("SERVER", "host", valueStr);

    // Ulozeni portu
    valueStr = wxString(wxString::Format(_("%d"), mSpinPort->GetValue()));
    xmlConfig->SetParam("SERVER", "port", valueStr);
    mSpinPort->SetRange(1, 99000);
    mSpinPort->SetValue(atoi(valueStr.mb_str()));

    // Ulozeni zpozdeni
    valueStr = wxString(wxString::Format(_("%d"), mSpinDelay->GetValue()));
    xmlConfig->SetParam("REPLAY", "delay", valueStr);
    myParent->mDelayTime = atoi(valueStr.mb_str()) * 1000;

    // Ulozeni logovani
    bool log = mLog->GetValue();
    valueStr = wxString(wxString::Format(_("%d"), log));
    xmlConfig->SetParam("LOG", "enable", valueStr);
    myParent->mLog = static_cast<bool>(atoi(valueStr.mb_str()));

    wxMessageBox(_("Zmena prihlasovacich udaju k serveru se projevi az pri dalsim prihlaseni."), _("Upozorneni"), wxICON_INFORMATION);
    Close();
}

// ----------------------------------------------------------------------------
// LOGDIALOG
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(LogDialog, wxDialog)
    EVT_BUTTON(ID_BUTTON_LOG, LogDialog::OnLog)
    EVT_BUTTON(ID_BUTTON_REGISTER, LogDialog::OnRegister)
END_EVENT_TABLE()

LogDialog::LogDialog(ClientFrame *parent, XmlConfig *xmlConfigDoc) : wxDialog(parent, ID_MODAL, wxString(_T("Prihlaseni")), wxDefaultPosition, wxSize(400, 175), wxID_OK|wxID_CANCEL|ID_BUTTON_REGISTER)
{
    myParent = parent;
    xmlConfig = xmlConfigDoc;

    // Nastaveni prihlasovaciho jmena
    wxString valueStr;
    mTitleName = new wxStaticText(this, wxID_STATIC, wxT("Zadejte jmeno hrace:"), wxPoint(10, 15), wxDefaultSize, wxALIGN_LEFT);
    mInputName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(200, 10), wxSize(190, 25));
    mInputName->SetMaxLength(SockCom::PLAYER_LEN);

    // Nastaveni hostname
    xmlConfig->GetParam("SERVER", "host", valueStr);
    mTitleHostname = new wxStaticText(this, wxID_STATIC, wxT("Adresa serveru:"), wxPoint(10, 45), wxDefaultSize, wxALIGN_LEFT);
    mInputHostname = new wxTextCtrl(this, wxID_ANY, valueStr, wxPoint(200, 40), wxSize(190, 25));

    // Nastaveni portu
    xmlConfig->GetParam("SERVER", "port", valueStr);
    mTitlePort = new wxStaticText(this, wxID_STATIC, wxT("Port:"), wxPoint(10, 70), wxDefaultSize, wxALIGN_LEFT);
    mSpinPort = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxPoint(200, 70), wxSize(190, 25));
    mSpinPort->SetRange(1, 99000);
    mSpinPort->SetValue(atoi(valueStr.mb_str()));

    // Nastaveni logovani
    xmlConfig->GetParam("LOG", "enable" ,valueStr);
    mTitleLog = new wxStaticText(this, wxID_STATIC, wxT("Logovani:"), wxPoint(10, 105), wxDefaultSize, wxALIGN_LEFT);
    mLogCheckBox = new wxCheckBox(this, wxID_ANY , wxEmptyString, wxPoint(200, 100), wxDefaultSize);
    if(valueStr == _("1"))  // Nastaveni podle ulozeneho
    {
        mLogCheckBox->SetValue(true);
    }

    mButtonLog = new wxButton(this, ID_BUTTON_LOG, _T("Prihlasit"), wxPoint(10,135), wxSize(120,30));
    mButtonRegister = new wxButton(this, ID_BUTTON_REGISTER, _T("Registrovat"), wxPoint(140,135), wxSize(120,30));
    mButtonClose = new wxButton(this, wxID_CANCEL, _T("Zrusit"), wxPoint(270,135), wxSize(120,30));

    mButtonLog->SetDefault();
    mInputName->SetFocus();
}

void LogDialog::SaveConfig()
{
    myParent->mName = wxString(mInputName->GetValue());

    // Ulozeni hostname
    wxString valueStr = wxString(mInputHostname->GetValue());
    xmlConfig->SetParam("SERVER", "host", valueStr);
    myParent->mAddr.Hostname(valueStr);

    // Ulozeni portu
    valueStr = wxString(wxString::Format(_("%d"), mSpinPort->GetValue()));
    xmlConfig->SetParam("SERVER", "port", valueStr);
    myParent->mAddr.Service(mSpinPort->GetValue());

    // Ulozeni logovani
    bool log = mLogCheckBox->GetValue();
    valueStr = wxString(wxString::Format(_("%d"), log));
    xmlConfig->SetParam("LOG", "enable", valueStr);
    myParent->mLog = static_cast<bool>(atoi(valueStr.mb_str()));
}

void LogDialog::OnLog(wxCommandEvent& event)
{
    if(mInputName->GetValue().Len() < 3)
    {
        wxMessageBox(_("Jmena musi mit alespon 3 znaky."), _("Chyba"), wxICON_INFORMATION);
    }
    else
    {
        SaveConfig();
        EndModal(ID_BUTTON_LOG);
    }
}

void LogDialog::OnRegister(wxCommandEvent& event)
{
    if(mInputName->GetValue().Len() < 3)
    {
        wxMessageBox(_("Jmena musi mit alespon 3 znaky."), _("Chyba"), wxICON_INFORMATION);
    }
    else
    {
        SaveConfig();
        EndModal(ID_BUTTON_REGISTER);
    }
}
