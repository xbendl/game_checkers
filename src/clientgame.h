/**
 * \file clientgame.h
 * Rozhraní modulu implementujícího logiku hry
 * \author Maksym Kryzhanovskyy (xkryzh00), Vít Kopøiva (xkopri05)
**/


#ifndef CLIENTGAME_H
#define CLIENTGAME_H 1

#include <map>
#include <string>
#include <vector>

#include "clientcom.h"

class ClientGame;

/// Reprezentace kamene
class Checker : public wxObject
{
    std::string /*.....*/ m_type;   ///< Typ figurky
    wxColour /*........*/ m_colour; ///< Barva figurky
    wxColour /*........*/ m_border; ///< Barva okraje
    ClientGame* /*.....*/ m_board;  ///< Nadøazená tøída

    /// Výètový typ definující stav políèka
    enum State {
        EMPTY,             ///< Prázdné políèko
        RIVAL_CHECKER,     ///< Cizí figurka
        MY_CHECKER,        ///< Vlastní figurka
        OUT                ///< Nová pozice je za hranicí pole
    };
    
    friend class Dchecker;

public:

   /**
    * Konstruktor
    * @param colour Barva kamene
   **/
    Checker(const std::string& colour);

   /**
    * Vykreslí figurku na pozici zadané parametrem
    * @param dc Kreslítko z nadøazeného okna
    * @param pt Bod umístìní figurky
   **/
    virtual void Draw(wxDC& dc, wxPoint pt);

   /**
    * Test zvedatelnosti figurky v daném kontextu
    * @param coord Souøadnice umístìní figurky
   **/
    virtual bool IsLifting(wxPoint& coord);

   /**
    * Test zda je platná nová pozice figurky
    * @param coord Souøadnice umístìní figurky
   **/
    virtual bool IsSituated(wxPoint& coord);

   /**
    * Ohodnocení zda kámen ohro¾uje soupeøovy figurky
    * @param coord Souøadnice umístìní figurky
    * @return true ohro¾uje-li figurku protihráèe
   **/
    virtual bool EnPrise(wxPoint& coord);

   /**
    * Pøemísti figurku z výchozí pozice na cílovou
    * @param from Výchozí umístìní figurky
    * @param to Cílové umístìní figurky
   **/
    virtual void Jump(int from, int to);
    
   /**
    * Vyhodnotí stav políèka zda obsahuje kamen
    * @param coord Výchozí souøadnice
    * @param dx Pøírùstek v ose x
    * @param dy Pøírùstek v ose y
    * @return Hodnota typu 'State' urèující stav políèka
   **/
    enum State StepCheck(wxPoint& coord, int dx, int dy);

   /**
    * Nastavení nadøazené tøídy
    * @param cg Ukazatel na nadøazenou tøídu
   **/
    void SetBoard(ClientGame* cg);

   /**
    * Pøetí¾ení operatoru porovnání dvou figurek
    * @param ch Porovnávaná figurka
   **/
    bool operator==(Checker* ch) const;

   /**
    * Zjistí typ figurky
    * @return Typ figurky
   **/
    std::string& GetType();
};

///Reprezentace dámy
class Dchecker : public Checker
{
    wxColour m_dsign; ///< Oznaèení dámy

public:

   /**
    * Konstruktor
    * @param colour Barva kamene
   **/
    Dchecker(const std::string& colour);

   /**
    * Vykreslí dámu na pozici zadané parametrem
    * @param pt Bod umístìní figurky
   **/
    virtual void Draw(wxDC&, wxPoint pt);

   /**
    * Test zveladatelnosti dámy v daném kontextu
    * @param coord Souøadnice umístìní dámy
   **/
    virtual bool IsLifting(wxPoint& coord);

   /**
    * Test zda je platná nova pozice dámy
    * @param coord Souøadnice umístìní dámy
   **/
    virtual bool IsSituated(wxPoint& coord);

   /**
    * Ohodnoceni zda dáma ohro¾uje soupeøovy figurky
    * @param coord Souøadnice umístìní figurky
    * @return True ohro¾uje-li figurku protihráèe
   **/
    virtual bool EnPrise(wxPoint& coord);

   /**
    * Pøemístí dámu z výchozí pozice na cílovou
    * @param from Výchozí umístìní 
    * @param to Cílové umístìní 
   **/
    virtual void Jump(int from, int to);

   /**
    * Ponocná funkce pro vyhodnocení zda dáma 
    * ohro¾uje soupeøovu figurku v daném smìru
    * @param coord Souøadnice umístìní dámy
    * @param dx Pøírùstek v ose x
    * @param dy Pøírùstek v ose y
    * @return True ohro¾uje-li figurku protihráèe
   **/
    bool DiagEnPrise(wxPoint coord,int dx,int dy);

   /**
    * Pomocná funkce pro vyhodnocení, zda je diagonála mezi
    * body prázdná
    * @param start Poèáteèní bod
    * @param end Koncový bod 
    * @param dx Pøírùstek v ose x
    * @param dy Pøírùstek v ose y
   * @return První neprázdné políèko
   **/
    int DiagCheck(wxPoint start,wxPoint end,int dx,int dy);
};

