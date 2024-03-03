#ifndef BNB_AND_DP_ATSP_H
#define BNB_AND_DP_ATSP_H


#include <iostream>
#include <vector>
#include <set>
#include <limits>

using namespace std;

// Wszystkie metody i ich działanie opisano w pliku ATSP.cpp

class ATSP {
public:
    void initializeDistanceMatrix(int newDimension);

    void printDistanceMatrix();

    void branchAndBoundLowCost();

    void branchAndBoundDFS();

    int dynamicProgramming(int mask, int lastVisitedCity, vector<int>& currentPath);

    static void generateATSPFile(const string& fileName, const int& numOfCities);

    void loadATSPFile(const string& fileName);

private:
    const int INF = numeric_limits<int>::max();

    // Zmienna określająca rozmiar problemu
    int N;

    // Macierz przechowująca odległości miezy miastami
    vector<vector<int>> distanceMatrix;

    struct Node {

        // Przechowuje bieżący numer miasta
        int value;

        // Przechowuje poziom węzła w drzewie
        int level;

        // Dolne ograniczenie
        int lowerBound{};

        // Przechowuje drogę po węzłach
        vector<int> path;

        // Zredukowana macierz
        vector<vector<int>> reducedMatrix;

        Node(int number, int level, const vector<int>& path, const vector<vector<int>>& reducedMatrix);
    };

    // Struktura do porównywania węzłów w kolejce priorytetowej
    struct NodeCompare {
        bool operator()(const Node& a, const Node& b) {
            if (a.lowerBound == b.lowerBound) {
                // Jeśli ograniczenia są równe, porównujemy poziomy węzłów
                if (a.level == b.level) {
                    // Jeśli poziomy są równe, porównujemy numery węzłów
                    return a.value > b.value;
                }
                return a.level < b.level;
            }
            // W przeciwnym przypadku porównujemy ograniczenia
            return a.lowerBound > b.lowerBound;
        }
    };

    static bool isCityOnPath(int city, const vector<int>& path);

    int calculateReductionCost(Node& node) const;

    static int generateRandomNumber(const int& min, const int& max);
};


#endif //BNB_AND_DP_ATSP_H
