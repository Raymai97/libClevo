#ifndef RAYMAI_CLEVO__WBEM_ERR_MSG_H
#define RAYMAI_CLEVO__WBEM_ERR_MSG_H

#include "_Base.h"

HRESULT RaymaiClevo__New_WbemErrMsg(LPTSTR *ppsz, HRESULT hrErr);
void RaymaiClevo__Del_WbemErrMsg(LPTSTR psz);


#endif/* RAYMAI_CLEVO__WBEM_ERR_MSG_H */
