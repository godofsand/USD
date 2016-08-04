//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef ARCH_SYSTEMINFO_H
#define ARCH_SYSTEMINFO_H

#include "pxr/base/arch/api.h"
#include <sys/types.h>
#include <string>

/*!
 * \file systemInfo.h
 * \brief Provide architecture-specific system information.
 * \ingroup group_arch_SystemFunctions
 */

/*!
 * \brief Return current working directory as a string.
 * \ingroup group_arch_SystemFunctions
 */
ARCH_API
std::string ArchGetCwd();

/*!
 * \brief Return user's home directory.
 * \ingroup group_arch_SystemFunctions
 *
 * If \p login is not supplied, the home directory of the current user is
 * returned.  Otherwise, the home directory of the user with the specified
 * login is returned.  If the home directory cannot be determined, the empty
 * string is returned.
 */
ARCH_API
std::string ArchGetHomeDirectory(const std::string &login = std::string());

/*!
 * \brief Return user name.
 * \ingroup group_arch_SystemFunctions
 *
 * If the user name cannot determined, the empty string is returned.
 */
ARCH_API
std::string ArchGetUserName();

/*!
 * \brief Return the path to the program's executable.
 * \ingroup group_arch_SystemFunctions
 *
 * Return the path of the executing binary.
 */
ARCH_API
std::string ArchGetExecutablePath();

/*!
* \brief Get memory page size.
* \ingroup group_arch_SystemFunctions
*
* Return the memory page size.
*/
ARCH_API
size_t ArchGetPageSize();

#endif // ARCH_SYSTEMINFO_H