- kompilacja
	- inf115588.s.c  -o serwer 
	- inf115588.k.c -o klient

instrukcja uruchomienia: otwieramy terminal dla serwera, po kompilacji wpisujemy
./serwer. Nastepnie uruchamiamy osobny terminal, i po kompilacji wpisujemy ./klient
(w odpowiednim folderze). Mozemy otworzyc osobne terminale dla kolejnych klientow.

zawartosc plikow:
serwer1.c zawiera struktury i funkcje obslugujace klienta, takie jak logowanie(), wczytywanie(), lista_grup(), w funcji main() tworzy kolejke, i uzywa funckcje wczytywanie(), ktore powoduje wczytanie danych klientow z pliku dane.txt. nastepnie 
w nieskoniczonej petli while(1) czeka na komunikaty od klienta.

klient.c zawiera struktury, funkcje login() i odwolania do poszczegolnych opcji za pomoca switch.
