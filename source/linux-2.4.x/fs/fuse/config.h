/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* kernel has BLOCK option */
/* #undef HAVE_CONFIG_BLOCK */

/* kernel has exportfs.h */
/* #undef HAVE_EXPORTFS_H */

/* fs_subsys is defined */
#define HAVE_FS_SUBSYS 1

/* inode has i_blksize field */
/* #undef HAVE_I_BLKSIZE */

/* inode has i_mutex field */
#define HAVE_I_MUTEX 1

/* inode has i_private field */
#define HAVE_I_PRIVATE 1

/* kzalloc() is defined */
#define HAVE_KZALLOC 1

/* lookup_instantiate_filp() is defined */
/* #undef HAVE_LOOKUP_INSTANTIATE_FILP */

/* kernel has mutex.h */
#define HAVE_MUTEX_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "fuse-kernel"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "fuse-kernel 2.7.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "fuse-kernel"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.7.3"

/* umount_begin is passed a vfsmount */
#define UMOUNT_BEGIN_VFSMOUNT 1
