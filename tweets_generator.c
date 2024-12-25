#define MAX_LINE 1000
#define MAX_WORD 100
#include "markov_chain.h"
#include "linked_list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN_ARG_C 4
#define MAX_ARG_C 5
#define FAIL_STR_ALLOCATE "Allocation failure: exit program"
#define MAX_LEN_TWEET 20
#define BASE_10 10

static bool is_ends_with_period(void * data_word);

///pointers funkk
static int compare_str (void *a, void *b)
{
  char *temp_1 = (char*) a;
  char *temp_2 = (char *) b;
  return strcmp(temp_1,temp_2);
}


static void print_string(void* data)
{
  char *p = (char*) data;
  if(is_ends_with_period (p)){
    fprintf (stdout,"%s", p);
    return;
  }
  fprintf (stdout,"%s ", p);
}




static bool is_ends_with_period(void *data_word)
{
  char *word = (char*)data_word;
  size_t length = strlen(word);

  // Check if the last character is a period
  if (length > 0 && word[length - 1] == '.') {
    return true;  // True, the word ends with a period
  }
  else
  {
    return false;  // False, the word does not end with a period
  }
}



static void free_data_str(void *word)
{
//  char *data = (char *) word;
  free (word);
}



static void* copy_data(void* data)
{
  char* word = (char*) data;
  char* word_to_return = malloc (strlen (word) + 1);
  if (word_to_return == NULL)
  {
    return NULL;
  }
  memcpy(word_to_return,word, strlen (word) + 1);
  return (void *) word_to_return;
}








static bool create_data_base_for_line(char buffer_line[MAX_LINE],
                               MarkovChain *markov_chain,
                               int *max_word, int *number_word,bool x)
{
    char *token = strtok(buffer_line, " \n\r");


    char *prev_token = token;

    while ((token = strtok(NULL, " \n\r")) != NULL
        && token != NULL && (*number_word) < *(max_word)) {

        Node *current = add_to_database(markov_chain,prev_token);
        Node *next_word = add_to_database(markov_chain,token);
        if (current == NULL || next_word == NULL)
        {
            return false;
        }
        bool add = add_node_to_counter_list(current->data,
                                            next_word->data,
                                             markov_chain);
        if(add == false)
        {
            return false;
        }
        prev_token = token;
        (*number_word) ++;
        *(max_word) = x == false ? 2 * *(number_word) :*(max_word);

    }
    (*number_word) ++;
    return true;
}





static int fill_database(FILE *fp, int words_to_read,
                         MarkovChain *markov_chain)
{
    char buffer_line[MAX_LINE];
    int max_word = 1;
    bool x = words_to_read == 0 ? false : true;
    words_to_read = x == false ? 2 * max_word : words_to_read;
    while (fgets(buffer_line,MAX_LINE,fp)!= NULL && max_word < words_to_read)
    {

        bool one_line = create_data_base_for_line(buffer_line, markov_chain,
                                                  &words_to_read,
                                                  &max_word,x);
        if (one_line == false)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}









///


static void extract_info_argv(int argc ,char *argv[],int *num_tweet,
                       unsigned int *seed, int *num_to_read)
{
    if(argc == 4)
    {
       *seed = strtoul(*(argv + 1), NULL, BASE_10);
       *num_tweet = (int)strtoul(*(argv + 2), NULL, BASE_10);

    } else
    {
        *seed = strtoul(*(argv + 1), NULL,
                        BASE_10);
        *num_tweet = (int)strtoul(*(argv + 2), NULL,
                                  BASE_10);
        *num_to_read = (int) strtoul(*(argv + 4),
                                     NULL, BASE_10);
    }


}



static void fill_markov(MarkovChain *markov_chain, LinkedList *database)
{
  markov_chain->database = database;
  markov_chain->print_func = print_string;
  markov_chain->comp_func = compare_str;
  markov_chain->free_data = free_data_str;
  markov_chain->copy_func = copy_data;
  markov_chain->is_last = is_ends_with_period;
}






MarkovChain* create_markov(int argc,char *argv[],FILE *corpus,int *num_tweet,
                           unsigned int *seed,int *num_to_read)
{
  extract_info_argv(argc,argv,num_tweet,seed,num_to_read);
  MarkovChain *markov = malloc(sizeof (MarkovChain));

  if( markov == NULL){
    fprintf(stdout,"%s", FAIL_STR_ALLOCATE);
    fclose (corpus);
    return NULL;
  }

  LinkedList *list  = malloc(sizeof (LinkedList));
  if(list == NULL){
    fprintf(stdout,"%s", FAIL_STR_ALLOCATE);
    fclose (corpus);
    free(markov);
    return NULL;
  }
  *list =(LinkedList) {NULL,NULL,0};
  fill_markov(markov,list);
  if(fill_database(corpus,*num_to_read,markov))
  {
    fclose (corpus);
    free_markov_chain(&(markov));
    return NULL;
  }
  return markov;
}






int main(int argc,char *argv[])
{
    if (argc < MIN_ARG_C|| argc > MAX_ARG_C)
    {
        fprintf(stdout,"Usage: not valid args!");
        return EXIT_FAILURE;
    }

    int num_tweet = 0;
    unsigned int seed = 0;
    int num_to_read = 0;
    FILE *corpus = fopen(*(argv + 3),"r");

    if (corpus == NULL)
    {
        fprintf(stdout, "Error: cant open file");
        return EXIT_FAILURE;
    }
    MarkovChain *markov = create_markov (argc,argv,corpus,&num_tweet,&seed,
                                         &num_to_read);
    if(markov == NULL)
    {
      return EXIT_FAILURE;
    }
    srand(seed);
  for(int i = 1; i < num_tweet + 1; i++)
  {
      fprintf(stdout,"Tweet %d: ",i);
      generate_random_sequence(markov,NULL,
                               MAX_LEN_TWEET);
      fprintf(stdout,"\n");
  }
    fclose(corpus);
    free_markov_chain(&(markov));
    return EXIT_SUCCESS;
}