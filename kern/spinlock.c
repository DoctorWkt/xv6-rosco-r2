// Mutual exclusion spin locks.

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/proc.h>
#include <xv6/spinlock.h>

void initlock(struct spinlock *lk, char *name) {
  lk->name = name;
  lk->locked = 0;
}

// Acquire a lock.
void acquire(struct spinlock *lk) {
  pushcli(); // disable interrupts to avoid deadlock.
  if(holding(lk))
    panic("acquire");

  setlock(&lk->locked);

  // Record info about lock acquisition for debugging.
  getcallerpcs(&lk, lk->pcs);
}

// Release a lock.
void release(struct spinlock *lk) {
  if(!holding(lk))
    panic("release");

  lk->pcs[0] = 0;
  lk->locked = 0;
  popcli();
}

// Record the current call stack in pcs[]
void getcallerpcs(void *v, uint pcs[]) {
  // TO DO
}

// Check whether the lock is already held
int holding(struct spinlock *lock)
{
  return lock->locked;
}

// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off, i.e when ncli > 0.

static int ncli= 0;

void pushcli(void) {
  cli();	// Disable interrupts
  ncli++;
}

void popcli(void) {
  ncli--;
  if (ncli<0)
    panic("popcli");
  if (ncli==0)
    sti();	// Enable interrupts when ncli is zero
}
