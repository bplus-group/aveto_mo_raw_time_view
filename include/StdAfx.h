/*! \file
*
* \verbatim
******************************************************************************
*                                                                            *
*  Copyright (c) 2020, b-plus GmbH. All rights reserved!                     *
*                                                                            *
*    All rights are exclusively reserved by b-plus GmbH,                     *
*    unless explicitly agreed otherwise.                                     *
*                                                                            *
*    Redistribution in source or any other form,                             *
*    with or without modification, is not permitted.                         *
*                                                                            *
*    You may use this code according to the license terms of b-plus.         *
*    Please contact b-plus at services@b-plus.com to get the actual          *
*    terms and conditions.                                                   *
*                                                                            *
******************************************************************************
\endverbatim
*
* \brief Standard AFX
* \author Harald Birkholz
* \copyright (C)2015-2022 b-plus technologies GmbH
* \date 25.04.2022
* \version 2.4.3
*
******************************************************************************/
#pragma once

// Product information
#define VERSIONINFO_PRODUCT             "Raw Time View"
#define VERSIONINFO_AUTHOR              "Harald Birkholz"
#define VERSIONINFO_COMMENTS            "This Table MO shall visualize data timestamps"

#if defined(_MSC_VER)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include "targetver.h"
#endif

#include <Core/AvCore.h>
#include <common/AVETO_Version.h>				// Framework version
#include <Visual/Support/AvVisMeasObj.h> // The AVETO Visual Measurement Object
