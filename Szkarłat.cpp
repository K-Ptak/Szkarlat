#include <iostream>
#include <string>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <cctype>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

//-----------------------------KOLORY----------------------------//

int sprawdzanieKoloru(string nazwaKoloru) { //Pobiera nazwę koloru i zwraca odpowiadającą mu liczbę
	if (nazwaKoloru == "blue") return 1;
	else if (nazwaKoloru == "green") return 2;
	else if (nazwaKoloru == "cyan") return 3;
	else if (nazwaKoloru == "red") return 4;
	else if (nazwaKoloru == "purple") return 5;
	else if (nazwaKoloru == "yellow") return 6;
	else if (nazwaKoloru == "lightGray") return 7;
	else if (nazwaKoloru == "gray") return 8;
	else if (nazwaKoloru == "lightBlue") return 9;
	else if (nazwaKoloru == "lightGreen") return 10;
	else if (nazwaKoloru == "neonBlue") return 11;
	else if (nazwaKoloru == "rose") return 12;
	else if (nazwaKoloru == "pink") return 13;
	else if (nazwaKoloru == "lightYellow") return 14;
	else if (nazwaKoloru == "white") return 15;
	else return 15;
}
void resetKoloru() { //Resetuje kolor tekstu (zmienia na biały)
	SetConsoleTextAttribute(hConsole, 15);
}
void kolorowyTekst(string tekst, string kolor) { //Funkcja wypisująca podany tekst w danym kolorze
	SetConsoleTextAttribute(hConsole, sprawdzanieKoloru(kolor));
	cout << tekst << " ";
	resetKoloru();
}

void kolorowyTekstZPliku(string adresPliku, string kolor) {
	ifstream plikIntro(adresPliku);
	string tekst{};
	SetConsoleTextAttribute(hConsole, sprawdzanieKoloru(kolor));
	while (getline(plikIntro, tekst))
	{
		cout << tekst << "\n";
	}
	resetKoloru();
}

//---------------------------BAZY ZASOBÓW---------------------------//

vector<string> ZbrojeBZ;
vector<string> PrzedmiotyBZ;
vector<string> BronieBZ;
vector<string> PrzeciwnicyBZ;
vector<string> NpcsBZ;
vector<string> UmiejetnosciBZ;
vector<string> LokacjeBZ;
vector<string> KlasyBZ;

vector<int> MapaGry;
int x_max = 4;	//te zmienne służą jako maksymalne pozycje w MapaGry zanim wyjdzie się poza zakres
int x_min = 0;
int y_max = 4;
int y_min = 0;

bool wczytajDoBazy(string adresBazy, vector<string>&baza, int liczbaObiektow, int liczbaParametrow) {//Wczytuje dane do podanej bazy zasobów i zwraca true/false zależnie czy operacja się udała
	ifstream plikBazy("../sources/databases/" + adresBazy);

	if (!plikBazy.is_open() || plikBazy.fail()) {
		return false;
	}
	else {
		string dane{};
		int rozmiar = liczbaObiektow * liczbaParametrow;
		baza.resize(rozmiar);
		for (int i = 0; i < rozmiar; i++) {
			getline(plikBazy, dane);
			baza[i] = dane;
		}
			plikBazy.close();
		}
	return true;
}

string wypiszZBazy(vector<string>& baza, int id, int index) {
	int dane{};
	if (baza == KlasyBZ) {
		dane = (id * 2) + index;
	}
	if (baza == BronieBZ || baza == ZbrojeBZ) {
		dane = (id * 4) + index;
	}
	if (baza == LokacjeBZ || baza == UmiejetnosciBZ || baza == PrzedmiotyBZ) {
		dane = (id * 5) + index;
	}
	return baza[dane];
}

