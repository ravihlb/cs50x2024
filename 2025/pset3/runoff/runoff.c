#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
// i means voter index
// j means candidate preference index for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
} candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;
int curr_pref_index;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);
void print_candidates();
void print_preferences();

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }

    // init candidates array of structs
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (candidates[i].eliminated)
                    continue;

                printf("%s\n", candidates[i].name);
            }

            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }

    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    for (int j = 0; j < candidate_count; j++)
    {
        if (strcmp(name, candidates[j].name) != 0)
            continue;

        preferences[voter][rank] = j;

        return true;
    }

    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    // iterate over preferences using voter_count as the bound
    // for each voter, iterate over their preferences using candidate_count as the bound
    // if the given candidate index has eliminated == true, we skip
    // else, we count their vote, meaning we update their entry on the candidates array
    // essentially, candidates[j].votes += 1;

    // iterates over voters
    for (int i = 0; i < voter_count; i++)
    {
        // count vote for the highest non-eliminated candidate
        for (int j = 0; j < candidate_count; j++)
        {
            int chosen = preferences[i][j];

            if (candidates[chosen].eliminated)
                continue;

            candidates[chosen].votes += 1;
            break;
        }
    }

    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    for (int j = 0; j < candidate_count; j++)
    {
        candidate preferred_candidate = candidates[j];

        if (preferred_candidate.eliminated)
            continue;

        // if more than 50% of voter_count has voted for the same candidate in this iteration,
        // that candidate wins
        if (preferred_candidate.votes / (float) voter_count > 0.5)
        {
            printf("%s\n", preferred_candidate.name);
            return true;
        }
    }

    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    int min;

    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated)
            continue;

        int curr_votes = candidates[i].votes;

        if (i == 0)
        {
            min = curr_votes;
            continue;
        }

        if (curr_votes < min)
            min = curr_votes;
    }

    return min;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    // iterate over current preferences
    // if candidate is elimininated, skip

    int max_votes;

    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated)
            continue;

        if (i == 0)
        {
            max_votes = candidates[i].votes;
            continue;
        }

        if (max_votes < candidates[i].votes)
            max_votes = candidates[i].votes;
    }

    if (max_votes == min)
        return true;

    return false;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{

    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated)
            continue;

        if (candidates[i].votes == min)
            candidates[i].eliminated = true;
    }

    return;
}

void print_candidates()
{
    printf("Current candidates:\n");

    for (int c = 0; c < candidate_count; c++)
    {
        candidate can = candidates[c];
        printf("Name: %s\n", can.name);
        printf("Votes: %i\n", can.votes);
        printf("Eliminated: %i\n", can.eliminated);
        printf("\n");
    }

    printf("\n");
}

void print_preferences()
{
    printf("Current preferences:\n");

    // iterates over voters
    for (int i = 0; i < voter_count; i++)
    {
        // count vote for the highest non-eliminated candidate
        for (int j = 0; j < candidate_count; j++)
        {
            candidate chosen = candidates[preferences[i][j]];
            if (chosen.votes < 1)
                continue;

            printf("Candidate: %s\n", chosen.name);
            printf("Votes: %i\n", chosen.votes);
            printf("\n");
        }
    }

    printf("\n");
}
