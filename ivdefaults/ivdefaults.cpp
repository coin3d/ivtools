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

#include <cstdio>
#include <cassert>
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