//----------------------------Gracz-----------------------------//
class Bohater {
public:
	string imie{};
	int klasaPostaci{};
	int poziomPostaci{};
	int doswiadczenie{};
	int zdrowieMax{};
	int zdrowie{};
	int zloto{};
	int szkarlatMax{};
	int szkarlat{};
	float punktyReputacji{};
	int poz_x{};
	int poz_y{};
	string reputacja{};
	int bron;
	int zbroja;
	int ekwipunek[10]{};
	int umiejetnosci[10]{};
	void kalkulowanieReputacji() {//Returns players' reputacja
		if (punktyReputacji < 25) reputacja = "Wróg publiczny";
		else if (punktyReputacji > 75) reputacja = "Nadzieja ludu";
		else reputacja = "Neutralna";
	}
	void zwracanieKlasy() {
		switch (klasaPostaci) {
		case 0:
			cout<<wypiszZBazy(KlasyBZ, 0, 1);
			break;
		case 1:
			cout << wypiszZBazy(KlasyBZ, 1, 1);
			break;
		case 2:
			cout << wypiszZBazy(KlasyBZ, 2, 1);
			break;
		}
	}
	void statystyki() {//Wypisuje statystyki gracza
		kalkulowanieReputacji();
		zwracanieKlasy();
		cout << " " << imie << ", poziom " << poziomPostaci << "." << endl;
		cout << "Posiadasz " << doswiadczenie << " punktów doświadczenia." << endl;
		cout << "Aktualnie masz " << zloto << " złota." << endl;
		cout << "Opinia publiczna: " << reputacja << "." << endl;
	}
	void wypiszUmiejetnosci() {//Wypisuje poznane przez gracza umiejetnosci
		cout << "Poznane umiejętności:" << endl;
		for (int i = 0; i < 10; i++) {
			if (umiejetnosci[i] != -1) {
				kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, ekwipunek[i], 1), "blue");
				cout << "\t";
			}
			if (i > 0 && i % 2 == 0) {
				cout << endl;
			}
		}
	}
	void sortowanieEkwipunku() { // "przesuwa" przedmioty do pierwszych pozycji ekwipunku, a puste pola (-1) przesuwa na koniec
		for (int i = 0; i < 9; i++) {
			if (ekwipunek[i] == -1) {
				swap(ekwipunek[i], ekwipunek[(i + 1)]);
			}
		}
	}
	void wypiszEkwipunek() { //wypisuje ekwipunek gracza
		cout << "-----------------------------------------" << endl;
		cout << "W ręce: ";
		kolorowyTekst(wypiszZBazy(BronieBZ, bron, 1), "red");
		cout << endl;
		cout << "Na sobie: ";
		kolorowyTekst(wypiszZBazy(ZbrojeBZ, zbroja, 1), "cyan");
		cout << endl;
		cout << "-----------------------------------------" << endl;
		cout << "W ekwipunku: " << endl;

		for (int i = 0; i < 10; i++) {
			if (ekwipunek[i] != -1) {
				kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, ekwipunek[i], 1),"green");
				cout << "\t";
			}
			if (i > 0 && i % 2 == 0) {
				cout << endl;
			}
		}
	}
	void zapisz() {//Zapisuje dane gracza
		string adresPlikuZapisu = "../saves/" + imie + ".txt";
		ofstream PlikZapisu(adresPlikuZapisu);
		PlikZapisu << imie << '\n' << klasaPostaci << '\n' << poziomPostaci << '\n' << doswiadczenie << '\n' << zdrowieMax << '\n' << zdrowie << '\n' << zloto << '\n' << szkarlatMax << '\n' << szkarlat << '\n' << punktyReputacji << '\n' << poz_x << '\n' << poz_y << '\n' << reputacja << '\n' << bron << '\n' << zbroja << '\n';
		int inventorySize = 10;
		for (int i = 0; i < inventorySize; i++) {
			PlikZapisu << ekwipunek[i] << ';';
		}
		PlikZapisu << '\n';

		int iloscUmiejetnosci = 10;
		for (int i = 0; i < iloscUmiejetnosci; i++) {
			PlikZapisu << umiejetnosci[i] << ';';
		}
		PlikZapisu.close();
	}
	void wczytaj() {//Wczytuje dane Gracza
		string adresOdczytywanegoPliku = "../saves/" + imie + ".txt";
		ifstream odczytywanyPlik(adresOdczytywanegoPliku);

		char bufor; //do pomijania znaku ';'
		odczytywanyPlik >> imie >> klasaPostaci >> poziomPostaci >> doswiadczenie >> zdrowieMax >> zdrowie >> zloto >> szkarlatMax >> szkarlat >> punktyReputacji >> poz_x >> poz_y >> reputacja >> bron >> zbroja;
		for (int i = 0; i < 10; i++) {
			odczytywanyPlik >> ekwipunek[i];
			odczytywanyPlik >> bufor;
		}
		for (int i = 0; i < 10; i++) {
			odczytywanyPlik >> umiejetnosci[i];
			odczytywanyPlik >> bufor;
		}
		odczytywanyPlik.close();
	}
	void czyNowyPoziom() { //funkcja sprawdza czy gracz osiągnął nowy poziom, jesli tak to informuje go o tym i zwieksza statystyki
		int wzorNaPoziom = poziomPostaci * 20;
		if (doswiadczenie >= wzorNaPoziom) {
			poziomPostaci++;
			cout << "---------------------" << endl;
			cout << "Nowy poziom!" << endl;
			cout << "Twój aktualny poziom to: " << poziomPostaci << endl;
			zdrowieMax += 10;
			szkarlatMax += 5;
		}
	}
	void krotkieStatystyki() {//funkcja wypisuje tylko liczbe punktów życia, szkarłatu i doświadczenia. Wyświetlana jest przed każdym promptem od użytkownika
		cout << "<" << zdrowie << "pz " << szkarlat << "szk " << doswiadczenie << "exp> ";
	}
};
Bohater Gracz;

