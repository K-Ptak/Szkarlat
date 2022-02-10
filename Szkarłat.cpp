//Kamil Ptak Grupa 4/8
#include <iostream>
#include <string>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <cctype>
#include <vector>
#include <sstream>
#include <fstream>
#include <random>
#include <ctime>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //Uchwyt konsoli, na jego podstawie działają funkcje kolorowyTekst() i kolorowyTekstZPliku(), pozwala na modyfikacje bufora konsoli

//--------------------LICZBY PSEUDOLOSOWE------------------------//
mt19937 generator(time(nullptr));
uniform_int_distribution<int> losowanieZeroJedynkowe(0, 1);
uniform_int_distribution<int> losowanieJednaCzwarta(0, 3);

//--------------------------TOKENY-------------------------------//

//tokeny mają na celu upewnienie się czy program funkcjonuje prawidłowo pomijając/wywołując daną funkcje zależnie od sytuacji, w której są użyte.

int tokenWyjscia = 1; //zmienna globalna używana by kończyć grę (kończy pętlę rozgrywki)

int tokenKontrolny = 1; //zmienna używana by funkcja wprowadzPolecenie() nie została wywoływana sama z siebie po wykonaniu danych funkcji (niektóre funkcje interpretera które pobierają od użytkownika input sprawiają że wprowadzPolecenie() samo się wykonuje, wyrzucając krotkieStatystyki() i niepoprawnePolecenie());

int tokenIntro = 0; //zmienna używana w celu wywołania wprowadzającego dialogu, jeśli wynosi ona 1 (jej wartość zmienia się zależnie od wyboru użytkownika czy chce zobaczyć wprowadzenie)

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

void kolorowyTekstZPliku(string adresPliku, string kolor) { //Modyfikacja funkcji kolorowyTekst(), która zamiast pobierać string, odczytuje dane z pliku (tylko używane przy Intrze gry)
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

//zmienne graniczne mapy
int x_max = 4;	//te zmienne służą jako maksymalne pozycje mapy gry, mają uniemożliwić graczowi wyjście "poza mape" (wykroczenie poza wektor)
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

string wypiszZBazy(vector<string>& baza, int id, int index) {//funkcja wypisuje z podanej bazy daną linijkę danych. Każdy element w danej bazie ma tą samą liczbę elementów, przez co elementy występują na zasadzie wielokrotności czynnika (z wyjątkiem dla pierwszego elementu, dla którego jest to 0)
	int dane{};
	if (baza == KlasyBZ) {
		dane = (id * 3) + index;
	}
	if (baza == BronieBZ || baza == ZbrojeBZ) {
		dane = (id * 4) + index;
	}
	if (baza == LokacjeBZ || baza == UmiejetnosciBZ || baza == PrzedmiotyBZ) {
		dane = (id * 5) + index;
	}
	if (baza == PrzeciwnicyBZ || baza == NpcsBZ) {
		dane = (id * 6) + index;
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
	void kalkulowanieReputacji() {//Zwraca reputację gracza
		if (punktyReputacji < 25) reputacja = "Wróg publiczny";
		else if (punktyReputacji > 75) reputacja = "Nadzieja ludu";
		else reputacja = "Neutralna";
	}
	void zwracanieKlasy() {//Wypisuje z bazy nazwę klasy gracza
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
				kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, umiejetnosci[i], 1), "blue");
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
		cout << "W kieszeni: ";
		stringstream ss;	//stringstream służy tutaj aby dokonać konwersji z int do string, żeby funkcja kolorowyTekst() wyświetlała dobre wartości
		string iloscZlota = "";
		ss << zloto;
		iloscZlota = ss.str();
		kolorowyTekst(iloscZlota, "yellow");
		cout << "złota"<<endl;
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
			if (poziomPostaci == 3) {
				for (int i = 0; i < 10; i++) {
					if (umiejetnosci[i] == -1) {
						int idUmiejetnosci = poziomPostaci + klasaPostaci;
						umiejetnosci[i] = stoi(wypiszZBazy(UmiejetnosciBZ, idUmiejetnosci, 0));
						break;
					}
				}
			}
		}
	}
	void krotkieStatystyki() {//funkcja wypisuje tylko liczbe punktów życia, szkarłatu i doświadczenia. Wyświetlana jest przed każdym promptem od użytkownika
		string ks="";
		string str;
		stringstream ss;	//stringstream służy tutaj aby dokonać konwersji z int do string, żeby funkcja kolorowyTekst() wyświetlała dobre wartości
		ks += "<";
		ss << zdrowie;
		str = ss.str();
		ss.str("");
		ks += str;
		ks += "pz ";
		ss << szkarlat;
		str = ss.str();
		ss.str("");
		ks += str;
		ks += "szk ";
		ss << doswiadczenie;
		str = ss.str();
		ks += str;
		ks += "exp> ";
		kolorowyTekst(ks, "cyan");
	}
};
Bohater Gracz;

//-------------------------SYSTEM WALKI--------------------------//

