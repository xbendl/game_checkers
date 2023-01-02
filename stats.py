#!/usr/bin/python
import os
import sys
import re
import string

class Report:
    """Trida uchovavajici informace o prohledavanych souborech"""
    def __init__(self, aFilePath, aFileName, aNumber, aCommentLength):
        self.filePath = aFilePath               # Cela cesta k souboru
        self.fileName = aFileName               # Jmeno souboru
        self.number = aNumber                   # Vysledek operace nad danym souborem
        self.commentLength = aCommentLength     # Delka komentaru v danem souboru

reports = [];               # Uchova seznam zdrojovych souboru a vysledky operaci nad nimi

def printHelp():
    """Vytiskne napovedu na standardni vystup"""
    print """   --help
       vypise tuto napovedu

    -k
       vypise pocet vsech klicovych slov v kazdem zdrojovem souboru a celkem
       mimo poznamky a retezce

    -o
       vypise pocet operatoru (nikoliv oddelovacu apod.) v kazdem zdrojovem
       souboru a celkem mimo poznamky a retezce
       _____
       * Bere v potaz pouze operatory se znamou a fixni posloupnosti
       nepismennych znaku
       * Vynechava operator carky (,)

    -ik
       vypise pocet identifikatoru v kazdem zdrojovem souboru a celkem
       - zahrnuje klicova slova ale mimo poznamky a retezce

    -i
       vypise pocet identifikatoru v kazdem zdrojovem souboru a celkem
       - nezahrnuje klicova slova a mimo poznamky a retezce
       _____
       * Mezi identifikatory se pocita i makro definovane direktivou #DEFINE 
       a take NULL			        

    -w <pattern>
       vyhleda textovy retezec pattern ve vsech zdrojovych kodech
       a vypise pocet vyskytu na soubor i celkem (jelikoz se nejedna o
       identifikator, hleda i v poznamkach a retezcovych literalech, zatimco
       v predchozich pripadech se poznamky a retezcove literaly vynechavaji!)
       _____
       * Pocita neprekryvajici se vyskyty

    -c
       vypise celkovou delku poznamek v bytech na soubor a celkem
       _____
       * U jednoradkovych komentaru (//) pocitan mezi znaky komentare i znak
       noveho radku, u viceradkovych komentaru jen presahuje-li na dalsi radek
       * Mezi znaky komentare se pocitaji i uvozujici znaky (// ci /**/)

    -p
       v kombinaci s predchozimi (az na --help) zpusobi, ze soubory se budou
       vypisovat bez uplne cesty k souboru - ostatni parametry nelze vzajemne
       kombinovat"""


def searchDirectory(directory, printMode, longestPath):
    """Do seznamu zdrojovych souboru vlozi cestu k zdrojovym souborum v aktualnich adresarich i podadresarich"""
    for fileName in os.listdir(directory):
        path = os.path.join(directory, fileName)
        if not os.path.isfile(path):
            longestPath = searchDirectory(path, printMode, longestPath)   # Rekurzivni volani teto funkce pro vnoreny adresar
            continue
        splitName = fileName.split(".")
        lenName = len(splitName) - 1
        if(lenName > 0):                        # Nalezeni souboru s priponou c, c++, cpp, h
            if((splitName[lenName] == "c") or (splitName[lenName] == "cc") or (splitName[lenName] == "cpp") or (splitName[lenName] == "c++") or (splitName[lenName] == "h")):
                if(printMode == "normal"):              # Zjisteni nejdelsi cesty k souboru
                    if(len(path) > longestPath):
                        longestPath = len(path)
                else:                                   # Zjisteni nejdelsiho jmena souboru
                    if(len(fileName) > longestPath):
                        longestPath = len(fileName)
                reports.append(Report(path,fileName,0,0)) # Nalezene soubory se vlozi do fronty
    return longestPath