//---------------------------MAPA GRY----------------------------//

bool inicjalizacjaMapy(int liczbaRzedow, int liczbaKolumn) {
	int idLokacji = 0;
	int rozmiar = liczbaRzedow * liczbaKolumn;
	MapaGry.resize(rozmiar);

	for (int i = 0; i < rozmiar; i++) {
		MapaGry[i] = idLokacji;
		idLokacji++;
	}
	if (idLokacji != (rozmiar)) return false;
	else return true;
}

void wypiszLokacje() {
	cout << endl;
	int id = ((5 * Gracz.poz_y) + Gracz.poz_x); //obliczanie id lokacji na mapie
	cout << wypiszZBazy(LokacjeBZ, id, 1);
	cout << endl;
	cout << wypiszZBazy(LokacjeBZ, id, 2);
	cout << endl;
	cout << wypiszZBazy(LokacjeBZ, id, 3);
	cout << endl;
	if (wypiszZBazy(LokacjeBZ, id, 4) != "0") {
		cout << wypiszZBazy(LokacjeBZ, id, 4);
		cout << endl;
	}
}

bool CzyMoznaPrzejsc(string kierunek) { //funkcja sprawdza czy gracz może przejść ze "startowej" lokacji do kolejnej (przeciwdziała wychodzeniu poza mape albo przez fabularne "ściany")
	int id = ((5 * Gracz.poz_y) + Gracz.poz_x);
	if (kierunek == "n" && (id == 5 || id == 6 || id == 7 || id == 8 || id == 9 || id == 11 || id == 12 || id == 13 || id == 14 || id == 16 || id == 17 || id == 19 || id == 20 || id == 22 || id == 23 || id == 24)) {
		return true;
	}
	else if (kierunek == "e" && (id == 0 || id == 2 || id == 3 || id == 5 || id == 7 || id == 8 || id == 10 || id == 11 || id == 12 || id == 13 || id == 15 || id == 16 || id == 18 || id == 20 || id == 23)) {
		return true;
	}
	else if (kierunek == "s" && (id == 0 || id == 1 || id == 2 || id == 3 || id == 4 || id == 6 || id == 7 || id == 8 || id == 9 || id == 11 || id == 12 || id == 14 || id == 15 || id == 17 || id == 18 || id == 19)) {
		return true;
	}
	else if (kierunek == "w" && (id == 1 || id == 3 || id == 4 || id == 6 || id == 8 || id == 9 || id == 11 || id == 12 || id == 13 || id == 14 || id == 16 || id == 17 || id == 19 || id == 21 || id == 24)) {
		return true;
	}
	else {
		return false;
	}
}

