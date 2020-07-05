/* Provide a real file - not a symlink - as it would cause multiarch conflicts
   when multiple different arch releases are installed simultaneously.  */

#ifndef UNW_REMOTE_ONLY

# include "tdep-mips/jmpbuf.h"

#endif /* !UNW_REMOTE_ONLY */