def getClearBody(item):
    """Na predanym souborem provede odstraneni komentaru a retezcu a takto upraveny retezec vrati"""
    DEF_STATE = {"TEXT":1, "FIRSTSLASH":2, "LONGCOMMENT":3, "STAR":4, "COMMA":5, "QUOTE":6, "COMMENT":7, "ESCAPE_COMMA":8, "ESCAPE_QUOTE":9}  # Asociativni pole pouzivane jakozto obdoba vyctu pri zpracovani souboru
    inFile = file(item.filePath, "r")   # Otevreni souboru
    state = DEF_STATE["TEXT"]
    newBody = []
    while 1:                            # Cteni souboru po radcich
        line = inFile.readline()
        if not line: break

        for x in line:                  # Zpracovani aktualniho radku
            if(x == "\r"):                             # Znak '\r' preskakuji
                continue;
            if(state == DEF_STATE["TEXT"]):            # Faze zpracovani bezneho textu souboru
                if(x == "/"):
                    item.commentLength += 1
                    state = DEF_STATE["FIRSTSLASH"]
                elif(x == "\""):
                    state = DEF_STATE["COMMA"]
                    continue
                elif(x == "\'"):
                    state = DEF_STATE["QUOTE"]
                    continue
                newBody.append(x);
            elif(state == DEF_STATE["FIRSTSLASH"]):    # Faze zpracovani pri nalezeni "/"
                if(x == "/"):
                    item.commentLength += 1
                    newBody.pop(len(newBody)-1)
                    state = DEF_STATE["COMMENT"]
                elif(x == "*"):
                    item.commentLength += 1
                    newBody.pop(len(newBody)-1);
                    state = DEF_STATE["LONGCOMMENT"]
                else:
                    item.commentLength -= 1
                    state = DEF_STATE["TEXT"]
                    newBody.append(x);
            elif(state == DEF_STATE["COMMENT"]):
                if(x == "\n"):
                    item.commentLength += 1
                    state = DEF_STATE["TEXT"]
                else:
                    item.commentLength += 1
            elif(state == DEF_STATE["LONGCOMMENT"]):   # Faze zpracovani ve viceradkovem komentari
                if(x == "*"):
                    state = DEF_STATE["STAR"]
                item.commentLength += 1
            elif(state == DEF_STATE["STAR"]):          # Faze zpracovani ve viceradkovem komentari po nalezeni "*"
                item.commentLength += 1
                if(x == "/"):
                    state = DEF_STATE["TEXT"]
                elif(x == "*"):
                    continue
                else:
                    state = DEF_STATE["LONGCOMMENT"]
            elif(state == DEF_STATE["COMMA"]):         # Faze zpracovani retezce (po vyskytu ")
                if(x == "\\"):
                    state = DEF_STATE["ESCAPE_COMMA"]
                elif(x == "\""):
                    state = DEF_STATE["TEXT"]
                continue
            elif(state == DEF_STATE["QUOTE"]):  # Faze zpracovani retezce (po vyskytu ')
                if(x == "\\"):
                    state = DEF_STATE["ESCAPE_QUOTE"]
                elif(x == "\'"):
                    state = DEF_STATE["TEXT"]
                continue
            elif(state == DEF_STATE["ESCAPE_COMMA"]):  # Faze zpracovani escape sekvence v retezci uvozenem pomoci "
                state = DEF_STATE["COMMA"]
            elif(state == DEF_STATE["ESCAPE_QUOTE"]):  # Faze zpracovani escape sekvence v retezci uvozenem pomoci '
                state = DEF_STATE["QUOTE"]

    newBody.insert(0, " ");     # Na zacatek pridam space, abych mohl najit klicove slovo zacinajici na prvni pozici
    inFile.close()
    listToTuple = tuple(newBody)                            # Uprava na typ tuple
    bodyString = ('%c' * len(listToTuple)) % listToTuple    # Prevod tuple na string
    return bodyString


def getFullBody(item):
    """Vrati retezec obsahujici telo urceneho souboru"""
    inFile = file(item.filePath, "r")   # Otevreni souboru
    newBody = []
    while 1:                            # Cteni souboru po radcich
        line = inFile.readline()
        if not line:
            break

        newBody.append(line)

    inFile.close()
    listToTuple = tuple(newBody)                            # Uprava na typ tuple
    bodyString = ('%s' * len(listToTuple)) % listToTuple    # Prevod tuple na string
    return bodyString


