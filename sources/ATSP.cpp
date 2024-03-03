#include <fstream>
#include <algorithm>
#include <iomanip>
#include <random>
#include <queue>
#include <stack>
#include <map>

#include "ATSP.h"

// Struktura reprezentująca wierzchołek drzewa przeszukiwań
ATSP::Node::Node(int value, int level, const vector<int>& path, const vector<vector<int>>& reducedMatrix) :
        value(value), level(level), path(path), reducedMatrix(reducedMatrix) {}

// Funkcja inicjalizująca macierz odległości
void ATSP::initializeDistanceMatrix(int newDimension) {
    // Uaktualnienie nowego rozmiaru macierzy odległości
    N = newDimension;

    // Inicjalizacja macierzy o odpowiednich wymiarach
    distanceMatrix = vector<vector<int>>(N, vector<int>(N));
}

// Funkcja wyświetlająca zawartość macierzy odległości
void ATSP::printDistanceMatrix() {

    // Ustawienie szerokość wyświetlanego pola
    int fieldWidth = 4;

    // Pętle iterujące po każdym elemencie macierzy
    for (const auto& row: distanceMatrix) {
        for (const auto& distance: row) {
            cout << setw(fieldWidth) << distance;
        }
        cout << endl;
    }
}

// Algorytm Branch and Bound typu Low Cost
void ATSP::branchAndBoundLowCost() {
    // Kolejka prorytetowa służąca do przeszukiwania wszystkich węzłów
    // w kolejności od najmniejszej wartości
    priority_queue<Node, vector<Node>, NodeCompare> nodeQueue;

    // Inicjalizacja korzenia drzewa
    Node root(0, 0, vector<int>{0}, distanceMatrix);

    // Obliczamy dolne ograniczenie dla korzenia
    root.lowerBound = calculateReductionCost(root);

    // Zapisanie najlepszego rozwiązania
    int bestCost = INF;
    vector<int> bestPath;

    // Wstawienie początkowego węzła do kolejki priorytetowej
    nodeQueue.push(root);

    while (!nodeQueue.empty()) {

        // Pobierz węzeł z najniższym dolnym ograniczeniem
        Node currentNode = nodeQueue.top();
        nodeQueue.pop();

        // Obliczenia na ostatnim poziomie drzewa
        if (currentNode.level == N - 1) {
            bestCost = currentNode.lowerBound;
            bestPath = currentNode.path;
            break;

        } else {
            // Pętla iterująca po wszystkich możliwych potomkach (można nie zaczynać od 0)
            for (int i = 0; i < N; i++) {

                // Sprawdzamy czy dane miasto nie jest już obecne na ścieżce
                if (!isCityOnPath(i, currentNode.path)) {

                    // Nowa droga do potomka
                    vector<int> newPath = currentNode.path;
                    newPath.push_back(i);

                    // Dodanie nowego potomka dla aktualnego węzła
                    Node childNode(i, currentNode.level + 1, newPath, currentNode.reducedMatrix);

                    // Aktualizacja macierzy kosztów potomka
                    // W tym miejscu eliminujemy drogi to miast już niemożliwych do odzwiedzenia
                    for (int j = 0; j < N; j++) {
                        childNode.reducedMatrix[currentNode.value][j] = -1;
                        childNode.reducedMatrix[j][i] = -1;
                    }
                    for (int ancestor: childNode.path) {
                        childNode.reducedMatrix[i][ancestor] = -1;
                    }

                    // Funkcja obliczająca granicę
                    childNode.lowerBound = currentNode.lowerBound + calculateReductionCost(childNode) +
                                           currentNode.reducedMatrix[currentNode.value][i];

                    // Dodaj potomka do kolejki priorytetowej
                    nodeQueue.push(childNode);
                }
            }
        }
    }

    // Wyświetlenie wyniku
    cout << "Najlepsza trasa znaleziona algorytmem B&B (metoda Low Cost): ";
    for (int city : bestPath) {
        cout << city << "->";
    }
    cout << bestPath[0] << endl;
    cout << "Koszt najlepszej trasy: " << bestCost  << endl;
}