void ruchNaMapie(string kierunek) {
	if ((kierunek == "n" || kierunek == "N") && Gracz.poz_y!=y_min && CzyMoznaPrzejsc("n")) {
		Gracz.poz_y--;
		wypiszLokacje();
	}
	else if ((kierunek == "s" || kierunek == "S") && Gracz.poz_y!=y_max && CzyMoznaPrzejsc("s")) {
		Gracz.poz_y++;
		wypiszLokacje();
	}
	else if ((kierunek == "w" || kierunek == "W") && Gracz.poz_x!=x_min && CzyMoznaPrzejsc("w")) {
		Gracz.poz_x--;
		wypiszLokacje();
	}
	else if ((kierunek == "e" || kierunek == "E") && Gracz.poz_x!=x_max && CzyMoznaPrzejsc("e")) {
		Gracz.poz_x++;
		wypiszLokacje();
	}
	else {
		cout << endl;
		cout << "Tam niestety nie pójdziesz" << endl;
	}
}

//---------------------CZY PLIK ISTNIEJE?-----------------------//
bool istnieje(string adresPliku){//Funkcja zwracająca true/false zależnie od tego czy dany plik istnieje
	ifstream plik(adresPliku);
	if (!plik) {
		return false;
	}
	else {
		return true;
	}
}

//---------------------------INTERPRETER-------------------------//

int tokenWyjscia = 1; //zmienna globalna używana by kończyć grę (kończy pętlę rozgrywki)

int tokenKontrolny = 1; //zmienna używana by funkcja wprowadzPolecenie() nie została wywoływana sama z siebie po wykonaniu danych funkcji (niektóre funkcje interpretera które pobierają od użytkownika input sprawiają że wprowadzPolecenie() samo się wykonuje, wyrzucając krotkieStatystyki() i niepoprawnePolecenie());

void niepoprawnePolecenie() {
	cout << "hmm?" << endl;
}