def printResult(totalCount, longestPath, printMode):
    """Vytiskne vysledek na standardni vystup bud v normalnim, nebo zkracenem tvaru"""
    numberOfDigits = len(('%s') % totalCount)
    if(printMode == "normal"):      # Vypis s celou cestou k souboru
        for item in reports:
            whiteSpaces = ' ' * ((longestPath - len(item.filePath)) + (numberOfDigits - len(('%s') % item.number)))
            print "%s%s  %d" % (item.filePath, whiteSpaces, item.number)
    else:                           # Vypis pouze s nazvem souboru
        for item in reports:
            whiteSpaces = ' ' * ((longestPath - len(item.fileName)) + (numberOfDigits - len(('%s') % item.number)))
            print "%s%s  %d" % (item.fileName, whiteSpaces, item.number)
    whiteSpaces = ' ' * (longestPath - len("CELKEM:"))
    print "CELKEM:%s  %d" % (whiteSpaces, totalCount)


def keyWords(printMode):
    """Zjisti pocet klicovych slov v jednotlivych souborech a celkem dohromady"""
    longestPath = searchDirectory(os.getcwd(), printMode, 0)  # Ziskani seznamu zdrojovych souboru
    totalCount = 0
    pattern = "([^A-Za-z0-9_#](asm|auto|bool|break|case|catch|char|class|const|const_cast|continue|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)(?!\w))"
    my_re = re.compile(pattern, 0)

    for item in reports:            # Postupne prochazeni zdrojovych souboru
        bodyString = getClearBody(item);  # Uprava souboru - smazani komentaru a retezcu
        found = my_re.findall(bodyString) 
        keywordsCount = len(found)
        item.number = keywordsCount       # Nastaveni poctu vyskytu vsech klicovych slov v souboru
        totalCount += keywordsCount;      # Pricteni poctu vyskytu klicovych slov v tomto souboru k celkovemu poctu ve vsech souborech

    printResult(totalCount, longestPath, printMode)    # Tisk vysledku


def operators(printMode):
    """Zjisti pocet operatoru v jednotlivych souborech a celkem dohromady"""
    longestPath = searchDirectory(os.getcwd(), printMode, 0)  # Ziskani seznamu zdrojovych souboru
    totalCount = 0

    # Vzory k odmazani
    pattern_1 = "(\W(#include[^\n]*|static_cast<[^>]*>|dynamic_cast<[^>]*>|reinterpret_cast<[^>]*>))"

    # Operatory   ->       ::   .            ~   +          -       ++     --   &        *         .*     ->*    /       %       <<       >>       <        <=   >        >=   ==   !=   ^        |         &&   ||     =       *=    /=   %=   +=    -=   &=   ^=    |=    <<=   >>=
    pattern_2 = "((->[^*])|(::)|(\D\.[^\d*])|(~)|(\+[^+=])|(-[^-=])|(\+\+)|(--)|(&[^&=])|(\*[^*=])|(\.\*)|(->\*)|(/[^=])|(%[^=])|(<<[^=])|(>>[^=])|(<[^<=])|(<=)|(>[^>=])|(>=)|(==)|(!=)|(\^[^=])|(\|[^|=])|(&&)|(\|\|)|(=[^=])|(\*=)|(/=)|(%=)|(\+=)|(-=)|(&=)|(\^=)|(\|=)|(<<=)|(>>=)|(![^=]))"
    my_re = re.compile(pattern_2, 0)

    for item in reports:            # Postupne prochazeni zdrojovych souboru
        bodyString = getClearBody(item);    # Uprava souboru - smazani komentaru a retezcu
        bodyList = list(bodyString)
        for match in re.finditer(pattern_1, bodyString):        # Vyjmuti klicovych slov
            bodyList[match.start() : match.end()] = (' ' * (match.end() - match.start()))
        listToTuple = tuple(bodyList)                           # Uprava na typ tuple
        bodyString = ('%c' * len(listToTuple)) % listToTuple    # Prevod tuple na string
        found = my_re.findall(bodyString)
        operatorsCount = len(found)
        item.number = operatorsCount       # Nastaveni poctu vyskytu vsech operatoru v souboru
        totalCount += operatorsCount;      # Pricteni poctu vyskytu operatoru v tomto souboru k celkovemu poctu ve vsech souborech

    printResult(totalCount, longestPath, printMode) # Tisk vysledku


