#include "../headers/knapsack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define MUTATION_RATIO 0.3

void FreeMultipleAddresses(int numberOfAddresses, ...){
    va_list addresses;
    va_start(addresses, numberOfAddresses);
    void* address = NULL;
    for (int i = 0; i < numberOfAddresses; ++i){
        address = va_arg(addresses, void*);
        free(address);
    }
    va_end(addresses);
}

int CalculateValueOfSelectedItems(struct KnapsackProblemInfo *knapsackProblemInfo, int *itemsSelection){
    int sumOfSelectedItemsValue = 0;
    int sumOfSelectedItemsWeight = 0;
    for (int i = 0; i < knapsackProblemInfo->numberOfItems; ++i)
    {
        if (itemsSelection[i] == 1){
            sumOfSelectedItemsValue += knapsackProblemInfo->items[i].value;
            sumOfSelectedItemsWeight += knapsackProblemInfo->items[i].weight;
        }
    }
    if (sumOfSelectedItemsWeight > knapsackProblemInfo->maxWeightOfItems) 
    {
        return 0;
    }
    return sumOfSelectedItemsValue;
}

int CompareInts(const void *item1, const void *item2){
    int i1 = *((int*)item1);
    int i2 = *((int*)item2);
    return i1 - i2;
}

struct KnapsackProblemInfo *InitKnapsackInfoProblem(){
    struct KnapsackProblemInfo *knapsackProblemInfo = malloc(sizeof(struct KnapsackProblemInfo));
    if (knapsackProblemInfo == NULL)
        return NULL;
    
    knapsackProblemInfo->mutationRate = 0.1;
    knapsackProblemInfo->numberOfEliteIndividuals = 2;
    knapsackProblemInfo->numberOfGenerations = 1000;
    knapsackProblemInfo->numberOfIndividualsInGeneration = 200;
    knapsackProblemInfo->randomInitModulo = 10;
    knapsackProblemInfo->items = NULL;
    knapsackProblemInfo->maxWeightOfItems = 0;
    knapsackProblemInfo->numberOfItems = 0;
    knapsackProblemInfo->algorithm = Genetic;
    knapsackProblemInfo->fileName = NULL;
    knapsackProblemInfo->numberOfCrossoverPoints = 2;
    return knapsackProblemInfo;
}

void ReadDataFromFileToKnapsackProblemInfo(char* fileName, struct KnapsackProblemInfo *knapsackProblemInfo){
    FILE* file = fopen(fileName, "r");
    if (file == NULL)
        return;
    fscanf(file, "%d %d\n", &knapsackProblemInfo->numberOfItems, &knapsackProblemInfo->maxWeightOfItems);
    knapsackProblemInfo->items = malloc(sizeof(struct ItemInfo) * knapsackProblemInfo->numberOfItems);
    if (knapsackProblemInfo->items == NULL){
        fclose(file);
        return;
    }
    for (int i = 0; i < knapsackProblemInfo->numberOfItems; ++i)
    {
        fscanf(file, "%d %d\n", &knapsackProblemInfo->items[i].value, &knapsackProblemInfo->items[i].weight);
    }
    fclose(file);
    return;
}

void GetNextSolutionByModifyingCurrent(int *itemsSelection, int numberOfItems){
    for (int i = 0; i < numberOfItems; ++i){
        if (itemsSelection[i] == 1){
            itemsSelection[i] = 0;
        }
        else
        {
            itemsSelection[i] = 1;
            break;
        }
    }
}

int *GenerateRandomSelection(int numberOfItems){
    int *selection = malloc(numberOfItems * sizeof(int));
    if (selection == NULL)
        return NULL;
    for (int i = 0; i < numberOfItems; ++i){
        selection[i] = 0;
        if (rand() % 100 == 0){
            selection[i] = 1;
        }
    }
    return selection;
}

int **GenerateRandomPopulation(int numberOfItems, int numberOfIndividuals){
    int **population = malloc(numberOfIndividuals * sizeof(int *));
    if (population == NULL)
        return NULL;
    for (int i = 0; i < numberOfIndividuals; ++i){
        population[i] = GenerateRandomSelection(numberOfItems);
        if (population[i] == NULL){
            for (int j = 0; j < i; ++j){
                free(population[j]);
            }
            free(population);
            return NULL;
        }
    }
    return population;
}

void MutateIndividual(int *individual, int numberOfItems, double mutationRate){
    int mutationChance = rand() % 100;
    if (mutationChance > 100 * mutationRate)
        return;
    int position = rand() % numberOfItems;
    if (individual[position] == 0)
        individual[position] = 1;
    else
        individual[position] = 0;
    return;
}

int **Crossover(int *parent1, int *parent2, int numberOfItems, int numberOfCrossoverPoints){
    int **children = malloc(2 * sizeof(int *));
    if (children == NULL)
        return NULL;
    int *crossoverPoints = malloc((numberOfCrossoverPoints + 2) * sizeof(int));
    if (crossoverPoints == NULL){
        free(children);
        return NULL;
    }
    children[0] = malloc(numberOfItems * sizeof(int));
    children[1] = malloc(numberOfItems * sizeof(int));
    if (children[0] == NULL || children[1] == NULL){
        FreeMultipleAddresses(4, children[0], children[1], children, crossoverPoints);
        return NULL;
    }
    
    crossoverPoints[0] = 0;
    crossoverPoints[numberOfCrossoverPoints + 1] = numberOfItems;
    for (int i = 1; i < numberOfCrossoverPoints + 1; ++i){
        crossoverPoints[i] = rand() % numberOfItems;
    }
    
    qsort(crossoverPoints, numberOfCrossoverPoints + 2, sizeof(int), CompareInts);
    int phase = 0;
    for (int i = 0; i < numberOfCrossoverPoints + 1; ++i){
        if (phase == 0){
            for (int j = crossoverPoints[i]; j < crossoverPoints[i + 1]; ++j){
                children[0][j] = parent1[j];
                children[1][j] = parent2[j];
            }
            phase = 1;
        }else{
            for (int j = crossoverPoints[i]; j < crossoverPoints[i + 1]; ++j){
                children[1][j] = parent1[j];
                children[0][j] = parent2[j];
            }
            phase = 0;
        }
    }
    free(crossoverPoints);
    return children;
}