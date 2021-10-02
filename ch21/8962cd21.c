/*
 * Chapter 21
 * Genetic Algorithms
 *
 * A simple program demonstrating the use of a genetic
 * algorithm to recommend stock trading decisions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

/*
 * Control Constants
 */

/* The number of quotes available in the data files */
#define HISTORY_LENGTH     253

/* The highest level a stock price is likely to go */
#define MAX_PRICE         1000

/* How many organisms are assigned to each stock */
#define SPECIES_POPULATION 100

/* How many stock ticker symbols there are */
#define NUM_STOCKS           3

/* End the program after this many generations */
#define NUM_GENERATIONS    100

/* How many generations pass between each migration between species */
#define MIGRATION_RATE      30

/* Mutation should afflict one in this many organisms every generation */
#define MUTATION_FACTOR     32

/* Descriptive labels for our stocks */
char const* const ticker[NUM_STOCKS] =
{
    "IBM",
    "MSFT",
    "INTC"
};

/* File names of the stock data */
char const* const filename[NUM_STOCKS] =
{
    "IBM.DAT",
    "MSFT.DAT",
    "INTC.DAT"
};


/*
 * Data Types
 */

typedef unsigned int gene_t;

typedef struct
{
    gene_t buy_price;
    gene_t sell_price;
} chromosome_t;

typedef struct
{
    chromosome_t individual[SPECIES_POPULATION];
    long record_cash;
} species_t;

typedef struct
{
    species_t species[NUM_STOCKS];
} population_t;

typedef struct
{
    unsigned int price[NUM_STOCKS][HISTORY_LENGTH];
} history_t;


/*
 * Randomization function
 * This function needn't return very random values.
 */
unsigned int rand_rng(unsigned int max)
{
    return rand() / (RAND_MAX / max + 1);
}


/*
 * Gray Code Conversion Functions
 */

unsigned int bin_to_Gray(unsigned int n)
{
    return n ^ (n >> 1);
}

unsigned int Gray_to_bin(unsigned int n)
{
    int i;
    for(i = 0; (1 << i) < sizeof(n) * CHAR_BIT; i++)
    {
        n ^= n >> (1 << i);
    }
    return n;
}


/*
 * Problem-specific Functions
 */

void initialize_chromosome(chromosome_t* c)
{
    c->buy_price = bin_to_Gray(rand_rng(MAX_PRICE));
    c->sell_price = bin_to_Gray(rand_rng(MAX_PRICE));
}

void initialize_species(species_t* s)
{
    int i;
    for(i = 0; i < SPECIES_POPULATION; i++)
    {
        initialize_chromosome(&s->individual[i]);
    }
}

void initialize_population(population_t* p)
{
    int i;
    for(i = 0; i < NUM_STOCKS; i++)
    {
        initialize_species(&p->species[i]);
    }
}

void mutate_gene(gene_t* g)
{
    /* Flip a random bit in the gene */
    *g ^= 1 << rand_rng(sizeof(gene_t) * CHAR_BIT);
}

void mutate_chromosome(chromosome_t* c)
{
    switch(rand_rng(2))
    {
        case 0:
            mutate_gene(&c->buy_price);
            break;
        case 1:
            mutate_gene(&c->sell_price);
            break;
    }
}

void mutate_species(species_t* s)
{
    unsigned int i;
    
    for(i = 0; i < SPECIES_POPULATION; i++)
    {
        /* Mutate only one in every MUTATION_FACTOR */
        if(rand_rng(MUTATION_FACTOR) == 0)
        {
            mutate_chromosome(&s->individual[i]);
        }
    }
}

long fitness(
    /* The chromosome to be judged */
    chromosome_t* c,
    /* Historic prices */
    unsigned int* h
  )
{
    long total_price = 0;
    long avg_price;
    long price;
    long shares = 0;
    long cash = 0;
    unsigned int t;

    for(t = 0; t < HISTORY_LENGTH; t++)
    {
        price = h[t];
        total_price += price;
        avg_price = total_price / (t + 1);

        /* Make price relative to average */
        price -= avg_price;

        /* Convert price to percentage value */
        price = price * 100 / avg_price;

        if(-price >= (long)Gray_to_bin(c->buy_price))
        {
            /* Buy another share */
            ++shares;
            cash -= (long)h[t];
        }
        else if(price >= (long)Gray_to_bin(c->sell_price))
        {
            /* Sell another share */
            --shares;
            cash += (long)h[t];
        }
    }

    /* Cash out of holdings at the end for comparisons */
    cash += shares * (long)h[t-1];

    return cash;
}

void evaluate_fitness(
    species_t*     sp,
    unsigned int*  h,
    long* fit
  )
{
    int i;

    for(i = 0; i < SPECIES_POPULATION; i++)
    {
        fit[i] = fitness(&sp->individual[i], h);
    }
}