void Walka(int id_przeciwnika) { //funkcja aktywowana jeśli gracz zainicjuje walke, konczy się gdy gracz albo przeciwnik będzie miał 0 lub mniej punktów życia
	int tokenWalki = 0; //token używany by kończyć walkę, przyjmuje wartość 1, po wygranej i przegranej walce
	
	//zmienne przeciwnika
	string przeciwnikNazwa = wypiszZBazy(PrzeciwnicyBZ, id_przeciwnika, 1);
	int przeciwnikObrazenia = stoi(wypiszZBazy(PrzeciwnicyBZ, id_przeciwnika, 2));
	int przeciwnikZdrowie = stoi(wypiszZBazy(PrzeciwnicyBZ, id_przeciwnika, 3));
	int przeciwnikMaxZdrowie = przeciwnikZdrowie;
	int przeciwnikPrzedmiot = stoi(wypiszZBazy(PrzeciwnicyBZ, id_przeciwnika, 4));
	int przeciwnikDoswiadczenie = stoi(wypiszZBazy(PrzeciwnicyBZ, id_przeciwnika, 5));
	//zmienne gracza
	int graczObrazenia = stoi(wypiszZBazy(BronieBZ, Gracz.bron, 2));
	int graczPancerz = stoi(wypiszZBazy(ZbrojeBZ, Gracz.zbroja, 2));
	//obrazenia - pancerz gracza
	przeciwnikObrazenia -= graczPancerz;
	if (przeciwnikObrazenia < 0) {
		przeciwnikObrazenia = 0;
	}
	//
	cout << endl;
	cout << endl;
	kolorowyTekst("---------------------------------","red");
	Sleep(1000);
	cout << endl;
	kolorowyTekst("              WALKA              ", "red");
	cout << endl;
	Sleep(1000);
	kolorowyTekst("---------------------------------", "red");
	Sleep(1000);
	cout << endl;
	cout << endl;

	int wybor{};
	int umiejetnosc{};
	int zajeteMiejsca{};
	int przedmiot{};
	int szansaNaTrafienie{};
	while (tokenWalki != 1) {
		kolorowyTekst(przeciwnikNazwa, "rose");
		cout << "\t" << przeciwnikZdrowie << "/" << przeciwnikMaxZdrowie << "pz" << endl;
		cout << endl;
		//------------------------------------------------RUCH GRACZA-------------------------------------------------------//
		Gracz.krotkieStatystyki();
		cout << "     1.";
		kolorowyTekst("Atak", "red");
		cout << "     2.";
		kolorowyTekst("Umiejętności", "blue");
		cout << "     3.";
		kolorowyTekst("Przedmioty", "green");
		cout << endl;
		if (wybor != 1 || wybor != 2 || wybor != 3) {
			cin >> wybor;
		}
		switch (wybor) {
		case 1:// atak
			cout << Gracz.imie << " atakuje " << przeciwnikNazwa << " za " << graczObrazenia << " obrażeń.";
			przeciwnikZdrowie -= graczObrazenia;
			break;
		case 2:// uzycie umiejetnosci
			zajeteMiejsca = 0;
			cout << "Szkarłat: " << Gracz.szkarlat << "/" << Gracz.szkarlatMax << endl;
			for (int i = 0; i < 10; i++) {
				if (Gracz.umiejetnosci[i] != -1) {
					cout << i << ". ";
					kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[i], 1), "blue");
					cout << " ";
					kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[i], 2), "red");
					kolorowyTekst(" szk","red");
					cout << "\t";
					zajeteMiejsca++;
				}
				if (i > 0 && i % 2 == 0) {
					cout << endl;
				}
			}

			cout << endl;
			umiejetnosc = -1;
			while(umiejetnosc<=-1 || umiejetnosc>=zajeteMiejsca){
				cin >> umiejetnosc;
			}
			cout << endl;
			if (Gracz.szkarlat >= (stoi(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 2)))) {
				cout << Gracz.imie << " używa ";
				kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 1), "blue");
				cout << " zadając ";
				cout << wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 3);
				cout << " obrażeń.";

				Gracz.szkarlat -= stoi(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 2));
				przeciwnikZdrowie -= stoi(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 3));
			}
			else if(Gracz.szkarlat < (stoi(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 2)))) {
				cout << "Próbujesz użyć umiejętności, ale nie masz wystarczająco szkarłatu." << endl;
			}
			break;
		case 3://użycie przedmiotu
			zajeteMiejsca = 0;
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

				cout << endl;
				przedmiot = -1;
				while(przedmiot <= -1 || przedmiot>=zajeteMiejsca) {
					cin >> przedmiot;
				}
				cout << endl;
				if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 3) == "leczenie") {
					Gracz.zdrowie += stoi(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 2));
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
					Gracz.szkarlat += stoi(wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[przedmiot], 2));
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
					cout << "Nie można użyć tego przedmiotu!" << endl;
				}
			}
			else {
				cout << "Ekwipunek jest pusty."<<endl;
			}
			break;
		}
		cout << endl;
		if (przeciwnikZdrowie <= 0) {
			tokenWalki = 1;
			cout << endl;
			cout << przeciwnikNazwa << " został pokonany!" << endl;
			cout << "Zdobywasz " << przeciwnikDoswiadczenie << "punktów doświadczenia!" << endl;
			Gracz.doswiadczenie += przeciwnikDoswiadczenie;
			for (int i = 0; i < 10; i++) {
				if (Gracz.ekwipunek[i] == -1) {
					Gracz.ekwipunek[i] = przeciwnikPrzedmiot;
					cout << "Zdobywasz ";
					cout << wypiszZBazy(PrzedmiotyBZ, przeciwnikPrzedmiot, 1);
					cout << "!"<<endl;
					break;
				}
			}
			if (Gracz.szkarlat < Gracz.szkarlatMax) {
				Gracz.szkarlat += 10;
				if (Gracz.szkarlat > Gracz.szkarlatMax) {
					Gracz.szkarlat = Gracz.szkarlatMax;
				}
				cout << "Odzyskujesz część szkarłatu" << endl;
			}
			Gracz.punktyReputacji += 0.5;
			cout << "Zyskujesz trochę reputacji" << endl;
			Gracz.czyNowyPoziom();
			Sleep(1000);
			tokenKontrolny = 0;
		}
		else {	//------------------------------------------------RUCH PRZECIWNIKA-------------------------------------------------------//
			szansaNaTrafienie = losowanieZeroJedynkowe(generator);
			if (szansaNaTrafienie == 1) {
				cout << przeciwnikNazwa << " atakuje " << Gracz.imie << " zadając " << przeciwnikObrazenia << " obrażeń." << endl;
				Gracz.zdrowie -= przeciwnikObrazenia;
			}
			else {
				cout << "Udaje Ci się uniknąć ataku!" << endl;
			}
			if (Gracz.zdrowie <= 0) {
				tokenWalki = 1;
				tokenKontrolny = 0;
				tokenWyjscia = 0;
				cout << "Zostałeś pokonany!" << endl;
				Sleep(1000);
				cout << "Robi Ci się ciemno przed oczami" << endl;
				cout << "Twoje ciało powoli bezwładnie osuwa się na podłogę" << endl;
				Sleep(1000);
				cout << "---------------------------" << endl;
				Sleep(1000);
				cout << "          Zginąłeś         " << endl;
				Sleep(1000);
				cout << "---------------------------" << endl;
				Sleep(1000);
			}
		}
	}
}

