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
