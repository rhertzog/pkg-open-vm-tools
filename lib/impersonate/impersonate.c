/*********************************************************
 * Copyright (C) 2003 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 2.1 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the Lesser GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 *********************************************************/

/*
 * impersonate.c --
 *
 * Description:
 *      Code to impersonate as a user when running under a privileged account. 
 *      Nested impersonation is not supported.
 */

#include <string.h>
#include "auth.h"

#include "impersonateInt.h"

Bool impersonationEnabled = FALSE;


/*
 *----------------------------------------------------------------------
 *
 * Impersonate_Init --
 *
 *      Initialize the impersonation module. On windows also load
 *      userenv.dll.
 *      Without calling this, code calling into this module will 
 *      essentially be noops.
 *
 * Side effects:
 *
 *	Loads the library. We keep the library loaded thereafter.
 *
 *----------------------------------------------------------------------
 */

void
Impersonate_Init(void)
{
   if (!impersonationEnabled) {
      ImpersonateInit();
      impersonationEnabled = TRUE;
   }
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_Runas --
 *
 *      Impersonate as the appropriate runas user. In linux this is always
 *      the config file owner regardless the calling context. In windows, the
 *      runas user is the caller passed into the method, except when the VM has
 *      a preconfigured runas user, in which case we will impersonate using his
 *      credentials instead.
 *
 *      In windows, if caller is not set, fail if preconfigured runas user is
 *      not found.
 *
 * Results:  
 *      TRUE if impersonation succeeds, FALSE otherwise.
 *
 * Side effects:
 *      imp.impersonatedUser may be updated.
 * 
 *----------------------------------------------------------------------------
 */

Bool
Impersonate_Runas(const char *cfg,           // IN
                  const char *caller,        // IN
                  AuthToken callerToken)     // IN
{
   if (!impersonationEnabled) {
      return TRUE;
   } else {
      return ImpersonateRunas(cfg, caller, callerToken);
   }
   return TRUE;
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_Owner --
 *
 *      Impersonate as the owner of the specified file.
 *
 * Results:  
 *      TRUE if impersonation succeeds, FALSE otherwise.
 *
 * Side effects:
 *      imp.impersonatedUser may be updated.
 * 
 *----------------------------------------------------------------------------
 */

Bool
Impersonate_Owner(const char *file)        // IN
{
   if (!impersonationEnabled) {
      return TRUE;
   } else {
      return ImpersonateOwner(file);
   }
   return TRUE;
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_Do --
 *
 *      Impersonate as user. Can be nested if impersonated as that same user 
 *      each time. Can switch back to root temporarily regardless of nesting
 *      level via Impersonate_ForceRoot. Calling Impersonate_UnforceRoot will
 *      return to original impersonation at the same nesting level.
 *
 * Results:  
 *      TRUE if impersonation succeeds, FALSE otherwise.
 *
 * Side effects:
 *      imp.impersonatedUser may be updated.
 *      imp.impersonatedToken(Win32 only) may be updated.
 * 
 *----------------------------------------------------------------------------
 */

Bool
Impersonate_Do(const char *user,             // IN 
               AuthToken token)              // IN
{
   if (!impersonationEnabled) {
      return TRUE;
   } else {
      return ImpersonateDo(user, token);
   }
   return TRUE;
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_Undo --
 *
 *      Undoes a previous impersonation. When we undo the last in the nesting
 *      of impersonation ops, switch back to root.
 *
 * Results:  
 *      TRUE on success, FALSE otherwise
 *
 * Side effects:
 *      On reverting back to root,
 *      imp.impersonatedUser is freed.
 *      imp.impersonatedToken (win32) is invalid.
 * 
 *----------------------------------------------------------------------------
 */

Bool
Impersonate_Undo(void)
{
   ImpersonationState *imp = NULL;

   if (!impersonationEnabled) {
      return TRUE;
   }

   imp = ImpersonateGetTLS();
   ASSERT(imp);

   WIN32_ONLY(ASSERT(!imp->forceRoot));
   imp->refCount--;

   POSIX_ONLY(IMPWARN(("Impersonate_Undo (%x %x) drop refcount to %d\n", 
                       getpid(), imp, imp->refCount)));
   WIN32_ONLY(IMPWARN(("Impersonate_Undo (%x) drop refcount to %d\n", 
                       (int) imp, imp->refCount)));

   if (imp->refCount > 0) {
      return TRUE;
   }

   return ImpersonateUndo();
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_Who --
 *
 *      Returns currently impersonated user name. If not impersonated,
 *      returns NULL.
 *
 * Results:  
 *      Currently impersonated user name. NULL if not impersonated.
 *
 * Side effects:
 *      None.
 * 
 *----------------------------------------------------------------------------
 */

char *
Impersonate_Who(void)
{
   char *impUser;
   ImpersonationState *imp = NULL;

   if (!impersonationEnabled) {
      return strdup("");
   }
   
   imp = ImpersonateGetTLS();
   ASSERT(imp);

   impUser = strdup(imp->impersonatedUser);
   ASSERT_MEM_ALLOC(impUser);
   return impUser;
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_ForceRoot --
 *
 *      Go back to base impersonate level (LocalSystem/root) for a brief period
 *      of time. Doesnt do anything on linux.
 *      Should only be used when already impersonated. This call is not nestable.     
 *      No other impersonation is permitted before calling Impersonate_UnforceRoot.
 *
 * Results:  
 *      TRUE on success, FALSE on failure.
 *
 * Side effects:
 *      imp.forceRoot is set to TRUE on success.
 * 
 *----------------------------------------------------------------------------
 */

Bool 
Impersonate_ForceRoot(void) 
{
   if (!impersonationEnabled) {
      return TRUE;
   } else {
      return ImpersonateForceRoot();
   }
   return TRUE;
}


/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_UnforceRoot --
 *
 *      Go back to impersonate the user that we switched to root from.
 *      See Impersonate_ForceRoot.
 *
 * Results:  
 *      TRUE on success, FALSE on failure.
 *
 * Side effects:
 *      imp.forceRoot is set to FALSE on success.
 * 
 *----------------------------------------------------------------------------
 */

Bool 
Impersonate_UnforceRoot(void) 
{
   if (!impersonationEnabled) {
      return TRUE;
   } else {
      return ImpersonateUnforceRoot();
   }
   return TRUE;
}


#ifdef _WIN32
/*
 *----------------------------------------------------------------------------
 *
 * Impersonate_CfgRunasOnly --
 *
 *      Impersonate as the preconfigured runas user for the VM. 
 *      Fails if runas user credentials are not found.
 *
 * Results:  
 *      TRUE if preconfigured runas user is found impersonation succeeds,
 *      FALSE otherwise.
 *
 * Side effects:
 *      imp.impersonatedUser may be updated.
 * 
 *----------------------------------------------------------------------------
 */

Bool
Impersonate_CfgRunasOnly(const char *cfg)        // IN
{
   return Impersonate_Runas(cfg, NULL, NULL);
}
#endif //_WIN32