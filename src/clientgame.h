/**
 * \file clientgame.h
 * Rozhran� modulu implementuj�c�ho logiku hry
 * \author Maksym Kryzhanovskyy (xkryzh00), V�t Kop�iva (xkopri05)
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
    ClientGame* /*.....*/ m_board;  ///< Nad�azen� t��da

    /// V��tov� typ definuj�c� stav pol��ka
    enum State {
        EMPTY,             ///< Pr�zdn� pol��ko
        RIVAL_CHECKER,     ///< Ciz� figurka
        MY_CHECKER,        ///< Vlastn� figurka
        OUT                ///< Nov� pozice je za hranic� pole
    };
    
    friend class Dchecker;

public:

   /**
    * Konstruktor
    * @param colour Barva kamene
   **/
    Checker(const std::string& colour);

   /**
    * Vykresl� figurku na pozici zadan� parametrem
    * @param dc Kresl�tko z nad�azen�ho okna
    * @param pt Bod um�st�n� figurky
   **/
    virtual void Draw(wxDC& dc, wxPoint pt);

   /**
    * Test zvedatelnosti figurky v dan�m kontextu
    * @param coord Sou�adnice um�st�n� figurky
   **/
    virtual bool IsLifting(wxPoint& coord);

   /**
    * Test zda je platn� nov� pozice figurky
    * @param coord Sou�adnice um�st�n� figurky
   **/
    virtual bool IsSituated(wxPoint& coord);

   /**
    * Ohodnocen� zda k�men ohro�uje soupe�ovy figurky
    * @param coord Sou�adnice um�st�n� figurky
    * @return true ohro�uje-li figurku protihr��e
   **/
    virtual bool EnPrise(wxPoint& coord);

   /**
    * P�em�sti figurku z v�choz� pozice na c�lovou
    * @param from V�choz� um�st�n� figurky
    * @param to C�lov� um�st�n� figurky
   **/
    virtual void Jump(int from, int to);
    
   /**
    * Vyhodnot� stav pol��ka zda obsahuje kamen
    * @param coord V�choz� sou�adnice
    * @param dx P��r�stek v ose x
    * @param dy P��r�stek v ose y
    * @return Hodnota typu 'State' ur�uj�c� stav pol��ka
   **/
    enum State StepCheck(wxPoint& coord, int dx, int dy);

   /**
    * Nastaven� nad�azen� t��dy
    * @param cg Ukazatel na nad�azenou t��du
   **/
    void SetBoard(ClientGame* cg);

   /**
    * P�et�en� operatoru porovn�n� dvou figurek
    * @param ch Porovn�van� figurka
   **/
    bool operator==(Checker* ch) const;

   /**
    * Zjist� typ figurky
    * @return Typ figurky
   **/
    std::string& GetType();
};

///Reprezentace d�my
class Dchecker : public Checker
{
    wxColour m_dsign; ///< Ozna�en� d�my

public:

   /**
    * Konstruktor
    * @param colour Barva kamene
   **/
    Dchecker(const std::string& colour);

   /**
    * Vykresl� d�mu na pozici zadan� parametrem
    * @param pt Bod um�st�n� figurky
   **/
    virtual void Draw(wxDC&, wxPoint pt);

   /**
    * Test zveladatelnosti d�my v dan�m kontextu
    * @param coord Sou�adnice um�st�n� d�my
   **/
    virtual bool IsLifting(wxPoint& coord);

   /**
    * Test zda je platn� nova pozice d�my
    * @param coord Sou�adnice um�st�n� d�my
   **/
    virtual bool IsSituated(wxPoint& coord);

   /**
    * Ohodnoceni zda d�ma ohro�uje soupe�ovy figurky
    * @param coord Sou�adnice um�st�n� figurky
    * @return True ohro�uje-li figurku protihr��e
   **/
    virtual bool EnPrise(wxPoint& coord);

   /**
    * P�em�st� d�mu z v�choz� pozice na c�lovou
    * @param from V�choz� um�st�n� 
    * @param to C�lov� um�st�n� 
   **/
    virtual void Jump(int from, int to);

   /**
    * Ponocn� funkce pro vyhodnocen� zda d�ma 
    * ohro�uje soupe�ovu figurku v dan�m sm�ru
    * @param coord Sou�adnice um�st�n� d�my
    * @param dx P��r�stek v ose x
    * @param dy P��r�stek v ose y
    * @return True ohro�uje-li figurku protihr��e
   **/
    bool DiagEnPrise(wxPoint coord,int dx,int dy);

   /**
    * Pomocn� funkce pro vyhodnocen�, zda je diagon�la mezi
    * body pr�zdn�
    * @param start Po��te�n� bod
    * @param end Koncov� bod 
    * @param dx P��r�stek v ose x
    * @param dy P��r�stek v ose y
   * @return Prvn� nepr�zdn� pol��ko
   **/
    int DiagCheck(wxPoint start,wxPoint end,int dx,int dy);
};

/// Tov�rna figurek
class CheckerBox : wxObject
{
    ///Uchovav� ji� vytvo�en� figurky
    std::map<std::string, Checker*> m_box; 
public:
    ~CheckerBox();

