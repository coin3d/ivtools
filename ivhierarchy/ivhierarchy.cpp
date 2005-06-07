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

#include <string.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoLists.h>
#include <Inventor/SbPList.h>
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
