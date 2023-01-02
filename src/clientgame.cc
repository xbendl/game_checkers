// clientgame.cc
// Modul implementujici logiku hry
// author: Maksym Kryzhanovskyy (xkryzh00), Vit Kopriva (xkopri05)

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "clientgame.h"

// Konstruktor
Checker::Checker(const std::string& s)
    : m_type(s),
      m_board(NULL)
{
    // Urceni typu figurky
    if(s == "WHITE" || s == "DWHITE") {
        m_colour = *wxWHITE;
        m_border = *wxBLACK;
    }
    else { // "BLACK" || "DBLACK"
        m_colour = *wxBLACK;
        m_border = *wxWHITE;
    }

    return;
}

// Pretizeni operatotru porovnavani
bool Checker::operator==(Checker *checker) const 
{
    // Ziskani typu figurky
    std::string type = checker->GetType();
    if((m_type == "WHITE" || m_type == "DWHITE") &&
        (type == "WHITE" || type == "DWHITE"))
    {
        return true;
    }
    if((m_type == "BLACK" || m_type == "DBLACK") &&
       (type == "BLACK" || type == "DBLACK"))
    {
        return true;
    }

    return false;
}

// Prekresleni figurek
void Checker::Draw(wxDC& dc, wxPoint pt)
{
    dc.SetPen(wxPen(m_border, 3));
    dc.SetBrush(wxBrush(m_colour,wxSOLID));
    dc.DrawCircle(pt.x, pt.y, 16);
    return;
}

// Vyhodnoceni ohrozeni
bool Checker::EnPrise(wxPoint& coord)
{
    int right_fwd = StepCheck(coord,1,-1);
    int f_right_fwd = StepCheck(coord,2,-2);
    if(right_fwd == RIVAL_CHECKER && f_right_fwd == EMPTY)
        return true;

    int left_fwd = StepCheck(coord,-1,-1);
    int f_left_fwd = StepCheck(coord,-2,-2);
    if(left_fwd == RIVAL_CHECKER && f_left_fwd == EMPTY)
        return true;

    return false;
}

// Ziskani stavu policka
enum Checker::State Checker::StepCheck(wxPoint& coord, int dx, int dy)
{
    int x(coord.x+dx), y(coord.y+dy);
    int map(x/2+y*4);
    //kontrola hranice
    if(x>=0 && x<8 && y>=0 && y<8) {
      if(!(m_board->m_checkers[map]))
          return EMPTY;
      //jestli tam lezi kamen
      if (*this == m_board->m_checkers[map])
          return MY_CHECKER; //vlastni
      else
         return RIVAL_CHECKER; //sloperuv
    }
    return OUT;
}

// Test zvedatelnosti kamene
bool Checker::IsLifting(wxPoint& coord)
{
    // Souperuv kamen nelze zvednout
    if(this->GetType() != m_board->m_own)
        return false;

    int left_fwd = StepCheck(coord,-1,-1);
    int right_fwd = StepCheck(coord,1,-1);
    int f_left_fwd = StepCheck(coord,-2,-2);
    int f_right_fwd = StepCheck(coord,2,-2);

    if(m_board->m_d_en_prise ) { //dama ma prednost ohrozuje
        return false;
    }
    if(m_board->m_en_prise) {
        if(f_right_fwd == EMPTY && right_fwd == RIVAL_CHECKER)
            return true;
        if(f_left_fwd == EMPTY && left_fwd == RIVAL_CHECKER)
            return true;
    }
    else {
        if(left_fwd == EMPTY || right_fwd == EMPTY)
            return true;
        if(f_left_fwd == EMPTY && left_fwd == RIVAL_CHECKER)
            return true;
        if(f_right_fwd == EMPTY && right_fwd == RIVAL_CHECKER)
            return true;
    }

    return false;
}