int get_recommendation(
    species_t* sp,
    unsigned int* h
  )
{
    long total_price;
    long avg_price;
    long price;
    long shares;
    long cash;
    long record_cash = 0;
    int last_choice = 0;
    int record_choice = 0;
    unsigned int i;
    unsigned int t;

    for(i = 0; i < SPECIES_POPULATION; i++)
    {
        cash = 0;
        total_price = 0;
        shares = 0;
        for(t = 0; t < HISTORY_LENGTH; t++)
        {
            price = h[t];
            total_price += price;
            avg_price = total_price / (t + 1);

            /* Make price relative to average */
            price -= avg_price;

            /* Convert price to percentage value */
            price = price * 100 / avg_price;

            if(-price >= 
                (long)Gray_to_bin(
                    sp->individual[i].buy_price
                  )
              )
            {
                /* Buy another share */
                ++shares;
                cash -= (long)h[t];
                last_choice = 1;
            }
            else if(price >= 
                (long)Gray_to_bin(
                    sp->individual[i].sell_price
                  )
              )
            {
                /* Sell another share */
                --shares;
                cash += (long)h[t];
                last_choice = -1;
            }
            else
            {
                last_choice = 0;
            }
        }

        cash += shares * (long)h[t-1];

        if(cash > record_cash) {
            record_cash = cash;
            record_choice = last_choice;
        }
    }

    sp->record_cash = record_cash;
    
    return record_choice;
}

unsigned int select_survivors(
    /* Existing species */
    species_t const* before,
    /* The surviving organisms */
    species_t* after,
    /* The fitness value of each organism */
    long* fit
  )
{
    unsigned int i;
    unsigned int j;
    long record;
    long temp;

    /* Copy 1/4 of the most fit to after[] */
    for(i = 0; i < SPECIES_POPULATION / 4; i++)
    {
        record = i;
        for(j = i+1; j < SPECIES_POPULATION; j++)
        {
            if(fit[j] > fit[record])
            {
                record = j;
            }
        }

        /* Swap fitness values */
        temp = fit[record];
        fit[record] = fit[i];
        fit[i] = temp;

        after->individual[i] = before->individual[record];
    }

    return i;
}

void mate_genes(
    gene_t* parent_1,
    gene_t* parent_2,
    gene_t* child
  )
{
    switch(rand_rng(3))
    {
        case 0:
            /* Inherit from parent 1 */
            *child = *parent_1;
            break;
        case 1:
            /* Inherit from parent 2 */
            *child = *parent_2;
            break;
        case 2:
            /* Inherit from both parents */
            *child = *parent_1 + *parent_2 >> 1;
            break;
    }
}

void mate_chromosomes(
    chromosome_t* parent_1,
    chromosome_t* parent_2,
    chromosome_t* child
  )
{
    mate_genes(
        &parent_1->buy_price,
        &parent_2->buy_price,
        &child->buy_price
      );
    mate_genes(
        &parent_1->sell_price,
        &parent_2->sell_price,
        &child->sell_price
      );
}

void recombine(
    species_t* s,
    unsigned int num_survivors
  )
{
    unsigned int i;
    unsigned int parent_1;
    unsigned int parent_2;

    for(i = num_survivors; i < SPECIES_POPULATION; i++)
    {
        /* Select two random parents from survivors */
        parent_1 = rand_rng(num_survivors);
        parent_2 = rand_rng(num_survivors);
        mate_chromosomes(
            &s->individual[parent_1],
            &s->individual[parent_2],
            &s->individual[i]
          );
    }
}

void next_generation(population_t* p, history_t* h)
{
    int s;
    unsigned int num_survivors;
    species_t new_species;
    long fit[SPECIES_POPULATION];

    for(s = 0; s < NUM_STOCKS; s++)
    {
        evaluate_fitness(
            &p->species[s],
            h->price[s],
            fit
          );

        num_survivors = select_survivors(
            &p->species[s],
            &new_species,
            fit
          );

        recombine(&new_species, num_survivors);

        mutate_species(&new_species);

        p->species[s] = new_species;
    }
}

void migrate(population_t* p)
{
    int i, j;
    chromosome_t temp;

    /* Take one individual from each species and trade to the left */
    temp =
      p->species[NUM_STOCKS-1].individual[0];
    for(i = 1; i < NUM_STOCKS; i++)
    {
        p->species[i].individual[0] =
            p->species[i-1].individual[0];
    }
    p->species[0].individual[0] = temp;
}

int main()
{
    population_t pop;
    history_t hist;
    int i, j;
    float temp;
    FILE* fp;

    /* Get things started */
    srand((unsigned)time(NULL));
    printf("Loading data files...");
    fflush(stdout);
    for(j = 0; j < NUM_STOCKS; j++)
    {
        fp = fopen(filename[j], "rt");
        if(fp == NULL)
        {
            fprintf(stderr, "Failed to open input file '%s'.\n", filename[j]);
            return EXIT_FAILURE;
        }

        for(i = 0; i < HISTORY_LENGTH; i++)
        {
            fscanf(fp, "%f", &temp);
            hist.price[j][HISTORY_LENGTH - 1 - i] = temp * 2;/* Accurate to the nearest whole point */
        }

        fclose(fp);
    }

    /* Perform the real work */
    initialize_population(&pop);
    printf("\nEvolving %i generations...", (int)NUM_GENERATIONS);
    fflush(stdout);
    for(i = 0; i < NUM_GENERATIONS; i++)
    {
        next_generation(&pop, &hist);

        /* Time for exchanging chromosomes between species? */
        if(i % MIGRATION_RATE == 0)
        {
            migrate(&pop);
        }
    }
    printf("\nDone.\n\n");

    /* Report results */
    for(i = 0; i < NUM_STOCKS; i++)
    {
        j = get_recommendation(&pop.species[i], hist.price[i]);
        printf("Recommend %sing %s.\n",
            (j == 0 ? "hold" : (j == 1 ? "buy" : "sell")),
            ticker[i]
          );
    }

    return 0;
}
