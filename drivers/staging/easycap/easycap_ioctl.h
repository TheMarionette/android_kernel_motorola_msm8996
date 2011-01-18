/*****************************************************************************
*                                                                            *
*  easycap_ioctl.h                                                           *
*                                                                            *
*****************************************************************************/
/*
 *
 *  Copyright (C) 2010 R.M. Thomas  <rmthomas@sciolus.org>
 *
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
*/
/*****************************************************************************/
#if !defined(EASYCAP_IOCTL_H)
#define EASYCAP_IOCTL_H

extern struct easycap_dongle easycapdc60_dongle[];
extern struct easycap_standard easycap_standard[];
extern struct easycap_format easycap_format[];
extern struct v4l2_queryctrl easycap_control[];

#endif /*EASYCAP_IOCTL_H*/
