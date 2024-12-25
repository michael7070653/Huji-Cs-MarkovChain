# Makefile

# שם הבינארי המיוצר
TWEETS_OUTPUT = tweets_generator
SNAKE_OUTPUT = snakes_and_ladders

# קבצי המקור
SOURCES_TWEETS = tweets_generator.c
SOURCES_SNAKE = snakes_and_ladders.c
COMMON_SOURCES = linked_list.c markov_chain.c

# קבצי האובייקט
OBJECTS_TWEETS = $(SOURCES_TWEETS:.c=.o) $(COMMON_SOURCES:.c=.o)
OBJECTS_SNAKE = $(SOURCES_SNAKE:.c=.o) $(COMMON_SOURCES:.c=.o)

# Docker image
DOCKER_IMAGE = valgrind-env

# מטרה להרצת הקומפילציה בתוך Docker
%.o: %.c
	docker run --rm -v $(PWD):/app $(DOCKER_IMAGE) gcc -Wall -Wvla -Werror -std=c99 -c $< -o $@

tweets: $(OBJECTS_TWEETS)
	docker run --rm -v $(PWD):/app $(DOCKER_IMAGE) gcc -Wall -Wvla -Werror -std=c99 $(OBJECTS_TWEETS) -o $(TWEETS_OUTPUT)

snake: $(OBJECTS_SNAKE)
	docker run --rm -v $(PWD):/app $(DOCKER_IMAGE) gcc -Wall -Wvla -Werror -std=c99 $(OBJECTS_SNAKE) -o $(SNAKE_OUTPUT)

clean:
	rm -f *.o $(TWEETS_OUTPUT) $(SNAKE_OUTPUT)