// Zda nova pozice je platna
bool Checker::IsSituated(wxPoint& coord)
{
    int map(coord.x/2+coord.y*4);
    int dx(m_board->m_oldcoord.x-coord.x);
    int dy(m_board->m_oldcoord.y-coord.y);

    if(m_board->m_en_prise && abs(dx) != 2) {
        return false; //ohrozuje a neskace
    }
    if(m_board->m_checkers[map])
        return false;
    if(dy > 2 || (abs(dx) != dy))
        return false;
    if(abs(dx)==2)//preskok
    {
        int px = (dx>0)?(-1):(1);
        if(StepCheck(m_board->m_oldcoord,px,-1) != RIVAL_CHECKER)
            return false;
    }
    if(coord.y == 0) //kamen se stane dammou
        m_board->SetDchecker();

    return true;
}

// Nastaveni ukazatele na sachovnici
void Checker::SetBoard(ClientGame* cg)
{
    m_board = cg;
    return;
}

// Ziskani typu figurky
std::string& Checker::GetType()
{
    return m_type;
}

// Premisteni figurky
void Checker::Jump(int from, int to)
{
    if(to/4 == 7 || to/4 ==0 ) { //nastaveni damy
        m_board->m_checkers[to] = m_board->m_box->GetChecker(
        (this->GetType()=="WHITE")?"DWHITE":"DBLACK");
    }
    else {
        m_board->m_checkers[to] = m_board->m_checkers[from];
    }
    m_board->m_checkers[from] = NULL;
    return;
}

// Konstruktor
Dchecker::Dchecker(const std::string& s)
    : Checker(s)
{
    m_dsign = (s == "DWHITE")?*wxBLACK:*wxWHITE;
    return;
}

// Vykresleni damy
void Dchecker::Draw(wxDC& dc, wxPoint pt)
{
    //std::cout << "Dchecker::Draw" << std::endl;
    Checker::Draw(dc, pt);
    dc.SetPen(wxPen(m_dsign, 1));
    dc.SetBrush(wxBrush(m_dsign,wxSOLID));
    dc.DrawCircle(pt.x, pt.y, 6);
    return;
}

// Zda dama ohrozuje
bool Dchecker::EnPrise(wxPoint& coord)
{
    if(DiagEnPrise(coord,-1,-1))return true;
    if(DiagEnPrise(coord,1,-1))return true;
    if(DiagEnPrise(coord,-1,1))return true;
    if(DiagEnPrise(coord,1,1))return true;

    return false;
}

// Test zvedatelnosti damy
bool Dchecker::IsLifting(wxPoint& coord)
{
    // Souperovou damu nelze zvednout
    if(this->GetType() != m_board->m_down)
        return false;

    int left_fwd = StepCheck(coord,-1,-1);
    int right_fwd = StepCheck(coord,1,-1);
    int left_back = StepCheck(coord,-1,1);
    int right_back = StepCheck(coord,1,1);

    int f_left_fwd = StepCheck(coord,-2,-2);
    int f_right_fwd = StepCheck(coord,2,-2);
    int f_left_back = StepCheck(coord,-2,2);
    int f_right_back = StepCheck(coord,2,2);

    if(m_board->m_d_en_prise) {  //dama ohrozuje

        //prohleda diagonaly
        if(DiagEnPrise(coord,-1,-1))return true;
        if(DiagEnPrise(coord,1,-1))return true;
        if(DiagEnPrise(coord,-1,1))return true;
        if(DiagEnPrise(coord,1,1))return true;
    }
    else {
        if(m_board->m_en_prise) //dama neaohrozuje ale kamen ano 
            return false;
        if(left_fwd == EMPTY || right_fwd == EMPTY)
            return true;
        if(left_back == EMPTY || right_back == EMPTY)
            return true;

        if(DiagEnPrise(coord,-1,-1))return true;
        if(DiagEnPrise(coord,1,-1))return true;
        if(DiagEnPrise(coord,-1,1))return true;
        if(DiagEnPrise(coord,1,1))return true;

    }

    return false;
}

