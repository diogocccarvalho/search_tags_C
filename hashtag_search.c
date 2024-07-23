#include <glib.h>
#include <json-c/json.h>
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

  // Buffer for reading lines from the file
  char line[1000];
  while (fgets(line, sizeof(line), file) != NULL) {
    // Remove trailing newline character, if present
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    for (size_t i = 0; i < len; i++) {
      if (line[i] == ';') {
        line[i] = ',';
      }
    }

    struct json_tokener *tokener = json_tokener_new();
    struct json_object *root = json_tokener_parse_ex(tokener, line, -1);

    if (json_tokener_get_error(tokener) != json_tokener_success) {
      fprintf(stderr, "Failed to parse JSON: %s\n",
              json_tokener_error_desc(json_tokener_get_error(tokener)));
      json_tokener_free(tokener);
      continue;
    }

    // Get the "hashtags" array
    struct json_object *hashtags;
    if (json_object_object_get_ex(root, "hashtags", &hashtags) &&
        json_object_is_type(hashtags, json_type_array)) {
      size_t i;
      for (i = 0; i < json_object_array_length(hashtags); i++) {
        struct json_object *hashtag = json_object_array_get_idx(hashtags, i);
        if (json_object_is_type(hashtag, json_type_string)) {
          const char *hashtag_str = json_object_get_string(hashtag);
          int *count = g_hash_table_lookup(table, hashtag_str);
          if (count != NULL) {
            (*count)++;
          }
        }
      }
    }

    // Clean up
    json_object_put(root);
    json_tokener_free(tokener);
  }

  // Close the file
  fclose(file);

  // print counts for search terms
  GHashTableIter iter;
  gpointer key, value;
  g_hash_table_iter_init(&iter, table);
  while (g_hash_table_iter_next(&iter, &key, &value)) {
    printf("%s: %d\n", (char *)key, *(int *)value);
  }

  g_hash_table_destroy(table);

  return 0;
}
