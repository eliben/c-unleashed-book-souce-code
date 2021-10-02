#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie.h"

extern int validate_trie( struct trie *trie );
extern void print_trie( struct trie *trie );

void error( const char *s ) {
  printf( "%s\n", s );
  exit(0);
}

void add( struct trie *trie, char *s, char *t ) {
  if (0 == trie_insert( trie, (unsigned char*)s, strlen( s ), t ))
    error( "add" );
}

void delete( struct trie *trie, char *s ) {
  if (0 == trie_delete( trie, (unsigned char*)s, strlen(s))) 
    error( "delete" );
}

void check( struct trie *trie, char *s, char *t ) {
  char *p = trie_search( trie, (unsigned char*)s, strlen(s) );
  if (!p) {
    print_trie( trie );
    printf( "%s: ", s );
    error( "not found" );
  }
  if (0 != strcmp( p, t ))
    error( "found wrong " );
}

void check_strings( struct trie *p, char *strings[][2], int num_strings ) {
  int i;

  if (!validate_trie( p )) error( "validate" );
  for (i=0; i<num_strings; i++)
    check( p, strings[i][0], strings[i][1] );
}

void add_string( struct trie *p, char *strings[][2], int num_strings, int i ) {
  char str[11];
  int len_string;
  for (;;) {
    int i;
    len_string = rand() % 5;
    for (i=0; i<len_string; i++)
       str[i] = '0' + (rand() % 10);
    str[i] = 0;
    for (i=0; i<num_strings; i++)
       if (0 == strcmp( str, strings[i][0] ))
          break;
    if (i == num_strings)
       break;
  }
  strings[num_strings][0] = malloc( strlen(str) + 1 );
  strcpy( strings[num_strings][0], str );
  strings[num_strings][1] = malloc( 8 );
  sprintf( strings[num_strings][1], "%d", i );
  add( p, strings[num_strings][0], strings[num_strings][1] );
}

void remove_string( struct trie *p, char *s ) {
  delete( p, s );
  if (0 != trie_search( p, (unsigned char*)s, strlen(s) ))
    error( "remove failed" );
  if (rand() > RAND_MAX/2)
    if (0 != trie_delete( p, (unsigned char*)s, strlen(s))) 
      error( "second delete failed" );
}

int main() {
  struct trie *p;
  int i;
  char *strings[200][2];
  int num_strings = 0;

  p = trie_create();
  if (!p) error( "trie_create" );

  for (i=0; i<100000; i++) {
    check_strings( p, strings, num_strings );

    if (num_strings < rand() % 200) {
      add_string( p, strings, num_strings, i );
      printf( "%d: Adding %s (%s)\n", i, strings[num_strings][0], strings[num_strings][1] );
      num_strings += 1;
    } else {
      int n = rand() % num_strings;
      printf( "%d: Removing %s (%s)\n", i, strings[n][0], strings[n][1] ); fflush(stdout);
      remove_string( p, strings[n][0] );
      num_strings -= 1;
      strings[n][0] = strings[num_strings][0];
      strings[n][1] = strings[num_strings][1];
    }
  }
  print_trie( p );
  if (!validate_trie( p )) error( "validate" );

  printf( "Success!\n" );

  trie_destroy( p );

  return 0;
}
