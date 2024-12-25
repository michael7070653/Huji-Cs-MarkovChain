#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define ARGC_NOT_VALID "Usage: num of args not valid"
#define ALLOC_FAILURE "Allocation failure: there isn't enough memory"
#define TWO_ARGS 3
#define SEED_LOC 1
#define NUM_OF_GAMES 2
#define STARTING_WALK "Random Walk"


/**
 * reppresents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */






static MarkovChain* create_mark_of_chane();









const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * ssstruct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump
    // of the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump
    // of the snake in case there is one from this square
    //both ladder_to and snake_to should be -1 if the
    // Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_markov_chain(database);
    }
    return EXIT_FAILURE;
}






static void print_snake(void* cell)
{
  Cell *data = (Cell*) cell;

  if(data->ladder_to != EMPTY)
  {
    fprintf (stdout, "[%d]-ladder to %d -> ",
             data->number,data->ladder_to);
    return;
  }

  else if(data->snake_to != EMPTY)
  {
    fprintf (stdout, "[%d]-snake to %d -> ",
             data->number,data->snake_to);
    return;
  }
  else if (data->number == BOARD_SIZE)
  {
    fprintf (stdout,"[%d]", data->number);
    return;
  }
  else{
    fprintf (stdout,"[%d] -> ", data->number);

  }

}



static void* copy_cell(void* cell)
{
  Cell *data = (Cell*) cell;
  Cell *new_allocation = malloc (sizeof (Cell));
  if( new_allocation == NULL)
  {
    return NULL;
  }
  *new_allocation = (Cell){data->number,data->ladder_to,data->snake_to};
  return (void *) new_allocation;

}


static int compare_snake(void* cell_0, void * cell_1)
{
  Cell *data = (Cell*) cell_0;
  Cell *data_1 = (Cell*) cell_1;
  return data->number - data_1->number;

}



static bool is_last_or_not(void* cell)
{
  Cell *data = (Cell*) cell;
  if (data-> number == BOARD_SIZE)
  {
    return true;
  }
  return false;
}




static void free_snake(void *cell)
{
  Cell *data = (Cell*) cell;
  free (data);
}














static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if(create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain,cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])
                    ->data;
            add_node_to_counter_list(from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])
                        ->data;
                add_node_to_counter_list(from_node,
                                         to_node, markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}


static MarkovChain* create_mark_of_chane()
{
  LinkedList *list = malloc (sizeof (LinkedList));
  if(list == NULL)
  {
    return NULL;
  }
  MarkovChain *markov = malloc (sizeof (MarkovChain));
  if(markov == NULL)
  {
    free (list);
    return NULL;
  }
  *list =(LinkedList) {NULL,NULL, 0};
  *markov = (MarkovChain){list,print_snake,
                          compare_snake,free_snake,
                          copy_cell,is_last_or_not};
  return markov;

}









/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */






int main(int argc, char *argv[])
{
  if(argc != TWO_ARGS)
  {
    fprintf (stdout,ARGC_NOT_VALID);
    return EXIT_FAILURE;
  }
  MarkovChain *markov = create_mark_of_chane();
  if(markov== NULL)
  {
    fprintf (stdout,ALLOCATION_ERROR);
    return EXIT_FAILURE;
  }
  int check = fill_database (markov);
  if(check)
  {
    fprintf (stdout,ALLOCATION_ERROR);
    free_markov_chain (&markov);
    return EXIT_FAILURE;
  }

  int int_seed = 0;
  int num_games = 0;
  sscanf (argv[1],"%d",&int_seed);
  sscanf (argv[2],"%d",&num_games);
  srand (int_seed);
  for(int i = 1; i < num_games + 1; i++)
  {
    fprintf (stdout,"Random Walk %d: ",i);
    generate_random_sequence (markov,
                              markov->database->first->data,
                              MAX_GENERATION_LENGTH);
    fprintf (stdout,"\n");
  }

  free_markov_chain (&markov);
  return EXIT_SUCCESS;
}