//Przeładowana funkcja *interpreter* przetwarza komende daną przez użytkownika, zależnie od tego jak złożona była (ile miała słów) i wykonuje daną akcje
void interpreter(string arg1) {
	if (arg1 == "ekwipunek" || arg1 == "ekw" || arg1 == "eq") {
		Gracz.wypiszEkwipunek();
	}
	else if (arg1 == "n" || arg1 == "N" || arg1 == "s" || arg1 == "S" || arg1 == "e" || arg1 == "E" || arg1 == "w" || arg1 == "W") {
		ruchNaMapie(arg1);
	}
	else if (arg1 == "zapisz" || arg1 == "save") {
		Gracz.zapisz();
		cout << endl;
		kolorowyTekst("----------------------------------", "green");
		cout << endl;
		cout << "\t";
		kolorowyTekst("Zapisano grę", "green");
		cout << endl;
		kolorowyTekst("----------------------------------", "green");
		cout << endl;
	}
	else if (arg1 == "spojrz") {
		wypiszLokacje();
	}
	else if (arg1 == "statystyki" || arg1 == "staty") {
		Gracz.statystyki();
	}else if (arg1 == "umi" || arg1 == "umiejetnosci" || arg1 == "skille") {
		Gracz.wypiszUmiejetnosci();
	}else if (arg1 == "wyjscie" || arg1 == "wyjdz" || arg1 == "exit") {
		cout << "Chcesz zapisać grę przed wyjściem? tak/nie" << endl;
		string odp;
		cin >> odp;
		if (odp == "tak") {
			Gracz.zapisz();
		}
		tokenWyjscia = 0;
	}else {
		niepoprawnePolecenie();
	}
}
void interpreter(string arg1, string arg2) {
	if (arg1 == "obejrz" || arg1 == "obejrzyj") {
		cout << endl;
		if (arg2 == "pancerz" || arg2 == "zbroja" || arg2 == "ubior" || arg2 == "ubranie") {
			kolorowyTekst(wypiszZBazy(ZbrojeBZ, Gracz.zbroja, 1), "cyan");
			cout << endl;
			cout << wypiszZBazy(ZbrojeBZ, Gracz.zbroja, 3) << endl;
		}
		else if (arg2 == "bron") {
			kolorowyTekst(wypiszZBazy(BronieBZ, Gracz.bron, 1), "red");
			cout << endl;
			cout << wypiszZBazy(BronieBZ, Gracz.bron, 3) << endl;
		}
		else if (arg2 == "przedmiot" || arg2 == "przedmioty") {
			cout << endl;
			int zajeteMiejsca = 0;
			for (int i = 0; i < 10; i++) {
				if (Gracz.ekwipunek[i] != -1) {
					zajeteMiejsca++;
				}
			}
			if (zajeteMiejsca > 0) {
				kolorowyTekst("Wybierz przedmiot który chcesz obejrzeć:", "green");
				cout << endl;
				for (int i = 0; i < 10; i++) {
					if (Gracz.ekwipunek[i] != -1) {
						cout << i << ". ";
						kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[i], 1), "green");
						cout << "\t";
					}
					if (i > 0 && i % 2 == 0) {
						cout << endl;
					}
				}

				int ogladane;
				cin >> ogladane;
				if (ogladane >= 0 && ogladane <= zajeteMiejsca) {
					cout << endl;
					kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 1), "green");
					cout << endl;
					if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 3) == "leczenie") {
						cout << "Leczy ";
						cout<<wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 2);
						cout << " punktów zdrowia." << endl;
					}
					else if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 3) == "szkarlat") {
						cout << "Przywraca ";
						cout<<wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 2);
						cout << " szkarłatu." << endl;
					}
					cout<<wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 4);
					cout << endl;
					cout << endl;
				}
				else {
					cout << "Nie masz takiego przedmiotu" << endl;
				}
			}
			else {
				cout << "Twój ekwipunek jest pusty." << endl;
			}
		}
		else {
			niepoprawnePolecenie();
		}
		cout << endl;
	}
	else if (arg1 == "opis" && (arg2 == "umi" || arg2 == "umiejetnosci" || arg2 == "skilli")) {
		cout << endl;
		int zajeteMiejsca = 0;
		for (int i = 0; i < 10; i++) {
			if (Gracz.umiejetnosci[i] != -1) {
				zajeteMiejsca++;
			}
		}

		kolorowyTekst("Wybierz umiejętność:", "cyan");
		cout << endl;
		for (int i = 0; i < 10; i++) {
			if (Gracz.umiejetnosci[i] != -1) {
				cout << i << ". ";
				kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[i], 1), "blue");
				cout << "\t";
			}
			if (i > 0 && i % 2 == 0) {
				cout << endl;
			}
		}

		int umiejetnosc;
		cin >> umiejetnosc;
		cout << endl;
		if (umiejetnosc >= 0 && umiejetnosc <= zajeteMiejsca) {
			kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 1), "blue");
			cout << endl;
			cout << "Koszt szkarłatu: ";
			kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 2), "red");
			cout << endl;
			cout << "Obrażenia: ";
			kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 2), "rose");
			cout << endl;
			cout << wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 4);
			cout << endl;
		}
		else {
			cout << "Nie znasz takiej umiejętności." << endl;
		}

	}
	else if (arg1 == "uzyj" && (arg2 == "przedmiot" || arg2 == "przedmiotu")) {
		cout << endl;
		int zajeteMiejsca = 0;
		for (int i = 0; i < 10; i++) {
			if (Gracz.ekwipunek[i] != -1) {
				zajeteMiejsca++;
			}
		}
		if (zajeteMiejsca > 0) {
			kolorowyTekst("Którego przedmiotu chcesz użyć?", "cyan");
			cout << endl;
			for (int i = 0; i < 10; i++) {
				if (Gracz.ekwipunek[i] != -1) {
					cout << i << ". ";
					kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[i], 1), "green");
					cout << "\t";
				}
				if (i > 0 && i % 2 == 0) {
					cout << endl;
				}
			}

			int przedmiot;
			cin >> przedmiot;
			if (przedmiot >= 0 && przedmiot <= zajeteMiejsca) {
				int wartosc{};
				wartosc = stoi(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 2));
				if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 3) == "leczenie") {
					Gracz.zdrowie += wartosc;
					if (Gracz.zdrowie > Gracz.zdrowieMax) {
						Gracz.zdrowie = Gracz.zdrowieMax;
					}
					cout << "Użyłeś ";
					kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 1), "green");
					cout << endl;
					Gracz.ekwipunek[przedmiot] = -1;
					Gracz.sortowanieEkwipunku();
				}
				else if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 3) == "szkarlat") {
					Gracz.szkarlat += wartosc;
					if (Gracz.szkarlat > Gracz.szkarlatMax) {
						Gracz.szkarlat = Gracz.szkarlatMax;
					}
					cout << "Użyłeś ";
					kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 1), "green");
					cout << endl;
					Gracz.ekwipunek[przedmiot] = -1;
					Gracz.sortowanieEkwipunku();
				}
				else {
					cout << "Nie można użyć tego przedmiotu" << endl;
				}

			}
			else {
				cout << "Nie masz takiego przedmiotu." << endl;
			}
		}
		else {
			cout << "Twój ekwipunek jest pusty." << endl;
		}
	}
	else if ((arg1 == "wyrzuc" || arg1 == "zostaw") && arg2 == "przedmiot") {
		cout << endl;
		int zajeteMiejsca = 0;
		for (int i = 0; i < 10; i++) {
			if (Gracz.ekwipunek[i] != -1) {
				zajeteMiejsca++;
			}
		}
		if (zajeteMiejsca > 0) {
			kolorowyTekst("Który przedmiot chcesz wyrzucić?", "rose");
			cout << endl;
			for (int i = 0; i < 10; i++) {
				if (Gracz.ekwipunek[i] != -1) {
					cout << i << ". ";
					kolorowyTekst(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[i], 1), "green");
					cout << "\t";
				}
				if (i > 0 && i % 2 == 0) {
					cout << endl;
				}
			}

			int wyrzucane;
			cin >> wyrzucane;
			if (wyrzucane >= 0 && wyrzucane <= zajeteMiejsca) {
				Gracz.ekwipunek[wyrzucane] = -1;
				cout << "Przedmiot został wyrzucony." << endl;
			}
			else {
				cout << "Nie masz takiego przedmiotu" << endl;
			}
		}
		else {
			cout << "Twój ekwipunek jest pusty." << endl;
		}
	}
	else {
		niepoprawnePolecenie();
	}
}