bool szansaNaWalke() {//funkcja losuje czy gracz wchodząc na to dane pole zainicjuje walkę czy nie (1/4 szansy że tak)
	int los = losowanieJednaCzwarta(generator);//losuje liczbe z zakresu {0, 1, 2, 3}
	if (los == 3) {
		return true;
	}
	else {
		return false;
	}
}

void inicjacjaWalki() { //Rozpoczyna sekwencje walki, jeśli gracz będzie chciał udać się na jedno z wymienionych pól
	int id = ((5 * Gracz.poz_y) + Gracz.poz_x);
	if ((id == 18 || id == 19 || id == 23 || id == 24) && szansaNaWalke()) {
		int id_przeciwnika = losowanieZeroJedynkowe(generator); //losuje liczbe z zakresu {0, 1}
		Walka(id_przeciwnika);
	}
}

//---------------------------MAPA GRY----------------------------//

void wypiszLokacje() { // funkcja wypisuje pełny opis danej lokacji (nazwa, "wyjścia", opis, kogo można tu spotkać)
	cout << endl;
	int id = ((5 * Gracz.poz_y) + Gracz.poz_x); //obliczanie id lokacji na mapie
	kolorowyTekst(wypiszZBazy(LokacjeBZ, id, 1), "green"); //nazwa lokacji
	cout << endl;
	kolorowyTekst(wypiszZBazy(LokacjeBZ, id, 2), "purple"); //możliwe kierunki ruchu z lokacji
	cout << endl;
	cout << wypiszZBazy(LokacjeBZ, id, 3);//opis lokacji
	cout << endl;
	if (wypiszZBazy(LokacjeBZ, id, 4) != "-1") { //-1 oznacza że nikogo tu nie ma
		kolorowyTekst(wypiszZBazy(NpcsBZ, stoi(wypiszZBazy(LokacjeBZ, id, 4)), 2), "rose"); //pobiera liczbę z LokacjeBZ oraz porównuje ją w bazie NpcsBZ, zwracając opis postaci która się tu znajduje
		cout << endl;
	}
	cout << endl;
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

void ruchNaMapie(string kierunek) {//funkcja przesuwa gracza na dane pole, zmieniając jego pozycje w osi x i y, dodatkowo sprawdza czy dane pole posiada szanse na walkę
	if ((kierunek == "n" || kierunek == "N") && Gracz.poz_y!=y_min && CzyMoznaPrzejsc("n")) {
		Gracz.poz_y--;
		inicjacjaWalki();
		if (tokenWyjscia != 0) {
			wypiszLokacje();
		}
	}
	else if ((kierunek == "s" || kierunek == "S") && Gracz.poz_y!=y_max && CzyMoznaPrzejsc("s")) {
		Gracz.poz_y++;
		inicjacjaWalki();
		if (tokenWyjscia != 0) {
			wypiszLokacje();
		}
	}
	else if ((kierunek == "w" || kierunek == "W") && Gracz.poz_x!=x_min && CzyMoznaPrzejsc("w")) {
		Gracz.poz_x--;
		inicjacjaWalki();
		if (tokenWyjscia != 0) {
			wypiszLokacje();
		}
	}
	else if ((kierunek == "e" || kierunek == "E") && Gracz.poz_x!=x_max && CzyMoznaPrzejsc("e")) {
		Gracz.poz_x++;
		inicjacjaWalki();
		if (tokenWyjscia != 0) {
			wypiszLokacje();
		}
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

//---------------------------DIALOGI-----------------------------//

string dobierzDialog(int numerPliku) { // zwraca adres pliku z którego pobierane są dane do dialogu
	string adresDialogu;
	stringstream ss;	//stringstream służy tutaj aby dokonać konwersji z int do string, żeby funkcja kolorowyTekst() wyświetlała dobre wartości
	string zwracanyAdres;
	ss << numerPliku;
	adresDialogu = ss.str();
	zwracanyAdres = "../sources/dialogs/dialog_" + adresDialogu + ".txt";
	return zwracanyAdres;
}

void wyswietlDialog(int idNpc) { //pobiera id postaci niezależnej i na jego podstawie wyświetla dialog
	ifstream dialog(dobierzDialog(idNpc));
	string linijka;
	int wybor = -1;
	if (idNpc == 0) {
		string pobraneKwestie[4]{};
		int i = 0;
		while (getline(dialog, linijka)) {
			pobraneKwestie[i]=linijka;
			i++;
		}
		while (wybor != 0) {
			cout << endl;
			cout << "1. Co ja tu robie?" << endl;
			cout << "2. Sfera?" << endl;
			cout << "3. Da się jakoś stąd wydostać?" << endl;
			cout << "4. O co chodzi z tym całym szkarłatem?" << endl;
			cout << "0. [Skończ rozmowę]" << endl;
			cin >> wybor;
			if (wybor > 0 && wybor <= 4) {
				cout << pobraneKwestie[(wybor - 1)] << endl;
			}
		}
		cout << endl;
		wypiszLokacje();
	}
	if (idNpc == 1) {
		string pobraneKwestie[3]{};
		int i = 0;
		while (getline(dialog, linijka)) {
			pobraneKwestie[i] = linijka;
			i++;
		}
		while (wybor != 0) {
			cout << endl;
			cout << "1. Opłaca się taki interes?" << endl;
			cout << "2. Co można u Ciebie kupić?" << endl;
			cout << "3. Skąd macie materiały w takim miejscu jak to?" << endl;
			cout << "0. [Skończ rozmowę]" << endl;
			cin >> wybor;
			if (wybor > 0 && wybor <= 3) {
				cout << pobraneKwestie[(wybor - 1)] << endl;
			}
		}
		cout << endl;
		wypiszLokacje();
	}
	if (idNpc == 4) {
		string pobraneKwestie[2]{};
		int i = 0;
		while (getline(dialog, linijka)) {
			pobraneKwestie[i] = linijka;
			i++;
		}
		while (wybor != 0) {
			cout << endl;
			cout << "1. Karczmarzu, masz może jakieś wieści co się dzieje poza sferą?" << endl;
			cout << "2. Dobrze wiedzie się interes?" << endl;
			cout << "0. [Skończ rozmowę]" << endl;
			cin >> wybor;
			if (wybor > 0 && wybor <= 2) {
				cout << pobraneKwestie[(wybor - 1)] << endl;
			}
		}
		cout << endl;
		wypiszLokacje();
	}
	if (idNpc == 2 || idNpc == 3 || idNpc == 5 || idNpc == 6) {
		cout << endl;
		while (getline(dialog, linijka)) {
			cout << linijka << endl;
			Sleep(1000);
		}
		cout << endl;
	}

	dialog.close();
	tokenKontrolny = 0;
}

void dobierzKatalog(int numerKatalogu) {//Wypisuje z odpowiedniego "katalogu handlu" wszystkie pozycje które dany gracz może kupić (dana klasa postaci może kupić tylko poszczególne przedmioty)
	string linijkaKatalogu{};
	int numerProduktu = 1;
	if (numerKatalogu == 1) {
		ifstream plikKatalogu("../sources/dialogs/katalog_kowal.txt");
		for (int i = 0; i < 3; i++) {
			if (i == Gracz.klasaPostaci) {
				getline(plikKatalogu, linijkaKatalogu);
				getline(plikKatalogu, linijkaKatalogu);
				cout << numerProduktu << ". ";
				cout << linijkaKatalogu << "\t";
				getline(plikKatalogu, linijkaKatalogu);
				cout << linijkaKatalogu << " złota";
				cout << endl;
				numerProduktu++;
			}
			else {
				getline(plikKatalogu, linijkaKatalogu);
				getline(plikKatalogu, linijkaKatalogu);
				getline(plikKatalogu, linijkaKatalogu);
			}
		}
		plikKatalogu.close();
	}
	else if (numerKatalogu == 2) {
		ifstream plikKatalogu("../sources/dialogs/katalog_zbrojmistrz.txt");
		for (int i = 0; i < 3; i++) {
			if (i == Gracz.klasaPostaci) {
				getline(plikKatalogu, linijkaKatalogu);
				getline(plikKatalogu, linijkaKatalogu);
				cout << numerProduktu << ". ";
				cout << linijkaKatalogu << "\t";
				getline(plikKatalogu, linijkaKatalogu);
				cout << linijkaKatalogu << "złota";
				cout << endl;
				numerProduktu++;
			}
			else {
				getline(plikKatalogu, linijkaKatalogu);
				getline(plikKatalogu, linijkaKatalogu);
				getline(plikKatalogu, linijkaKatalogu);
			}
		}
		plikKatalogu.close();
	}else if(numerKatalogu == 5){
		ifstream plikKatalogu("../sources/dialogs/katalog_sprzedawca.txt");
		for (int i = 0; i < 2; i++) {
			getline(plikKatalogu, linijkaKatalogu);
			getline(plikKatalogu, linijkaKatalogu);
			cout << numerProduktu << ". ";
			cout << linijkaKatalogu << "\t";
			getline(plikKatalogu, linijkaKatalogu);
			cout << linijkaKatalogu << "złota";
			cout << endl;
			numerProduktu++;
		}
		plikKatalogu.close();
	}
	cout << endl;
	cout << "----------------------------------------------------------" <<endl;
	cout << "Wpisz 0, by anulować handel";
	cout << endl;

}

void handel(int idNpc) {//funkcja która prowadzi handel gracza z postacią niezależną, kupiona broń oraz zbroja zostaje od razu wymieniona (zastępuje poprzednie), a przedmioty o ile jest miejsce w ekwipunku zostają do niego dodane
	int wybor{};
	cout << endl;
	cout << "----------------------------------------------------------" << endl;
	cout << endl;
	dobierzKatalog(idNpc);
	cin >> wybor;
	if (idNpc == 1) {
		if (wybor == 1) {
			if (Gracz.zloto >= 50) {
				Gracz.bron = (Gracz.bron + 3);
				Gracz.zloto -= 50;
			}
			else {
				cout << "Nie masz wystarczająco złota!" << endl;
			}
		}
	}

	if (idNpc == 2) {
		if (wybor == 1) {
			if (Gracz.zloto >= 60) {
				Gracz.zbroja = (Gracz.zbroja + 3);
				Gracz.zloto -= 60;
			}
			else {
				cout << "Nie masz wystarczająco złota!" << endl;
			}
		}
	}

	if (idNpc == 5) {
		if (wybor == 1) {
			if (Gracz.zloto >= 10) {
				for (int i = 0; i < 10; i++) {
					if (Gracz.ekwipunek[i] == -1) {
						Gracz.ekwipunek[i] = 0;
						Gracz.zloto -= 10;
						break;
					}
				}
			}
			else {
				cout << "Nie masz wystarczająco złota!" << endl;
			}
		}
		if (wybor == 2) {
			if (Gracz.zloto >= 10) {
				for (int i = 0; i < 10; i++) {
					if (Gracz.ekwipunek[i] == -1) {
						Gracz.ekwipunek[i] = 1;
						Gracz.zloto -= 10;
						break;
					}
				}
			}
			else {
				cout << "Nie masz wystarczająco złota!" << endl;
			}
		}
	}
	tokenKontrolny = 0;
}

void wymienTrofea() {//funkcja sprawdza czy gracz ma jakieś przedmioty typu trofea w ekwipunku, jeśli tak to je sprzedaje
	cout << "Pokazujesz swoje trofea" << endl;
	int tranzakcje=0;
	int ile_zlota=0;
	for (int i = 0; i < 10; i++) {
		if (Gracz.ekwipunek[i] == 2) {
			Gracz.ekwipunek[i] = -1;
			Gracz.zloto += stoi(wypiszZBazy(PrzedmiotyBZ, 2, 2));
			ile_zlota += stoi(wypiszZBazy(PrzedmiotyBZ, 2, 2));
			tranzakcje++;
		}
		if (Gracz.ekwipunek[i] == 3) {
			Gracz.ekwipunek[i] = -1;
			Gracz.zloto += stoi(wypiszZBazy(PrzedmiotyBZ, 3, 2));
			ile_zlota += stoi(wypiszZBazy(PrzedmiotyBZ, 3, 2));
			tranzakcje++;
		}
		Gracz.sortowanieEkwipunku();
	}
	if (tranzakcje > 0) {
		cout << "Wymieniasz " << tranzakcje << " trofea na " << ile_zlota << " złota!" << endl;
		cout << "Aktualnie masz " << Gracz.zloto << "złota." << endl;
	}
	else {
		cout << "Nie masz przy sobie żadnych trofeów" << endl;
	}

	cout << endl;
}

//---------------------------INTERPRETER-------------------------//

void niepoprawnePolecenie() { //wyświetla się kiedy program nie rozpozna komendy
	cout << "hmm?" << endl;
}

//Przeładowana funkcja *interpreter* przetwarza komende daną przez użytkownika, zależnie od tego jak złożona była (ile miała słów) i wykonuje daną akcje
void interpreter(string arg1) {
	if (arg1 == "ekwipunek" || arg1 == "ekw" || arg1 == "eq") {// wyświetla ekwipunek gracza
		Gracz.wypiszEkwipunek();
	}
	else if (arg1 == "n" || arg1 == "N" || arg1 == "s" || arg1 == "S" || arg1 == "e" || arg1 == "E" || arg1 == "w" || arg1 == "W") {//służy do poruszania się gracza na mapie gry
		ruchNaMapie(arg1);
	}
	else if (arg1 == "zapisz" || arg1 == "save") {//zapisuje grę
		Gracz.zapisz();
		cout << endl;
		kolorowyTekst("----------------------------------", "green");
		cout << endl;
		kolorowyTekst("           Zapisano grę           ", "green");
		cout << endl;
		kolorowyTekst("----------------------------------", "green");
		cout << endl;
	}
	else if (arg1 == "spojrz") { //argument spojrz ma na celu wypisanie pełnego opisu danej lokacji
		wypiszLokacje();
	}
	else if (arg1 == "statystyki" || arg1 == "staty") {//wyświetla statystyki gracza
		Gracz.statystyki();
	}else if (arg1 == "umi" || arg1 == "umiejetnosci" || arg1 == "skille") {//wyświetla umiejetności gracza
		Gracz.wypiszUmiejetnosci();
	}else if (arg1 == "wyjscie" || arg1 == "wyjdz" || arg1 == "exit") {//kończy grę (można wybrać czy dodatkowo zapisuje gre czy nie
		cout << "Chcesz zapisać grę przed wyjściem? tak/nie" << endl;
		string odp;
		cin >> odp;
		if (odp == "tak") {
			Gracz.zapisz();
		}
		tokenWyjscia = 0;
	}else if (arg1 == "pomoc" || arg1 == "komendy") { //wypisuje wszystkie komendy dostępne w grze (poza ich alternatywnymi wersjami)
		cout << endl;
		cout << "------------------------------" << endl;
		cout << "Ruch: N E S W" << endl;
		cout << "ekwipunek\tumiejetnosci" << endl;
		cout << "statystyki" << endl;
		cout << "spojrz" << endl;
		cout << "rozmawiaj" << endl;
		cout << "zapisz\twyjdz" << endl;
		cout << "handluj" << endl;
		cout << "wymien" << endl;
		cout << "------------------------------" << endl;
		cout << "obejrzyj pancerz" << endl;
		cout << "obejrzyj bron" << endl;
		cout << "obejrzyj przedmiot" << endl;
		cout << "uzyj przedmiotu" << endl;
		cout << "wyrzuc przedmiot" << endl;
		cout << "opis umiejetnosci" << endl;
		cout << "przyrzyj sie" << endl;
		cout << "------------------------------" << endl;
	}
	else if (arg1 == "rozmawiaj" || arg1 == "zagadaj") {//wypisuje dialog
		int id = ((5 * Gracz.poz_y) + Gracz.poz_x);
		if (wypiszZBazy(LokacjeBZ, id, 4) != "-1") {
			wyswietlDialog(stoi(wypiszZBazy(LokacjeBZ, id, 4)));
		}
		else {
			cout << "Nikogo tutaj nie ma" << endl;
		}
	}
	else if (arg1 == "handel" || arg1 == "handluj") {//rozpoczyna handel
		int id = ((5 * Gracz.poz_y) + Gracz.poz_x);
		string idNpc = wypiszZBazy(LokacjeBZ, id, 4);
		if (idNpc == "1" || idNpc == "2" || idNpc == "5") {
			handel(stoi(wypiszZBazy(LokacjeBZ, id, 4)));
		}
		else {
			cout << "Nie ma tu nikogo z kim można handlować" << endl;
		}
	}
	else if(arg1 == "wymien"){//wymienia zdobyte przez gracza trofea na zloto
		int id = ((5 * Gracz.poz_y) + Gracz.poz_x);
		if (wypiszZBazy(LokacjeBZ, id, 4)=="6") {
			wymienTrofea();
		}
		else {
			cout << "Trofea wymieniać można tylko u Naganiacza" << endl;
		}
	}
	else {
		niepoprawnePolecenie();
	}
}
void interpreter(string arg1, string arg2) {
	if ((arg1 == "obejrz" || arg1 == "obejrzyj")&&(arg2 == "pancerz" || arg2 == "zbroja" || arg2 == "ubior" || arg2 == "ubranie")) { //wypisuje statystyki i opis Gracz.zbroja
		cout << endl;
		kolorowyTekst(wypiszZBazy(ZbrojeBZ, Gracz.zbroja, 1), "cyan");
		cout << endl;
		cout << "Wartość pancerza: ";
		kolorowyTekst(wypiszZBazy(ZbrojeBZ, Gracz.zbroja, 2), "cyan");
		cout << endl;
		cout << wypiszZBazy(ZbrojeBZ, Gracz.zbroja, 3) << endl;
		cout << endl;
		tokenKontrolny = 0;
	}
	else if ((arg1 == "obejrz" || arg1 == "obejrzyj") && arg2 == "bron") {//wypisuje statystyki i opis Gracz.bron
		kolorowyTekst(wypiszZBazy(BronieBZ, Gracz.bron, 1), "red");
		cout << endl;
		cout << "Obrażenia: ";
		kolorowyTekst(wypiszZBazy(BronieBZ, Gracz.bron, 2), "red");
		cout << endl;
		cout << wypiszZBazy(BronieBZ, Gracz.bron, 3) << endl;
		cout << endl;
		tokenKontrolny = 0;
	}
	else if ((arg1 == "obejrz" || arg1 == "obejrzyj") && (arg2 == "przedmiot" || arg2 == "przedmioty")) {//wypisuje statystyki i opis wybranego przez gracza przedmiotu
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
					cout << wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 2);
					cout << " punktów zdrowia." << endl;
				}
				else if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 3) == "szkarlat") {
					cout << "Przywraca ";
					cout << wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 2);
					cout << " szkarłatu." << endl;
				}
				else if (wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 3) == "trofeum") {
					cout << "Warte ";
					cout << wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 2);
					cout << " złota." << endl;
				}
				cout << wypiszZBazy(PrzedmiotyBZ, Gracz.ekwipunek[ogladane], 4);
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
		tokenKontrolny = 0;
	}
	else if (arg1 == "opis" && (arg2 == "umi" || arg2 == "umiejetnosci" || arg2 == "skilli")) { //wyswietla posiadanie przez gracza umiejętności, następnie umożliwia wybranie poszczególnej umiejetności by wyświetlić jej pełny opis
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
			kolorowyTekst(wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 3), "rose");
			cout << endl;
			cout << wypiszZBazy(UmiejetnosciBZ, Gracz.umiejetnosci[umiejetnosc], 4);
			cout << endl;
		}
		else {
			cout << "Nie znasz takiej umiejętności." << endl;
		}

	}
	else if (arg1 == "uzyj" && (arg2 == "przedmiot" || arg2 == "przedmiotu")) { //pozwala użytkownikowi na wybór przedmiotu, a następnie używa go, zwiększając dane statystyki gracza
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
	else if ((arg1 == "wyrzuc" || arg1 == "zostaw") && arg2 == "przedmiot") { //wyrzuca przedmiot z ekwipunku gracza
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
				Gracz.sortowanieEkwipunku();
			}
			else {
				cout << "Nie masz takiego przedmiotu" << endl;
			}
		}
		else {
			cout << "Twój ekwipunek jest pusty." << endl;
		}
	}
	else if (arg1 == "przyjrzyj" && arg2 == "sie") {//wypisuje opis postaci niezależnej
		int id = ((5 * Gracz.poz_y) + Gracz.poz_x);
		if (wypiszZBazy(LokacjeBZ, id, 4) != "-1") {
			cout << endl;
			cout << wypiszZBazy(NpcsBZ, stoi(wypiszZBazy(LokacjeBZ, id, 4)), 3);
			cout << endl;
			tokenKontrolny = 0;
		}
		else {
			cout << "Nikogo tutaj nie ma" << endl;
		}
	}
	else {
		niepoprawnePolecenie();
	}
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
		//poniższy fragment ma na celu pozyskanie od użytkownika danych w postaci string, rozdziela go na poszczególne słowa, sprawdza ilość słów i wysyła do odpowiedniej funkcji interpreter
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
		default:
			niepoprawnePolecenie();
			break;
		}
	}
}

