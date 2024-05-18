#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "../headers/knapsack.h"

#define FILE_NAME 'f'
#define MUTATION_RATE 'm'
#define NUMBER_OF_ELITE_I 'e'
#define HELP 'h'
#define MODULO 'r'
#define ALGORITHM 'a'
#define NUMBER_OF_GENERATIONS 'g'
#define NUMBER_OF_INDIVIDUALS_IN_GENERATION 'i'
#define NUMBER_OF_CROSSOVER_POINTS 'c'
#define QUIT 'q'
#define START 's'
#define DISPLAY_PARAMETERS 'd'
#define LINE_SIZE 256

struct ValueAndPosition{
    int value;
    int position;
};

void DisplayHelp(){
    printf("%c - help\n", HELP);
    printf("%c <algorithm name> - choose algorithm; genetic or brute\n", ALGORITHM);
    printf("%c <double 0.0 to 1.0> - to set mutation rate\n", MUTATION_RATE);
    printf("%c <integer> - to set number of elite individuals passed between generations\n", NUMBER_OF_ELITE_I);
    printf("%c <integer> - to set x in <rand() %% x> when generating random individual\n", MODULO);
    printf("%c <file name> - to pick a file you want to work with\n", FILE_NAME);
    printf("%c <integer> - to set number of generations\n", NUMBER_OF_GENERATIONS);
    printf("%c <integer> - to set number of individuals in generation\n", NUMBER_OF_INDIVIDUALS_IN_GENERATION);
    printf("%c - to set number of crossover point\n", NUMBER_OF_CROSSOVER_POINTS);
    printf("%c - to start algorithm\n", START);
    printf("%c - to display parameters\n", DISPLAY_PARAMETERS);
    printf("%c - to quit\n", QUIT);
    printf("The only necessary parameter is file name other parameters will be set to default\n");
}

void DisplayParameters(struct KnapsackProblemInfo *knapsackProblemInfo){
    printf("Algorithm:\t\t\t\t\t%s\n", (knapsackProblemInfo->algorithm == Genetic) ? "Genetic Algorithm" : "Brute-force");
    printf("Number of items:\t\t\t\t%d\n", knapsackProblemInfo->numberOfItems);
    printf("Knapsack capacity:\t\t\t\t%d\n", knapsackProblemInfo->maxWeightOfItems);
    printf("Mutation rate:\t\t\t\t\t%.2lf\n", knapsackProblemInfo->mutationRate);
    printf("Number of elite individuals:\t\t\t%d\n", knapsackProblemInfo->numberOfEliteIndividuals);
    printf("Number of generations:\t\t\t\t%d\n", knapsackProblemInfo->numberOfGenerations);
    printf("Number of individuals in generation:\t\t%d\n", knapsackProblemInfo->numberOfIndividualsInGeneration);
    printf("Random population generating parameter:\t\t%d\n", knapsackProblemInfo->randomInitModulo);
    printf("Number of crossover points: \t\t\t%d\n", knapsackProblemInfo->numberOfCrossoverPoints);
    printf("Knapsack problem data file name:\t\t%s\n", (knapsackProblemInfo->fileName == NULL) ? "None" : knapsackProblemInfo->fileName);
}

int CompareStrings(char *string1, char *string2, int length){
    for (int i = 0; i < length; ++i){
        if (string1[i] == 0 || string1[i] != string2[i])
            return 0;
    }
    return 1;
}

int Compare(const void *item1, const void *item2){
    struct ValueAndPosition i1 = *((struct ValueAndPosition*)item1);
    struct ValueAndPosition i2 = *((struct ValueAndPosition*)item2);
    return i1.value - i2.value;
}

int *CopyArray(int *array, int arraySize){
    int *newArray = malloc(arraySize * sizeof(int));
    if (newArray == NULL)
        return NULL;
    for (int i = 0; i < arraySize; ++i) {
        newArray[i] = array[i];
    }
    return newArray;
}

int ContainsOnlyOnes(int *array, int arraySize){
    for (int i = 0; i < arraySize; ++i){
        if (array[i] == 0)
            return 0;
    }
    return 1;
}

