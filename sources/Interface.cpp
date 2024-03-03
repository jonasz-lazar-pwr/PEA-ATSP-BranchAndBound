#include <windows.h>

#include "Interface.h"
#include "ATSP.h"

void Interface::menu() {

    ATSP matrixATSP;

    // Fragment kodu odpowiedzialny za pomiary czasu
    long long int frequency, start, elapsed;
    QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);

    char option;
    string fileName;
    int numOfCities;

    int startingCity; // Miasto startowe (DP)
    int mask; // Początkowa maska z ustawionym bitem odpowiadającym miastu startowemu (DP)
    vector<int> currentPath; // Początkowa trasa z miastem startowym (DP)
    int bestCost; // Koszt najlepszej trasy (DP)

    do {
        cout << endl << "-----MENU GLOWNE-----\n";
        cout << "[1] Wczytanie danych z pliku\n";
        cout << "[2] Wyswietlenie ostatnio wczytanych danych\n";
        cout << "[3] Wygenerowanie pliku z losowymi danymi\n";
        cout << "[4] Algorytm Branch and Bound (metoda Low Cost)\n";
        cout << "[5] Algorytm Branch and Bound (metoda DFS)\n";
        cout << "[6] Programowanie dynamiczne (DP)\n";
        cout << "[0] Wyjscie\n";

        cout << "Podaj opcje:";
        cin >> option;

        switch (option) {
            case '0':
                cout << "\nWyjscie\n";
                break;
            case '1':
                cout << "\nOpcja 1\n";
                // Pliki muszą znajdować się w tym samym folderze co plik .exe,
                // oraz muszą mieć charakterystyczną budowę ze specjalnym nagłówkiem,
                // zgodnie z przykładami na stronie: http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/atsp/
                cout << "Podaj nazwe pliku wraz z rozszerzeniem (.atsp):";
                cin >> fileName;
                matrixATSP.loadATSPFile(fileName);
                break;
            case '2':
                cout << "\nOpcja 2\n";
                matrixATSP.printDistanceMatrix();
                break;
            case '3':
                cout << "\nOpcja 3\n";
                cout << "Podaj nazwe pliku wraz z rozszerzeniem (.atsp):";
                cin >> fileName;
                cout << "Podaj liczbe miast:";
                cin >> numOfCities;
                ATSP::generateATSPFile(fileName, numOfCities);
                break;
            case '4':
                cout << "\nOpcja 4\n";
                start = read_QPC();
                matrixATSP.branchAndBoundLowCost();
                elapsed = read_QPC() - start;
                cout << "Czas wykonania : " << (1000.0 * elapsed)/frequency << " ms" << endl;
                break;
            case '5':
                cout << "\nOpcja 5\n";
                start = read_QPC();
                matrixATSP.branchAndBoundDFS();
                elapsed = read_QPC() - start;
                cout << "Czas wykonania: " << (1000.0 * elapsed)/frequency << " ms" << endl;
                break;
            case '6':
                cout << "\nOpcja 6\n";
                startingCity = 0;
                mask = 1 << startingCity;
                currentPath = {startingCity};

                start = read_QPC();
                bestCost = matrixATSP.dynamicProgramming(mask, startingCity, currentPath);
                elapsed = read_QPC() - start;

                cout << "Koszt najlepszej trasy znalezionej algorytmem DP: " << bestCost << endl;
                cout << "Czas wykonania: " << (1000.0 * elapsed)/frequency << " ms" << endl;
                break;
            default:
                cout << "\nPodana opcja nie istnieje!\n";
                break;
        }
    } while (option != '0');
}

// Pomiaru czasu przy użyciu QueryPerformanceCounter()
long long int Interface::read_QPC() {
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return((long long int)count.QuadPart);
}
