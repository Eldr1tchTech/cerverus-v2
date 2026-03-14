#include "network_util.h"

#include "core/util/util.h"
#include "core/memory/cmem.h"

#include <string.h>

darray* parse_URI(char* URI) {
    darray* new_darr = darray_create(strchrc(URI, '/'), sizeof(route_segment));

    URI++; // Skip the initial /

    int count = new_darr->size;
    for (int i = 0; i < count; i++)
    {
        route_segment segment = {0};

        if (URI[0] == ':')
        {
            segment.is_dynamic = true;
            URI++;
        }

        int char_index = strchri(URI, '/');
        int seg_len = (char_index == -1) ? (int)strlen(URI) : char_index;

        segment.path_segment = cmem_alloc(memory_tag_string, (seg_len + 1) * sizeof(char));
        cmem_mcpy(segment.path_segment, URI, seg_len);
        // +1 byte is already zeroed by cmem_alloc, so null terminator is handled

        URI += seg_len + (char_index == -1 ? 0 : 1);

        darray_add(new_darr, &segment);
    }

    return new_darr;
}