// Zda nova pozice damy je platna
bool Dchecker::IsSituated(wxPoint& coord)
{
    int map(coord.x/2+coord.y*4);
    int dx(m_board->m_oldcoord.x-coord.x);
    int dy(m_board->m_oldcoord.y-coord.y);
    int Adx = abs(dx);
    if(m_board->m_checkers[map])  //na neprazdne policko
        return false;
    if( (abs(dx) != abs(dy))) //na diagonale
        return false;

    int prev = OUT;
    if(Adx > 1)
    {   // prirustky souradnic
        int px = (dx>0)?(-1):(1);
        int py = (dy>0)?(-1):(1);
        prev = StepCheck(coord,-px,-py); //obsah predchoziho pole

        if(prev == MY_CHECKER ) 
        {
            return false;  //na predchozim poli je muj kamen
        }
        if(Adx >2) //skace dale po diagonale
        {
            wxPoint start(m_board->m_oldcoord.x ,m_board->m_oldcoord.y );
            wxPoint end(coord.x - 2*px,coord.y -2*py);
            //vse mezi cilem a startem  na diagonale musi byt prazdne
            int diag = DiagCheck(start,end,px,py);
            if(diag != EMPTY)
            {
                return false;
            }
        }
    }
    if(m_board->m_d_en_prise && prev != RIVAL_CHECKER){
        return false; //ohrozuje a neskace
    }
    return true;
}

// Realizace tahu
void Dchecker::Jump(int from, int to)
{
    m_board->m_checkers[to] = m_board->m_checkers[from];
    m_board->m_checkers[from] = NULL;
    return;
}

// Vyhodnoti zda dama v damem smeru ohrozuje soupere
bool Dchecker::DiagEnPrise(wxPoint coord,int dx,int dy)
{
    wxPoint old_coord = coord;
    int point = EMPTY;
    do {
        point = StepCheck(coord,dx,dy);
        coord.x+=dx;
        coord.y+=dy;
    } while(point == EMPTY);

    switch(point)
    {
        case OUT: {if(wxPoint(coord.x-dx,coord.y-dy) != old_coord) return false; break;}  //diagonala je prazdna
        case MY_CHECKER: return false;break;                                 //na diagonale je vlastni kamen
        case RIVAL_CHECKER: {if( EMPTY == StepCheck(coord,dx,dy)) return true; break;}    
        //ohrozije pokud je za souperem  volne misto
    }
    return false;
}

// Prohleda diagonalu po prvni kamen
int Dchecker::DiagCheck(wxPoint start,wxPoint end,int dx,int dy)
{
    int point=EMPTY; //inicializace
    while( (start != end) && (point == EMPTY))
    {
        point = StepCheck(start,dx,dy);
        start.x+=dx;
        start.y+=dy;
    }
    return point;
}

// Ziskani ukazatele na figurku
Checker* CheckerBox::GetChecker(const std::string& s) try
{
    if(m_box[s] == NULL) {
        if(s == "WHITE")
            m_box[s] = new Checker(s);
        else if(s == "BLACK")
            m_box[s] = new Checker(s);
        else if(s == "DWHITE")
            m_box[s] = new Dchecker(s);
        else // "DBLACK"
            m_box[s] = new Dchecker(s);
    }
    return m_box[s];
}
catch(...) {
    std::cerr << "Nastala vyjimka" << std::endl;
}

CheckerBox::~CheckerBox()
{
    std::map<std::string, Checker*>::iterator i;
    i = m_box.begin();
    while(i != m_box.end()) {
        delete i->second;
        i++;
    }
    return;
}

