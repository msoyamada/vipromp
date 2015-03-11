
#ifndef BITMAP_H
#define BITMAP_H

/* BITMAPs:
     BMAP: int * to an array of ints
     SZ: number of ints in the bitmap
*/

/* declare a bitmap type */
#define BITMAP_SIZE(BITS)	(((BITS)+31)/32)
#define BITMAP_TYPE(BITS, NAME)	unsigned int (NAME)[BITMAP_SIZE(BITS)]

typedef unsigned int BITMAP_ENT_TYPE;
typedef unsigned int *BITMAP_PTR_TYPE;

/* set entire bitmap */
#define BITMAP_SET_MAP(BMAP, SZ)				\
  { int i; for (i=0; i<(SZ); i++) (BMAP)[i] = 0xffffffff; }

/* clear entire bitmap */
#define BITMAP_CLEAR_MAP(BMAP, SZ)				\
  { int i; for (i=0; i<(SZ); i++) (BMAP)[i] = 0; }

/* set bit BIT in bitmap BMAP, returns BMAP */
#define BITMAP_SET(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] |= (1 << ((BIT) % 32))), (BMAP))

/* clear bit BIT in bitmap BMAP, returns BMAP */
#define BITMAP_CLEAR(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] &= ~(1 << ((BIT) % 32))), (BMAP))

/* copy bitmap SRC to DEST */
#define BITMAP_COPY(DESTMAP, SRCMAP, SZ)			\
  { int i; for (i=0; i<(SZ); i++) (DESTMAP)[i] = (SRCMAP)[i]; }

/* store bitmap B2 OP B3 into B1 */
#define __BITMAP_OP(B1, B2, B3, SZ, OP)				\
  { int i; for (i=0; i<(SZ); i++) (B1)[i] = (B2)[i] OP (B3)[i]; }

/* store bitmap B2 | B3 into B1 */
#define BITMAP_IOR(B1, B2, B3, SZ)				\
  __BITMAP_OP(B1, B2, B3, SZ, |)

/* store bitmap B2 ^ B3 into B1 */
#define BITMAP_XOR(B1, B2, B3, SZ)				\
  __BITMAP_OP(B1, B2, B3, SZ, ^)

/* store bitmap B2 & B3 into B1 */
#define BITMAP_AND(B1, B2, B3, SZ)				\
  __BITMAP_OP(B1, B2, B3, SZ, &)

/* store ~B2 into B1 */
#define BITMAP_NOT(B1, B2, SZ)					\
  { int i; for (i=0; i<(SZ); i++) (B1)[i] = ~((B2)[i]); }

/* return non-zero if bitmap is empty */
#define BITMAP_EMPTY_P(BMAP, SZ)				\
  ({ int i, res=0; for (i=0; i<(SZ); i++) res |= (BMAP)[i]; !res; })

/* return non-zero if the intersection of bitmaps B1 and B2 is non-empty */
#define BITMAP_DISJOINT_P(B1, B2, SZ)				\
  ({ int i, res=0; for (i=0; i<(SZ); i++) res |= (B1)[i] & (B2)[i]; !res; })

/* return non-zero if bit BIT is set in bitmap BMAP */
#define BITMAP_SET_P(BMAP, SZ, BIT)				\
  (((BMAP)[(BIT)/32] & (1 << ((BIT) % 32))) != 0)

/* return non-zero if bit BIT is clear in bitmap BMAP */
#define BITMAP_CLEAR_P(BMAP, SZ, BIT)				\
  (!BMAP_SET_P((BMAP), (SZ), (BIT)))

/* count the number of bits set in BMAP */
#define BITMAP_COUNT_ONES(BMAP, SZ)				\
({								\
  int i, j, n = 0;						\
  for (i = 0; i < (SZ) ; i++)					\
    {								\
      unsigned int word = (BMAP)[i];				\
      for (j=0; j < (sizeof(unsigned int)*8); j++)		\
        {							\
          unsigned int new_val, old_val = word;			\
          word >>= 1;						\
          new_val = word << 1;					\
          if (old_val != new_val)				\
            n++;						\
        }							\
    }								\
  n;								\
})

#endif /* BITMAP_H */

