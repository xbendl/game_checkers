/**
* \file clientboard.h
* Rozhraní modulu reprezentujícího hrácí pole
* \author Maksym Kryzhanovskyy (xkryzh00)
*/


#ifndef CLIENTBOARD_H
#define CLIENTBOARD_H 1

#include "wx/scrolwin.h"

#include "clientframe.h"
#include "clientgame.h"
#include "clientcom.h"

/// Reprezentace ¹achovnice
class ClientBoard : public wxScrolledWindow
{
    DECLARE_EVENT_TABLE()

    ClientFrame* /*..........*/ m_frame;     ///< Nadøazené okno
    wxColourDatabase* /*.....*/ m_coldbase;  ///< Databáze barev
    wxBrushList* /*..........*/ m_brushs;    ///< Vyplòovací barvy
    wxPenList* /*............*/ m_pens;      ///< Seznam per
    wxBitmap* /*.............*/ m_board;     ///< Hráèi pole
    wxBitmap* /*.............*/ m_boardshot; ///< Snímek hracího pole
    ClientGame* /*...........*/ m_game;      ///< Logika hry
    ClientPlay* /*...........*/ m_play;      ///< Pøehrávání hry
    Checker* /*..............*/ m_checker;   ///< Právì ta¾ená figurka
    wxPoint /*...............*/ m_oldcoord;  ///< Souøadnice figurky v pohybu
    bool /*..................*/ m_force;     ///< Pøíznak vynuceného tahu
    SockCom::DataVector /*...*/ m_datamove;  ///< Odesílány tah

public:

   /**
    * Konstruktor
    * @param parent Nadøazené okno
    * @param type Vzhled okna
   **/
    ClientBoard(ClientFrame* parent, long type);

   /**
    * Destruktor
   **/
    ~ClientBoard();

   /**
    * Pøipraví hráci pole k vykreslování
   **/
    void PrepareBoard();

   /**
    * Zajistí kopii hracího pole vèetnì figurek
    * @param dc Kreslítko z nadøazeného okna
   **/
    void BoardShot(wxDC& dc);

   /**
    * Hranice hracího pole
    * @param pt Souøadnice my¹i
    * @return True, le¾í-li bod v hranicích pole, jinak false
   **/
    bool BoardBound(wxPoint& pt);

   /**
    * Znovu vykreslí hraci pole, zmìni-li se jeho stav
    * @param dc Kreslitko z nadøazeného okna
    * @return
   **/
    virtual void OnDraw(wxDC& dc);

   /**
    * Obsluha události vyvolané èinnosti my¹í
    * @param event Konkrétní události
   **/
    void OnMouseEvent(wxMouseEvent& event);

   /**
    * Pøemístí kámen z poèáteèní pozice na cílovou
    * @param datamove Vektor pozic
   **/
    void MoveChecker(SockCom::DataVector& datamove);

   /**
    * Zahajení hry
    * @param owner Barva kamenù hráèe
    * @param rival Barva kamenù protihráèe
   **/
    void NewGame(const std::string owner, const std::string rival);

   /**
    * Ukonèení hry
   **/
    void EndGame();

   /**
    * Inicializace modu pøehrávání
    * @param first Urèi vyzyvatele
    * @param steps Vektor krokù hry
   **/
    void StartPlay(bool first, SockCom::StepVector steps);

   /**
    * Krokování dopøedu
    * @return False je-li posledním krokem hry
   **/
    bool PlayFwd();

   /**
    * Zpìtné krokování
    * @return False je-li prvním krokem hry
   **/
    bool PlayBack();

   /**
    * Výchozí rozlo¾ení kamenù
   **/
    void StopPlay();

   /**
    * Ukonèení módu pøehrávání
   **/
    void EndPlay();
};

#endif // CLIENTBOARD_H