// Konstruktor
ClientGame::ClientGame(const std::string owner, const std::string rival) try
    : m_dragged(NULL),
      m_en_prise(0),
      m_d_en_prise(0)
{
    // Kamen hrace
    m_own = owner;
    // Dama hrace
    m_down = "D"+owner;
    // Kamen protihrace
    m_riv = rival;
    // Dama protihrace
    m_driv = "D"+rival;
    // Instance tovarny kamenu
    m_box = new CheckerBox();
    // Instance kamene hrace
    Checker* white = m_box->GetChecker(m_own);
    // Instance damy hrace
    Checker* black = m_box->GetChecker(m_riv);
    // Instance kamene protihrace
    Checker* dwhite = m_box->GetChecker(m_down);
    // Instance damy protihrace
    Checker* dblack = m_box->GetChecker(m_driv);
    // Nastaveni sachovnice
    white->SetBoard(this);
    black->SetBoard(this);
    dwhite->SetBoard(this);
    dblack->SetBoard(this);
    // Vychozi rozlozeni figurek
    for(int i(0),x(0),y(0); i < 64; i++,x=i/8,y=i%8) {
        if((x+y)%2) {
            int map = x/2+y*4;
            if(y < 3) m_checkers[map] = black;
            else if(y > 4) m_checkers[map] = white;
            else m_checkers[map] = NULL;
            m_position[map] = wxPoint(x*51+25, y*51+25);
        }
    }
    // Nastaveni vyzyvatele
    if(owner == "WHITE")
        m_onstep = true;
    else
        m_onstep = false;

    return;
}
catch(...) {
    std::cerr << "Nastala vyjimka" << std::endl;
}

ClientGame::~ClientGame()
{
    // Uvolneni zdroju
    delete m_box;
    return;
}

// Prekresleni rozlozeni figurek
void ClientGame::ReDraw(wxDC& dc)
{
    for(int i(0); i < 32; i++) {
        if(m_checkers[i])
            m_checkers[i]->Draw(dc, m_position[i]);
    }
    return;
}

// "Zvednuti" figurky
Checker* ClientGame::LiftChecker(wxPoint& coord)
{
    if((coord.x+coord.y)%2) {
        m_oldcoord = coord;
        int map(coord.x/2+coord.y*4);
        if(m_checkers[map] && m_checkers[map]->IsLifting(coord)) {
            m_dragged = m_checkers[map];
            m_checkers[map] = NULL;
        }
    }
    return m_dragged;
}

// "Polozeni" figurky
void ClientGame::DropChecker(wxPoint& coord)
{
    int map(coord.x/2+coord.y*4);
    m_checkers[map] = m_dragged;
    m_dragged = NULL;
    return;
}

// "Korunovani" kamene
void ClientGame::SetDchecker()
{
    m_dragged = m_box->GetChecker(
        (m_dragged->GetType()=="WHITE")?"DWHITE":"DBLACK"
    );
    return;
}

// Odstraneni figurky
void ClientGame::DelChecker(int map)
{
    m_checkers[map] = NULL;
    return;
}

// Realizace tahu
void ClientGame::GotAJump(int from, int to)
{
    m_checkers[from]->Jump(from,to);
    return;
}

// Prevod mapy do souradnic
wxPoint ClientGame::TranstormMap(int map)
{
    int y = map/4;
    int dif = (y%2)? 0:1;
    int x = 2*(map - 4*y) + dif;
    return wxPoint(x,y); 
}

// Odstraneni figurky
void ClientGame::ReMoveChecker(int from, int to)
{
    //skok na sousedni radek
    if( abs(to-from) <= 5 )
        return;
    //priznak zmeny poradi pocatku a cile
    bool changed = false;
    if(from > to )
    {
        int pom = to;
        to = from;
        from =pom;
        changed = true;
    }
    //prevod na souradnic
    wxPoint pFrom = TranstormMap(from);
    wxPoint pTo = TranstormMap(to);
    //smer pohybu
    int dx = pTo.x - pFrom.x;
    int dy = pTo.y - pFrom.y;
    //prirustky smeru
    int px = (dx>0)?(1):(-1); 
    int py = (dy>0)?(1):(-1);
    //prevod zpet ze souradnic do pozice v poli
    int map=0; 
    if(changed){
        map = (pFrom.x +px)/2 + 4*(pFrom.y+py); //pozice preskakovaneho
    }
    else {
        map = (pTo.x -px)/2 + 4*(pTo.y-py); //pozice preskakovaneho
    }
    //vymazani
    DelChecker(map);
    return;
}

