/*****************************************************************************


DeclSpec.h

 */

#ifndef _MSERIALS_DECL_SPEC_H
#define _MSERIALS_DECL_SPEC_H

#if defined(_WIN32) && !defined(_NODLL)
#if defined(_MLibDLL)
#define MLibExport __declspec(dllexport)
#else
#define MLibExport __declspec(dllimport)
#endif
#if defined(_LIntDLL)
#define MIntExport __declspec(dllexport)
#else
#define LIntExport __declspec(dllimport)
#endif
#define MUserExport __declspec(dllexport)
#define MUserImport __declspec(dllimport)
#else /* not _WIN32 */
/*For linux user*/
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define MLibExport __attribute__((visibility("default")))
#define LIntExport __attribute__((visibility("default")))
#define HUserExport __attribute__((visibility("default")))
#else /* Not gcc 4.0 or higher */
#if defined(__sun) && (defined(__SUNPRO_C) || defined(__SUNPRO_CC))
#define MLibExport __global
#define LIntExport __global
#define MUserExport __global
#else /* Any other Unix */
#define MLibExport
#define LIntExport
#define MUserExport
#endif /* Any other Unix */
#endif /* Not gcc 4.0 or higher */
#define MUserImport
#endif /* not _WIN32 */

#endif