int* SolveKnapsackProblemBruteForce(struct KnapsackProblemInfo *knapsackProblemInfo){
    int bestSolutionValue = 0;
    int *bestSolution = NULL;
    int currentSolutionValue = 0;
    int *itemsSelection = malloc(knapsackProblemInfo->numberOfItems * sizeof(int));
    if (itemsSelection == NULL){
        return NULL;
    }
    for (int i = 0; i < knapsackProblemInfo->numberOfItems; ++i){
        itemsSelection[i] = 0;
    }
    int containsOnlyOnes = 0;
    while(containsOnlyOnes == 0){
        GetNextSolutionByModifyingCurrent(itemsSelection, knapsackProblemInfo->numberOfItems);
        containsOnlyOnes = ContainsOnlyOnes(itemsSelection, knapsackProblemInfo->numberOfItems);
        currentSolutionValue = CalculateValueOfSelectedItems(knapsackProblemInfo, itemsSelection);
        if (currentSolutionValue > bestSolutionValue){
            bestSolutionValue = currentSolutionValue;
            free(bestSolution);
            bestSolution = CopyArray(itemsSelection, knapsackProblemInfo->numberOfItems);
        }
    }
    printf("%d\n", bestSolutionValue);
    free(itemsSelection);
    return bestSolution;
}

int *SolveKnapsackProblemGenetic(struct KnapsackProblemInfo *knapsackProblemInfo){
    int *bestSolution = NULL;
    int *bestOfEachGeneration = malloc(knapsackProblemInfo->numberOfGenerations * sizeof(int));
    int **population = GenerateRandomPopulation(knapsackProblemInfo->numberOfItems, knapsackProblemInfo->numberOfIndividualsInGeneration);
    int **newPopulation = malloc(knapsackProblemInfo->numberOfIndividualsInGeneration * sizeof(int *));
    struct ValueAndPosition *resultsOfAllIndividuals = malloc(knapsackProblemInfo->numberOfIndividualsInGeneration * sizeof(struct ValueAndPosition));
    if (resultsOfAllIndividuals == NULL || bestOfEachGeneration == NULL || population == NULL || newPopulation == NULL){
        FreeMultipleAddresses(3, newPopulation, resultsOfAllIndividuals, bestOfEachGeneration);
        return NULL;
    }
    for (int j = 0; j < knapsackProblemInfo->numberOfGenerations; ++j){
        int numberOfPassedIndividuals = 0;
        for (int i = 0; i < knapsackProblemInfo->numberOfIndividualsInGeneration; ++i){
            resultsOfAllIndividuals[i].value = CalculateValueOfSelectedItems(knapsackProblemInfo, population[i]);
            resultsOfAllIndividuals[i].position = i;
        }
        qsort(resultsOfAllIndividuals, knapsackProblemInfo->numberOfIndividualsInGeneration, sizeof(struct ValueAndPosition), Compare);
        bestOfEachGeneration[j] = resultsOfAllIndividuals[knapsackProblemInfo->numberOfIndividualsInGeneration - 1].value;        
        for (int i = 0; i < knapsackProblemInfo->numberOfEliteIndividuals; ++i){
            newPopulation[i] = CopyArray(population[resultsOfAllIndividuals[knapsackProblemInfo->numberOfIndividualsInGeneration - i - 1].position], knapsackProblemInfo->numberOfItems);
            if (newPopulation[i] == NULL){
                for (int k = 0; k < i; ++k){
                    free(newPopulation[k]);
                }
                for (int k = 0; k < knapsackProblemInfo->numberOfIndividualsInGeneration; ++k){
                    free(population[k]);
                }
                FreeMultipleAddresses(4, newPopulation, population, resultsOfAllIndividuals, bestOfEachGeneration);
                return NULL;
            }
            ++numberOfPassedIndividuals;
        }
        while (numberOfPassedIndividuals < knapsackProblemInfo->numberOfIndividualsInGeneration){
            int parent1 = rand() % knapsackProblemInfo->numberOfIndividualsInGeneration;
            int parent2 = rand() % knapsackProblemInfo->numberOfIndividualsInGeneration;
            int parent3 = rand() % knapsackProblemInfo->numberOfIndividualsInGeneration;
            int parent4 = rand() % knapsackProblemInfo->numberOfIndividualsInGeneration;
            if (CalculateValueOfSelectedItems(knapsackProblemInfo, population[parent1]) < CalculateValueOfSelectedItems(knapsackProblemInfo, population[parent3]))
                parent1 = parent3;
            if (CalculateValueOfSelectedItems(knapsackProblemInfo, population[parent2]) < CalculateValueOfSelectedItems(knapsackProblemInfo, population[parent4]))
                parent2 = parent4;
            int **children = Crossover(population[parent1], population[parent2], knapsackProblemInfo->numberOfItems,knapsackProblemInfo->numberOfCrossoverPoints);
            if (children == NULL){
                    for (int k = 0; k < numberOfPassedIndividuals; ++k){
                    free(newPopulation[k]);
                }
                for (int k = 0; k < knapsackProblemInfo->numberOfIndividualsInGeneration; ++k){
                    free(population[k]);
                }
                FreeMultipleAddresses(4, newPopulation, population, resultsOfAllIndividuals, bestOfEachGeneration);
            }
            MutateIndividual(children[0], knapsackProblemInfo->numberOfItems, knapsackProblemInfo->mutationRate);
            newPopulation[numberOfPassedIndividuals] = children[0];         
            ++numberOfPassedIndividuals;  
            if (numberOfPassedIndividuals < knapsackProblemInfo->numberOfIndividualsInGeneration){                          // in case odd number of elite
                MutateIndividual(children[1], knapsackProblemInfo->numberOfItems, knapsackProblemInfo->mutationRate);
                newPopulation[numberOfPassedIndividuals] = children[1];
                ++numberOfPassedIndividuals;
            }
            else{
                free(children[1]);
            }
            free(children);
        }
        for (int i = 0; i < knapsackProblemInfo->numberOfIndividualsInGeneration; ++i){
            free(population[i]);
            population[i] = newPopulation[i];
        }
        
    }
    for (int i = 0; i < knapsackProblemInfo->numberOfIndividualsInGeneration; ++i){
            resultsOfAllIndividuals[i].value = CalculateValueOfSelectedItems(knapsackProblemInfo, population[i]);
            resultsOfAllIndividuals[i].position = i;
        }
    qsort(resultsOfAllIndividuals, knapsackProblemInfo->numberOfIndividualsInGeneration, sizeof(struct ValueAndPosition), Compare);
    bestSolution = CopyArray(population[resultsOfAllIndividuals[knapsackProblemInfo->numberOfIndividualsInGeneration - 1].position], knapsackProblemInfo->numberOfItems);
        if (bestSolution == NULL){
            for (int i = 0; i < knapsackProblemInfo->numberOfIndividualsInGeneration; ++i){
                free(population[i]);
            }
            FreeMultipleAddresses(2, population, resultsOfAllIndividuals);
            return NULL;
        }
    //free
    for (int i = 0; i < knapsackProblemInfo->numberOfIndividualsInGeneration; ++i){
        free(population[i]);
    }
    FreeMultipleAddresses(4, population, newPopulation, resultsOfAllIndividuals, bestOfEachGeneration);

    printf("%d\n", CalculateValueOfSelectedItems(knapsackProblemInfo, bestSolution));
    return bestSolution;
}

