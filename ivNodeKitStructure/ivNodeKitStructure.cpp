/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2005 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <stdio.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#if HAVE_UNISTD_H
#include <unistd.h> // getopt()
#endif /* HAVE_UNISTD_H */


#ifdef HAVE_GETOPT
/* These two externs are for interfacing against getopt(). */
extern int optind;
extern char * optarg;
#endif // HAVE_GETOPT


void
usage(const char * invname)
{
#ifdef HAVE_GETOPT
  fprintf(stderr, "\nUsage: %s [-h] <kitclassname>\n\n", invname);
  fprintf(stderr, "\t-h:\tshow usage\n\n");
#else // !HAVE_GETOPT
  fprintf(stderr, "\nUsage: %s <kitclassname>\n\n", invname);
#endif // !HAVE_GETOPT
}

int
main(int argc, char ** argv)
{
#ifdef HAVE_GETOPT
  /* Parse command line. */
  int getoptchar;
  while ((getoptchar = getopt(argc, argv, "h")) != EOF) {
    char c = (char)getoptchar;
    switch (c) {
    case '?':
    case 'h':
      usage(argv[0]);
      exit(0);
      break;
    }
  }

  int i = optind;
#else // !HAVE_GETOPT
  int i = 1;
#endif // !HAVE_GETOPT

  if ((argc - i) != 1) {
    usage(argv[0]);
    exit(1);
  }

  SoDB::init();
  SoNodeKit::init();
  SoInteraction::init();

  SoType kittype = SoType::fromName(argv[i]);
  if (kittype == SoType::badType()) {
    fprintf(stderr, "ERROR: unknown class type ``%s''.\n", argv[i]);
    exit(1);
  }
  else if (!kittype.isDerivedFrom(SoBaseKit::getClassTypeId())) {
    fprintf(stderr, "ERROR: type ``%s'' is not a nodekit.\n", argv[i]);
    exit(1);
  }

  SoBaseKit * bk = (SoBaseKit *)kittype.createInstance();

  bk->printDiagram();
  fprintf(stdout, "\n\n");
  bk->printTable();

  return 0;
}
