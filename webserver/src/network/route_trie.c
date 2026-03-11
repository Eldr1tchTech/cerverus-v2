#include "route_trie.h"

#include "core/memory/cmem.h"
#include "core/util/util.h"

#include <string.h>

trie* trie_create() {
    trie* new_trie = cmem_alloc(memory_tag_trie, sizeof(trie));
    new_trie->roots = cmem_alloc(memory_tag_trie, http_method_unknown * sizeof(trie_node*));
    for (int i = 0; i < http_method_unknown; i++)
    {
        new_trie->roots[i] = trie_node_create();
    }
    
    return new_trie;
}

void trie_destroy(trie* t) {
    for (int i = 0; i < http_method_unknown; i++)
    {
        trie_node_destroy(t->roots[i]);
    }
    cmem_free(memory_tag_trie, t->roots);
    cmem_free(memory_tag_trie, t);
}

trie_node* trie_node_create() {
    trie_node* new_node = cmem_alloc(memory_tag_trie, sizeof(trie_node));
    new_node->children = darray_create(2, sizeof(trie_node));

    return new_node;
}

void trie_node_destroy(trie_node* t_node) {
    trie_node* darr_data = t_node->children->data;
    for (int i = 0; i < t_node->children->length; i++)
    {
        trie_node_destroy(&darr_data[i]);
    }
    
    darray_destroy(t_node->children);
    cmem_free(memory_tag_trie, t_node);
}

void trie_add_route(trie* t, route* rt) {
    // Step 1: Fill out path segments.
    // NOTE: Maybe use a queue instead?
    darray* segment_darr = darray_create(4, sizeof(char*));
    rt->URI++;  // Skip the initial /
    char* segment = strtok(rt->URI, "/");

    trie_node* root = t->roots[rt->method];
    
    while (segment)
    {
        darray_add(segment_darr, segment);
        segment = strtok(NULL, "/");
    }
    
    // Step 2: Find the final node
    char** segment_darr_data = segment_darr->data;
    for (int i = 0; i < segment_darr->length; i++)
    {
        trie_node* children_darr_data = root->children->data;
        for (int j = 0; j < root->children->length; j++)
        {
            if (strcmp(segment_darr_data[i], children_darr_data[j].segment) == 0)
            {
                root = &children_darr_data[j];
                break;
            }

            if (j == root->children->length - 1)
            {
                // Not present in the children.
                trie_node* new_node = trie_node_create();
                new_node->segment = asprintf("%s", segment_darr_data[i]);
                root = new_node;
                break;
            }
        }
    }

    root->handler = &rt->callback;
    darray_destroy(segment_darr);
}

route_callback* trie_find_handler(trie* t, http_method method, char* URI) {
    // Step 1: Fill out path segments.
    // NOTE: Maybe use a queue instead?
    darray* segment_darr = darray_create(4, sizeof(char*));
    URI++;
    char* segment = strtok(URI, "/");

    trie_node* root = t->roots[method];
    
    while (segment)
    {
        darray_add(segment_darr, segment);
        segment = strtok(NULL, "/");
    }
    
    // Step 2: Find the final node
    char** segment_darr_data = segment_darr->data;
    for (int i = 0; i < segment_darr->length; i++)
    {
        trie_node* children_darr_data = root->children->data;
        for (int j = 0; j < root->children->length; j++)
        {
            if (strcmp(segment_darr_data[i], children_darr_data[j].segment) == 0)
            {
                root = &children_darr_data[j];
                break;
            }

            if (j == root->children->length - 1)
            {
                // Not present
                return NULL;
            }
        }
    }

    darray_destroy(segment_darr);

    return root->handler;
}