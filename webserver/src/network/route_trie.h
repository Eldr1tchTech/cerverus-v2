#pragma once

#include "network_types.inl"

#include "network/server.h"

trie* trie_create();
void trie_destroy(trie* t);

trie_node* trie_node_create();
void trie_node_destroy(trie_node* t_node);

void trie_add_route(trie* t, route* rt);

route_callback* trie_find_handler(trie* t, http_method method, char* URI);