// Algorytm Branch and Bound typu DFS
void ATSP::branchAndBoundDFS() {

    stack<Node> nodeStack;

    // Inicjalizacja korzenia drzewa
    Node root(0, 0, vector<int>{0}, distanceMatrix);

    // Obliczamy dolne ograniczenie dla korzenia
    root.lowerBound = calculateReductionCost(root);

    // Zapisanie najlepszego rozwiązania
    int bestCost = INF;
    vector<int> bestPath;

    // Wstawienie początkowego węzła do stosu
    nodeStack.push(root);

    while (!nodeStack.empty()) {

        Node currentNode = nodeStack.top();
        nodeStack.pop();

        if (currentNode.lowerBound >= bestCost) {
            continue;
        }

        // Obliczenia na ostatnim poziomie drzewa
        if (currentNode.level == N - 1) {
            bestCost = currentNode.lowerBound;
            bestPath = currentNode.path;
            continue;
        }

        // Pętla iterująca po wszystkich możliwych potomkach
        for (int i = N - 1; i >= 0; i--) {

            if (!isCityOnPath(i, currentNode.path)) {
                vector<int> newPath = currentNode.path;
                newPath.push_back(i);

                Node childNode(i, currentNode.level + 1, newPath, currentNode.reducedMatrix);

                // Ograniczenia - eliminacja nieoptymalnych węzłów
                for (int j = 0; j < N; j++) {
                    childNode.reducedMatrix[currentNode.value][j] = -1;
                    childNode.reducedMatrix[j][i] = -1;
                }
                for (int ancestor: childNode.path) {
                    childNode.reducedMatrix[i][ancestor] = -1;
                }

                childNode.lowerBound = currentNode.lowerBound + calculateReductionCost(childNode) +
                                       currentNode.reducedMatrix[currentNode.value][i];

                if (childNode.lowerBound < bestCost) {
                    nodeStack.push(childNode);
                }
            }
        }
    }

    // Wyświetlenie wyniku
    cout << "Najlepsza trasa znaleziona algorytmem B&B (metoda DFS): ";
    for (int city : bestPath) {
        cout << city << "->";
    }
    cout << bestPath[0] << endl;
    cout << "Koszt najlepszej trasy: " << bestCost  << endl;
}

bool ATSP::isCityOnPath(int city, const vector<int>& path) {
    return any_of(path.begin(), path.end(), [city](int i) { return i == city; });
}

// Funkcja obliczająca dolne ograniczenie
int ATSP::calculateReductionCost(ATSP::Node &node) const {

    int totalReductionCost = 0;

    // Redukowanie wierszy
    for (int i = 0; i < N; i++) {
        int minRowCost = INF;
        for (int j = 0; j < N; j++) {
            if (node.reducedMatrix[i][j] < minRowCost && node.reducedMatrix[i][j] != -1) {
                minRowCost = node.reducedMatrix[i][j];
            }
        }
        if (minRowCost != INF && minRowCost != 0) {
            totalReductionCost += minRowCost;
            for (int j = 0; j < N; j++) {
                if (node.reducedMatrix[i][j] != -1) {
                    node.reducedMatrix[i][j] -= minRowCost;
                }
            }
        }
    }

    // Redukowanie kolumn
    for (int i = 0; i < N; i++) {
        int minColCost = INF;
        for (int j = 0; j < N; j++) {
            if (node.reducedMatrix[j][i] < minColCost && node.reducedMatrix[j][i] != -1) {
                minColCost = node.reducedMatrix[j][i];
            }
        }
        if (minColCost != INF && minColCost != 0) {
            totalReductionCost += minColCost;
            for (int j = 0; j < N; j++) {
                if (node.reducedMatrix[j][i] != -1) {
                    node.reducedMatrix[j][i] -= minColCost;
                }
            }
        }
    }
    return totalReductionCost;
}

