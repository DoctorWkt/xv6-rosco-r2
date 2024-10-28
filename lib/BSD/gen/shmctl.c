#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$Id: shmctl.c,v 1.1 2024/08/28 23:40:23 wkt Exp $";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#if __STDC__
int shmctl(int shmid, int cmd, void *buf)
#else
int shmctl(shmid, cmd, buf)
	int shmid;
	int cmd;
	void *buf;
#endif
{
	return (shmsys(1, shmid, cmd, buf));
}
