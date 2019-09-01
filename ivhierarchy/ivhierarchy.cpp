/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

#include <cstring>
#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoLists.h>
#include <Inventor/SbPList.h>
#include <cstdio>

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
