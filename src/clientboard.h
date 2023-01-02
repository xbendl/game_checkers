/**
* \file clientboard.h
* Rozhran� modulu reprezentuj�c�ho hr�c� pole
* \author Maksym Kryzhanovskyy (xkryzh00)
*/


#ifndef CLIENTBOARD_H
#define CLIENTBOARD_H 1

#include "wx/scrolwin.h"

#include "clientframe.h"
#include "clientgame.h"
#include "clientcom.h"

/// Reprezentace �achovnice
class ClientBoard : public wxScrolledWindow
{
    DECLARE_EVENT_TABLE()

    ClientFrame* /*..........*/ m_frame;     ///< Nad�azen� okno
    wxColourDatabase* /*.....*/ m_coldbase;  ///< Datab�ze barev
    wxBrushList* /*..........*/ m_brushs;    ///< Vypl�ovac� barvy
    wxPenList* /*............*/ m_pens;      ///< Seznam per
    wxBitmap* /*.............*/ m_board;     ///< Hr��i pole
    wxBitmap* /*.............*/ m_boardshot; ///< Sn�mek hrac�ho pole
    ClientGame* /*...........*/ m_game;      ///< Logika hry
    ClientPlay* /*...........*/ m_play;      ///< P�ehr�v�n� hry
    Checker* /*..............*/ m_checker;   ///< Pr�v� ta�en� figurka
    wxPoint /*...............*/ m_oldcoord;  ///< Sou�adnice figurky v pohybu
    bool /*..................*/ m_force;     ///< P��znak vynucen�ho tahu
    SockCom::DataVector /*...*/ m_datamove;  ///< Odes�l�ny tah

public:

   /**
    * Konstruktor
    * @param parent Nad�azen� okno
    * @param type Vzhled okna
   **/
    ClientBoard(ClientFrame* parent, long type);

   /**
    * Destruktor
   **/
    ~ClientBoard();

   /**
    * P�iprav� hr�ci pole k vykreslov�n�
   **/
    void PrepareBoard();

   /**
    * Zajist� kopii hrac�ho pole v�etn� figurek
    * @param dc Kresl�tko z nad�azen�ho okna
   **/
    void BoardShot(wxDC& dc);

   /**
    * Hranice hrac�ho pole
    * @param pt Sou�adnice my�i
    * @return True, le��-li bod v hranic�ch pole, jinak false
   **/
    bool BoardBound(wxPoint& pt);

   /**
    * Znovu vykresl� hraci pole, zm�ni-li se jeho stav
    * @param dc Kreslitko z nad�azen�ho okna
    * @return
   **/
    virtual void OnDraw(wxDC& dc);

   /**
    * Obsluha ud�losti vyvolan� �innosti my��
    * @param event Konkr�tn� ud�losti
   **/
    void OnMouseEvent(wxMouseEvent& event);

   /**
    * P�em�st� k�men z po��te�n� pozice na c�lovou
    * @param datamove Vektor pozic
   **/
    void MoveChecker(SockCom::DataVector& datamove);

   /**
    * Zahajen� hry
    * @param owner Barva kamen� hr��e
    * @param rival Barva kamen� protihr��e
   **/
    void NewGame(const std::string owner, const std::string rival);

   /**
    * Ukon�en� hry
   **/
    void EndGame();

   /**
    * Inicializace modu p�ehr�v�n�
    * @param first Ur�i vyzyvatele
    * @param steps Vektor krok� hry
   **/
    void StartPlay(bool first, SockCom::StepVector steps);

   /**
    * Krokov�n� dop�edu
    * @return False je-li posledn�m krokem hry
   **/
    bool PlayFwd();

   /**
    * Zp�tn� krokov�n�
    * @return False je-li prvn�m krokem hry
   **/
    bool PlayBack();

   /**
    * V�choz� rozlo�en� kamen�
   **/
    void StopPlay();

   /**
    * Ukon�en� m�du p�ehr�v�n�
   **/
    void EndPlay();
};

#endif // CLIENTBOARD_H