void interpreter(string arg1, string arg2, string arg3) {
	cout << "3";
}
void interpreter(string arg1, string arg2, string arg3, string arg4) {
	cout << "4";
}

void wprowadzPolecenie() { //Pobiera i dzieli polecenie gracza na pojedyncze składowe komendy
	if (tokenKontrolny == 1) { //ten token zmienia się tylko przy niektórych poleceniach, które wymagają inputu od użytkownika
		Gracz.krotkieStatystyki();
	}
	else {
		tokenKontrolny = 1;
	}
	string LiniaKomend;
	getline(cin, LiniaKomend);

	if (LiniaKomend != "") {
		string bufor;
		stringstream ss(LiniaKomend);

		vector<string> parametry;

		while (ss >> bufor) {
			parametry.push_back(bufor);
		}

		switch (parametry.size()) {
		case 1:
			interpreter(parametry[0]);
			break;
		case 2:
			interpreter(parametry[0], parametry[1]);
			tokenKontrolny = 0;
			break;
		case 3:
			interpreter(parametry[0], parametry[1], parametry[2]);
			break;
		case 4:
			interpreter(parametry[0], parametry[1], parametry[2], parametry[3]);
			break;
		default:
			niepoprawnePolecenie();
			break;
		}
	}
}

//-----------------------TWORZENIE POSTACI-----------------------//

