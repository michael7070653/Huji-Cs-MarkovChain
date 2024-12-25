cmake_minimum_required(VERSION 3.25)
project(ex3b C)

set(CMAKE_C_STANDARD 11)

add_executable(ex3b tweets_generator.c markov_chain.h markov_chain.c linked_list.h linked_list.c)
