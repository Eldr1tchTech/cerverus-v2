#include "route_trie.h"

#include "core/memory/cmem.h"
#include "core/util/util.h"
#include "network/network_util.h"

#include <string.h>

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

// NOTE: For now it's just the first match, eventually it should be best match
void trie_add_route(trie* t, route* rt) {
    trie_node* root = t->roots[rt->method];
    
    // Find the final node
    route_segment* segment_darr_data = rt->segments->data;
    for (int i = 0; i < rt->segments->length; i++)
    {
        trie_node* children_darr_data = root->children->data;
        for (int j = 0; j < root->children->length; j++)
        {
            if (strcmp(segment_darr_data[i].path_segment, children_darr_data[j].segment.path_segment) == 0)
            {
                root = &children_darr_data[j];
                break;
            }

            if (j == root->children->length - 1)
            {
                // Not present in the children.
                trie_node* new_node = trie_node_create();
                new_node->segment = segment_darr_data[i];
                root = new_node;
                break;
            }
        }
    }

    root->callback = &rt->callback;
}

route_callback* trie_find_handler(trie* t, http_method method, char* URI) {
    darray* segment_darr = parse_URI(URI);

    trie_node* root = t->roots[method];
    
    // Find the final node
    route_segment* segment_darr_data = segment_darr->data;
    for (int i = 0; i < segment_darr->length; i++)
    {
        trie_node* children_darr_data = root->children->data;
        for (int j = 0; j < root->children->length; j++)
        {
            // Check for static match
            if (strcmp(segment_darr_data[i].path_segment, children_darr_data[j].segment.path_segment) == 0)
            {
                root = &children_darr_data[j];
                break;
            }

            // Check for dynamic "match" (not really)
            if (children_darr_data[j].segment.is_dynamic)
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

    return root->callback;
}