def allIdentifiers(printMode):
    """Zjisti pocet vsech identifikatoru vcetne klicovych slov v jednotlivych souborech a celkem dohromady"""
    longestPath = searchDirectory(os.getcwd(), printMode, 0)  # Ziskani seznamu zdrojovych souboru
    totalCount = 0
    
    pattern_1 = "(\W(#define|#elif|#else|#endif|#error|#if|#ifdef|#ifndef|#include[^\n]*|#line|#pragma|#undef)(?!\w))"
    pattern_2 = "(\W(asm|auto|bool|break|case|catch|char|class|const|const_cast|continue|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)(?!\w))|([a-zA-Z_]{1}[\w]*)"
    my_re = re.compile(pattern_2, 0)
    
    for item in reports:            # Postupne prochazeni zdrojovych souboru
        bodyString = getClearBody(item);    # Uprava souboru - smazani komentaru a retezcu
        bodyList = list(bodyString)
        for match in re.finditer(pattern_1, bodyString):        # Vyjmuti klicovych slov
            bodyList[match.start() : match.end()] = (' ' * (match.end() - match.start()))
        listToTuple = tuple(bodyList)                           # Uprava na typ tuple
        bodyString = ('%c' * len(listToTuple)) % listToTuple    # Prevod tuple na string
        found = my_re.findall(bodyString)
        identifiersCount = len(found)
        item.number = identifiersCount      # Nastaveni poctu vyskytu vsech identifikatoru v souboru
        totalCount += identifiersCount;     # Pricteni poctu vyskytu identifikatoru v tomto souboru k celkovemu poctu ve vsech souborech

    printResult(totalCount, longestPath, printMode) # Tisk vysledku


def identifiers(printMode):
    """Zjisti pocet vsech identifikatoru bez klicovych slov v jednotlivych souborech a celkem dohromady"""
    longestPath = searchDirectory(os.getcwd(), printMode, 0)  # Ziskani seznamu zdrojovych souboru
    totalCount = 0
    
    pattern_1 = "(\W(#define|#elif|#else|#endif|#error|#if|#ifdef|#ifndef|#include[^\n]*|#line|#pragma|#undef|asm|auto|bool|break|case|catch|char|class|const|const_cast|continue|default|delete|do|double|dynamic_cast|else|enum|explicit|export|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|throw|true|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)(?!\w))"
    pattern_2 = "[a-zA-Z_]{1}[\w]*"
    my_re = re.compile(pattern_2, 0)

    for item in reports:            # Postupne prochazeni zdrojovych souboru
        bodyString = getClearBody(item);    # Uprava souboru - smazani komentaru a retezcu
        bodyList = list(bodyString)

        for match in re.finditer(pattern_1, bodyString):        # Vyjmuti klicovych slov
            bodyList[match.start() : match.end()] = (' ' * (match.end() - match.start() ))
        listToTuple = tuple(bodyList)                           # Uprava na typ tuple
        bodyString = ('%c' * len(listToTuple)) % listToTuple    # Prevod tuple na string
        found = my_re.findall(bodyString)
        identifiersCount = len(found)
        item.number = identifiersCount      # Nastaveni poctu vyskytu vsech identifikatoru v souboru
        totalCount += identifiersCount;     # Pricteni poctu vyskytu identifikatoru v tomto souboru k celkovemu poctu ve vsech souborech

    printResult(totalCount, longestPath, printMode)  # Tisk vysledku


