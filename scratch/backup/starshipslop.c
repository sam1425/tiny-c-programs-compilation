#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <git2.h>
#include <fcntl.h>
#include <stdint.h>

/* ── gruvbox dark colours ────────────────────────────────────────── */
#define C_GREEN  "\033[1;38;2;184;187;38m"   /* bright green  #b8bb26 */
#define C_YELLOW "\033[1;38;2;250;189;47m"   /* bright yellow #fabd2f */
#define C_AQUA   "\033[1;38;2;142;192;124m"  /* bright aqua   #8ec07c */
#define C_ORANGE "\033[1;38;2;254;128;25m"   /* Gruvbox Orange #fe8019 */
#define C_RESET  "\033[0m"

static uint32_t fnv1a(const char *s) {
    uint32_t h = 2166136261u;
    while (*s) h = (h ^ (uint8_t)*s++) * 16777619u;
    return h;
}

static int status_cb(const char *path, unsigned int status_flags, void *payload) {
    (void)path;
    *(unsigned int *)payload |= status_flags;
    return 1; /* stop on first match for speed */
}

static void write_and_cache(const char *cache_path, const char *msg, size_t len) {
    write(1, msg, len);
    int fd = open(cache_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd >= 0) {
        write(fd, msg, len);
        close(fd);
    }
    _exit(0);
}

int main(int argc, char *argv[]) {
    const char *path = (argc >= 2) ? argv[1] : getenv("PWD");
    if (!path) return 0;

    /* 1. Fast Upward Discovery for .git */
    char cur[4096];
    struct stat st;
    if (strlen(path) >= sizeof(cur)) _exit(0);
    strcpy(cur, path);

    /* Normalize: remove trailing slashes */
    size_t path_len = strlen(cur);
    while (path_len > 1 && cur[path_len - 1] == '/') {
        cur[--path_len] = '\0';
    }

    int found_git = 0;
    while (1) {
        char gitdir[4112];
        snprintf(gitdir, sizeof(gitdir), "%s/.git", cur);
        if (stat(gitdir, &st) == 0) {
            found_git = 1;
            break;
        }
        char *last = strrchr(cur, '/');
        if (!last || last == cur) _exit(0);
        *last = '\0';
    }

    if (!found_git) _exit(0);

    /* index_path is only reliable if .git is a directory */
    char index_path[4120];
    snprintf(index_path, sizeof(index_path), "%s/.git/index", cur);

    char cache_path[64];
    snprintf(cache_path, sizeof(cache_path), "/tmp/gd-%08x", fnv1a(cur));

    struct stat idx_st, cache_st;
    int has_index = stat(index_path, &idx_st) == 0;
    int has_cache = stat(cache_path, &cache_st) == 0;

    if (has_index && has_cache) {
#ifdef __linux__
        if (cache_st.st_mtim.tv_sec > idx_st.st_mtim.tv_sec ||
           (cache_st.st_mtim.tv_sec == idx_st.st_mtim.tv_sec &&
            cache_st.st_mtim.tv_nsec >= idx_st.st_mtim.tv_nsec)) {
#else
        if (cache_st.st_mtime >= idx_st.st_mtime) {
#endif
            char buf[128];
            int fd = open(cache_path, O_RDONLY);
            if (fd >= 0) {
                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                close(fd);
                if (n > 0) {
                    write(1, buf, n);
                    _exit(0);
                }
            }
        }
    }

    /* 2. LIBGIT2 INIT & CONFIG BYPASS */
    git_libgit2_init();
    /* Disable expensive config searching */
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_SYSTEM, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_GLOBAL, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_XDG, NULL);
    git_libgit2_opts(GIT_OPT_SET_SEARCH_PATH, GIT_CONFIG_LEVEL_PROGRAMDATA, NULL);

    git_repository *repo = NULL;
    if (git_repository_open_ext(&repo, cur, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) != 0) _exit(0);

    /* 3. Status Scan (Stop on first match) */
    unsigned int status = 0;
    git_status_options s_opts = {0};
    git_status_init_options(&s_opts, GIT_STATUS_OPTIONS_VERSION);
    s_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    s_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | 
                   GIT_STATUS_OPT_EXCLUDE_SUBMODULES |
                   GIT_STATUS_OPT_NO_REFRESH |
                   GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH;
    
    git_status_foreach_ext(repo, &s_opts, status_cb, &status);

#define MSG(s) s, sizeof(s)-1

    if (status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED | GIT_STATUS_INDEX_DELETED | GIT_STATUS_INDEX_RENAMED | GIT_STATUS_INDEX_TYPECHANGE)) {
        write_and_cache(cache_path, MSG(C_YELLOW "🔘 " C_RESET));
    }
    
    if (status & (GIT_STATUS_WT_NEW | GIT_STATUS_WT_MODIFIED | GIT_STATUS_WT_DELETED | GIT_STATUS_WT_TYPECHANGE | GIT_STATUS_WT_RENAMED)) {
        write_and_cache(cache_path, MSG(C_ORANGE "꩜ " C_RESET));
    }

    /* 4. Ahead/Behind (Only if clean) */
    {
        git_reference *head = NULL;
        if (git_repository_head(&head, repo) == 0) {
            git_reference *upstream = NULL;
            if (git_branch_upstream(&upstream, head) == 0) {
                const git_oid *h = git_reference_target(head), *u = git_reference_target(upstream);
                if (h && u && git_oid_cmp(h, u) != 0) {
                    size_t a = 0, b = 0;
                    /* This can be slow in very large repos, but cached afterwards */
                    if (git_graph_ahead_behind(&a, &b, repo, h, u) == 0 && a > 0) {
                        write_and_cache(cache_path, MSG(C_AQUA "⬤↑" C_RESET));
                    }
                }
                git_reference_free(upstream);
            }
            git_reference_free(head);
        }
    }

    write_and_cache(cache_path, MSG(C_GREEN "⬤ " C_RESET));
    return 0;
}
