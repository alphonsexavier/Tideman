#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX]; //Candidates are stored here
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void print_preferences(void);
void print_pairs(void);
void print_locked(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int i;

    for (int r = 0; r < candidate_count - 1 ; r++)
    {
        i = ranks[r];
        //printf("i: %i\n",i);

        for (int j = 0; j < candidate_count; j++)
        {
            if (i == j)
            {
                preferences[i][j] = 0;
               // printf("i: %i, j: %i, preferences[i][j]: %i\n",i,j,preferences[i][j]);
            }
            else
            {
                for (int k = 0;; k++)
                {
                    // i is preferred over j
                    if (ranks[k] == i)
                    {
                        preferences[i][j] = preferences[i][j] + 1;
                        break;
                    }
                    // j is preferred over i
                    if (ranks[k] == j)
                    {
                        preferences[i][j] = preferences[i][j] + 0;
                        break;
                    }
                }
            }
        }
    }
    return;
}

//Prints the preferences matrix
void print_preferences(void)
{
    printf("Preferences matrix:\n");

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%i\t", preferences[i][j]);
        }

        printf("\n");
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int count = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] == preferences[j][i])
            {
                continue;
            }
            else if (preferences[i][j] > preferences[j][i])
            {
                pairs[count].winner = i;
                pairs[count].loser = j;
                ++count;
            }
            else
            {
                pairs[count].winner = j;
                pairs[count].loser = i;
                ++count;
            }
        }
    }

    pair_count = count;

    return;
}

void print_pairs(void)
{
    printf("\n");
    for (int i = 0; i < pair_count; i++)
    {
        printf("Winner: %i, Loser: %i", pairs[i].winner, pairs[i].loser);
        printf("\n");
    }
    printf("\n");
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int index_1 = 0, index_2 = 0;

    for (int i = 0; i < pair_count - 1; i++)
    {
        index_1 = pairs[i].winner;
        index_2 = pairs[i].loser;

        for (int j = i + 1; j < pair_count; j++)
        {
            int index_3 = pairs[j].winner, index_4 = pairs[j].loser;
            int temp_winner, temp_loser;

            if (preferences[index_3][index_4] > preferences[index_1][index_2])
            {
                temp_winner = pairs[i].winner;
                temp_loser = pairs[i].loser;
                pairs[i].winner = pairs[j].winner;
                pairs[i].loser = pairs[j].loser;
                pairs[j].winner = temp_winner;
                pairs[j].loser = temp_loser;
            }
        }
    }


    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int temp_1, temp_2, temp_3, flag = 0, count = 0, count_2 = 0, found = 0, index = 0;

    for (int i = 0; i < pair_count; i++)
    {
        temp_1 = pairs[i].winner;
        temp_2 = pairs[i].loser;

    //checking the final pair

    if (count == candidate_count - 1)
    {
        for (int j = i - 1; j >= 0; j--)
        {
            if (temp_2 == pairs[j].winner)
            {
                flag = 1;
                break;
            }
        }
    }

    //checking the middle pairs

    if ((count != candidate_count - 1) && (count >= 1))
    {
        temp_3 = temp_2;

        for (int j = 0; j < count; j++)
        {
            if (temp_3 == pairs[j].winner)
            {
                index = j;
                found = 1;
            }
        }

    if (found == 1)
    {

        for (int j = index; j < count; j++)
        {
            if ((temp_3 == pairs[j].winner) && (pairs[j].loser == pairs[j + 1].winner))
            {
                temp_3 = pairs[j].loser;
                flag = 1;
            }
            else
            {
                flag = 0;
                break;
            }
        }
    }
    }

        if (flag == 1 && (count == candidate_count - 1))
        {
            break;
        }

        if (flag == 1)
        {
            flag = 0;
            continue;
        }

        if (flag == 0)
        {
            locked[temp_1][temp_2] = true;
            ++count;
        }

    }

    return;
}

void print_locked(void)
{
    printf("\n");
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%i\t", locked[i][j]);
        }

    printf("\n");
    }
}

// Print the winner of the election
void print_winner(void)
{
    int arr[MAX] = {0};

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                arr[i] = 1;
            }
        }
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (arr[i] == 0)
        {
            printf("%s\n", candidates[i]);
        }
    }

    return;
}