// Brogramowanie dynamiczne dla problemu ATSP
int ATSP::dynamicProgramming(int mask, int lastVisitedCity, vector<int>& currentPath) {
    // Sprawdź, czy odwiedzono wszystkie miasta. Jeśli tak, wracamy do miasta początkowego i zapisujemy trasę.
    if (mask == (1 << N) - 1) {
        int currentCost = distanceMatrix[lastVisitedCity][0];
        return currentCost;
    }

    // Sprawdź, czy wynik dla danej maski i ostatnio odwiedzonego miasta jest już obliczony (memoizacja).
    static map<pair<int, int>, int> memoization;
    if (memoization.count({mask, lastVisitedCity})) {
        return memoization[{mask, lastVisitedCity}];
    }

    int bestCost = INF;

    // Rozważ odwiedzenie kolejnego miasta, jeśli jeszcze nie odwiedziliśmy go.
    for (int nextCity = 0; nextCity < N; nextCity++) {
        if ((mask & (1 << nextCity)) == 0) {
            currentPath.push_back(nextCity); // Dodaj miasto do aktualnej trasy
            // Oblicz koszt podróży z obecnego miasta do kolejnego miasta rekurencyjnie.
            int currentCost = distanceMatrix[lastVisitedCity][nextCity] + dynamicProgramming(mask | (1 << nextCity), nextCity, currentPath);
            currentPath.pop_back(); // Usuń ostatnio odwiedzone miasto z trasy
            bestCost = min(bestCost, currentCost);
        }
    }

    // Zapisz obliczony wynik w memoizacji i zwróć go.
    memoization[{mask, lastVisitedCity}] = bestCost;
    return bestCost;
}

// Funkcja generująca liczby losowe z zadanego przedziału
int ATSP::generateRandomNumber(const int& min, const int& max) {
    // Inicjalizacja generatora liczb losowych
    random_device rd;
    mt19937 generator(rd());

    // Definicja rozkładu prawdopodobieństwa
    uniform_int_distribution<> distribution(min,max);

    return distribution(generator);
}

// Funkcja do generowania pliku ATSP
void ATSP::generateATSPFile(const string& fileName, const int& numOfCities) {

    ofstream file(fileName);

    if (!file.is_open()) {
        cerr << "Nie można otworzyć pliku do zapisu." << endl;
        return;
    }

    // Nagłówek pliku ATSP
    file << "NAME: " << fileName << endl;
    file << "TYPE: ATSP" << endl;
    file << "COMMENT: Asymmetric TSP" << endl;
    file << "DIMENSION: " << numOfCities << endl;
    file << "EDGE_WEIGHT_TYPE: EXPLICIT" << endl;
    file << "EDGE_WEIGHT_FORMAT: FULL_MATRIX" << endl;
    file << "EDGE_WEIGHT_SECTION" << endl;

    // Generowanie macierzy odległości
    for (int i = 0; i < numOfCities; i++) {
        for (int j = 0; j < numOfCities; j++) {

            if (i == j) {
                file << setw(4) << "-1";
            } else {
                // Zapisywanie do pliku wartości losowych z podanego przedziału
                file << setw(4) << generateRandomNumber(0, 999);
            }
        }
        file << endl;
    }
    file << "EOF" << endl;
    file.close();
}

// Funkcja służąca do wczytywania pliku ATSP do tablicy dwuwymiarowej
void ATSP::loadATSPFile(const string& fileName) {

    // Otwórz plik do odczytu
    ifstream file;
    file.open(fileName, ifstream::in);

    if (file.is_open()) {
        string line;

        // Pętla iterująca po każdym wierszu tekstu w pliku
        while (getline(file, line)) {

            // Sprawdzenie, czy w obecnej linii znajduje się informacja o rozmiarze grafu
            if (line.find("DIMENSION: ") != string::npos) {
                size_t pos = line.find(": ");

                // Inicjalizacja macierzy odległości na podstawie rozmiaru
                initializeDistanceMatrix(stoi(line.substr(pos + 1)));

                // Rozpoczęcie procesu wczytywania odległości do macierzy
            } else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        file >> distanceMatrix[i][j];
                        if (i == j) {
                            // Ustawianie -1 na głównej przekątnej
                            distanceMatrix[i][j] = -1;
                        }
                    }
                }
            }
        }
    } else {
        // Komunikat o błędzie w przypadku problemu z otwarciem pliku
        cerr << "ERROR while opening the file." << endl;
    }
    file.close();
}
