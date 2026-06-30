/*
 * syscalls.c — minimal newlib syscall stubs for bare-metal.
 *
 * snprintf pulls in newlib which expects _sbrk and friends.
 * Since all dynamic allocation goes through FreeRTOS heap_4,
 * _sbrk just returns failure; the others are no-ops.
 */

#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

extern uint32_t _ebss;  /* defined in linker script — end of BSS */

void *_sbrk(int incr)
{
    /* FreeRTOS heap_4 owns the RAM — newlib malloc should not be used.
     * Return -1 to signal no heap available if something calls malloc
     * directly, which surfaces as a clear fault rather than silent
     * corruption. */
    (void)incr;
    errno = ENOMEM;
    return (void *)-1;
}

int _write(int fd, char *buf, int len) { (void)fd; (void)buf; return len; }
int _read(int fd, char *buf, int len)  { (void)fd; (void)buf; (void)len; return -1; }
int _close(int fd)                     { (void)fd; return -1; }
int _lseek(int fd, int off, int w)    { (void)fd; (void)off; (void)w; return -1; }
int _fstat(int fd, struct stat *st)   { (void)fd; (void)st; return -1; }
int _isatty(int fd)                   { (void)fd; return 1; }
int _getpid(void)                     { return 1; }
int _kill(int pid, int sig)           { (void)pid; (void)sig; return -1; }
void _exit(int status)                { (void)status; for (;;) {} }
