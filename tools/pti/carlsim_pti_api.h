

#ifndef CARLSIM_PTI_API_H
#define CARLSIM_PTI_API_H

#include <carlsim_conf_api.h>

#ifndef CARLSIM_PTI_API
#  ifdef carlsim_stopwatch_EXPORTS
#    define CARLSIM_PTI_API CARLSIM_EXPORT
#    define CARLSIM_PTI_EXTERN CARLSIM_EXPORT_EXTERN
#  else
#    define CARLSIM_PTI_API CARLSIM_IMPORT
#    define CARLSIM_PTI_EXTERN CARLSIM_EXPORT_EXTERN
#  endif
#endif


#endif // CARLSIM_PTI_API_H