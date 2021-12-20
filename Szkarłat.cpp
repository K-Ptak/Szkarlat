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

//-----------------------------COLORS----------------------------//

int colorMatching(string colorName) {
	if (colorName == "Blue") return 1;
	else if (colorName == "Green") return 2;
	else if (colorName == "Cyan") return 3;
	else if (colorName == "Red") return 4;
	else if (colorName == "Purple") return 5;
	else if (colorName == "Yellow") return 6;
	else if (colorName == "LightGray") return 7;
	else if (colorName == "Gray") return 8;
	else if (colorName == "LightBlue") return 9;
	else if (colorName == "LightGreen") return 10;
	else if (colorName == "NeonBlue") return 11;
	else if (colorName == "Rose") return 12;
	else if (colorName == "Pink") return 13;
	else if (colorName == "LightYellow") return 14;
	else if (colorName == "White") return 15;
	else return 15;
}//Matches given color name with it's numerical representation

void colorReset() {
	SetConsoleTextAttribute(hConsole, 15);
}//Sets line color to white again

void stylizedText(string text, string color) {
	SetConsoleTextAttribute(hConsole, colorMatching(color));
	cout << text << " ";
	colorReset();
}//Couts text with the choosen color

//----------------------READING FROM SOURCES---------------------//
void readTextFile(string fileAddress) {
	string textOutput;
	ifstream textSource(fileAddress);

	while (getline(textSource, textOutput)) {
		cout << textOutput << endl;
	}

	textSource.close();
}
//-------------------------SAVING TO FILE------------------------//

void saveFile(string fileName, string fileAddress, string data) {
	string fullFile = fileAddress + "/" + fileName + ".txt";
	ofstream file(fullFile);

	// Write to the file
	file << data;

	// Close the file
	file.close();
}


//---------------------------INTERPRETER-------------------------//

void invalidCommand() {
	cout << "hmm?" << endl;
}

void interpreter(string arg1) {
	cout << "1";
}
void interpreter(string arg1, string arg2) {
	cout << "2";
}
void interpreter(string arg1, string arg2, string arg3) {
	cout << "3";
}
void interpreter(string arg1, string arg2, string arg3, string arg4) {
	cout << "4";
}

void inputCommand() {
	string commandLine;
	getline(cin, commandLine);

	string buf;
	stringstream ss(commandLine);

	vector<string> parameters;

	while (ss >> buf){
		parameters.push_back(buf);
	}

	switch (parameters.size()) {
	case 1:
		interpreter(parameters[0]);
		break;
	case 2:
		interpreter(parameters[0], parameters[1]);
		break;
	case 3:
		interpreter(parameters[0], parameters[1], parameters[2]);
		break;
	case 4:
		interpreter(parameters[0], parameters[1], parameters[2], parameters[3]);
		break;
	default:
		invalidCommand();
		break;
	}
}//Takes players' command, divides it to singular words and sends it to the interpreter

//---------------------------------------------------------------//

int start() {
		cout << endl << endl;
		cout.width(20);
		cout << "\t\t\t\t\t[N] Nowa gra" << endl;
		cout.width(20);
		cout << "\t\t\t\t\t[Z] Załaduj poprzedni zapis" << endl;
		cout.width(20);
		cout << "\t\t\t\t\t[W] Wyjdź z gry" << endl;

	for(;;){
		char introValue = _getch();
		tolower(introValue);

		if (introValue == 'n') {
			return 1;
		}
		else if (introValue == 'z') {
			return 2;
		}
		else if (introValue == 'w') {
			return 3;
		}
	}
}

int main(){
	setlocale(LC_ALL, "Polish");
	/*switch (start()) {
	case 1:
		//newGame();
		break;
	case 2:
		//loadGame();
		break;
	default:
		exit(0);
		break;
	}
	inputCommand();
	*/
	readTextFile("../sources/placeholder.txt");
	saveFile("test", "../saves", "test");
}