/// Továrna figurek
class CheckerBox : wxObject
{
    ///Uchovavá ji¾ vytvoøené figurky
    std::map<std::string, Checker*> m_box; 
public:
    ~CheckerBox();

   /**
    * Získá ukazatel na figurku, pokud neexistuje tak ji vytvoøí
    * @param type Typ po¾adované figurky 
    * @return Ukazatel na figurku
   **/
    Checker* GetChecker(const std::string& type);
};

/// Logická reprezentace ¹achovnice
class ClientGame : public wxObject
{
    CheckerBox* /*...........*/ m_box;           ///< Továrna figurek
    Checker* /*..............*/ m_checkers[32];  ///< Pole figurek
    wxPoint /*..............*/  m_position[32];  ///< Fyzická umístìní figurek
    std::string /*...........*/ m_own;           ///< Kámen hráèe
    std::string /*...........*/ m_down;          ///< Dáma hráèe
    std::string /*...........*/ m_riv;	         ///< Kámen protihráèe
    std::string /*...........*/ m_driv;          ///< Dáma protihráèe
    Checker* /*..............*/ m_dragged;       ///< Právì ta¾ená figurka
    wxPoint /*...............*/ m_oldcoord;      ///< Poèáteèní pozice ta¾ené figurky
    wxPoint /*...............*/ m_newcoord;      ///< Cílová pozice ta¾ené figurky
    int /*...................*/ m_en_prise;      ///< Poèet ohro¾ujících kamenù
    int /*...................*/ m_d_en_prise;    ///< Poèet ohro¾ujících dam
    bool /*..................*/ m_onstep;        ///< Pøíznak zda je hráè na tahu 
    int /*...................*/ m_liftAbleCount; ///< Poèet zaveditelných figurek
    SockCom::DataVector /*...*/ dataVector;      ///< Vektor tahù hry

    friend class Checker;
    friend class Dchecker;
    friend class ClientPlay;
public:
    ~ClientGame();

   /**
    * Konstruktor
    * @param own Typ kamene hráèe
    * @param riv Typ kamene protihráèe
   **/
    ClientGame(const std::string own, const std::string riv);

   /**
    * Pøekreslení figurek
    * @param dc Kreslítko z nadøazeného okna
   **/
    void ReDraw(wxDC& dc);

   /**
    * Zvednuti figurky z políèka
    * @param coord Souøadnice umístìní figurky
    * @return Ukazatel na zvednutou figurku
   **/
    Checker* LiftChecker(wxPoint& coord);

   /**
    * Polo¾ení figurky na pozici zadané parametrem
    * @param coord Souøadnice nové pozice
   **/
    void DropChecker(wxPoint& coord);

   /**
    * Odstranìní figurky z urèené pozice
    * @param from Logické umístìní figurky
   **/
    void DelChecker(int from);

   /**
    * Test zda je hráè na tahu
    * @return True, pokud je hráè na tahu
   **/
    bool IsOnStep();

   /**
    * Pøedání tahu protihráèi
   **/
    void OnStep();

   /**
    * Pøemístìní figurky z výchozí pozice cílovou
    * @param from Výchozí pozice
    * @param to Cílová pozice
   **/
    void GotAJump(int from, int to);

   /**
    * Odstranìní kamene pøi skoku
    * @param from Výchozí pozice skoku
    * @param to Cílová pozice skoku
    * @return
   **/
    void ReMoveChecker(int from, int to);

    /**
    * Pøepoèítá mapovou pozici na souøadnice
    * @param map Mapová pozice
    * @return Bod se souøadnicemi
    **/
    wxPoint TranstormMap(int map);

   /**
    * Test zda na ¹achovnici je stav ohro¾ení
    * @return True, je-li stav ohro¾ení
   **/
    bool IsForsedMove();

   /**
    * Korunování kamene na dámu
   **/
    void SetDchecker();

   /**
    * Test zda je konec hry
    * @return True, je-li konec hry
   **/
    bool IsFinish();

   /**
    * Vyhodnocení stavu ohro¾ení
    *  a poètu zvedatelných figurek
   **/
    void CheckSum();
};

/// Implementace modu pøehrávání hry
class ClientPlay : public ClientGame
{
    /// Aktuální pozice v historii tahù
    int m_u_position;
    /// Historie tahù
    std::vector<std::vector<Checker*> > m_u;

public:

   /**
    * Konstruktor
    * @param own Typ figurky hráèe
    * @param riv Typ figurky protihráèe
   **/
    ClientPlay(const std::string own, const std::string riv);

   /**
    * Nahrání pøedem ulo¾ené hry
    * @param first Uurèí vyzyvatele
    * @param steps Historie tahù
   **/
    void LoadGame(bool first, SockCom::StepVector steps);

   /**
    * Krok dopøedu
    * @return False, je-li tah posledni
   **/
    bool StepFwd();

   /**
    * Krok zpìt
    * @return False, je-li tah první
   **/
    bool StepBack();

   /**
    * Výchozí rozestavení figurek
   **/
    void First();
};

#endif // CLIENTGAME_H