// Zda je hrac na tahu
bool ClientGame::IsOnStep()
{
    return m_onstep;
}

// Predani tahu
void ClientGame::OnStep()
{
    m_onstep = (m_onstep)?false:true;
    return;
}

// Vyhodnoceni stavu ohrozeni
void ClientGame::CheckSum()
{
    m_en_prise = 0;
    m_d_en_prise = 0;
    m_liftAbleCount = 0;
    std::string type;
    for(int i(0); i < 32; i++) {
        if(m_checkers[i]) {
            type = m_checkers[i]->GetType();
            if(type == m_own)
            {
                wxPoint coord(m_position[i].x/51, m_position[i].y/51);
                if(m_checkers[i]->EnPrise(coord))
                    m_en_prise++; 
                if(m_checkers[i]->IsLifting(coord))
                    m_liftAbleCount++;
            }
            else if(type == m_down)
            {
                wxPoint coord(m_position[i].x/51, m_position[i].y/51);
                if(m_checkers[i]->EnPrise(coord))
                    m_d_en_prise++; 
                if(m_checkers[i]->IsLifting(coord))
                    m_liftAbleCount++;
            }
        }
    }
    return;
}

// Zda nastal stav ohrozeni
bool ClientGame::IsForsedMove()
{
   if(m_en_prise || m_d_en_prise)
       return true;

   return false;
}

// Zda hra konci
bool ClientGame::IsFinish()
{
   return m_liftAbleCount == 0;
}

// Konstruktor modu prehravani
ClientPlay::ClientPlay(const std::string own, const std::string riv)
    : ClientGame(own, riv),
      m_u_position(0)
{
    return;
}

// Nahrani predam ulozene hry
void ClientPlay::LoadGame(bool first, SockCom::StepVector steps)
{
    for(int i(0); i < steps.size(); i++) {
        std::vector<Checker*> u;
        for(int k(0); k < 32; k++) {
            u.push_back(m_checkers[k]);
        }
        m_u.push_back(u);
        for(int m(0), n(1); n < steps[i].size(); m+=2, n+=2) {
            int from, to;
            if(first) {
                from = (i%2) ? steps[i][m] : 31-steps[i][m];
                to = (i%2) ? steps[i][n] : 31-steps[i][n];
            }
            else {
                from = (i%2) ? 31-steps[i][m] : steps[i][m];
                to = (i%2) ? 31-steps[i][n] : steps[i][n];
            }
            this->GotAJump(from,to);
            ReMoveChecker(from,to);
       }
    }
    std::vector<Checker*> u;
    for(int k(0); k < 32; k++) {
        u.push_back(m_checkers[k]);
    }
    m_u.push_back(u);
    First();
    return;
}

// Krok dopredu
bool ClientPlay::StepFwd()
{
    m_u_position++;
    for(int i(0); i<32; i++)
        m_checkers[i] = m_u[m_u_position][i];
    return (m_u_position+1 == m_u.size()) ? false:true;
}

// Krok zpet
bool ClientPlay::StepBack()
{
    m_u_position--;
    for(int i(0); i<32; i++)
        m_checkers[i] = m_u[m_u_position][i];
    return (m_u_position-1 < 0)?false:true;
}

// Vychozi rozlozeni figurek
void ClientPlay::First()
{
    for(int i(0); i<32; i++)
        m_checkers[i] = m_u[0][i];
    m_u_position = 0;
    return;
}
