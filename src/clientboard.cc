// clientboard.cc
// Modulu reprezentujici hraci pole
// Autor: Maksym Kryzhanovskyy (xkryzh00)

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "clientboard.h"

// Tbulka udalosti
BEGIN_EVENT_TABLE(ClientBoard, wxScrolledWindow)
    EVT_MOUSE_EVENTS(ClientBoard::OnMouseEvent)
END_EVENT_TABLE()

// Konstruktor
ClientBoard::ClientBoard(ClientFrame* parent, long id) try
      // Pocatecni inicializace
    : m_game(NULL),
      m_play(NULL),
      m_checker(NULL),
      m_force(false),
      m_frame(parent)
{
    // Pozice okna
    wxPoint pos(wxDefaultPosition);
    // Minimalni velikost okna
    wxSize size(412, 412);
    // Vytvoreni okna
    this->Create(parent, id, pos, size,
        wxSUNKEN_BORDER);
    // Nastaveni posunovacu
    this->SetScrollRate(0, 0);
    // Alokace kreslitek
    m_pens = new wxPenList();
    // Alokace vyplne
    m_brushs = new wxBrushList();
    // Alokace databaze barev
    m_coldbase = new wxColourDatabase();
    // Pridani barvy svetleho policka
    m_coldbase->AddColour(_("BROWN"),
        wxColour(200,150,70));
    // Pridani barvy tmaveho policka
    m_coldbase->AddColour(_("DARK BROWN"),
        wxColour(170,120,60));
    // Alokace bitmapy hraciho pole
    m_board = new wxBitmap(size.x, size.y);
    // Alokace bitmapy snimku hraciho pole
    m_boardshot = new wxBitmap(size.x, size.y);
    // Priprava vykresleni hraciho pole
    this->PrepareBoard();

    return;
}
catch(...) {
    std::cerr << "Nastala vyjimka" << std::endl;
}

// Destruktor
ClientBoard::~ClientBoard()
{
    // Uvolneni zdroju
    delete m_pens;
    delete m_brushs;
    delete m_coldbase;
    delete m_board;
    delete m_boardshot;
    if(m_game) delete m_game;
    if(m_play) delete m_play;
}

// Priprava bitmapy sachovnice
void ClientBoard::PrepareBoard()
{
    wxMemoryDC memDC;
    // Vykresleni sachovnice
    memDC.SelectObject(*m_board);
    for(int i(0), x(0), y(0); i < 64; i++, x=i/8, y=i%8) {
        if((x+y)%2) {
            memDC.SetPen(*m_pens->FindOrCreatePen(
                m_coldbase->Find(_("BROWN")), 1, wxSOLID));
            memDC.SetBrush(*m_brushs->FindOrCreateBrush(
                m_coldbase->Find(_("DARK BROWN")), wxSOLID));
        }
        else {
            memDC.SetPen(*m_pens->FindOrCreatePen(
                m_coldbase->Find(_("DARK BROWN")), 1, wxSOLID));
            memDC.SetBrush(*m_brushs->FindOrCreateBrush(
                m_coldbase->Find(_("BROWN")), wxSOLID));
        }
        memDC.DrawRectangle(x*51, y*51, 51, 51);
    }
    memDC.SelectObject(wxNullBitmap);
    return;
}

// Zajisti snimek sachovnice
void ClientBoard::BoardShot(wxDC& dc)
{
    wxMemoryDC memDC;
    // Kopie aktualniho rozlozeni figurek
    memDC.SelectObject(*m_boardshot);
    memDC.Blit(0, 0, 412, 412, &dc, 0, 0);
    memDC.SelectObject(wxNullBitmap);
    return;
}

// Zahaji mod hrani hry
void ClientBoard::NewGame(const std::string owner,
    const std::string rival) try
{
    wxClientDC dc(this);
    // Instance hry
    m_game = new ClientGame(owner, rival);
    this->OnDraw(dc);
    return;
}
catch(...) {
    std::cerr << "Nastala vyjimka" << std::endl;
}

// Ukonci mod hrani hry
void ClientBoard::EndGame()
{
    wxClientDC dc(this);
    // Uvolneni zdroju
    if(m_game) {
        delete m_game;
        m_game = NULL;
    }
    this->OnDraw(dc);
    return;
}

// Zahaji mod prehravani hry
void ClientBoard::StartPlay(bool first,
    SockCom::StepVector steps) try
{
    wxClientDC dc(this);
    // Urceni vyzyvatele
    std::string owner((first)?"WHITE":"BLACK");
    std::string rival((first)?"BLACK":"WHITE");
    // Instance prahravace
    m_play = new ClientPlay(owner, rival);
    m_play->LoadGame(first,steps);
    this->OnDraw(dc);
    return;
}
catch(...) {
    std::cerr << "Nastala vyjimka" << std::endl;
}

// Ukonceni modu prehravani hry
void ClientBoard::EndPlay()
{
    wxClientDC dc(this);
    // Uvolneni zdroju
    if(m_play) {
        delete m_play;
        m_play = NULL;
    }
    this->OnDraw(dc);
    return;
}

// Výchozí rozlo¾ení kamenù
void ClientBoard::StopPlay()
{
    wxClientDC dc(this);
    m_play->First();
    this->OnDraw(dc);
    return;
}

