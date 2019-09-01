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
#include <cstring>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSeparator.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#if HAVE_UNISTD_H
#include <unistd.h> // getopt(), isatty()
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_LOCAL_GETOPT_H
  #include "getopt.h"
#endif

void
usage(const char * argv_0)
{
  fprintf(stderr, "\nUsage: %s [-bfth] [-o filename] file ...\n", argv_0);

  fprintf(stderr, "\t-b:\twrite in binary format\n");
  fprintf(stderr, "\t-f:\tflatten SoFile nodes\n");
  fprintf(stderr, "\t-t:\tinline all textures\n");
  fprintf(stderr, "\t-h:\tshow usage\n");
  fprintf(stderr, "\t-o:\twrite to ``filename'' instead of stdout\n");

  fprintf(stderr,
          "\n  ``%s'' will read all files given on the command line,\n",
          argv_0);
  fprintf(stderr,
          "  compile them into a single scene and write them back out\n");
  fprintf(stderr,
          "  again on stdout (unless the ``-o'' option is used).\n\n");
  fprintf(stderr,
          "  Use ``-'' or no input files to read from stdin.\n\n");
}


#ifdef HAVE_ISATTY
#define CHECK_TTY(fileptr) \
  do { \
    if (isatty(fileno(fileptr))) { \
      usage(argv[0]); \
      fprintf(stderr, "Error: trying to read from standard input, but " \
              "standard input is a tty!\n\n"); \
      exit(1); \
    } \
  } while (0)
#else // !HAVE_ISATTY
#define CHECK_TTY(fileptr) do { } while (0)
#endif // !HAVE_ISATTY



int
main(int argc, char * argv[])
{
  int writeasbinary = 0;
  int flattenfiles = 0;
  int flattentextures = 0;
  const char * outname = NULL;

  /* Parse command line. */
  int getoptchar;
  while ((getoptchar = getopt(argc, argv, "bftho:")) != EOF) {
    char c = (char)getoptchar;
    switch (c) {
    case 'b':
      writeasbinary = 1;
      break;
    case 'f':
      flattenfiles = 1;
      break;
    case 't':
      flattentextures = 1;
      break;
    case 'h':
      usage(argv[0]);
      exit(0);
      break;
    case 'o':
      outname = optarg;
      if (!outname) {
        usage(argv[0]);
        exit(1);
      }
      break;
    case '?':
      usage(argv[0]);
      exit(1);
      break;
    }
  }
  int i = optind;

  SoDB::init();
  SoNodeKit::init();
  SoInteraction::init();

  SoInput stdinp;
  SoSeparator * root = new SoSeparator;

  if (i >= argc) {
    CHECK_TTY(stdin);
    /* No files specified, read from stdin only. */
    SoSeparator * tmp = SoDB::readAll(&stdinp);
    if (tmp) root->addChild(tmp);
    else {
      fprintf(stderr, "Error: not a valid file on stdin.\n");
    }
  }
  else {
    SoInput fileinp;

    for (; i < argc; i++) {
      SoInput * inp = NULL;

      if (strcmp("-", argv[i]) == 0) {
        CHECK_TTY(stdinp.getCurFile());
        inp = &stdinp;
      }
      else if (fileinp.openFile(argv[i])) {
        inp = &fileinp;
      }
      else {
        fprintf(stderr, "Couldn't open file '%s' -- skipping.\n", argv[i]);
      }

      if (inp) {
        SoSeparator * tmproot = SoDB::readAll(inp);
        if (tmproot) {
          root->addChild(tmproot->getNumChildren() == 1 ?
                         tmproot->getChild(0) : tmproot);
        }
        else {
          fprintf(stderr, "Couldn't read file '%s' -- skipping.\n", argv[i]);
        }

        if (inp == &fileinp) inp->closeFile();
      }
    }
  }

  if (flattenfiles) {
    // FIXME: flatten SoFile nodes. 19991009 mortene.
    fprintf(stderr, "Warning: SoFile flattening not supported yet!\n");
  }

  if (flattentextures) {
    // FIXME: inline SoTexture2 nodes. 19991009 mortene.
    fprintf(stderr, "Warning: texture inlining not supported yet!\n");
  }

  if (root->getNumChildren() > 0) {
    SoOutput out;
    if (outname) {
      if (!out.openFile(outname)) {
        fprintf(stderr, "Couldn't write output to '%s'.\n", outname);
        exit(1);
      }
    }
    out.setBinary(writeasbinary);

    SoWriteAction wa(&out);
    for (int i=0; i < root->getNumChildren(); i++) {
      wa.apply(root->getChild(i));
    }
  }

  return 0;
}
