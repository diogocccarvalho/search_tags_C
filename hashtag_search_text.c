#include <glib.h>
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  // open search terms file in read mode
  FILE *file = fopen("hashtags.txt", "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening search terms file\n");
    return 1;
  }

  // create hash table to store search terms and their counts
  GHashTable *table =
      g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

  // read search terms from file and add to hash table
  char term[100];
  while (fscanf(file, "%s", term) != EOF) {
    int *count = malloc(sizeof(int));
    *count = 0;
    g_hash_table_insert(table, strdup(term), count);
  }

  // close search terms file
  fclose(file);

  // open tweet file in read mode
  file = fopen("TweetFile.txt", "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening tweet file\n");
    return 1;
  }

  // read tweet file line by line and increment counts for search terms
  char line[1000];
  while (fgets(line, sizeof(line), file) != NULL) {
    // Find start of hashtags field
    char *token = strtok(line, ";");
    token = strtok(NULL, ";");
    char *prefix = strstr(token, "\"hashtags\": ");
    if (prefix != NULL) {
      memmove(prefix, prefix + strlen("\"hashtags\": "),
              strlen(prefix + strlen("\"hashtags\": ")) + 1);
      memmove(token, token + 1, strlen(token + 1) + 1);
      char temp[strlen(token) - 1];
      strncpy(temp, token + 1, strlen(token) - 2);
      temp[strlen(token) - 2] = '\0';

      char *elem = strtok(temp, ", ");
      while (elem != NULL) {
        size_t len = strlen(elem);
        elem[len - 1] = '\0'; // Remove the trailing quote
        memmove(elem, elem + 1, len - 1);

        // Debugging: Print the current hashtag (elem)
        printf("Processing hashtag: %s\n", elem);
        int *count = g_hash_table_lookup(table, elem);
        if (count != NULL) {
          (*count)++;
        }
        elem = strtok(NULL, ", ");
      }
    }
  }
  fclose(file);

  // print counts for search terms
  GHashTableIter iter;
  gpointer key, value;
  g_hash_table_iter_init(&iter, table);
  while (g_hash_table_iter_next(&iter, &key, &value)) {
    printf("%s: %d\n", (char *)key, *(int *)value);
  }

  // destroy hash table
  g_hash_table_destroy(table);

  return 0;
}
