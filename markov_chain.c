 #include "markov_chain.h"


#include "linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SAME_DATA 0
#define DATA_IN_DATABASE 0
#define DATA_NOT_IN_DATABASE 1
#define ALLOCATE_IN_LIST_FAIL 1
#define MEMORY_ALLOCATION_FAIL NULL
#define IN_FREQ_LIST 0
#define NOT_IN_FREQ_LIST 1
#define FREQ_ARR_SIZE  sizeof(NextNodeCounter)
#define EMPTY_ARR 0
#define FAIL_STR_ALLOCATE "Allocation failure: exit program"


///function call//;







int check_in_freq_list(MarkovChain *markov_chain,
                       NextNodeCounter *point_to_freq_list,
                       int size, void *data);



///Get random number between 0 and max_number [0, max_number).
 ///param max_number maximal number
 /// to return (not including)///@return Random number
///
int get_random_number(int max_number)
{
    return rand() % max_number;
}



/**
* Check if data_ptr is in database.
 * If so, return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */



Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{

    Node *head = markov_chain->database->first;
    while (head!= NULL)
    {
      comp compare = markov_chain->comp_func;
        if (compare(head->data->data,data_ptr) == SAME_DATA)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}








/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    ///check if node is already in database
    copy copy_function = markov_chain->copy_func;
    free_d free_function = markov_chain->free_data;
    Node *head = get_node_from_database(markov_chain, data_ptr);
    if (head != NULL)
    {
        return head;
    }

    ///create new markov node
    MarkovNode *new_markov = malloc(sizeof(MarkovNode));
    if(!new_markov)
    {
        fprintf(stdout,"%s", FAIL_STR_ALLOCATE);
        return NULL;
    }

    void *new_alloc_word = copy_function(data_ptr);

    if (!new_alloc_word)
    {
        fprintf(stdout,"%s", FAIL_STR_ALLOCATE);
        free(new_markov);
        return NULL;
    }
    *new_markov = (MarkovNode) {new_alloc_word,
                                NULL,0,
                                0};

    int check_add_to_database = add(markov_chain->database,
                                    new_markov);
    if(!check_add_to_database)
    {
        return markov_chain->database->last;
    }
    fprintf(stdout,"%s", FAIL_STR_ALLOCATE);

    free_function(new_markov->data);
    free(new_markov);///CHECK_VALGRIND OR SIGMANT FAULT
    return NULL;
}


/// HERE I CHECK IF NODE IS ALREADY IN FREQ LIST ,IF IN ADD 1.
int check_in_freq_list(MarkovChain *markov_chain,
                       NextNodeCounter *point_to_freq_list,
                       int size, void* data)
{
    for(int i = 0; i < size; i++)
    {
      comp compare = markov_chain->comp_func;
        if(compare((point_to_freq_list + i)
        ->markov_node->data, data)== SAME_DATA)
        {
            /// NODE is already in freq_list and we add 1.
            (point_to_freq_list + i)->frequency++;
            return IN_FREQ_LIST;
        }
    }
    return NOT_IN_FREQ_LIST;
}




void put_second_node_in_first(MarkovNode *first_node,
                              MarkovNode *second_node,
                              NextNodeCounter *temp_reallocate)
{
    first_node->frequency_lst = temp_reallocate;
    (first_node->frequency_lst + first_node->size_arr)
    ->markov_node = second_node;
    (first_node->frequency_lst + first_node->size_arr)
    ->frequency = 1;
    first_node->num_words_after ++;
    first_node->size_arr ++;
}


///allocate new place in freq list of node 1
bool allocate(MarkovNode *first_node, MarkovNode *second_node)
{

  NextNodeCounter *temp_reallocate =
            realloc(first_node->frequency_lst,
                    (first_node->size_arr+1) * FREQ_ARR_SIZE);

    if (temp_reallocate == MEMORY_ALLOCATION_FAIL)
    {
        fprintf(stdout,"%s", FAIL_STR_ALLOCATE);
        return false;
    }
    put_second_node_in_first(first_node,second_node,temp_reallocate);
    return true;
}




/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain)
{
    void *second_node_data = second_node->data;

    int check_list = check_in_freq_list(markov_chain,
                                        first_node->
                                        frequency_lst,
                                        first_node->size_arr,
                                        second_node_data);

    if (check_list == IN_FREQ_LIST)
    {
        first_node->num_words_after ++;
        return true;
    }

    bool reallocate = allocate(first_node, second_node);
    if(reallocate == true)
    {
        return true;
    }
    return false;
}



void free_markov_node(MarkovChain *markov_chain,Node *head)
{
    free_d free_data = markov_chain->free_data;
    if(head->data->size_arr !=0)
    {
        free(head->data->frequency_lst);
    }

    free_data(head->data->data);
    free(head->data);
    free(head);
}



/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain(MarkovChain **markov_chain)

{   if (!*markov_chain)
    {
        return;
    }
    int size_list = (*markov_chain)->database->size;
    Node *head = (*markov_chain)->database->first;
    for (int i = 0; i < size_list; i++)
    {
        Node *temp = head->next;
        free_markov_node(*markov_chain,head);
        head = temp;
    }
    head = NULL;
    free((*markov_chain)->database);
    free(*markov_chain);
    (*markov_chain) = NULL;
}





















/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */

MarkovNode* get_first_random_node(MarkovChain *markov_chain)

{
    int size = markov_chain->database->size;
    last_or_not is_ends_with_period = markov_chain->is_last;

  while (true)
    {
        Node *rand = markov_chain->database->first;
        int res = get_random_number(size);
        for(int i = 0; i < res;i++ )
        {
            rand = rand->next;
        }
        if(is_ends_with_period(rand->data->data) == false)///check
        {
            return rand->data;
        }
    }
}









/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr)


{
    int next_rand = get_random_number(state_struct_ptr
        ->num_words_after);
  NextNodeCounter *freq_list = state_struct_ptr->frequency_lst;
    int i = freq_list->frequency;
    while (i <= next_rand)
    {
        freq_list ++;
        i += freq_list->frequency;
    }
    return freq_list->markov_node;

}








/**
 * Receive markov_chain, generate and
 * print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to
 * start with, if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)

{
  print print_tweet_func = markov_chain->print_func;
  if (first_node == NULL)
    {
        first_node = get_first_random_node(markov_chain);
    }

    int x = 0;
    last_or_not is_ends_with_period = markov_chain->is_last;
    while (is_ends_with_period(first_node->data) == false && x <
    max_length - 1)///check!!
    {
      print_tweet_func(first_node->data);
      first_node = get_next_random_node(first_node);
        x++;
    }
  print_tweet_func(first_node->data);

}









