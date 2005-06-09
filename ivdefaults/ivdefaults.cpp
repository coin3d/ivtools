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

#include <stdio.h>
#include <assert.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/fields/SoFieldContainer.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/SoLists.h>

// *************************************************************************

void
usage(const char * invocationname)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "  Usage: %s [classname classname ...]\n\n", invocationname);
  fprintf(stderr, "  List the FILE FORMATS/DEFAULTS section used in the "
          "API documentation.\n\n");
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
    SoType t = SoType::fromName(argv[i]);
    if (t == SoType::badType()) {
      fprintf(stderr,
              "\nCouldn't find a valid type identifier for ``%s''.\n\n",
              argv[i]);
    }
    else if (!t.isDerivedFrom(SoFieldContainer::getClassTypeId())) {
      fprintf(stderr,
              "\n``%s'' type is not an SoFieldContainer.\n\n",
              argv[i]);
    }
    else if (!t.canCreateInstance()) {
      fprintf(stderr,
              "\n``%s'' type is an abstract class.\n\n",
              argv[i]);
    }
    else {
      const SoFieldContainer * fc = (const SoFieldContainer *)t.createInstance();

      fprintf(stdout, "FILE FORMAT/DEFAULTS\n\n");
      fprintf(stdout, "    %s {\n", t.getName().getString());

      SoFieldList l;
      const int nr = fc->getAllFields(l);
      for (int j=0; j < nr; j++) {
        SoField * f = l[j];
        SbName n;
        const SbBool ok = fc->getFieldName(f, n);
        assert(ok);

        if (fc->isOfType(SoBaseKit::getClassTypeId())) {
          SoBaseKit * bknode = (SoBaseKit *)fc;
          const SoNodekitCatalog * nkc = bknode->getNodekitCatalog();
          assert(nkc);

          if (nkc->getPartNumber(n) != SO_CATALOG_NAME_NOT_FOUND) {
            continue;
          }
        }

        fprintf(stdout, "        %s ", n.getString());

        SbString valuestring;
        f->get(valuestring);
        fprintf(stdout, "%s\n", valuestring.getString());
      }

      fprintf(stdout, "    }\n\n");
    }
  }

  return 0;
}

// *************************************************************************