char* CopyFileName(char* line, int lineLength){
    int fileNameLength = 0;
    for (int i = 0; i < lineLength; ++i){
        if (line[i] == 0 || line[i] == '\n'){
            fileNameLength = i + 1;
            break;
        }
    }
    char* fileName = malloc(fileNameLength * sizeof(char));
    if (fileName == NULL)
        return NULL;
    for (int i = 0; i < fileNameLength; ++i){
        fileName[i] = line[i];
    }
    fileName[fileNameLength - 1] = 0;
    return fileName;
}

int main(int argc, char *argv[]){
    int temp, converted;
    double tmp;
    time_t t;
    srand((unsigned)time(&t));
    char action = 0;
    char *line = malloc(LINE_SIZE * sizeof(char));
    if (line == NULL)
        return -1;
    DisplayHelp();
    struct KnapsackProblemInfo *knapsackProblemInfo = InitKnapsackInfoProblem();
    while (action != QUIT){
        action = fgetc(stdin);
        fgetc(stdin);
        if (action < 'a')
            action += 'a' - 'A';
        switch (action)
        {
            case QUIT:
                printf("Quitting!\n");
                break;
            case HELP:
                DisplayHelp();
                break;
            case START:
                if (knapsackProblemInfo->fileName != NULL && knapsackProblemInfo->items != NULL){
                    printf("Started solving!\n");
                    if (knapsackProblemInfo->algorithm == BruteForce){
                        int * result = SolveKnapsackProblemBruteForce(knapsackProblemInfo);
                        if (result == NULL){
                            printf("Something went wrong\n");
                            free(knapsackProblemInfo->fileName);
                            free(knapsackProblemInfo->items);
                            free(line);
                            return -1;
                        }
                        free(result);
                    }
                    else{
                        int *result = SolveKnapsackProblemGenetic(knapsackProblemInfo);
                        if (result == NULL){
                            printf("Something went wrong\n");
                            free(knapsackProblemInfo->fileName);
                            free(knapsackProblemInfo->items);
                            free(line);
                            return -1;
                        }
                        free(result);
                    }
                    free(knapsackProblemInfo->fileName);
                    free(knapsackProblemInfo->items);
                    knapsackProblemInfo->items = NULL;
                    knapsackProblemInfo->fileName = NULL;
                }
                else{
                    printf("Pick a file first\n");
                }
                
                break;
            case DISPLAY_PARAMETERS:
                DisplayParameters(knapsackProblemInfo);
                break;
            case FILE_NAME:
                printf("Enter file name\n");
                fgets(line, LINE_SIZE, stdin);
                char *fileName = CopyFileName(line, LINE_SIZE);
                if (fileName == NULL){
                    printf("Couldn't copy file name\n");
                    break;
                }
                if (knapsackProblemInfo->fileName != NULL){
                    free(knapsackProblemInfo->fileName);
                    knapsackProblemInfo->fileName = NULL;
                }

                if (knapsackProblemInfo->items != NULL){
                    free(knapsackProblemInfo->items);
                    knapsackProblemInfo->items = NULL;
                }
                ReadDataFromFileToKnapsackProblemInfo(fileName, knapsackProblemInfo);
                if (knapsackProblemInfo->items == NULL)
                    printf("Something is wrong with the file\n");
                else
                    knapsackProblemInfo->fileName = fileName;
                break;
            case MUTATION_RATE:
                printf("Enter mutation rate from 0 to 1\n");
                converted = scanf("%lf\n", &tmp);
                if (converted != 1 || tmp > 1 || tmp < 0){
                    printf("Wrong value\n");
                    break;
                }
                knapsackProblemInfo->mutationRate = temp;
                break;
            case NUMBER_OF_ELITE_I:
                printf("Enter number of elite individuals from 0 to 10\n");
                converted = scanf("%d\n", &temp);
                if (converted != 1 || temp < 0 || temp > 10){
                    printf("Wrong value\n");
                    break;
                }
                knapsackProblemInfo->numberOfEliteIndividuals = temp;
                break;
            case NUMBER_OF_GENERATIONS:
                printf("enter number of generations from 100 to 10000\n");
                converted = scanf("%d\n", &temp);
                if (converted != 1 || temp < 100 || temp > 10000){
                    printf("Wrong value\n");
                    break;
                }
                knapsackProblemInfo->numberOfGenerations = temp;
                break;
            case NUMBER_OF_INDIVIDUALS_IN_GENERATION:
                printf("Enter number of individuals in generation from 100 to 10000\n");
                converted = scanf("%d\n", &temp);
                if (converted != 1 || temp < 100 || temp > 10000){
                    printf("Wrong value\n");
                    break;
                }
                knapsackProblemInfo->numberOfIndividualsInGeneration = temp;
                break;
            case ALGORITHM:
            printf("b - for brute force\ng - for genetic\n");
                char algorithm = fgetc(stdin);
                fgetc(stdin);
                if (algorithm == 'b'){
                    knapsackProblemInfo->algorithm = BruteForce;
                }else if (algorithm == 'g'){
                    knapsackProblemInfo->algorithm = Genetic;
                }else{
                    printf("Couldn't recognize algorithm\n");
                }
                break;
            case MODULO:
                printf("Enter n, where n defines one out of n items will be randomly selected at the beggining from 1 to 10000\n");
                converted = scanf("%d\n", &temp);
                if (converted != 1 || temp < 1 || temp > 10000){
                    printf("Wrong value\n");
                    break;
                }
                knapsackProblemInfo->randomInitModulo = temp;
                break;
            case NUMBER_OF_CROSSOVER_POINTS:
                printf("Enter number of crossover points from 1 to 20\n");
                converted = scanf("%d\n", &temp);
                if (converted != 1 || temp < 1 || temp > 20){
                    printf("Wrong value\n");
                    break;
                }
                knapsackProblemInfo->numberOfCrossoverPoints = temp;
                break;
            default:
                printf("Couldn't recognize action! h for help\n");
                break;
        }
    }
    //ReadDataFromFileToKnapsackProblemInfo("knapPI_1_1000_1000_1", knapsackProblemInfo);
   // SolveKnapsackProblemGenetic(knapsackProblemInfo);
    if (knapsackProblemInfo->items != NULL)
        free(knapsackProblemInfo->items);
    if (knapsackProblemInfo->fileName != NULL)
        free(knapsackProblemInfo->fileName);
    free(knapsackProblemInfo);
    free(line);
    return 0;
}