void tworzeniePostaci() {
	string tekstTP;
	ifstream plikTP("../sources/tworzeniePostaci.txt");

	getline(plikTP, tekstTP);
	cout << tekstTP << " ";
	do {
		cin >> Gracz.imie;	//pobiera imie od gracza i przypisuje do Gracz.imie
	} while (Gracz.imie == "saves-list");
	cout << endl;

	getline(plikTP, tekstTP);
	cout << tekstTP << " ";

	cout << endl << endl;

	string kolorTP{};
	for (int i = 0; i <= 2; i++) {
		if (i == 0) kolorTP = "green";
		else if (i == 1) kolorTP = "cyan";
		else if (i == 2) kolorTP = "purple";
		getline(plikTP, tekstTP);
		cout << i + 1 << ". ";
		kolorowyTekst(tekstTP, kolorTP);
		getline(plikTP, tekstTP);
		cout << endl;
		cout << tekstTP;
		cout << endl << endl;
	}
	string klasaTP;
	for (;;) { // pobiera od gracza nazwe klasy do momentu az bedzie prawidlowa, nastepnie przypisuje adekwatną wartość do Gracz.klasaPostaci 
		cin >> klasaTP;
		if (klasaTP == "Lowca" || klasaTP == "lowca" || klasaTP == "1") {
			Gracz.klasaPostaci = 0;
			break;
		}
		else if (klasaTP == "Archont" || klasaTP == "archont" || klasaTP == "2") {
			Gracz.klasaPostaci = 1;
			break;
		}
		else if (klasaTP == "Klucznik" || klasaTP == "klucznik" || klasaTP == "3") {
			Gracz.klasaPostaci = 2;
			break;
		}
	}

	getline(plikTP, tekstTP);
	kolorowyTekst(tekstTP, "red");
	cout << "Tak/Nie" << endl << endl;

	string wstepTP;
	cin >> wstepTP;
	if (wstepTP == "Tak" || wstepTP == "tak" || wstepTP == "TAK" || wstepTP=="ok" || wstepTP=="OK" || wstepTP=="Ok") {
		system("cls");
			while (!plikTP.eof()) {
			getline(plikTP, tekstTP);
			cout << tekstTP << endl;
			Sleep(2000);
			}
	}

	plikTP.close();

	Gracz.poziomPostaci = 1;
	Gracz.doswiadczenie = 0;
	Gracz.zdrowieMax = 50;
	Gracz.zdrowie = Gracz.zdrowieMax;
	Gracz.zloto = 25;
	Gracz.szkarlatMax = 20;
	Gracz.szkarlat = Gracz.szkarlatMax;
	Gracz.punktyReputacji = 50;
	Gracz.poz_x = 2;
	Gracz.poz_y = 4;
	Gracz.kalkulowanieReputacji();

	for (int i = 0; i < 10; i++) {//zerowanie ekwipunku i umiejetnosci
		Gracz.umiejetnosci[i] = -1;
		Gracz.ekwipunek[i] = -1;
	}


	for (int i = 0; i <= 2; i++) {
		if (Gracz.klasaPostaci == i) {
			Gracz.bron = i;
			Gracz.zbroja = i;
			Gracz.umiejetnosci[0] = i;
		}
	}
	Gracz.ekwipunek[0] = 0;
	Gracz.ekwipunek[1] = 1;

	ofstream plikZZapisami("../saves/saves-list.txt", ios::app);
	plikZZapisami << Gracz.imie << "\n";
	plikZZapisami.close();

	Gracz.zapisz();
	tokenKontrolny = 0;
};

//--------------------WYPISYWANIE ZAPISÓW------------------------//

void wypiszPliki() {
	
	if (istnieje("../saves/saves-list.txt")) {
		cout << "Wybierz postać do wczytania:" << endl;

			string odczytane;
			string puste;
			int index = 0;
			ifstream zliczanie("../saves/saves-list.txt");
			while (getline(zliczanie, puste)) {
					index++;
			}
			zliczanie.close();

			string* zapisy = new string[index];
			index = 0;
		
			ifstream odczytywanieZapisow("../saves/saves-list.txt");
			while (getline(odczytywanieZapisow, odczytane)) {
				zapisy[index] = odczytane;
				cout << zapisy[index] << "\t";
				if (index % 3 == 0 && index!=0) {
					cout << endl;
				}
				index++;
			}
			odczytywanieZapisow.close();

			cout << endl;

			string nazwa;
			getline(cin,nazwa);

			for (int i = 0; i < index; i++) {
				if (nazwa == zapisy[i]) {
					Gracz.imie = nazwa;
					Gracz.wczytaj();
				}
			}
			system("cls");
	}else {
		cout << "Nie znaleziono żadnych plików zapisu!" << endl;
	}
}

