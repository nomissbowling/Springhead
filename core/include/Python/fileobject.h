#include "Add_decl.h"
/* File object interface (what's left of it -- see io.py) */

#ifndef Py_FILEOBJECT_H
#define Py_FILEOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

#define PY_STDIOTEXTMODE "b"

PyAPI_FUNC(PyObject *) __PYDECL  PyFile_FromFd(int, const char *, const char *, int,
                                     const char *, const char *,
                                     const char *, int);
PyAPI_FUNC(PyObject *) __PYDECL  PyFile_GetLine(PyObject *, int);
PyAPI_FUNC(int) __PYDECL  PyFile_WriteObject(PyObject *, PyObject *, int);
PyAPI_FUNC(int) __PYDECL  PyFile_WriteString(const char *, PyObject *);
PyAPI_FUNC(int) __PYDECL  PyObject_AsFileDescriptor(PyObject *);
#ifndef Py_LIMITED_API
PyAPI_FUNC(char *) __PYDECL  Py_UniversalNewlineFgets(char *, int, FILE*, PyObject *);
#endif

/* The default encoding used by the platform file system APIs
   If non-NULL, this is different than the default encoding for strings
*/
PyAPI_DATA(const char *) Py_FileSystemDefaultEncoding;
PyAPI_DATA(int) Py_HasFileSystemDefaultEncoding;

/* Internal API

   The std printer acts as a preliminary sys.stderr until the new io
   infrastructure is in place. */
#ifndef Py_LIMITED_API
PyAPI_FUNC(PyObject *) __PYDECL  PyFile_NewStdPrinter(int);
PyAPI_DATA(PyTypeObject) PyStdPrinter_Type;
#endif /* Py_LIMITED_API */

/* A routine to check if a file descriptor can be select()-ed. */
#ifdef HAVE_SELECT
 #define _PyIsSelectable_fd(FD) (((FD) >= 0) && ((FD) < FD_SETSIZE))
#else
 #define _PyIsSelectable_fd(FD) (1)
#endif /* HAVE_SELECT */

#ifdef __cplusplus
}
#endif
#endif /* !Py_FILEOBJECT_H */