def comments(printMode):
    """Zjisti delku komentaru v bytech v jednotlivych souborech a celkem dohromady"""
    longestPath = searchDirectory(os.getcwd(), printMode, 0)  # Ziskani seznamu zdrojovych souboru
    totalCount = 0

    for item in reports:            # Postupne prochazeni zdrojovych souboru
        bodyString = getClearBody(item);    # Uprava souboru - smazani komentaru a retezcu

        item.number = item.commentLength;
        totalCount += item.commentLength;     # Pricteni poctu vyskytu identifikatoru v tomto souboru k celkovemu poctu ve vsech souborech

    printResult(totalCount, longestPath, printMode)  # Tisk vysledku


def findpattern(pattern, printMode):
    """Vyhleda pocet vyskytu retezce v jednotlivych souborech a celkem dohromady"""
    longestPath = searchDirectory(os.getcwd(), printMode, 0)  # Ziskani seznamu zdrojovych souboru
    totalCount = 0
    my_re = re.compile(pattern, 0)

    for item in reports:            # Postupne prochazeni zdrojovych souboru
        bodyString = getFullBody(item);    # Uprava souboru - smazani komentaru a retezcu
        #print bodyString
        found = my_re.findall(bodyString)
        patternsCount = len(found)
        item.number = patternsCount;
        totalCount += patternsCount;        # Pricteni poctu vyskytu identifikatoru v tomto souboru k celkovemu poctu ve vsech souborech

    printResult(totalCount, longestPath, printMode)  # Tisk vysledku


def main():
    # Zpracuje parametry prikazove radky a spusti vykonani prislusne operace
    if((len(sys.argv) >= 2) or (len(sys.argv) <= 4)):
        if(len(sys.argv) == 2):
            if(sys.argv[1] == "--help"):
                printHelp()
            elif(sys.argv[1] == "-k"):
                keyWords("normal")
            elif(sys.argv[1] == "-o"):
                operators("normal")
            elif(sys.argv[1] == "-ik"):
                allIdentifiers("normal")
            elif(sys.argv[1] == "-i"):
                identifiers("normal")
            elif(sys.argv[1] == "-c"):
                comments("normal")
            else:
                print "CHYBA: Nezadali jste spravne parametry. Napovedu vypisete parametrem --help"
        elif(len(sys.argv) == 3):
            if(sys.argv[1] == "-w"):
                findpattern(sys.argv[2], "normal");
            elif((sys.argv[1] == "-p" and sys.argv[2] == "-k") or (sys.argv[1] == "-k" and sys.argv[2] == "-p")):
                keyWords("reduced")
            elif((sys.argv[1] == "-p" and sys.argv[2] == "-o") or (sys.argv[1] == "-o" and sys.argv[2] == "-p")):
                operators("reduced")
            elif((sys.argv[1] == "-p" and sys.argv[2] == "-ik") or (sys.argv[1] == "-ik" and sys.argv[2] == "-p")):
                allIdentifiers("reduced")
            elif((sys.argv[1] == "-p" and sys.argv[2] == "-i") or (sys.argv[1] == "-i" and sys.argv[2] == "-p")):
                identifiers("reduced")
            elif((sys.argv[1] == "-p" and sys.argv[2] == "-c") or (sys.argv[1] == "-c" and sys.argv[2] == "-p")):
                comments("reduced")
            else:
                print "CHYBA: Nezadali jste spravne parametry. Napovedu vypisete parametrem --help"
        elif(len(sys.argv) == 4):
            if(sys.argv[1] == "-w" and sys.argv[3] == "-p"):
                findpattern(sys.argv[2], "reduced")
            elif(sys.argv[1] == "-p" and sys.argv[2] == "-w"):
                findpattern(sys.argv[3], "reduced")
            else:
                print "CHYBA: Nezadali jste spravne parametry. Napovedu vypisete parametrem --help"
        else:
            print "CHYBA: Nezadali jste spravne parametry. Napovedu vypisete parametrem --help"

main()  # Spusteni skriptu
