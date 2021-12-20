#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

int colorMatching(string colorName) {
	//enum Colors { Blue = 1, Green, Cyan, Red, Purple, Yellow, LightGray, Gray, LightBlue, LightGreen, NeonBlue, Rose, Pink, LightYellow, White };
	//Colors colorInt = colorName;
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
	//return colorInt;
}

void colorReset() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, white);
}

int stylizedText(int text, int color) {
	//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleTextAttribute(hConsole, colorMatching(color));
	return text;
	//colorReset();
}

void stylizedText(string text, string color, string effect) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorMatching(color) | effectMatching(effect));
	cout << text << endl;
	colorReset();
}