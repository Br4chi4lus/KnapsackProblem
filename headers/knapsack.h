#ifndef KNAPSACK_H
#define KNAPSACK_H

struct ItemInfo
{
    int value;
    int weight;
};

enum Algorithm{
    BruteForce,
    Genetic
};

struct KnapsackProblemInfo
{
    int numberOfItems;
    int maxWeightOfItems;
    struct ItemInfo *items;
    double mutationRate;
    int numberOfEliteIndividuals;
    int randomInitModulo;
    int numberOfGenerations;
    int numberOfIndividualsInGeneration;
    enum Algorithm algorithm;
    char* fileName;
    int numberOfCrossoverPoints;
};
void FreeMultipleAddresses(int numberOfAddresses, ...);
int CalculateValueOfSelectedItems(struct KnapsackProblemInfo *knapsackProblemInfo, int *itemsSelection);
void ReadDataFromFileToKnapsackProblemInfo(char* fileName, struct KnapsackProblemInfo *knapsackProblemInfo);
void GetNextSolutionByModifyingCurrent(int *itemsSelection, int numberOfItems);
int *GenerateRandomSelection(int numberOfItems);
int **GenerateRandomPopulation(int numberOfItems, int numberOfIndividuals);
int **Crossover(int *parent1, int *parent2, int numberOfItems, int numberOfCrossoverPoints);
void MutateIndividual(int *individual, int numberOfItems, double mutationRate);
struct KnapsackProblemInfo *InitKnapsackInfoProblem();
#endif