//-----------------------TWORZENIE POSTACI-----------------------//

void tworzeniePostaci() { //funkcja mająca na celu przeprowadzić użytkownika przez proces tworzenia swojej postaci
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

	//poniższy fragment wyświetla możliwe klasy do wyboru przez gracza, użytkownik wybiera jedną i zostaje ona przypisana do Gracz.klasaPostaci
	string kolorTP{};
	int liczba = 1;
	for (int i = 0; i <= 2; i++) {
		if (i == 0) kolorTP = "green";
		else if (i == 1) kolorTP = "cyan";
		else if (i == 2) kolorTP = "purple";
		cout << liczba << ". ";
		kolorowyTekst(wypiszZBazy(KlasyBZ, i, 1), kolorTP);
		cout << endl;
		cout << wypiszZBazy(KlasyBZ, i, 2);
		cout << endl << endl;
		liczba++;
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

	//Pobiera od gracza informacje czy chce zobaczyć krótkie fabularne wprowadzenie i dialog, jeśli nie to jest to pomijane w pełni
	getline(plikTP, tekstTP);
	kolorowyTekst(tekstTP, "red");
	cout << "Tak/Nie" << endl << endl;

	string wstepTP;
	cin >> wstepTP;
	if (wstepTP == "Tak" || wstepTP == "tak" || wstepTP == "TAK" || wstepTP=="ok" || wstepTP=="OK" || wstepTP=="Ok") {
		tokenIntro = 1;
		system("cls");
			while (!plikTP.eof()) {
			getline(plikTP, tekstTP);
			cout << tekstTP << endl;
			Sleep(4500);
			}
	}

	plikTP.close();

	//Przypisuje do obiektu Gracz wszystkie wymagane parametry, oraz dobiera odpowiednie umiejetnosci, bron, zbroje oraz przedmioty zależnie od klasyPostaci
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
	plikZZapisami << Gracz.imie << "\n";//zapisuje nazwe postaci do pliku służącego jako rejestr zapisów
	plikZZapisami.close();

	Gracz.zapisz();
	tokenKontrolny = 0;

	cout << endl;
	cout << "-------------------------------------------------" << endl;
	cout << " wpisz \"pomoc\" by wyświetlić wszystkie komendy " << endl;
	cout << "-------------------------------------------------" << endl;
	cout << endl;
};

//--------------------WYPISYWANIE ZAPISÓW------------------------//

void wypiszPliki() {//funkcja wypisuje wszystkie pliki zapisów, które program wykrywa w folderze saves
	
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
			int czyZnalezionoZapis = 0;
			for (int i = 0; i < index; i++) {
				if (zapisy[i] == nazwa) {
					czyZnalezionoZapis = 1;
				}
			}

			if (czyZnalezionoZapis == 1) {
				Gracz.imie = nazwa;
				Gracz.wczytaj();
				system("cls");
			}
			else {
				cout << "Nie znaleziono takiego pliku zapisu!" << endl;
				tokenWyjscia = 0;
			}
			
	}else {
		cout << "Nie znaleziono żadnych plików zapisu!" << endl;
	}
}

