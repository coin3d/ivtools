/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2000 by Systems in Motion. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation. See the
 *  file LICENSE.LGPL at the root directory of the distribution for
 *  more details.
 *
 *  If you want to use Coin for applications not compatible with the
 *  LGPL, please contact SIM to acquire a Professional Edition license.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  http://www.sim.no support@sim.no Voice: +47 22114160 Fax: +47 22207097
 *
\**************************************************************************/

#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>
#if defined(COIN_INTERNAL)
#include <Inventor/lists/SbStringList.h>
#include <Inventor/lists/SoTypeList.h>
#else // Compiled outside the internal build process.
#include <Inventor/SoLists.h>
#include <Inventor/SbPList.h>
#endif
#include <stdio.h>

const int INDENTSIZE = 2;
static int indentation;


void
usage(const char * invocationname)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "  Usage: %s [classname classname ...]\n\n", invocationname);
  fprintf(stderr, "  List the full inheritance hierarchy below the\n");
  fprintf(stderr, "  given classnames and the list of parent classes\n");
  fprintf(stderr, "  leading down to each of the given classnames.\n\n");
  fprintf(stderr, "  EXAMPLES:\n\n");
  fprintf(stderr, "    $ %s SoEngine\n", invocationname);
  fprintf(stderr, "                  ...this will list the inheritance\n");
  fprintf(stderr, "                  hierarchy for the engine classes.\n\n");
  fprintf(stderr, "    $ %s Base Field Event Element Error SoDetail SoAction\n", invocationname);
  fprintf(stderr, "                  ...this will list _all_ \"typed\" classes.\n\n");
}

SbStringList
sort_types(SoTypeList & typelist)
{
  SbStringList strlist;

  for (int i=0; i < typelist.getLength(); i++) {
    SbString * s = new SbString(typelist[i].getName().getString());
    int j = 0;
    while ((j < strlist.getLength()) &&
           (strcmp(strlist[j]->getString(), s->getString()) < 0)) j++;
    if (j < strlist.getLength()) strlist.insert(s, j);
    else strlist.append(s);
  }

  return strlist;
}

void
print_type(SoType t, int marked)
{
  int i = 0;
  if (marked) i += fprintf(stdout, "->");
  for (; i < indentation; i++) fprintf(stdout, " ");
  fprintf(stdout, "%s\n", t.getName().getString());
}

void
print_parent_tree(SoType t)
{
  if (t == SoType::badType()) return;
  print_parent_tree(t.getParent());
  print_type(t, 0);
  indentation += INDENTSIZE;
}

void
print_children_trees(SoType t)
{
  SoTypeList children;
  t.getAllDerivedFrom(t, children);

  SbStringList strlist = sort_types(children);
  for (int i=0; i < strlist.getLength(); i++) {
    SoType derived = SoType::fromName(strlist[i]->getString());
    if ((derived != t) && (derived.getParent() == t)) {
      print_type(derived, 0);
      indentation += INDENTSIZE;
      print_children_trees(derived);
      indentation -= INDENTSIZE;
    }
  }
}


int
main(int argc, char ** argv)
{
  if (argc == 1) {
    usage(argv[0]);
    exit(1);
  }

  SoDB::init();
  SoNodeKit::init();
  SoInteraction::init();

  for (int i=1; i < argc; i++) {
    indentation = INDENTSIZE;

    SoType t = SoType::fromName(argv[i]);
    if (t == SoType::badType()) {
      fprintf(stderr,
              "\nCouldn't find a valid type identifier for ``%s''.\n\n",
              argv[i]);
    }
    else {
      print_parent_tree(t.getParent());
      print_type(t, 1);
      indentation += INDENTSIZE;
      print_children_trees(t);
    }

    if (i < argc-1) fprintf(stdout, "\n");
  }

  return 0;
}
