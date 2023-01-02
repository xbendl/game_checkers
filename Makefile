# Projekt: Dama
# Autor:   Jaroslav Bendl (xbendl00)
#          Vit Kopriva (xkopri05)
#          Milos Chmel (xchmel15)
#          Maxim Kryzhanovsky (xkryzh00)
# Datum:   20.4.2008
# 
# Pouziti:
#   - preklad:      make
#   - zabalit:      make pack
#   - vycistit:     make clean

all:
	$(MAKE) -C src

pack:
	tar cvzf xbendl00.tar.gz *

clean:
	$(MAKE) -C src clean
	rm dama2008 damaserver2008
