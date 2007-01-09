/* This file is part of Kiten, a KDE Japanese Reference Tool...

 Copyright (C) 2006  Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef LIBKITENEXPORTS_H
#define LIBKITENEXPORTS_H

#include <kdemacros.h>

#if defined Q_OS_WIN
#ifndef KITEN_EXPORT
# ifdef MAKE_KITEN_LIB
#  define KITEN_EXPORT KDE_EXPORT
# else
#  define KITEN_EXPORT KDE_IMPORT
# endif
#endif

#else /* UNIX */
#define KITEN_EXPORT KDE_EXPORT
#endif

#endif
