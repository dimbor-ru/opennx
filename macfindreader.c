#include <dlfcn.h>
#include <opensc/opensc.h>

#define CTXNAME "openssh"

typedef int (*Tsc_establish_context)(sc_context_t **ctx, const char *app_name);
typedef int (*Tsc_release_context)(sc_context_t *ctx);
typedef int (*Tsc_detect_card_presence)(sc_reader_t *reader, int slot_id);

static Tsc_establish_context FNsc_establish_context = NULL;
static Tsc_release_context FNsc_release_context = NULL;
static Tsc_detect_card_presence FNsc_detect_card_presence = NULL;

static int load_opensc() {
    void *lib = dlopen("libopensc.dylib", RTLD_NOW);
    if (NULL == lib)
        return 0;
    FNsc_establish_context = dlsym(lib, "sc_establish_context");
    if (NULL == FNsc_establish_context)
        return 0;
    FNsc_release_context = dlsym(lib, "sc_release_context");
    if (NULL == FNsc_release_context)
        return 0;
    FNsc_detect_card_presence = dlsym(lib, "sc_detect_card_presence");
    if (NULL == FNsc_detect_card_presence)
        return 0;
    return 1;
}

static int findreader() {
    unsigned int rc, i;
    int r, j;
    int found_id = -1;
    sc_context_t *ctx = NULL;

    if (!load_opensc())
        return -1;
    if (SC_SUCCESS != FNsc_establish_context(&ctx, CTXNAME))
        return -1;
    rc = ctx->reader_count;
    if (rc > 0) {
        unsigned int errc = 0;

        for (i = 0; i < rc; i++) {
            sc_reader_t *reader = ctx->reader[i];
            if (!reader)
                continue;
            for (j = 0; j < reader->slot_count; j++) {
                r = FNsc_detect_card_presence(reader, j);
                if (r > 0) {
                    found_id = i;
                    break;
                }
                if (r < 0) {
                    errc++;
                }
            }
            if (found_id != -1)
                return found_id;
        }
        if (errc >= rc) {
            FNsc_release_context(ctx);
            return -1;
        }
    }
    FNsc_release_context(ctx);
    return -1;
}

int main() {
    int rid = findreader();
    return (rid < 0) ? 255 : rid;
}