//----------------NOWA GRA I WCZYTAJ GRE-------------------------//

void nowaGra() { //funkcja wywoływana podczas wybrania opcji nowa gra
	tworzeniePostaci();
	wypiszLokacje();
}

void wczytajGre() { //funkcja wywoływana podczas wybrania opcji wczytaj gre
	wypiszPliki();
	if (tokenWyjscia != 0) { //warunek ten NIE zachodzi tylko kiedy użytkownik poda błędną nazwę wczytywanej postaci
		wypiszLokacje();
	}
}

//--------------SPRAWDZENIE I WCZYTANIE ASSETÓW-----------------//
bool WczytywanieAssetow(){//Sprawdza czy wszystkie wymagane pliki istnieją i wczytuje dane do odpowiednich baz zasobów
	kolorowyTekst("\tSzkarłat", "red");
	cout << endl;

	if (istnieje("../sources/tworzeniePostaci.txt") && istnieje("../sources/intro.txt") && istnieje("../sources/dialogs/katalog_kowal.txt") && istnieje("../sources/dialogs/katalog_zbrojmistrz.txt") && istnieje("../sources/dialogs/katalog_sprzedawca.txt")) {
		kolorowyTekst("Poprawnie zweryfikowano pliki źródłowe...","rose");
		cout << endl;
	}
	else {
		cout << "Wykryto błąd wczytywania plików";
		return false;
	}

	if (wczytajDoBazy("ZbrojeBZ.txt", ZbrojeBZ, 6, 4) && wczytajDoBazy("PrzedmiotyBZ.txt", PrzedmiotyBZ, 4, 5) && wczytajDoBazy("BronieBZ.txt", BronieBZ, 6, 4) && wczytajDoBazy("PrzeciwnicyBZ.txt", PrzeciwnicyBZ, 2, 6) && wczytajDoBazy("NpcsBZ.txt", NpcsBZ, 7, 6) && wczytajDoBazy("UmiejetnosciBZ.txt", UmiejetnosciBZ, 6, 5) && wczytajDoBazy("LokacjeBZ.txt", LokacjeBZ, 25, 5) && wczytajDoBazy("KlasyBZ.txt", KlasyBZ, 3, 3)) {
		kolorowyTekst("Poprawnie wczytano wszystkie bazy zasobów...", "rose");
		cout << endl;
	}
	else {
	cout << "Wykryto błąd wczytywania baz zasobów";
		return false;
	}

	kolorowyTekst("Wszystkie pliki są poprawne!","green");
	cout << endl << endl << endl << endl << endl;

		return true;
}

//----------------------------------------------------------------//

int start() { //funkcja wyświetla ekran startowy gry

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
			return 0;
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