   /**
    * Z�sk� ukazatel na figurku, pokud neexistuje tak ji vytvo��
    * @param type Typ po�adovan� figurky 
    * @return Ukazatel na figurku
   **/
    Checker* GetChecker(const std::string& type);
};

/// Logick� reprezentace �achovnice
class ClientGame : public wxObject
{
    CheckerBox* /*...........*/ m_box;           ///< Tov�rna figurek
    Checker* /*..............*/ m_checkers[32];  ///< Pole figurek
    wxPoint /*..............*/  m_position[32];  ///< Fyzick� um�st�n� figurek
    std::string /*...........*/ m_own;           ///< K�men hr��e
    std::string /*...........*/ m_down;          ///< D�ma hr��e
    std::string /*...........*/ m_riv;	         ///< K�men protihr��e
    std::string /*...........*/ m_driv;          ///< D�ma protihr��e
    Checker* /*..............*/ m_dragged;       ///< Pr�v� ta�en� figurka
    wxPoint /*...............*/ m_oldcoord;      ///< Po��te�n� pozice ta�en� figurky
    wxPoint /*...............*/ m_newcoord;      ///< C�lov� pozice ta�en� figurky
    int /*...................*/ m_en_prise;      ///< Po�et ohro�uj�c�ch kamen�
    int /*...................*/ m_d_en_prise;    ///< Po�et ohro�uj�c�ch dam
    bool /*..................*/ m_onstep;        ///< P��znak zda je hr�� na tahu 
    int /*...................*/ m_liftAbleCount; ///< Po�et zavediteln�ch figurek
    SockCom::DataVector /*...*/ dataVector;      ///< Vektor tah� hry

    friend class Checker;
    friend class Dchecker;
    friend class ClientPlay;
public:
    ~ClientGame();

   /**
    * Konstruktor
    * @param own Typ kamene hr��e
    * @param riv Typ kamene protihr��e
   **/
    ClientGame(const std::string own, const std::string riv);

   /**
    * P�ekreslen� figurek
    * @param dc Kresl�tko z nad�azen�ho okna
   **/
    void ReDraw(wxDC& dc);

   /**
    * Zvednuti figurky z pol��ka
    * @param coord Sou�adnice um�st�n� figurky
    * @return Ukazatel na zvednutou figurku
   **/
    Checker* LiftChecker(wxPoint& coord);

   /**
    * Polo�en� figurky na pozici zadan� parametrem
    * @param coord Sou�adnice nov� pozice
   **/
    void DropChecker(wxPoint& coord);

   /**
    * Odstran�n� figurky z ur�en� pozice
    * @param from Logick� um�st�n� figurky
   **/
    void DelChecker(int from);

   /**
    * Test zda je hr�� na tahu
    * @return True, pokud je hr�� na tahu
   **/
    bool IsOnStep();

   /**
    * P�ed�n� tahu protihr��i
   **/
    void OnStep();

   /**
    * P�em�st�n� figurky z v�choz� pozice c�lovou
    * @param from V�choz� pozice
    * @param to C�lov� pozice
   **/
    void GotAJump(int from, int to);

   /**
    * Odstran�n� kamene p�i skoku
    * @param from V�choz� pozice skoku
    * @param to C�lov� pozice skoku
    * @return
   **/
    void ReMoveChecker(int from, int to);

    /**
    * P�epo��t� mapovou pozici na sou�adnice
    * @param map Mapov� pozice
    * @return Bod se sou�adnicemi
    **/
    wxPoint TranstormMap(int map);

   /**
    * Test zda na �achovnici je stav ohro�en�
    * @return True, je-li stav ohro�en�
   **/
    bool IsForsedMove();

   /**
    * Korunov�n� kamene na d�mu
   **/
    void SetDchecker();

   /**
    * Test zda je konec hry
    * @return True, je-li konec hry
   **/
    bool IsFinish();

   /**
    * Vyhodnocen� stavu ohro�en�
    *  a po�tu zvedateln�ch figurek
   **/
    void CheckSum();
};

/// Implementace modu p�ehr�v�n� hry
class ClientPlay : public ClientGame
{
    /// Aktu�ln� pozice v historii tah�
    int m_u_position;
    /// Historie tah�
    std::vector<std::vector<Checker*> > m_u;

public:

   /**
    * Konstruktor
    * @param own Typ figurky hr��e
    * @param riv Typ figurky protihr��e
   **/
    ClientPlay(const std::string own, const std::string riv);

   /**
    * Nahr�n� p�edem ulo�en� hry
    * @param first Uur�� vyzyvatele
    * @param steps Historie tah�
   **/
    void LoadGame(bool first, SockCom::StepVector steps);

   /**
    * Krok dop�edu
    * @return False, je-li tah posledni
   **/
    bool StepFwd();

   /**
    * Krok zp�t
    * @return False, je-li tah prvn�
   **/
    bool StepBack();

   /**
    * V�choz� rozestaven� figurek
   **/
    void First();
};

#endif // CLIENTGAME_H