//----------------NOWA GRA I WCZYTAJ GRE-------------------------//

void nowaGra() {
	tworzeniePostaci();
	wypiszLokacje();
}

void wczytajGre() {
	wypiszPliki();
	wypiszLokacje();
}

//--------------SPRAWDZENIE I WCZYTANIE ASSETÓW-----------------//
bool WczytywanieAssetow(){//Sprawdza czy wszystkie wymagane pliki istnieją i wczytuje dane do odpowiednich baz zasobów
	kolorowyTekst("\tSzkarłat", "red");
	cout << endl;

	if (istnieje("../sources/tworzeniePostaci.txt") && istnieje("../sources/intro.txt") && istnieje("../sources/placeholder.txt")) {
		kolorowyTekst("Poprawnie zweryfikowano pliki źródłowe...","rose");
		cout << endl;
	}
	else {
		cout << "Wykryto błąd wczytywania plików";
		return false;
	}

	if (wczytajDoBazy("ZbrojeBZ.txt", ZbrojeBZ, 6, 4) && wczytajDoBazy("PrzedmiotyBZ.txt", PrzedmiotyBZ, 2, 5) && wczytajDoBazy("BronieBZ.txt", BronieBZ, 6, 4) && wczytajDoBazy("PrzeciwnicyBZ.txt", PrzeciwnicyBZ, 2, 6) && wczytajDoBazy("NpcsBZ.txt", NpcsBZ, 6, 5) && wczytajDoBazy("UmiejetnosciBZ.txt", UmiejetnosciBZ, 6, 5) && wczytajDoBazy("LokacjeBZ.txt", LokacjeBZ, 25, 5) && wczytajDoBazy("KlasyBZ.txt", KlasyBZ, 3, 2)) {
		kolorowyTekst("Poprawnie wczytano wszystkie bazy zasobów...", "rose");
		cout << endl;
	}
	else {
	cout << "Wykryto błąd wczytywania baz zasobów";
		return false;
	}

	if (inicjalizacjaMapy(5, 5)) {
		kolorowyTekst("Poprawnie zainicjowano mapę gry...", "rose");
		cout << endl;
	}
	else {
		cout << "Wykryto błąd inicjowania mapy";
		return false;
	}


	kolorowyTekst("Wszystkie pliki są poprawne!","green");
	cout << endl << endl << endl << endl << endl;

		return true;
}

//----------------------------------------------------------------//

int start() {

		kolorowyTekstZPliku("../sources/intro.txt", "red");
		cout << endl << endl;
		cout.width(20);
		cout << "\t\t\t\t\t[N] Nowa gra" << endl;
		cout.width(20);
		if (istnieje("../saves/saves-list.txt")) {
			cout << "\t\t\t\t\t[Z] Załaduj poprzedni zapis" << endl;
			cout.width(20);
		}
		cout << "\t\t\t\t\t[W] Wyjdź z gry" << endl;

	for(;;){
		char introValue = _getch();
		tolower(introValue);

		if (introValue == 'n') return 1;
		else if (istnieje("../saves/saves-list.txt") && introValue == 'z') return 2;
		else if (introValue == 'w') return 3;
	}
}

int main(){
	setlocale(LC_ALL, "Polish");
	if (WczytywanieAssetow()) {
		switch (start()) {
		case 1:
			nowaGra();
			break;
		case 2:
			wczytajGre();
			break;
		default:
			return EXIT_SUCCESS;
			break;
		}
		while (tokenWyjscia != 0) {
			wprowadzPolecenie();
		}
	}
	else {
		return 0;
	}
}