//Krokování dopøedu
bool ClientBoard::PlayFwd()
{
    wxClientDC dc(this);
    bool retval(m_play->StepFwd());
    this->OnDraw(dc);
    return retval;
}

// Zpìtné krokování
bool ClientBoard::PlayBack()
{
    wxClientDC dc(this);
    bool retval(m_play->StepBack());
    this->OnDraw(dc);
    return retval;
}

// Znovu vykresli sachovnici
void ClientBoard::OnDraw(wxDC& dc)
{
    dc.DrawBitmap(*m_board, 0, 0, true);
    if(m_game) m_game->ReDraw(dc);
    if(m_play) m_play->ReDraw(dc);
    return;
}

// Obsluha události vyvolané èinnosti my¹í
void ClientBoard::OnMouseEvent(wxMouseEvent& event)
{
    // hraje-li se hra
    if(m_game) {
        // Ziskani kraslitka
        wxClientDC dc(this);
        // Pozice mysi v pixelech
        wxPoint pos(event.GetPosition());
        // Souradnice pozice mysi
        wxPoint coord(pos.x/51,pos.y/51);
        // Zmacknuti leveho tlacitka mysi v pripade vicenasobneho tahu
        if(event.LeftDown() && m_force && m_game->IsOnStep() &&
            coord == m_oldcoord) {
            // Ziskani ukazatele na figurku
            m_checker = m_game->LiftChecker(m_oldcoord);
            // Uspesne ziskani ukazatele
            if(m_checker) {
                // Prekresleni sachovnice
                this->OnDraw(dc);
                // Ziskani snimku
                this->BoardShot(dc);
                // Vykresleni snimku
                dc.DrawBitmap(*m_boardshot,0,0,true);
                // Vykresleni figurky
                m_checker->Draw(dc,pos);
                // Zapamatovani pozice
                m_oldcoord = coord;
            }
        }
        // Zmacknuti praveho tlacitka mysi
        else if(event.LeftDown() && !m_checker && m_game->IsOnStep()) {
            // Ziskani ukazatele na figurku
            m_checker = m_game->LiftChecker(coord);
            // Uspesne ziskani ukazatele
            if(m_checker) {
                // Prekresleni sachovnice
                this->OnDraw(dc);
                // Ziskani snimku
                this->BoardShot(dc);
                // Vykresleni snimku
                dc.DrawBitmap(*m_boardshot,0,0,true);
                // Vykresleni figurky
                m_checker->Draw(dc,pos);
                // Zapamatovani pozice
                m_oldcoord = coord;
            }
        }
        // Pusteni leveho tlacitka mysi
        else if(event.LeftUp()) {
            // Predchozi ziskani ukazatele bylo uspesne
            if(m_checker) {
                // Nova pozice neni stejan, platne hranice, platne nove umisteni
                if(m_oldcoord != coord && this->BoardBound(pos) &&
                    m_checker->IsSituated(coord)) {
                    // Polozeni figurky na novou pozici
                    m_game->DropChecker(coord);
                    // Priprava tahu k odeslani
                    int oldmap(m_oldcoord.x/2+m_oldcoord.y*4);
                    int newmap(coord.x/2+coord.y*4);
                    m_datamove.push_back((7-m_oldcoord.x)/2+(7-m_oldcoord.y)*4);
                    m_datamove.push_back((7-coord.x)/2+(7-coord.y)*4);
                    // Pripadne odstraneni figurky
                    m_game->ReMoveChecker(oldmap,newmap);
                    // Vicenasobny tah
                    if(m_game->IsForsedMove() && m_checker->EnPrise(coord)) {
                        m_oldcoord = coord;
                        m_force = true;
                    }
                    else {
                        // Odeslani tahu
                        m_frame->SendStep(m_datamove);
                        // Predani tahu
                        m_game->OnStep();
                        m_force = false;
                    }
                }
                else {
                    // Navrat na novou pozici
                    m_game->DropChecker(m_oldcoord);
                }
                m_checker = NULL;
                this->OnDraw(dc);

            }
        }
        // Pohyb mysi s pridrzenim leveho tlacitka
        else if(event.LeftIsDown() && event.Dragging()) {
            // Hranice pole
            if(m_checker && this->BoardBound(pos)) {
                // Vykresleni snimku
                dc.DrawBitmap(*m_boardshot,0,0,true);
                // Pohyb figurkou
                m_checker->Draw(dc,pos);
            }
        }
    }
    return;
}

// Premisti kamen z pocatecni pozice na cilovou
void ClientBoard::MoveChecker(SockCom::DataVector& datamove)
{
    wxClientDC dc(this);
    // Aplikace tahu
    for(int i(0), j(1); j < datamove.size(); i+=2, j+=2) {
        m_game->GotAJump(datamove[i],datamove[j]);
        m_game->ReMoveChecker(datamove[i],datamove[j]);
    }
    // Vypocet stavu ohrozeni
    m_game->CheckSum();
    // Test podminky ukonceni hry
    if(m_game->IsFinish())
        m_frame->SendWinInfo();
    // Zobraceni noveho rozlozeni
    this->OnDraw(dc);
    // Predani tahu
    m_game->OnStep();
    return;
}

// Hranice sachovnice
bool ClientBoard::BoardBound(wxPoint& pt)
{
    return pt.x>0 && pt.x<408 && pt.y>0 && pt.y<408;
}
